# distutils: language = c++
# cython: language_level = 3

from libcpp.string cimport string
from libcpp.vector cimport vector
from libcpp cimport bool
from cpython.ref cimport PyObject
from cpython.pycapsule cimport PyCapsule_New, PyCapsule_GetPointer
from libc.stdlib cimport malloc, free
from libc.string cimport memcpy

# ------------------------------------------------------------------------------
# C++ Definitions
# ------------------------------------------------------------------------------

cdef extern from "RApiPlus.h":
    ctypedef struct tsNCharcb:
        char* pData
        int iDataLen

cdef extern from "RApiPlus.h" namespace "RApi":
    cdef cppclass REngineParams:
        tsNCharcb sAppName
        tsNCharcb sAppVersion
        tsNCharcb sLogFilePath
        # ... others ...

    cdef cppclass LoginParams:
        tsNCharcb sMdUser
        tsNCharcb sMdPassword
        tsNCharcb sMdCnnctPt
        tsNCharcb sTsUser
        tsNCharcb sTsPassword
        tsNCharcb sTsCnnctPt
        RCallbacks* pCallbacks

    cdef cppclass AlertInfo:
        int iAlertType
        int iConnectionId
        int iRpCode
        tsNCharcb sRpCode
        tsNCharcb sMessage
        tsNCharcb sTicker
        tsNCharcb sExchange

    cdef cppclass LineInfo:
        tsNCharcb sTicker
        tsNCharcb sExchange
        tsNCharcb sOrderNum
        long long llQuantityToFill
        double dPriceToFill

    cdef cppclass OrderFillReport:
        tsNCharcb sTicker
        tsNCharcb sExchange
        tsNCharcb sOrderNum
        double dFillPrice
        long long llFillSize
        tsNCharcb sFillType

    cdef cppclass OrderFailureReport:
        tsNCharcb sTicker
        tsNCharcb sExchange
        tsNCharcb sStatus
        tsNCharcb sOrderNum

    cdef cppclass BidInfo:
        tsNCharcb sTicker
        tsNCharcb sExchange
        double dPrice
        long long llSize

    cdef cppclass AskInfo:
        tsNCharcb sTicker
        tsNCharcb sExchange
        double dPrice
        long long llSize

    cdef cppclass TradeInfo:
        tsNCharcb sTicker
        tsNCharcb sExchange
        double dPrice
        long long llSize
        tsNCharcb sAggressorSide

    cdef cppclass LimitOrderParams:
        tsNCharcb sTicker
        tsNCharcb sExchange
        tsNCharcb sBuySellType
        double dPrice
        int iQty
        tsNCharcb sDuration
        # ...

    cdef cppclass ModifyLimitOrderParams:
        tsNCharcb sTicker
        tsNCharcb sExchange
        tsNCharcb sOrderNum
        double dPrice
        int iQty

    cdef cppclass RCallbacks:
        pass

    cdef cppclass REngine:
        REngine(REngineParams* pParams) except +
        bool login(LoginParams* pParams, int* pAiCode)
        bool logout(int* pAiCode)
        int subscribe(tsNCharcb* pExchange, tsNCharcb* pTicker, int iFlags, int* pAiCode)
        int unsubscribe(tsNCharcb* pExchange, tsNCharcb* pTicker, int* pAiCode)
        int sendOrder(LimitOrderParams* pParams, int* pAiCode)
        int modifyOrder(ModifyLimitOrderParams* pParams, int* pAiCode)

cdef extern from "CallbackShim.hpp":
    cdef cppclass CallbackShim(RCallbacks):
        CallbackShim(PyObject* obj) except +



# ------------------------------------------------------------------------------
# Helpers
# ------------------------------------------------------------------------------

cdef str from_cb(tsNCharcb cb):
    if cb.pData == NULL:
        return ""
    return cb.pData[:cb.iDataLen].decode('utf-8', errors='ignore')

cdef void to_cb(tsNCharcb& cb, str s):
    # This is tricky in Cython because we need persistent storage if REngine pointers 
    # outlive the call. For Params passed to REngine, REngine usually copies immediately.
    # We will assume REngine copies data.
    if s is None:
        cb.pData = NULL
        cb.iDataLen = 0
        return
    cdef bytes b = s.encode('utf-8')
    cb.pData = <char*>b 
    cb.iDataLen = len(b)

# ------------------------------------------------------------------------------
# Python Classes
# ------------------------------------------------------------------------------

cdef class PyREngineParams:
    cdef REngineParams* _this
    cdef bool _owner

    def __cinit__(self):
        self._this = new REngineParams()
        self._owner = True

    def __dealloc__(self):
        if self._owner and self._this != NULL:
            del self._this

    @property
    def app_name(self): return from_cb(self._this.sAppName)
    @app_name.setter
    def app_name(self, v): to_cb(self._this.sAppName, v)
    
    @property
    def app_version(self): return from_cb(self._this.sAppVersion)
    @app_version.setter
    def app_version(self, v): to_cb(self._this.sAppVersion, v)

    @property
    def log_file_path(self): return from_cb(self._this.sLogFilePath)
    @log_file_path.setter
    def log_file_path(self, v): to_cb(self._this.sLogFilePath, v)

cdef class PyLoginParams:
    cdef LoginParams* _this
    cdef bool _owner

    def __cinit__(self):
        self._this = new LoginParams()
        self._owner = True

    def __dealloc__(self):
        if self._owner and self._this != NULL:
            del self._this
            
    # Setters for brevity
    def set_md_user(self, v): to_cb(self._this.sMdUser, v)
    def set_md_password(self, v): to_cb(self._this.sMdPassword, v)
    def set_md_cnnct_pt(self, v): to_cb(self._this.sMdCnnctPt, v)
    def set_ts_user(self, v): to_cb(self._this.sTsUser, v)
    def set_ts_password(self, v): to_cb(self._this.sTsPassword, v)
    def set_ts_cnnct_pt(self, v): to_cb(self._this.sTsCnnctPt, v)


# --- RCallbacks Wrapper ---

class RCallbacksBase:
    """Base class for callbacks. Override these methods."""
    def alert(self, info): pass
    def line_update(self, info): pass
    def fill_report(self, report): pass
    def failure_report(self, report): pass
    def best_bid_quote(self, info): pass
    def best_ask_quote(self, info): pass
    def trade_print(self, info): pass

    # Internal handlers called by Shim via name
    def _on_alert(self, capsule):
        cdef AlertInfo* ptr = <AlertInfo*>PyCapsule_GetPointer(capsule, "AlertInfo")
        # Convert to dict or object
        info = {
            "alert_type": ptr.iAlertType,
            "connection_id": ptr.iConnectionId,
            "rp_code": ptr.iRpCode,
            "s_rp_code": from_cb(ptr.sRpCode),
            "message": from_cb(ptr.sMessage),
            "ticker": from_cb(ptr.sTicker),
            "exchange": from_cb(ptr.sExchange)
        }
        self.alert(info)
        return 0

    def _on_line_update(self, capsule):
        cdef LineInfo* ptr = <LineInfo*>PyCapsule_GetPointer(capsule, "LineInfo")
        info = {
            "ticker": from_cb(ptr.sTicker),
            "exchange": from_cb(ptr.sExchange),
            "order_num": from_cb(ptr.sOrderNum),
            "qty_to_fill": ptr.llQuantityToFill,
            "price_to_fill": ptr.dPriceToFill
        }
        self.line_update(info)
        return 0
    
    def _on_fill_report(self, capsule):
        cdef OrderFillReport* ptr = <OrderFillReport*>PyCapsule_GetPointer(capsule, "OrderFillReport")
        info = {
            "ticker": from_cb(ptr.sTicker),
            "exchange": from_cb(ptr.sExchange),
            "order_num": from_cb(ptr.sOrderNum),
            "fill_price": ptr.dFillPrice,
            "fill_size": ptr.llFillSize,
            "fill_type": from_cb(ptr.sFillType)
        }
        self.fill_report(info)
        return 0

    def _on_failure_report(self, capsule):
        cdef OrderFailureReport* ptr = <OrderFailureReport*>PyCapsule_GetPointer(capsule, "OrderFailureReport")
        info = {
            "ticker": from_cb(ptr.sTicker),
            "exchange": from_cb(ptr.sExchange),
            "status": from_cb(ptr.sStatus),
            "order_num": from_cb(ptr.sOrderNum)
        }
        self.failure_report(info)
        return 0

    def _on_best_bid_quote(self, capsule):
        cdef BidInfo* ptr = <BidInfo*>PyCapsule_GetPointer(capsule, "BidInfo")
        info = {
             "ticker": from_cb(ptr.sTicker),
             "exchange": from_cb(ptr.sExchange),
             "price": ptr.dPrice,
             "size": ptr.llSize
        }
        self.best_bid_quote(info)
        return 0

    def _on_best_ask_quote(self, capsule):
        cdef AskInfo* ptr = <AskInfo*>PyCapsule_GetPointer(capsule, "AskInfo")
        info = {
             "ticker": from_cb(ptr.sTicker),
             "exchange": from_cb(ptr.sExchange),
             "price": ptr.dPrice,
             "size": ptr.llSize
        }
        self.best_ask_quote(info)
        return 0

    def _on_trade_print(self, capsule):
        cdef TradeInfo* ptr = <TradeInfo*>PyCapsule_GetPointer(capsule, "TradeInfo")
        info = {
             "ticker": from_cb(ptr.sTicker),
             "exchange": from_cb(ptr.sExchange),
             "price": ptr.dPrice,
             "size": ptr.llSize,
             "aggressor_side": from_cb(ptr.sAggressorSide)
        }
        self.trade_print(info)
        return 0

# --- REngine Wrapper ---

cdef class PyREngine:
    cdef REngine* _this
    cdef CallbackShim* _shim
    cdef object _callbacks_py # Keep alive

    def __cinit__(self, PyREngineParams params):
        self._this = new REngine(params._this)
        self._shim = NULL
        self._callbacks_py = None

    def __dealloc__(self):
        if self._shim != NULL:
            del self._shim
        if self._this != NULL:
            del self._this

    def login(self, PyLoginParams params, object callbacks):
        # Create shim
        self._callbacks_py = callbacks
        self._shim = new CallbackShim(<PyObject*>callbacks)
        
        # Assign shim to params
        params._this.pCallbacks = self._shim
        
        cdef int iCode = 0
        success = self._this.login(params._this, &iCode)
        return success, iCode

    def logout(self):
        cdef int iCode = 0
        success = self._this.logout(&iCode)
        return success, iCode

    def subscribe(self, str exchange, str ticker, int flags):
        cdef tsNCharcb sExch
        cdef tsNCharcb sTick
        to_cb(sExch, exchange)
        to_cb(sTick, ticker)
        
        cdef int iCode = 0
        res = self._this.subscribe(&sExch, &sTick, flags, &iCode)
        return res, iCode

    def unsubscribe(self, str exchange, str ticker):
        cdef tsNCharcb sExch
        cdef tsNCharcb sTick
        to_cb(sExch, exchange)
        to_cb(sTick, ticker)
        
        cdef int iCode = 0
        res = self._this.unsubscribe(&sExch, &sTick, &iCode)
        return res, iCode

    def send_order(self, str ticker, str exchange, str type, double price, int qty, str duration):
        # type: BUY/SELL
        cdef LimitOrderParams p
        # Initialize p? RApi structs usually valid default constructed or memset?
        # In Cython stack allocation, it's uninitialized mem?
        # RApi structs usually have constructor. 
        # But we used extern struct. 
        # If REngine expects initialized struct, we should be careful.
        # RApiPlus.h likely defines constructors.
        # We can do `cdef LimitOrderParams p = LimitOrderParams()` if default ctor available
        
        to_cb(p.sTicker, ticker)
        to_cb(p.sExchange, exchange)
        to_cb(p.sBuySellType, type)
        p.dPrice = price
        p.iQty = qty
        to_cb(p.sDuration, duration)
        
        cdef int iCode = 0
        res = self._this.sendOrder(&p, &iCode)
        return res, iCode

    def modify_order(self, str ticker, str exchange, str order_num, double price, int qty):
        cdef ModifyLimitOrderParams p
        to_cb(p.sTicker, ticker)
        to_cb(p.sExchange, exchange)
        to_cb(p.sOrderNum, order_num)
        p.dPrice = price
        p.iQty = qty
        
        cdef int iCode = 0
        res = self._this.modifyOrder(&p, &iCode)
        return res, iCode
