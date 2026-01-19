package main

import (
	"bufio"
	"encoding/json"
	"log"
	"net"
	"os"
	"time"
)

// Config
var (
	SierraHost = "localhost"
	SierraPort = "11099"
	BridgeKey  = ""
)

// DTC Messages (Minimal JSON Subset)
const (
	EncodingRequestType = 6
	LogonRequestType    = 1
	LogonResponseType   = 2
)

type BaseMessage struct {
	Type int `json:"Type"`
}

type EncodingRequest struct {
	Type            int    `json:"Type"`
	ProtocolVersion int32  `json:"ProtocolVersion"`
	Encoding        string `json:"Encoding"`
	ProtocolType    string `json:"ProtocolType"`
}

type LogonRequest struct {
	Type                       int    `json:"Type"`
	ProtocolVersion            int32  `json:"ProtocolVersion"`
	Username                   string `json:"Username"`
	Password                   string `json:"Password"`
	GeneralTextData            string `json:"GeneralTextData"`
	Integer_1                  int32  `json:"Integer_1"`
	Integer_2                  int32  `json:"Integer_2"`
	HeartbeatIntervalInSeconds int32  `json:"HeartbeatIntervalInSeconds"`
	TradeAccount               string `json:"TradeAccount"`
	HardwareIdentifier         string `json:"HardwareIdentifier"`
	ClientName                 string `json:"ClientName"`
}

func main() {
	// 1. Config
	if h := os.Getenv("QUANUX_SIERRA_HOST"); h != "" {
		SierraHost = h
	}
	if p := os.Getenv("QUANUX_SIERRA_PORT"); p != "" {
		SierraPort = p
	}
	BridgeKey = os.Getenv("QUANUX_BRIDGE_KEY") // For Auth back to Core
	if BridgeKey == "" {
		log.Println("Warning: QUANUX_BRIDGE_KEY not set. Core auth will fail.")
	}

	addr := net.JoinHostPort(SierraHost, SierraPort)
	log.Printf("Connecting to Sierra Chart DTC Server at %s...", addr)

	// 2. Connect via TCP
	conn, err := net.DialTimeout("tcp", addr, 5*time.Second)
	if err != nil {
		log.Fatalf("Failed to connect: %v", err)
	}
	defer conn.Close()
	log.Println("Connected!")

	// 3. Send Encoding Request (Ask for JSON)
	// DTC requires the first message to establish encoding.
	// We send EncodingRequest formatted as JSON to request JSON.
	encReq := EncodingRequest{
		Type:            EncodingRequestType,
		ProtocolVersion: 8,
		Encoding:        "JSON",
		ProtocolType:    "DTC",
	}
	sendJSON(conn, encReq)

	// 4. Send Logon
	logonReq := LogonRequest{
		Type:                       LogonRequestType,
		ProtocolVersion:            8,
		Username:                   "QuanuX",
		ClientName:                 "QuanuX Extension",
		HeartbeatIntervalInSeconds: 10,
	}
	sendJSON(conn, logonReq)

	// 5. Read Loop (Dump messages for pilot)
	reader := bufio.NewReader(conn)
	for {
		// DTC JSON messages are null-terminated or newline terminated depending on config
		// usually standard JSON requires parsing stream.
		// For simplicity in this pilot, assuming line-delimited or reading chunks.
		// Sierra Chart JSON is often netstring-like or just stream.
		// Let's read until we see '}' for basic testing, strictly a Pilot.
		// BETTER: Read byte by byte until 0x00 (Null) if using older DTC,
		// but JSON often doesn't use null.
		// Actually DTC JSON specs say: messages are not frame-delimited easily without custom parser.
		// WE WILL ASSUME basic read for now to verify connection.

		line, err := reader.ReadBytes('}')
		if err != nil {
			log.Printf("Read error: %v", err)
			break
		}
		log.Printf("RX: %s", string(line))
	}
}

func sendJSON(conn net.Conn, v interface{}) {
	data, err := json.Marshal(v)
	if err != nil {
		log.Printf("Marshal error: %v", err)
		return
	}
	// Note: DTC JSON spec might mandate specific framing.
	// Sending raw JSON is usually accepted for the handshake.
	log.Printf("TX: %s", string(data))
	conn.Write(data)
}
