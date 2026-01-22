
import logging
from extensions.python.wrappers import rithmic

logger = logging.getLogger(__name__)

class RithmicBridge(rithmic.RCallbacks):
    def __init__(self, user, password, server, app_name="QuanuX", app_version="1.0.0"):
        super().__init__()
        
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

    def connect(self):
        logger.info("Connecting to Rithmic...")
        # Note: login is blocking/synchronous in the wrapper binding for now
        # Ideally should be async or threaded in production
        retval = self.engine.login(self.login_params)
        if retval == 1: # OK
            logger.info("Login successful!")
        else:
            logger.error(f"Login failed: {retval}")
            
    def disconnect(self):
        self.engine.logout()

    def subscribe_market_data(self, exchange, ticker):
        logger.info(f"Subscribing to {exchange}:{ticker}")
        # Flags: 1=MD_IMAGE_CB, 2=MD_UPDATE_CB (update only usually preferable for latency?)
        # 4=MD_BEST_BID_OFFER
        # Need to check RApi header constants. Usually we want BEST (4) and TRAde (?).
        # For now assume 6 (BEST | TRADE ?)
        # Let's use 10 (MD_PRINTS | MD_BEST ?)
        # RApi header: 4=BEST, 8=CLOSE, 16=PRINTS.
        flags = 4 | 16 # Best + Prints
        self.engine.subscribe(exchange, ticker, flags)

    def send_limit_order(self, exchange, ticker, qty, price, is_buy):
        params = rithmic.LimitOrderParams()
        params.sExchange = exchange
        params.sTicker = ticker
        params.iQty = qty
        params.dPrice = price
        params.sBuySellType = "B" if is_buy else "S"
        params.sDuration = "Day"
        params.sOrderType = "L" # Although not in Struct, might be needed if implicit or bound elsewhere?
        # Actually in our binding we removed sOrderType from LimitOrderParams because it wasn't there.
        # RApi probably infers it from the param type or we set it in OrderParams constructor.
        # But sendOrder takes LimitOrderParams directly.
        
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

    # --- Callbacks ---
    
    def Alert(self, info, context, code):
        """Handle system alerts (disconnects, login status, etc)"""
        # info has sMessage, iAlertType
        logger.info(f"Alert: {info.iAlertType} - {info.sMessage}")
        return 1

    def BestBidQuote(self, info, context, code):
        print(f"BID: {info.sTicker} {info.dPrice} x {info.llSize}")
        return 1

    def BestAskQuote(self, info, context, code):
        print(f"ASK: {info.sTicker} {info.dPrice} x {info.llSize}")
        return 1

    def TradePrint(self, info, context, code):
        print(f"TRADE: {info.sTicker} {info.dPrice} x {info.llSize} ({info.sAggressorSide})")
        return 1

    def LineUpdate(self, info, context, code):
        """Order updates"""
        status = getattr(info, 'sStatus', 'Unknown') # sStatus might be in LineInfo
        # In our binding LineInfo has sOrderNum, sTicker, llQuantityToFill, dPriceToFill
        # sStatus is NOT in standard LineInfo? 
        # Check bindings.cpp: LineInfo has sTicker, sExchange, sOrderNum
        # sStatus is NOT bound in LineInfo in bindings.cpp? 
        # Let's check bindings.cpp again.
        print(f"ORDER UPDATE: {info.sOrderNum} Qty:{info.llQuantityToFill}")
        return 1

    def FillReport(self, report, context, code):
        print(f"FILL: {report.sTicker} {report.iFillQty} @ {report.dFillPrice}")
        return 1
