package main

import (
	"encoding/binary"
	"encoding/json"
	"fmt"
	"log"
	"os"
	"time"

	pb "quant/extensions/rithmic/api"

	"github.com/gorilla/websocket"
	zmq "github.com/pebbe/zmq4"
	"google.golang.org/protobuf/proto"
)

// Constants from Rithmic Spec
const (
	RITHMIC_TEST_SYSTEM = "Rithmic Test"
	RITHMIC_PAPER_URL   = "wss://rituz00100.rithmic.com:443"

	// Message Type IDs
	MSG_REQUEST_LOGIN        = 10
	MSG_RESPONSE_LOGIN       = 11
	MSG_REQUEST_SYSTEM_INFO  = 16
	MSG_RESPONSE_SYSTEM_INFO = 17
	MSG_REQUEST_HEARTBEAT    = 18
	MSG_RESPONSE_HEARTBEAT   = 19
	MSG_REQUEST_MKT_DATA     = 100
	MSG_RESPONSE_MKT_DATA    = 101
	MSG_LAST_TRADE           = 150
	MSG_REJECT               = 75
)

var (
	RithmicUser = os.Getenv("QUANUX_RITHMIC_USER")
	RithmicPass = os.Getenv("QUANUX_RITHMIC_PASS")
	ZmqPubPort  = os.Getenv("QUANUX_ZMQ_PUB_PORT")
)

type Tick struct {
	Type      string  `json:"type"`
	Symbol    string  `json:"symbol"`
	Price     float64 `json:"price"`
	Volume    int     `json:"volume"`
	Timestamp string  `json:"timestamp"`
}

func main() {
	if ZmqPubPort == "" {
		ZmqPubPort = "5557"
	}

	log.Println("🚀 Starting Rithmic Bridge (Official Proto)...")

	// 1. ZMQ Setup
	zctx, _ := zmq.NewContext()
	pub, _ := zctx.NewSocket(zmq.PUB)
	pub.Bind(fmt.Sprintf("tcp://*:%s", ZmqPubPort))
	tickChan := make(chan Tick, 1000)
	go func() {
		for tick := range tickChan {
			b, _ := json.Marshal(tick)
			pub.SendBytes(b, 0)
		}
	}()

	// 2. Discover Systems (Step 1)
	log.Println("🔍 Step 1: Discovering Systems...")
	systems, err := getSystemInfo()
	if err != nil {
		log.Fatalf("System Info Failed: %v", err)
	}
	log.Printf("📋 Available Systems: %v", systems)

	// 3. Login & Subscribe (Step 2)
	log.Println("🔐 Step 2: Logging in to 'Rithmic Test'...")
	runSession(tickChan)
}

func getSystemInfo() ([]string, error) {
	c, _, err := websocket.DefaultDialer.Dial(RITHMIC_PAPER_URL, nil)
	if err != nil {
		return nil, err
	}
	defer c.Close()

	// Send RequestRithmicSystemInfo
	req := &pb.RequestRithmicSystemInfo{
		TemplateId: proto.Int32(MSG_REQUEST_SYSTEM_INFO),
		UserMsg:    []string{"sys_info_req"},
	}
	if err := sendProto(c, req); err != nil {
		return nil, err
	}

	// Read and parse until we get a response
	for {
		msg, err := readProto(c)
		if err != nil {
			return nil, err
		}
		if msg == nil {
			continue
		}

		var resp pb.ResponseRithmicSystemInfo
		if err := proto.Unmarshal(msg, &resp); err == nil {
			if resp.TemplateId != nil && *resp.TemplateId == MSG_RESPONSE_SYSTEM_INFO {
				return resp.SystemName, nil
			}
		}
	}
}

func runSession(ch chan<- Tick) {
	c, _, err := websocket.DefaultDialer.Dial(RITHMIC_PAPER_URL, nil)
	if err != nil {
		log.Fatal(err)
	}
	defer c.Close()

	// 1. Login
	infraType := pb.RequestLogin_TICKER_PLANT
	loginReq := &pb.RequestLogin{
		TemplateId:      proto.Int32(MSG_REQUEST_LOGIN),
		TemplateVersion: proto.String("3.9"),
		User:            proto.String(RithmicUser),
		Password:        proto.String(RithmicPass),
		AppName:         proto.String("QuanuX"),
		AppVersion:      proto.String("1.0.0"),
		SystemName:      proto.String(RITHMIC_TEST_SYSTEM),
		InfraType:       &infraType,
	}
	if err := sendProto(c, loginReq); err != nil {
		log.Fatal(err)
	}

	heartbeatTicker := time.NewTicker(30 * time.Second)
	defer heartbeatTicker.Stop()

	loggedIn := false

	// Read Loop
	go func() {
		for {
			msg, err := readProto(c)
			if err != nil {
				log.Println("Read Error:", err)
				return
			}
			if msg == nil {
				continue
			}

			// Try Decode ResponseLogin
			var loginResp pb.ResponseLogin
			if err := proto.Unmarshal(msg, &loginResp); err == nil {
				if loginResp.TemplateId != nil && *loginResp.TemplateId == MSG_RESPONSE_LOGIN {
					if len(loginResp.RpCode) > 0 && loginResp.RpCode[0] == "0" {
						log.Println("✅ Login Successful!")
						loggedIn = true
						// Subscribe
						reqType := pb.RequestMarketDataUpdate_SUBSCRIBE
						subReq := &pb.RequestMarketDataUpdate{
							TemplateId: proto.Int32(MSG_REQUEST_MKT_DATA),
							Symbol:     proto.String("ESH6"),
							Exchange:   proto.String("CME"),
							Request:    &reqType,
						}
						sendProto(c, subReq)
						log.Println("📡 Subscribed to ESH6")
					} else {
						log.Printf("❌ Login Failed: %v", loginResp.RpCode)
					}
					continue
				}
			}

			// Try Decode LastTrade
			var trade pb.LastTrade
			if err := proto.Unmarshal(msg, &trade); err == nil {
				if trade.TemplateId != nil && *trade.TemplateId == MSG_LAST_TRADE {
					price := 0.0
					if trade.TradePrice != nil {
						price = *trade.TradePrice
					}
					size := int32(0)
					if trade.TradeSize != nil {
						size = *trade.TradeSize
					}
					sym := ""
					if trade.Symbol != nil {
						sym = *trade.Symbol
					}

					log.Printf("💰 TICK: %s %f (Vol: %d)", sym, price, size)
					ch <- Tick{
						Type: "tick", Symbol: sym, Price: price,
						Volume: int(size), Timestamp: time.Now().Format(time.RFC3339),
					}
					continue
				}
			}
		}
	}()

	// Heartbeat Loop
	for range heartbeatTicker.C {
		if loggedIn {
			hb := &pb.RequestHeartbeat{TemplateId: proto.Int32(MSG_REQUEST_HEARTBEAT)}
			sendProto(c, hb)
		}
	}
}

// Helper: Send Length-Prefixed Proto
func sendProto(c *websocket.Conn, msg proto.Message) error {
	out, _ := proto.Marshal(msg)
	buf := make([]byte, 4+len(out))
	binary.BigEndian.PutUint32(buf[:4], uint32(len(out)))
	copy(buf[4:], out)
	return c.WriteMessage(websocket.BinaryMessage, buf)
}

// Helper: Read Length-Prefixed Proto
func readProto(c *websocket.Conn) ([]byte, error) {
	_, msg, err := c.ReadMessage()
	if err != nil {
		return nil, err
	}
	if len(msg) > 4 {
		return msg[4:], nil
	}
	return nil, nil // Invalid or empty
}
