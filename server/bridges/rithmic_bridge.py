
import logging
import asyncio
from extensions.python.wrappers import rithmic

logger = logging.getLogger(__name__)

class RithmicBridge(rithmic.RCallbacks):
    def __init__(self, user, password, server, app_name="QuanuX", app_version="1.0.0", loop=None):
        super().__init__()
        self.loop = loop or asyncio.get_event_loop()
        self.queue = asyncio.Queue()
        
        # Engine setup
        self.engine_params = rithmic.REngineParams()
        self.engine_params.sAppName = app_name
        self.engine_params.sAppVersion = app_version
        self.engine_params.sLogFilePath = "rithmic.log"
        
        self.engine = rithmic.REngine(self.engine_params)
        
        # Connection params
        self.login_params = rithmic.LoginParams()
        self.login_params.sMdUser = user
        self.login_params.sMdPassword = password
        self.login_params.sMdCnnctPt = server # e.g. "Rithmic Paper Trading"
        
        self.login_params.sTsUser = user
        self.login_params.sTsPassword = password
        self.login_params.sTsCnnctPt = server
        
        # Link callbacks
        self.login_params.pCallbacks = self

    async def connect(self):
        logger.info("Connecting to Rithmic...")
        # Login is blocking, so run in executor to avoid blocking the event loop
        retval = await self.loop.run_in_executor(None, self.engine.login, self.login_params)
        
        if retval == 1: # OK
            logger.info("Login successful!")
            # Start processing events
            self.loop.create_task(self.process_events())
        else:
            logger.error(f"Login failed: {retval}")
            
    async def disconnect(self):
        await self.loop.run_in_executor(None, self.engine.logout)

    def subscribe_market_data(self, exchange, ticker):
        logger.info(f"Subscribing to {exchange}:{ticker}")
        # Flags: 4=MD_BEST_BID_OFFER, 16=MD_PRINTS
        flags = 4 | 16 
        self.engine.subscribe(exchange, ticker, flags)

    def send_limit_order(self, exchange, ticker, qty, price, is_buy):
        params = rithmic.LimitOrderParams()
        params.sExchange = exchange
        params.sTicker = ticker
        params.iQty = qty
        params.dPrice = price
        params.sBuySellType = "B" if is_buy else "S"
        params.sDuration = "Day"
        
        ret = self.engine.sendOrder(params)
        logger.info(f"Sent order: {ret}")

    def modify_order(self, exchange, ticker, order_num, qty, price):
        params = rithmic.ModifyLimitOrderParams()
        params.sExchange = exchange
        params.sTicker = ticker
        params.sOrderNum = order_num
        params.iQty = qty
        params.dPrice = price
        
        ret = self.engine.modifyOrder(params)
        logger.info(f"Modified order {order_num}: {ret}")

    async def process_events(self):
        """Consume events from the thread-safe queue"""
        while True:
            item = await self.queue.get()
            try:
                msg_type, data = item
                if msg_type == "BID":
                    print(f"BID_ASYNC: {data.sTicker} {data.dPrice} x {data.llSize}")
                elif msg_type == "ASK":
                    print(f"ASK_ASYNC: {data.sTicker} {data.dPrice} x {data.llSize}")
                elif msg_type == "TRADE":
                    print(f"TRADE_ASYNC: {data.sTicker} {data.dPrice} x {data.llSize} ({data.sAggressorSide})")
                elif msg_type == "FILL":
                    print(f"FILL_ASYNC: {data.sTicker} {data.iFillQty} @ {data.dFillPrice}")
            except Exception as e:
                logger.error(f"Error processing event: {e}")
            finally:
                self.queue.task_done()

    # --- Callbacks (Thread-Safe) ---
    
    def _enqueue(self, msg_type, info):
        # This runs in the C++ thread, so we must use call_soon_threadsafe
        # We might need to copy info if it's a pointer that expires (common in C++ APIs)
        # Assuming bindings handle copy or these are value structs/Python objects now
        self.loop.call_soon_threadsafe(self.queue.put_nowait, (msg_type, info))

    def Alert(self, info, context, code):
        logger.info(f"Alert: {info.iAlertType} - {info.sMessage}")
        return 1

    def BestBidQuote(self, info, context, code):
        self._enqueue("BID", info)
        return 1

    def BestAskQuote(self, info, context, code):
        self._enqueue("ASK", info)
        return 1

    def TradePrint(self, info, context, code):
        self._enqueue("TRADE", info)
        return 1

    def LineUpdate(self, info, context, code):
        print(f"ORDER UPDATE: {info.sOrderNum} Qty:{info.llQuantityToFill}")
        return 1

    def FillReport(self, report, context, code):
        self._enqueue("FILL", report)
        return 1
