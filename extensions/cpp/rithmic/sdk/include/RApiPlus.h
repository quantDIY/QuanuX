#ifndef __RAPI_H_INCL
#define __RAPI_H_INCL

/*   =====================================================================   */

#ifdef MAKING_RAPI_LIB
#include "OmneStreamEngine.h"
#else

/*   =====================================================================   */
/*                                      KIT                                  */
/*   =====================================================================   */

#define OK               (1)
#define NOT_OK           (0)

typedef struct sNCharcb
     {
     char * pData;
     int    iDataLen;
     }
     tsNCharcb;

/*   =====================================================================   */
/*                                 error codes                               */
/*   =====================================================================   */

#define API_OK                 0
#define API_OS_ERROR           1
#define API_TBL_ERROR          2
#define API_MNM_ERROR          3
#define API_MISC_ERROR         4
#define API_LL_ERROR           5
#define API_BAD_INPUT          6
#define API_NO_DATA            7
#define API_ALREADY_EXISTS     8
#define API_HANDLE_IN_USE      9
#define API_IN_PROGRESS       10
#define API_NO_HANDLE         11
#define API_OUTPUT_INHIBITED  12
#define API_PERMISSION_DENIED 13
#define API_UNKNOWN_REQUEST   14
#define API_LEX_ERROR         15
#define API_HASH_ERROR        16
#define API_IGNORED           17
#define API_HSTGRM_ERROR      18
#define API_DUO_ERROR         19
#define API_SNM_ERROR         20
#define API_TOKEN_ERROR       21
#define API_KID_ERROR         22
#define API_MINOR_ERROR       23
#define API_ENVIRONMENT_ERROR 24
#define API_LIFO_ERROR        25
#define API_SQ_ERROR          26
#define API_FSM_ERROR         27
#define API_PARSE_ERROR       28
#define API_BTREE_ERROR       29
#define API_RDM_ERROR         30
#define API_STATE_ERROR       31
#define API_IDX_ERROR         32
#define API_VEC_ERROR         33
#define API_MIME_ERROR        34
#define API_MEM_ERROR         35
#define API_MTF_ERROR         36
#define API_EC_ERROR          37
#define API_DICT_ERROR        38
#define API_TNT_ERROR         39
#define API_ATRI_ERROR        40
#define API_NHASH_ERROR       41
#define API_AM_ERROR          42
#define API_OMC_ERROR         43
#define API_NCV_ERROR         44
#define API_OV_ERROR          45
#define API_NEO_ERROR         46
#define API_ENT_ERROR         47
#define API_AMI_ERROR         48
#define API_FIX_ERROR         49
#define API_CODEC_ERROR       50
#define API_LOC_ERROR         51
#define API_LIMIT_ERROR       52
#define API_DESTROYED         53
#define API_DNS_ERROR         54
#define API_FF_ERROR          55
#define API_BIT_ERROR         56
#define API_HUFF_ERROR        57
#define API_MHASH_ERROR       58
#define API_SB_ERROR          59
#define API_TB_ERROR          60
#define API_SDM_ERROR         61
#define API_SHV_ERROR         62
#define API_OTHER_ERROR       63
#define API_SUSPENDED         64
#define API_SBE_ERROR         65
#define API_MC_ERROR          66
#define API_SAM_ERROR         67
#define API_CQ_ERROR          68
#define API_PQ_ERROR          69
#define API_PRQ_ERROR         70
#define API_ONM_ERROR         71
#define API_NOT_IMPLEMENTED   72
#define API_TOO_BIG           73
#define API_RAD_ERROR         74
#define API_TIMED_OUT         75
#define API_DISCONNECTED      76

#define API_NUM_ERROR_CODES   77

/*   =====================================================================   */
/*                                 OmneEngine                                */
/*   =====================================================================   */

class OmneException
     {
     public :

     OmneException(int iError);

     int          getErrorCode();
     const char * getErrorString();

     virtual ~OmneException();

     private :

     int PRI_iError;
     };

namespace OmneEngineSpace
{
class OmneTimerCB
     {
     public :

     virtual int call(tsNCharcb * pTimerName,
                      void *      pContext,
                      int *       aiCode) = 0;

     virtual ~OmneTimerCB()
     {
     }
     };
};

#endif                                                  /* MAKING_RAPI_LIB   */
/*   =====================================================================   */

namespace RApi
{
const int ALERT_CONNECTION_OPENED =  1;
const int ALERT_CONNECTION_CLOSED =  2;
const int ALERT_CONNECTION_BROKEN =  3;
const int ALERT_LOGIN_COMPLETE    =  4;
const int ALERT_LOGIN_FAILED      =  5;
const int ALERT_SERVICE_ERROR     =  6;
const int ALERT_FORCED_LOGOUT     =  7;
const int ALERT_QUIET_HEARTBEAT   =  8;
const int ALERT_TRADING_DISABLED  =  9;
const int ALERT_TRADING_ENABLED   = 10;
const int ALERT_SHUTDOWN_SIGNAL   = 11;

const int MARKET_DATA_CONNECTION_ID      = 1;
const int TRADING_SYSTEM_CONNECTION_ID   = 2;
const int PNL_CONNECTION_ID              = 3;
const int INTRADAY_HISTORY_CONNECTION_ID = 4;
const int REPOSITORY_CONNECTION_ID       = 5;

const tsNCharcb sUSER_TYPE_FCM    = {(char *)"1", 1};
const tsNCharcb sUSER_TYPE_IB     = {(char *)"2", 1};
const tsNCharcb sUSER_TYPE_TRADER = {(char *)"3", 1};

const tsNCharcb sDEFAULT_ENVIRONMENT_KEY = {(char *)"system", 6};

const int MD_NONE                 = 0x00;
const int MD_PRINTS               = 0x01;
const int MD_QUOTES               = 0x02;
const int MD_BEST                 = 0x04;
const int MD_CLOSE                = 0x08;
const int MD_PRINTS_COND          = 0x10;
const int MD_SETTLEMENT           = 0x20;
const int MD_OPEN                 = 0x40;
const int MD_MARKET_MODE          = 0x80;
const int MD_HIGH_LOW             = 0x100;
const int MD_TRADE_VOLUME         = 0x200;
const int MD_CLOSING_INDICATOR    = 0x400;
const int MD_OPENING_INDICATOR    = 0x800;
const int MD_OPEN_INTEREST        = 0x1000;
const int MD_REF_DATA             = 0x2000;
const int MD_MID_PRICE            = 0x4000;
const int MD_HIGH_BID_LOW_ASK     = 0x8000;
const int MD_PRICE_LIMIT          = 0x10000;
const int MD_PROJECTED_SETTLEMENT = 0x20000;
const int MD_ALL                  = MD_PRINTS |
                                    MD_QUOTES |
                                    MD_BEST |
                                    MD_CLOSE |
                                    MD_PRINTS_COND |
                                    MD_SETTLEMENT |
                                    MD_OPEN |
                                    MD_MARKET_MODE |
                                    MD_HIGH_LOW |
                                    MD_TRADE_VOLUME |
                                    MD_CLOSING_INDICATOR |
                                    MD_OPENING_INDICATOR |
                                    MD_OPEN_INTEREST |
                                    MD_REF_DATA |
                                    MD_MID_PRICE |
                                    MD_HIGH_BID_LOW_ASK |
                                    MD_PRICE_LIMIT |
                                    MD_PROJECTED_SETTLEMENT;

const int MD_IMAGE_CB   = 1;
const int MD_UPDATE_CB  = 2;
const int MD_HISTORY_CB = 3;

const int UPDATE_TYPE_UNDEFINED  = 0;
const int UPDATE_TYPE_SOLO       = 1;
const int UPDATE_TYPE_BEGIN      = 2;
const int UPDATE_TYPE_MIDDLE     = 3;
const int UPDATE_TYPE_END        = 4;
const int UPDATE_TYPE_CLEAR      = 5;
const int UPDATE_TYPE_AGGREGATED = 6;

const int VALUE_STATE_IGNORE = 0;
const int VALUE_STATE_CLEAR  = 1;
const int VALUE_STATE_USE    = 2;

const int OP_NONE                     = (0);
const int OP_EQUAL_TO                 = (1);
const int OP_LESS_THAN                = (2);
const int OP_GREATER_THAN             = (3);
const int OP_LESS_THAN_OR_EQUAL_TO    = (4);
const int OP_GREATER_THAN_OR_EQUAL_TO = (5);
const int OP_NOT_EQUAL_TO             = (6);
const int OP_NOT_GREATER_THAN         = (7);
const int OP_NOT_LESS_THAN            = (8);

const int BEST_ASK_PRICE = 25;
const int BEST_ASK_SIZE  = 31;
const int BEST_BID_PRICE = 22;
const int BEST_BID_SIZE  = 30;
const int LEAN_PRICE     = 54909;
const int TRADE_PRICE    = 6;

const int BAR_TYPE_NONE   = 0;
const int BAR_TYPE_RANGE  = 1;
const int BAR_TYPE_TICK   = 2;
const int BAR_TYPE_VOLUME = 3;
const int BAR_TYPE_MINUTE = 4;
const int BAR_TYPE_DAILY  = 5;
const int BAR_TYPE_WEEKLY = 6;
const int BAR_TYPE_SECOND = 7;

const tsNCharcb sTRADE_ROUTE_STATUS_UP     = {(char *)"UP",   2};
const tsNCharcb sTRADE_ROUTE_STATUS_DOWN   = {(char *)"DOWN", 4};

const tsNCharcb sORDER_TYPE_LIMIT          = {(char *)"L",    1};
const tsNCharcb sORDER_TYPE_MARKET         = {(char *)"M",    1};
const tsNCharcb sORDER_TYPE_STOP_MARKET    = {(char *)"STP",  3};
const tsNCharcb sORDER_TYPE_STOP_LIMIT     = {(char *)"SLMT", 4};
const tsNCharcb sORDER_TYPE_MKT_IF_TOUCHED = {(char *)"MIT",  3};
const tsNCharcb sORDER_TYPE_LMT_IF_TOUCHED = {(char *)"LIT",  3};
const tsNCharcb sORDER_TYPE_EXTERNAL       = {(char *)"E",    1};

const tsNCharcb sBUY_SELL_TYPE_BUY               = {(char *)"B",   1};
const tsNCharcb sBUY_SELL_TYPE_SELL              = {(char *)"S",   1};
const tsNCharcb sBUY_SELL_TYPE_SELL_SHORT        = {(char *)"SS",  2};
const tsNCharcb sBUY_SELL_TYPE_SELL_SHORT_EXEMPT = {(char *)"SSE", 3};

const tsNCharcb sORDER_DURATION_DAY = {(char *)"DAY", 3};
const tsNCharcb sORDER_DURATION_FOK = {(char *)"FOK", 3};
const tsNCharcb sORDER_DURATION_GTC = {(char *)"GTC", 3};
const tsNCharcb sORDER_DURATION_IOC = {(char *)"OC",  2};

const tsNCharcb sRELEASE_CMD_HOLD_ORDER    = {(char *)"hold_order", 10};

const tsNCharcb sORDER_ENTRY_TYPE_AUTO   = {(char *)"A", 1};
const tsNCharcb sORDER_ENTRY_TYPE_MANUAL = {(char *)"M", 1};

const tsNCharcb sORDER_OPERATION_TYPE_AFOCCA = {(char *)"AFOCCA", 6};
const tsNCharcb sORDER_OPERATION_TYPE_FOCCA  = {(char *)"FOCCA",  5};
const tsNCharcb sORDER_OPERATION_TYPE_CCA    = {(char *)"CCA",    3};
const tsNCharcb sORDER_OPERATION_TYPE_FCA    = {(char *)"FCA",    3};

/*   =====================================================================   */
/*                          order completion reasons                         */
/*   =====================================================================   */
/*   PFBC = partially filled but cancelled */
const tsNCharcb sCOMPLETION_REASON_CANCEL  = {(char *)"C",    1};
const tsNCharcb sCOMPLETION_REASON_REJECT  = {(char *)"R",    1};
const tsNCharcb sCOMPLETION_REASON_FILL    = {(char *)"F",    1};
const tsNCharcb sCOMPLETION_REASON_FAILURE = {(char *)"FA",   2};
const tsNCharcb sCOMPLETION_REASON_UNKNOWN = {(char *)"U",    1};
const tsNCharcb sCOMPLETION_REASON_PFBC    = {(char *)"PFBC", 4};

/*   =====================================================================   */
/*                       values for LineInfo::sStatus                        */
/*   =====================================================================   */

const tsNCharcb
sLINE_STATUS_OPEN_PENDING        = {(char *)"open pending",              12};
const tsNCharcb
sLINE_STATUS_OPEN                = {(char *)"open",                       4};
const tsNCharcb
sLINE_STATUS_MOD_TRIGGER_PENDING = {(char *)"modified, trigger pending", 25};
const tsNCharcb
sLINE_STATUS_TRIGGER_PENDING     = {(char *)"trigger pending",           15};
const tsNCharcb
sLINE_STATUS_PARTIAL             = {(char *)"partial",                    7};
const tsNCharcb
sLINE_STATUS_CANCEL_PENDING      = {(char *)"cancel pending",            14};
const tsNCharcb
sLINE_STATUS_COMPLETE            = {(char *)"complete",                   8};
const tsNCharcb
sLINE_STATUS_MODIFY_PENDING      = {(char *)"modify pending",            14};
const tsNCharcb
sLINE_STATUS_MODIFIED            = {(char *)"modified",                   8};

const tsNCharcb 
sLINE_STATUS_ORDER_SENT_TO_EXCH  = {(char *)"order sent to exch",        18};
const tsNCharcb 
sLINE_STATUS_CANCEL_SENT_TO_EXCH = {(char *)"cancel sent to exch",       19};
const tsNCharcb 
sLINE_STATUS_MODIFY_SENT_TO_EXCH = {(char *)"modify sent to exch",       19};

const tsNCharcb 
sLINE_STATUS_ORDER_RCVD_BY_EXCH_GWAY  = {(char *)"order received by exch gateway",  30};
const tsNCharcb 
sLINE_STATUS_CANCEL_RCVD_BY_EXCH_GWAY = {(char *)"cancel received by exch gateway", 31};
const tsNCharcb 
sLINE_STATUS_MODIFY_RCVD_BY_EXCH_GWAY = {(char *)"modify received by exch gateway", 31};

/*   =====================================================================   */
/*                             order statuses by OM                          */
/*   =====================================================================   */

const tsNCharcb
sLINE_STATUS_ORDER_RECEIVED  = {(char *)"Order received from client",  26};
const tsNCharcb
sLINE_STATUS_MODIFY_RECEIVED = {(char *)"Modify received from client", 27};
const tsNCharcb
sLINE_STATUS_CANCEL_RECEIVED = {(char *)"Cancel received from client", 27};
const tsNCharcb
sLINE_STATUS_MODIFY_FAILED   = {(char *)"Modification Failed",         19};
const tsNCharcb
sLINE_STATUS_CANCEL_FAILED   = {(char *)"Cancellation Failed",         19};

/*   =====================================================================   */
/*                              bracket order types                          */
/*   =====================================================================   */

const tsNCharcb 
sBRACKET_TYPE_TARGET_AND_STOP = {(char *)"target and stop", 15};
const tsNCharcb 
sBRACKET_TYPE_TARGET_ONLY     = {(char *)"target only",     11};
const tsNCharcb 
sBRACKET_TYPE_STOP_ONLY       = {(char *)"stop only",        9};

const tsNCharcb 
sBRACKET_TYPE_TARGET_AND_STOP_STATIC = {(char *)"target and stop static", 22};
const tsNCharcb 
sBRACKET_TYPE_TARGET_ONLY_STATIC     = {(char *)"target only static",     18};
const tsNCharcb 
sBRACKET_TYPE_STOP_ONLY_STATIC       = {(char *)"stop only static",       16};

/*   =====================================================================   */
/*                user and account statuses and update types                 */
/*   =====================================================================   */

const tsNCharcb sACCOUNT_STATUS_ACTIVE       = {(char *)"active",        6};
const tsNCharcb sACCOUNT_STATUS_INACTIVE     = {(char *)"inactive",      8};
const tsNCharcb sACCOUNT_STATUS_ADMIN_ONLY   = {(char *)"admin only",   10};
const tsNCharcb sACCOUNT_STATUS_DONE_FOR_DAY = {(char *)"done for day", 12};

const tsNCharcb 
sADD_ACCOUNT              = {(char *)"add_account",   11};
const tsNCharcb 
sASSIGN_ACCOUNT_TO_USER   = {(char *)"assign_account_to_user",   22};
const tsNCharcb 
sMODIFY_ACCOUNT           = {(char *)"modify_account",           14};
const tsNCharcb 
sREMOVE_ACCOUNT_FROM_USER = {(char *)"remove_account_from_user", 24};


const tsNCharcb sUSER_STATUS_ENABLED  = {(char *)"enabled",      7};
const tsNCharcb sUSER_STATUS_DISABLED = {(char *)"disabled",     8};

const tsNCharcb sADD_USER             = {(char *)"add_user",     8};
const tsNCharcb sMODIFY_USER          = {(char *)"modify_user", 11};

/*   =====================================================================   */
/*                          instrument search related                        */
/*   =====================================================================   */

enum SearchField
     {
     Any,
     Exchange,
     ProductCode,
     InstrumentType,
     Ticker,
     Description,
     ExpirationDate
     };

enum SearchOperator
     {
     Equals,
     Contains,
     StartsWith,
     EndsWith
     };

class SearchTerm
     {
     public :
     SearchField    eField;
     SearchOperator eOperator;
     tsNCharcb      sTerm;
     bool           bCaseSensitive;
     };

/*   =====================================================================   */
/*                           callback info classes                           */
/*   =====================================================================   */

class AccountInfo;
class AccountListInfo;
class AccountUpdateInfo;
class AgreementInfo;
class AgreementListInfo;
class AlertInfo;
class AskInfo;
class AssignedUserListInfo;
class AutoLiquidateInfo;
class AuxRefDataInfo;
class BarInfo;
class BarReplayInfo;
class BidInfo;
class BinaryContractListInfo;
class BracketInfo;
class BracketReplayInfo;
class BracketTier;
class BracketTierModifyInfo;
class CloseMidPriceInfo;
class ClosePriceInfo;
class ClosingIndicatorInfo;
class DboBookRebuildInfo;
class DboInfo;
class EasyToBorrowInfo;
class EasyToBorrowListInfo;
class EndQuoteInfo;
class EnvironmentInfo;
class EnvironmentListInfo;
class EnvironmentVariable;
class EquityOptionStrategyListInfo;
class ExchangeListInfo;
class ExecutionReplayInfo;
class HighBidPriceInfo;
class HighPriceInfo;
class HighPriceLimitInfo;
class IbListInfo;
class InstrumentByUnderlyingInfo;
class InstrumentSearchInfo;
class LimitOrderBookInfo;
class LineInfo;
class LowPriceInfo;
class LowAskPriceInfo;
class LowPriceLimitInfo;
class MarketModeInfo;
class MidPriceInfo;
class OpenInterestInfo;
class OpenPriceInfo;
class OpeningIndicatorInfo;
class OptionListInfo;
class OrderHistoryDatesInfo;
class OrderReplayInfo;
class PasswordChangeInfo;
class PingInfo;
class PnlInfo;
class PnlReplayInfo;
class PositionExitInfo;
class PriceIncrInfo;
class PriceIncrRow;
class ProjectedSettlementPriceInfo;
class ProductRmsInfo;
class ProductRmsListInfo;
class QuoteInfo;
class QuoteReplayInfo;
class QuoteReport;
class RefDataInfo;
class RmsInfo;
class SettlementPriceInfo;
class SingleOrderReplayInfo;
class StrategyInfo;
class StrategyLegInfo;
class StrategyListInfo;
class TradeInfo;
class TradeReplayInfo;
class TradeRouteInfo;
class TradeRouteListInfo;
class TradeVolumeInfo;
class UserInfo;
class UserDefinedSpreadCreateInfo;
class UserListInfo;
class UserProfileInfo;
class VolumeAtPriceInfo;

class SodReport;

/* order reports */

class OrderReport;
class OrderBustReport;
class OrderCancelReport;
class OrderFailureReport;
class OrderFillReport;
class OrderModifyReport;
class OrderNotCancelledReport;
class OrderNotModifiedReport;
class OrderRejectReport;
class OrderStatusReport;
class OrderTradeCorrectReport;
class OrderTriggerPulledReport;
class OrderTriggerReport;

/*   =====================================================================   */
/*                           parameter info classes                          */
/*   =====================================================================   */

class BarParams;
class ReplayBarParams;

class OrderParams;
class LimitOrderParams;
class MarketOrderParams;
class StopLimitOrderParams;
class StopMarketOrderParams;

class ModifyLimitOrderParams;
class ModifyStopLimitOrderParams;
class ModifyStopMarketOrderParams;
class ModifyOrderParams;

class BracketParams;

class QuoteParams;
class QuoteCancelParams;

/*   =====================================================================   */
/*                                working classes                            */
/*   =====================================================================   */

class AdmCallbacks;
class REngineParams;
class RCallbacks;
class REngine;
class LoginParams;

/*   =====================================================================   */
/*                           callback info classes                           */
/*   =====================================================================   */

class AccountInfo
     {
     public :
     AccountInfo();
     virtual ~AccountInfo();

     tsNCharcb sFcmId;
     tsNCharcb sIbId;
     tsNCharcb sAccountId;

     tsNCharcb sAccountName;
     int       iCreationSsboe;
     int       iCreationUsecs;
     
     RmsInfo *           pRmsInfo;
     AutoLiquidateInfo * pAutoLiquidateInfo;

     int clearHandles(int * aiCode);
     int dump(int * aiCode);

     private :
     };

/*   =====================================================================   */

class AccountListInfo
     {
     public :
     AccountListInfo();
     virtual ~AccountListInfo();

     AccountInfo * asAccountInfoArray;
     int           iArrayLen;
     int           iRpCode;
     tsNCharcb     sRpCode;

     int dump(int * aiCode);

     private :
     };

/*   =====================================================================   */

class AccountUpdateInfo
     {
     public :
     AccountUpdateInfo();
     virtual ~AccountUpdateInfo();

     AccountInfo oAccount;
     tsNCharcb   sAction;
     tsNCharcb   sUser;

     int dump(int * aiCode);

     private :
     };

/*   =====================================================================   */

class AggregatorInfo
     {
     public :
     AggregatorInfo();
     virtual ~AggregatorInfo();

     int iRpCode;

     int dump(int * aiCode);

     private :
     };

/*   =====================================================================   */

class AgreementInfo
     {
     public :
     AgreementInfo();
     virtual ~AgreementInfo();

     tsNCharcb sTitle;                        // Document
     tsNCharcb sStatus;                       // (active/inactive)
     bool      bMandatory;                    // Type
     int       iPublicationSsboe;             // Published
     int       iInForceSsboe;                 // Submit By

     tsNCharcb sEndUserAcceptanceStatus;      // Status
     int       iEndUserAcceptanceSsboe;       // Date
     tsNCharcb sMarketDataUsageCapacity;

     int dump(int * aiCode);

     private :
     };

/*   =====================================================================   */

class AgreementListInfo
     {
     public :
     AgreementListInfo();
     virtual ~AgreementListInfo();

     bool            bAccepted;
     AgreementInfo * asAgreementInfoArray;
     int             iArrayLen;

     int             iRpCode;
     tsNCharcb       sRpCode;

     void *          pContext;

     int dump(int * aiCode);

     private :
     };

/*   =====================================================================   */

class AlertInfo
     {
     public :
     AlertInfo();
     virtual ~AlertInfo();

     tsNCharcb sTicker;
     tsNCharcb sExchange;
     tsNCharcb sMessage;
     int       iAlertType;
     int       iConnectionId;
     int       iRpCode;
     tsNCharcb sRpCode;

     int dump(int * aiCode);

     private :
     };

/*   =====================================================================   */

class AskInfo
     {
     public :
     AskInfo();
     virtual ~AskInfo();

     tsNCharcb sTicker;
     tsNCharcb sExchange;
     double    dPrice;
     bool      bPriceFlag;
     long long llSize;
     bool      bSizeFlag;
     long long llImpliedSize;
     int       iNumOrders;

     bool      bLeanPriceFlag;
     double    dLeanPrice;

     int       iUpdateType;

     int       iType;

     int       iConnId;

     int       iSsboe;
     int       iUsecs;

     int dump(int * aiCode);

     private :
     };

/*   =====================================================================   */

class AssignedUserListInfo
     {
     public :
     AssignedUserListInfo();
     virtual ~AssignedUserListInfo();

     int dump(int * aiCode);

     AccountInfo   oAccount;

     tsNCharcb *   pUserArray;
     int           iUserArrayLen;

     int           iRpCode;

     void *        pContext;

     private :
     };

/*   =====================================================================   */

class AutoLiquidateInfo
     {
     public :
     AutoLiquidateInfo();
     virtual ~AutoLiquidateInfo();

     virtual int clearHandles(int * aiCode);
     int dump(int * aiCode);

     /*   ----------------------------------------------------------------   */

     AccountInfo oAccount;

     int         eAutoLiquidateThresholdCurrentValue;
     double      dAutoLiquidateThresholdCurrentValue;

     int         eAutoLiquidateThresholdCurrentValueSsboe;
     int         iAutoLiquidateThresholdCurrentValueSsboe;

     int         iCallbackType;
     };

/*   =====================================================================   */

class AuxRefDataInfo
     {
     public :
     AuxRefDataInfo();
     virtual ~AuxRefDataInfo();

     tsNCharcb sTicker;
     tsNCharcb sExchange;

     tsNCharcb sFirstNoticeDate;
     tsNCharcb sLastNoticeDate;

     tsNCharcb sFirstTradingDate;
     tsNCharcb sLastTradingDate;

     tsNCharcb sFirstDeliveryDate;
     tsNCharcb sLastDeliveryDate;

     tsNCharcb sFirstPositionDate;
     tsNCharcb sLastPositionDate;

     tsNCharcb sSettlementMethod;

     tsNCharcb sUnitOfMeasure;
     bool      bUnitOfMeasureQty;
     double    dUnitOfMeasureQty;

     int       iSsboe;

     int       iRpCode;

     int       iType;

     int dump(int * aiCode);

     private :
     };

/*   =====================================================================   */

class BidInfo
     {
     public :
     BidInfo();
     virtual ~BidInfo();

     tsNCharcb sTicker;
     tsNCharcb sExchange;
     double    dPrice;
     bool      bPriceFlag;
     long long llSize;
     bool      bSizeFlag;
     long long llImpliedSize;
     int       iNumOrders;

     bool      bLeanPriceFlag;
     double    dLeanPrice;

     int       iUpdateType;

     int       iType;

     int       iConnId;

     int       iSsboe;
     int       iUsecs;

     int dump(int * aiCode);

     private :
     };

/*   =====================================================================   */

class BinaryContractListInfo
     {
     public :
     BinaryContractListInfo();
     virtual ~BinaryContractListInfo();

     virtual int clearHandles(int * aiCode);
     int dump(int * aiCode);

     tsNCharcb *   pExchangeArray;
     int           iExchangeArrayLen;
     tsNCharcb *   pProductCodeArray;
     int           iProductCodeArrayLen;
     tsNCharcb *   pBinaryContractTypeArray;
     int           iBinaryContractTypeArrayLen;
     tsNCharcb *   pPeriodCodeArray;
     int           iPeriodCodeArrayLen;
     tsNCharcb *   pExpirationTimeArray;
     int           iExpirationTimeArrayLen;
     RefDataInfo * asRefDataInfoArray;
     int           iArrayLen;

     int           iRpCode;

     void *        pContext;

     private :
     };

/*   =====================================================================   */

class BracketTierModifyInfo
     {
     public :
     BracketTierModifyInfo();
     virtual ~BracketTierModifyInfo();

     int dump(int * aiCode);

     AccountInfo oAccount;
     tsNCharcb   sOrderNum;
     bool        bTarget;
     int         iOldTickOffset;
     int         iNewTickOffset;
     void *      pContext;
     int         iRpCode;
     };

class BracketTier
     {
     public :
     BracketTier(const int       iTicksIn,
		 const long long llQtyIn,
		 const long long llQtyReleasedIn);
     BracketTier();
     virtual ~BracketTier();

     int       iTicks;
     long long llQty;
     long long llQtyReleased;
     };

class BracketInfo
     {
     public :
     BracketInfo();
     virtual ~BracketInfo();

     virtual int clearHandles(int * aiCode);
     int dump(int * aiCode);

     /*   ----------------------------------------------------------------   */

     AccountInfo   oAccount;
     tsNCharcb     sOrderNum;

     /*   ----------------------------------------------------------------   */

     tsNCharcb     sTargetsToMarketWhenTouched;

     bool          bTargetsToMarketAt;
     int           iTargetsToMarketAtSsboe;
     int           iTargetsToMarketAtUsecs;

     bool          bTargetsToMarketAfterPartialFillInSeconds;
     int           iTargetsToMarketAfterPartialFillInSeconds;

     int           iTargetArrayLen;
     BracketTier * asTargetArray;

     /*   ----------------------------------------------------------------   */

     tsNCharcb     sStopsToMarketOnReject;

     bool          bStopsToMarketAt;
     int           iStopsToMarketAtSsboe;
     int           iStopsToMarketAtUsecs;

     bool          bTrailingStop;
     int           iTrailByPriceId;
     bool          bTrailingStopTriggerTicks;
     int           iTrailingStopTriggerTicks;

     int           iStopArrayLen;
     BracketTier * asStopArray;
     };

class BracketReplayInfo
     {
     public :
     BracketReplayInfo();
     virtual ~BracketReplayInfo();

     int dump(int * aiCode);

     AccountInfo   oAccount;

     int           iArrayLen;
     BracketInfo * asBracketArray;

     int           iRpCode;
     };

/*   =====================================================================   */

class CloseMidPriceInfo
     {
     public :
     CloseMidPriceInfo();
     virtual ~CloseMidPriceInfo();

     tsNCharcb sTicker;
     tsNCharcb sExchange;

     bool      bClosePrice;
     double    dClosePrice;
     tsNCharcb sClosePriceDate;

     int       iType;

     int       iSsboe;
     int       iUsecs;

     int dump(int * aiCode);

     private :
     };

/*   =====================================================================   */

class ClosePriceInfo
     {
     public :
     ClosePriceInfo();
     virtual ~ClosePriceInfo();

     tsNCharcb sTicker;
     tsNCharcb sExchange;
     double    dPrice;
     bool      bPriceFlag;

     int       iType;

     int       iSsboe;
     int       iUsecs;

     int dump(int * aiCode);

     private :
     };

/*   =====================================================================   */

class ClosingIndicatorInfo
     {
     public :
     ClosingIndicatorInfo();
     virtual ~ClosingIndicatorInfo();

     tsNCharcb sTicker;
     tsNCharcb sExchange;
     double    dPrice;
     bool      bPriceFlag;

     int       iType;

     int       iSsboe;
     int       iUsecs;

     int dump(int * aiCode);

     private :
     };

/*   =====================================================================   */

class DboInfo
     {
     public :
     DboInfo();
     virtual ~DboInfo();

     tsNCharcb sTicker;
     tsNCharcb sExchange;

     tsNCharcb sSide;
     double    dPrice;
     long long llSize;

     tsNCharcb sDboUpdateType;

     tsNCharcb sPriority;
     tsNCharcb sExchOrdId;

     bool      bPreviousPrice;
     double    dPreviousPrice;

     int       iSourceSsboe;
     int       iSourceUsecs;
     int       iSourceNsecs;

     int       iJopSsboe;
     int       iJopNsecs;

     int       iSsboe;
     int       iUsecs;

     int       iType;

     void *    pContext;

     bool      bPrice;
     bool      bSize;

     int dump(int * aiCode);

     private :
     };

/*   =====================================================================   */

class DboBookRebuildInfo
     {
     public :
     DboBookRebuildInfo();
     virtual ~DboBookRebuildInfo();

     int dump(int * aiCode);

     tsNCharcb sExchange;
     tsNCharcb sTicker;

     bool      bPrice;
     double    dPrice;

     void *    pContext;

     int       iRpCode;
     tsNCharcb sRpCode;
     };

/*   =====================================================================   */

class EasyToBorrowInfo
     {
     public :
     EasyToBorrowInfo();
     virtual ~EasyToBorrowInfo();

     int dump(int * aiCode);

     tsNCharcb sBrokerDealer;
     tsNCharcb sTicker;
     tsNCharcb sDescription;

     long long llQtyAvailable;
     long long llQtyNeeded;

     bool      bBorrowable;

     int       iType;

     void *    pContext;
     };

/*   =====================================================================   */

class EasyToBorrowListInfo
     {
     public :
     EasyToBorrowListInfo();
     virtual ~EasyToBorrowListInfo();

     int dump(int * aiCode);

     int       iRpCode;
     void *    pContext;
     };

/*   =====================================================================   */

class EndQuoteInfo
     {
     public :
     EndQuoteInfo();
     virtual ~EndQuoteInfo();

     tsNCharcb sTicker;
     tsNCharcb sExchange;

     int       iUpdateType;

     int       iType;

     int       iConnId;

     int       iSsboe;
     int       iUsecs;

     int dump(int * aiCode);

     private :
     };

/*   =====================================================================   */

class EnvironmentVariable
     {
     public :
     EnvironmentVariable();
     virtual ~EnvironmentVariable();

     tsNCharcb sName;
     tsNCharcb sValue;

     private :
     };

/*   =====================================================================   */

class EnvironmentInfo
     {
     public :
     EnvironmentInfo();
     virtual ~EnvironmentInfo();

     tsNCharcb   sKey;

     EnvironmentVariable * asVariableArray;
     int                   iArrayLen;

     void *    pContext;

     int dump(int * aiCode);

     private :
     };

/*   =====================================================================   */

class EnvironmentListInfo
     {
     public :
     EnvironmentListInfo();
     virtual ~EnvironmentListInfo();

     tsNCharcb * asKeyArray;
     int         iArrayLen;

     void *      pContext;

     int dump(int * aiCode);

     private :
     };

/*   =====================================================================   */

class EquityOptionStrategyListInfo
     {
     public :
     EquityOptionStrategyListInfo();
     virtual ~EquityOptionStrategyListInfo();

     virtual int clearHandles(int * aiCode);
     int dump(int * aiCode);

     tsNCharcb *   pExchangeArray;
     int           iExchangeArrayLen;
     tsNCharcb *   pUnderlyingArray;
     int           iUnderlyingArrayLen;
     tsNCharcb *   pStrategyTypeArray;
     int           iStrategyTypeArrayLen;
     tsNCharcb *   pExpirationArray;
     int           iExpirationArrayLen;
     tsNCharcb *   pTickerArray;
     int           iTickerArrayLen;

     int           iRpCode;

     void *        pContext;

     private :
     };

/*   =====================================================================   */

class ExchangeListInfo
     {
     public :
     ExchangeListInfo();
     virtual ~ExchangeListInfo();

     tsNCharcb * asExchangeArray;
     int         iArrayLen;
     int         iRpCode;
     tsNCharcb   sRpCode;

     void *      pContext;

     int dump(int * aiCode);

     private :
     };

/*   =====================================================================   */

class ExecutionReplayInfo
     {
     public :
     ExecutionReplayInfo();
     virtual ~ExecutionReplayInfo();

     AccountInfo       oAccount;

     OrderFillReport * asFillReportArray;
     int               iArrayLen;
     int               iRpCode;
     tsNCharcb         sRpCode;

     int dump(int * aiCode);

     private :
     };

/*   =====================================================================   */

class HighBidPriceInfo
     {
     public :
     HighBidPriceInfo();
     virtual ~HighBidPriceInfo();

     tsNCharcb sTicker;
     tsNCharcb sExchange;
     double    dPrice;
     bool      bPriceFlag;

     int       iType;

     int       iSsboe;
     int       iUsecs;

     int dump(int * aiCode);

     private :
     };

/*   =====================================================================   */

class HighPriceInfo
     {
     public :
     HighPriceInfo();
     virtual ~HighPriceInfo();

     tsNCharcb sTicker;
     tsNCharcb sExchange;
     double    dPrice;
     bool      bPriceFlag;

     int       iType;

     int       iSsboe;
     int       iUsecs;

     int dump(int * aiCode);

     private :
     };

/*   =====================================================================   */

class HighPriceLimitInfo
     {
     public :
     HighPriceLimitInfo();
     virtual ~HighPriceLimitInfo();

     tsNCharcb sTicker;
     tsNCharcb sExchange;
     double    dPrice;
     bool      bPriceFlag;

     int       iType;

     int       iSsboe;
     int       iUsecs;

     int dump(int * aiCode);

     private :
     };

/*   =====================================================================   */

class IbListInfo
     {
     public :
     IbListInfo();
     virtual ~IbListInfo();

     int dump(int * aiCode);

     tsNCharcb *   pIbIdArray;
     int           iIbIdArrayLen;

     int           iRpCode;

     void *        pContext;

     private :
     };

/*   =====================================================================   */

class InstrumentByUnderlyingInfo
     {
     public :
     InstrumentByUnderlyingInfo();
     virtual ~InstrumentByUnderlyingInfo();

     virtual int clearHandles(int * aiCode);
     int dump(int * aiCode);

     tsNCharcb *   pUnderlyingArray;
     int           iUnderlyingArrayLen;
     tsNCharcb *   pExchangeArray;
     int           iExchangeArrayLen;
     tsNCharcb *   pExpirationArray;
     int           iExpirationArrayLen;
     RefDataInfo * asRefDataInfoArray;
     int           iArrayLen;

     int           iRpCode;
     tsNCharcb     sRpCode;

     void *        pContext;

     private :
     };

/*   =====================================================================   */

class InstrumentSearchInfo
     {
     public :
     InstrumentSearchInfo();
     virtual ~InstrumentSearchInfo();

     int dump(int * aiCode);

     /*   ----------------------------------------------------------------   */

     tsNCharcb     sExchange;
     SearchTerm *  aoTerms;
     int           iNumTerms;

     RefDataInfo * asArray;
     int           iArrayLen;

     int           iRpCode;

     void *        pContext;
     };

/*   =====================================================================   */

class LimitOrderBookInfo
     {
     public :
     LimitOrderBookInfo();
     virtual ~LimitOrderBookInfo();

     tsNCharcb   sTicker;
     tsNCharcb   sExchange;

     double *    adAskPriceArray;
     long long * allAskSizeArray;
     long long * allAskImpliedSizeArray;
     int *       aiAskNumOrdersArray;
     int *       aiAskSsboeArray;
     int *       aiAskUsecsArray;
     int         iAskArrayLen;

     double *    adBidPriceArray;
     long long * allBidSizeArray;
     long long * allBidImpliedSizeArray;
     int *       aiBidNumOrdersArray;
     int *       aiBidSsboeArray;
     int *       aiBidUsecsArray;
     int         iBidArrayLen;

     int         iType;

     int         iConnId;

     int         iRpCode;
     tsNCharcb   sRpCode;

     int         iSsboe;
     int         iUsecs;

     int clearHandles(int * aiCode);
     int dump(int * aiCode);

     private :
     };

/*   =====================================================================   */

class LineInfo
     {
     public :
     LineInfo();
     virtual ~LineInfo();

     AccountInfo oAccount;

     tsNCharcb   sTicker;
     tsNCharcb   sExchange;

     tsNCharcb   sOrderNum;
     tsNCharcb   sOriginalOrderNum;
     tsNCharcb   sInitialSequenceNumber;
     tsNCharcb   sCurrentSequenceNumber;
     tsNCharcb   sExchOrdId;
     tsNCharcb   sTickerPlantExchOrdId;
     tsNCharcb   sOmnibusAccount;

     tsNCharcb   sOrderType;
     tsNCharcb   sOriginalOrderType;
     tsNCharcb   sBuySellType;
     tsNCharcb   sOrderDuration;
     tsNCharcb   sEntryType;
     tsNCharcb   sTradingAlgorithm;

     tsNCharcb   sTradeRoute;
     tsNCharcb   sRoutingInstructions;

     tsNCharcb   sOrderOperationType;
     tsNCharcb   sBracketType;
     tsNCharcb   sLinkedOrderNums;

     tsNCharcb   sOriginatorWindowName;
     tsNCharcb   sWindowName;

     tsNCharcb   sReleaseCommand;
     bool        bReleaseSsboe;
     int         iReleaseSsboe;
     bool        bReleaseUsecs;
     int         iReleaseUsecs;

     bool        bCancelSsboe;
     int         iCancelSsboe;
     bool        bCancelUsecs;
     int         iCancelUsecs;

     bool        bCancelAfter;
     int         iCancelAfter;

     bool        bToMarketAt;
     int         iToMarketAtSsboe;
     int         iToMarketAtUsecs;
     bool        bToMarketAfterPartialFillInSeconds;
     int         iToMarketAfterPartialFillInSeconds;
     tsNCharcb   sToMarketOnReject;
     tsNCharcb   sToMarketWhenLimitPriceTouched;

     tsNCharcb   sStatus;
     tsNCharcb   sCompletionReason;
     tsNCharcb   sRemarks;
     tsNCharcb   sText;

     tsNCharcb   sTag;
     tsNCharcb   sUserTag;
     tsNCharcb   sUserMsg;
     void *      pContext;

     tsNCharcb   sUser;

     int         iGatewayBufferBytesBehind;                /* experimental   */
     int         iGatewaySsboe;                            /* experimental   */
     int         iGatewayUsecs;                            /* experimental   */

     int         iSsboe;
     int         iUsecs;

     long long   llQuantityToFill;
     long long   llMinQty;
     long long   llMaxShowQty;
     long long   llFilled;
     long long   llUnfilled;
     long long   llCancelled;

     long long   llReleasePendingSize;
     long long   llCancelPendingSize;
     long long   llModifyPendingSize;

     double      dPriceToFill;
     bool        bPriceToFillFlag;
     double      dAvgFillPrice;
     bool        bAvgFillPriceFlag;
     double      dTriggerPrice;
     bool        bTriggerPriceFlag;

     tsNCharcb   sTriggerTicker;
     tsNCharcb   sTriggerExchange;
     int         iTriggerPriceId;
     int         iTriggerCmpOperator;

     int         iTrailByTicks;
     int         iTrailByPriceId;

     int         iType;

     int         iRpCode;
     tsNCharcb   sRpCode;

     int clearHandles(int * aiCode);
     int dump(int * aiCode);

     private :
     };

/*   =====================================================================   */

class LowAskPriceInfo
     {
     public :
     LowAskPriceInfo();
     virtual ~LowAskPriceInfo();

     tsNCharcb sTicker;
     tsNCharcb sExchange;
     double    dPrice;
     bool      bPriceFlag;

     int       iType;

     int       iSsboe;
     int       iUsecs;

     int dump(int * aiCode);

     private :
     };

/*   =====================================================================   */

class LowPriceInfo
     {
     public :
     LowPriceInfo();
     virtual ~LowPriceInfo();

     tsNCharcb sTicker;
     tsNCharcb sExchange;
     double    dPrice;
     bool      bPriceFlag;

     int       iType;

     int       iSsboe;
     int       iUsecs;

     int dump(int * aiCode);

     private :
     };

/*   =====================================================================   */

class LowPriceLimitInfo
     {
     public :
     LowPriceLimitInfo();
     virtual ~LowPriceLimitInfo();

     tsNCharcb sTicker;
     tsNCharcb sExchange;
     double    dPrice;
     bool      bPriceFlag;

     int       iType;

     int       iSsboe;
     int       iUsecs;

     int dump(int * aiCode);

     private :
     };

/*   =====================================================================   */

class MarketModeInfo
     {
     public :
     MarketModeInfo();
     virtual ~MarketModeInfo();

     tsNCharcb sTicker;
     tsNCharcb sExchange;
     tsNCharcb sMarketMode;
     tsNCharcb sReason;
     tsNCharcb sEvent;

     int       iType;

     int       iSourceSsboe;
     int       iSourceUsecs;
     int       iSourceNsecs;

     int       iJopSsboe;
     int       iJopNsecs;

     int       iSsboe;
     int       iUsecs;

     int dump(int * aiCode);

     private :
     };

/*   =====================================================================   */

class MidPriceInfo
     {
     public :
     MidPriceInfo();
     virtual ~MidPriceInfo();

     tsNCharcb sTicker;
     tsNCharcb sExchange;

     bool      bLastPrice;
     double    dLastPrice;

     bool      bOpenPrice;
     double    dOpenPrice;

     bool      bHighPrice;
     double    dHighPrice;

     bool      bLowPrice;
     double    dLowPrice;

     bool      bNetChange;
     double    dNetChange;
     bool      bPercentChange;
     double    dPercentChange;

     int       iType;

     int       iSsboe;
     int       iUsecs;

     int dump(int * aiCode);

     private :
     };

/*   =====================================================================   */

class OpenInterestInfo
     {
     public :
     OpenInterestInfo();
     virtual ~OpenInterestInfo();

     tsNCharcb sTicker;
     tsNCharcb sExchange;
     long long llQuantity;
     bool      bQuantityFlag;

     int       iType;

     int       iSsboe;
     int       iUsecs;

     int dump(int * aiCode);

     private :
     };

/*   =====================================================================   */

class OpenPriceInfo
     {
     public :
     OpenPriceInfo();
     virtual ~OpenPriceInfo();

     tsNCharcb sTicker;
     tsNCharcb sExchange;
     double    dPrice;
     bool      bPriceFlag;

     int       iType;

     int       iSsboe;
     int       iUsecs;

     int dump(int * aiCode);

     private :
     };

/*   =====================================================================   */

class OpeningIndicatorInfo
     {
     public :
     OpeningIndicatorInfo();
     virtual ~OpeningIndicatorInfo();

     tsNCharcb sTicker;
     tsNCharcb sExchange;
     double    dPrice;
     bool      bPriceFlag;
     long long llVolume;
     bool      bVolumeFlag;

     int       iType;

     int       iSsboe;
     int       iUsecs;

     int dump(int * aiCode);

     private :
     };

/*   =====================================================================   */

class OptionListInfo
     {
     public :
     OptionListInfo();
     virtual ~OptionListInfo();

     virtual int clearHandles(int * aiCode);
     int dump(int * aiCode);

     tsNCharcb *   pExchangeArray;
     int           iExchangeArrayLen;
     tsNCharcb *   pProductCodeArray;
     int           iProductCodeArrayLen;
     tsNCharcb *   pExpirationCCYYMMArray;
     int           iExpirationCCYYMMArrayLen;
     RefDataInfo * asRefDataInfoArray;
     int           iArrayLen;

     int           iRpCode;
     tsNCharcb     sRpCode;

     void *        pContext;

     private :
     };

/*   =====================================================================   */

class OrderHistoryDatesInfo
     {
     public :
     OrderHistoryDatesInfo();
     virtual ~OrderHistoryDatesInfo();

     tsNCharcb * asDateArray;
     int         iArrayLen;
     int         iRpCode;
     tsNCharcb   sRpCode;

     void *      pContext;

     int dump(int * aiCode);

     private :
     };

/*   =====================================================================   */

class OrderReplayInfo
     {
     public :
     OrderReplayInfo();
     virtual ~OrderReplayInfo();

     AccountInfo oAccount;
     tsNCharcb   sDate;

     LineInfo *  asLineInfoArray;
     int         iArrayLen;
     int         iRpCode;
     tsNCharcb   sRpCode;

     int dump(int * aiCode);

     private :
     };

/*   =====================================================================   */

class PasswordChangeInfo
     {
     public :
     PasswordChangeInfo();
     virtual ~PasswordChangeInfo();

     int         iRpCode;
     tsNCharcb   sRpCode;

     int dump(int * aiCode);

     private :
     };

/*   =====================================================================   */

class PingInfo
     {
     public :
     PingInfo();
     virtual ~PingInfo();

     int    iConnId;
     double dElapsedSeconds;

     void * pContext;

     int dump(int * aiCode);

     private :
     };

/*   =====================================================================   */

class PnlInfo
     {
     public :
     PnlInfo();
     virtual ~PnlInfo();

     AccountInfo oAccount;

     tsNCharcb   sTicker;
     tsNCharcb   sExchange;

     int         eOpenPnl;
     double      dOpenPnl;

     int         eClosedPnl;
     double      dClosedPnl;

     int         eCashOnHand;
     double      dCashOnHand;

     int         eAccountBalance;
     double      dAccountBalance;

     int         eMarginBalance;
     double      dMarginBalance;

     int         eReservedMargin;
     double      dReservedMargin;

     int         eAvailableBuyingPower;
     double      dAvailableBuyingPower;

     int         eReservedBuyingPower;
     double      dReservedBuyingPower;

     int         eUsedBuyingPower;
     double      dUsedBuyingPower;

     int         ePosition;
     long long   llPosition;

     int         eBuyQty;
     long long   llBuyQty;

     int         eSellQty;
     long long   llSellQty;

     int         eAvgOpenFillPrice;
     double      dAvgOpenFillPrice;

     int         eImpliedNetQty;
     long long   llImpliedNetQty;

     int         eImpliedMarginReserved;
     double      dImpliedMarginReserved;

     int         iSsboe;
     int         iUsecs;

     bool        bPossOutOfOrder;

     int         eLongExposure;
     long long   llLongExposure;

     int         eShortExposure;
     long long   llShortExposure;

     int         eBuyWorkingQty;
     long long   llBuyWorkingQty;

     int         eSellWorkingQty;
     long long   llSellWorkingQty;

     int clearHandles(int * aiCode);
     int dump(int * aiCode);

     private :
     };

/*   =====================================================================   */

class PnlReplayInfo
     {
     public :
     PnlReplayInfo();
     virtual ~PnlReplayInfo();

     AccountInfo oAccount;

     PnlInfo *   asPnlInfoArray;
     int         iArrayLen;
     int         iRpCode;
     tsNCharcb   sRpCode;

     int dump(int * aiCode);

     private :
     };

/*   =====================================================================   */

class PositionExitInfo
     {
     public :
     PositionExitInfo();
     virtual ~PositionExitInfo();

     AccountInfo oAccount;

     tsNCharcb   sTicker;
     tsNCharcb   sExchange;

     tsNCharcb   sEntryType;
     tsNCharcb   sTag;
     tsNCharcb   sTradingAlgorithm;
     tsNCharcb   sUserMsg;
     tsNCharcb   sWindowName;

     void *      pContext;     

     int         iRpCode;

     int dump(int * aiCode);

     private :
     };

/*   =====================================================================   */

class PriceIncrInfo
     {
     public :
     PriceIncrInfo();
     virtual ~PriceIncrInfo();

     /*   ----------------------------------------------------------------   */

     tsNCharcb sExchange;
     tsNCharcb sTicker;

     /*   ----------------------------------------------------------------   */

     int            iArrayLen;
     PriceIncrRow * asPriceIncrArray;

     /*   ----------------------------------------------------------------   */

     int         iRpCode;
     tsNCharcb   sRpCode;

     int dump(int * aiCode);

     private :
     };

/*   =====================================================================   */

class PriceIncrRow
     {
     public :
     PriceIncrRow();
     virtual ~PriceIncrRow();

     /*   ----------------------------------------------------------------   */

     double    dPriceIncr;
     int       iPrecision;          /* double-to-string conversion precision */

     int       iFirstOperator;
     double    dFirstPrice;

     int       iLastOperator;
     double    dLastPrice;

     /*   ----------------------------------------------------------------   */

     int dump(int * aiCode);

     private :
     };

/*   =====================================================================   */

class ProductRmsInfo
     {
     public :
     ProductRmsInfo();
     virtual ~ProductRmsInfo();

     /*   ----------------------------------------------------------------   */

     AccountInfo oAccount;
     tsNCharcb   sProductCode;

     bool        bBuyLimit;
     long long   llBuyLimit;

     bool        bSellLimit;
     long long   llSellLimit;

     bool        bLossLimit;
     double      dLossLimit;

     bool        bMaxOrderQty;
     long long   llMaxOrderQty;

     bool        bCommissionFillRate;
     double      dCommissionFillRate;

     bool        bBuyLongMarginRate;
     double      dBuyLongMarginRate;

     bool        bSellShortMarginRate;
     double      dSellShortMarginRate;

     /*   ----------------------------------------------------------------   */

     int clearHandles(int * aiCode);
     int dump(int * aiCode);

     private :
     };

/*   =====================================================================   */

class ProductRmsListInfo
     {
     public :
     ProductRmsListInfo();
     virtual ~ProductRmsListInfo();

     ProductRmsInfo * asProductRmsInfoArray;
     int              iArrayLen;
     int              iRpCode;
     tsNCharcb        sRpCode;

     int dump(int * aiCode);

     private :
     };

/*   =====================================================================   */

class ProjectedSettlementPriceInfo
     {
     public :
     ProjectedSettlementPriceInfo();
     virtual ~ProjectedSettlementPriceInfo();

     tsNCharcb sTicker;
     tsNCharcb sExchange;
     double    dPrice;
     bool      bPriceFlag;

     int       iType;

     int       iSsboe;
     int       iUsecs;

     int dump(int * aiCode);

     private :
     };

/*   =====================================================================   */

class QuoteInfo
     {
     public :
     QuoteInfo();
     virtual ~QuoteInfo();

     AccountInfo oAccount;

     tsNCharcb   sTicker;
     tsNCharcb   sExchange;

     tsNCharcb   sQuoteId;
     tsNCharcb   sQuoteMsgId;

     tsNCharcb   sStatus;
     tsNCharcb   sText;

     /*   ----------------------------------------------------------------   */

     bool        bBid;
     double      dBidPriceToFill;
     long long   llBidQuantityToFill;

     bool        bAsk;
     double      dAskPriceToFill;
     long long   llAskQuantityToFill;

     /*   ----------------------------------------------------------------   */

     tsNCharcb   sUserMsg;

     /*   ----------------------------------------------------------------   */

     tsNCharcb   sUser;

     /*   ----------------------------------------------------------------   */

     int         iSsboe;
     int         iUsecs;

     int         iType;

     /*   ----------------------------------------------------------------   */

     virtual int clearHandles(int * aiCode);
     virtual int dump(int * aiCode);

     private :
     };

/*   =====================================================================   */

class QuoteReplayInfo
     {
     public :
     QuoteReplayInfo();
     virtual ~QuoteReplayInfo();

     AccountInfo oAccount;

     void *      pContext;

     int         iRpCode;
     tsNCharcb   sRpCode;

     int dump(int * aiCode);

     private :
     };

/*   =====================================================================   */

class QuoteReport
     {
     public :
     QuoteReport();
     virtual ~QuoteReport();

     AccountInfo oAccount;

     tsNCharcb   sTicker;
     tsNCharcb   sExchange;

     tsNCharcb   sQuoteId;
     tsNCharcb   sQuoteMsgId;

     tsNCharcb   sReportType;
     tsNCharcb   sReportId;

     tsNCharcb   sRemarks;
     tsNCharcb   sUser;
     tsNCharcb   sUserMsg;

     /*   ----------------------------------------------------------------   */

     bool        bBid;
     double      dBidPriceToFill;
     long long   llBidQuantityToFill;

     bool        bAsk;
     double      dAskPriceToFill;
     long long   llAskQuantityToFill;

     /*   ----------------------------------------------------------------   */

     bool        bFill;
     tsNCharcb   sFillSide;
     double      dFillPrice;
     long long   llFillSize;

     /*   ----------------------------------------------------------------   */

     int         iConnId;

     int         iSourceSsboe;
     int         iSourceNsecs;

     int         iSsboe;
     int         iUsecs;

     int         iType;

     virtual int clearHandles(int * aiCode);
     virtual int dump(int * aiCode);

     private :
     };

/*   =====================================================================   */

class RefDataInfo
     {
     public :
     RefDataInfo();
     virtual ~RefDataInfo();

     tsNCharcb sTicker;
     tsNCharcb sExchange;
     tsNCharcb sDescription;
     tsNCharcb sExpiration;
     tsNCharcb sProductCode;
     tsNCharcb sInstrumentType;
     tsNCharcb sIsTradable;
     tsNCharcb sBinaryContractType;
     tsNCharcb sPutCallIndicator;
     tsNCharcb sUnderlying;

     tsNCharcb sTradingTicker;
     tsNCharcb sTradingExchange;

     tsNCharcb sOpenTime;
     tsNCharcb sPeriodCode;
     tsNCharcb sExpirationTime;
     tsNCharcb sStrikeOperator;

     bool      bCapPrice;
     double    dCapPrice;
     bool      bFloorPrice;
     double    dFloorPrice;

     bool      bSinglePointValue;
     double    dSinglePointValue;
     tsNCharcb sCurrency;

     bool      bMaxPriceVariation;
     double    dMaxPriceVariation;

     bool      bStrikePrice;
     double    dStrikePrice;

     bool      bMinSizeIncrement;
     long long llMinSizeIncrement;
     bool      bSizeMultiplier;
     double    dSizeMultiplier;
     int       iSizeMultiplierPrecision;

     int       iSsboe;

     int       iType;

     int       iRpCode;
     tsNCharcb sRpCode;

     int dump(int * aiCode);

     private :
     };

/*   =====================================================================   */

class RmsInfo
     {
     public :
     RmsInfo();
     virtual ~RmsInfo();

     tsNCharcb   sStatus;
     tsNCharcb   sCurrency;

     long long   llBuyLimit;
     long long   llSellLimit;
     long long   llMaxOrderQty;

     double      dMinAccountBalance;
     double      dLossLimit;

     bool        bCheckMinAccountBalance;

     bool        bMinMarginBalance;
     double      dMinMarginBalance;

     bool        bDefaultCommission;
     double      dDefaultCommission;

     tsNCharcb   sAlgorithm;

     tsNCharcb   sAutoLiquidate;
     tsNCharcb   sAutoLiquidateCriteria;
     bool        bAutoLiquidateThreshold;
     double      dAutoLiquidateThreshold;
     bool        bAutoLiquidateMaxMinAccountBalance;
     double      dAutoLiquidateMaxMinAccountBalance;
     tsNCharcb   sDisableOnAutoLiquidate;
     bool        bAutoLiquidateAtSsm;
     int         iAutoLiquidateAtSsm;
     tsNCharcb   sAutoLiquidateIgnoreOpenPnlWhenTrailing;

     int clearHandles(int * aiCode);
     int dump(int * aiCode);

     private :
     };

/*   =====================================================================   */

class SettlementPriceInfo
     {
     public :
     SettlementPriceInfo();
     virtual ~SettlementPriceInfo();

     tsNCharcb sTicker;
     tsNCharcb sExchange;
     double    dPrice;
     bool      bPriceFlag;
     tsNCharcb sPriceType;
     tsNCharcb sDate;

     int       iType;

     int       iSsboe;
     int       iUsecs;

     int dump(int * aiCode);

     private :
     };

/*   =====================================================================   */

class SingleOrderReplayInfo
     {
     public :
     SingleOrderReplayInfo();
     virtual ~SingleOrderReplayInfo();

     AccountInfo oAccount;
     tsNCharcb   sOrderNum;
     tsNCharcb   sDate;
     void *      pContext;

     int         iRpCode;
     tsNCharcb   sRpCode;

     int dump(int * aiCode);

     private :
     };

/*   =====================================================================   */

class StrategyLegInfo
     {
     public :
     StrategyLegInfo();
     virtual ~StrategyLegInfo();

     tsNCharcb sTicker;
     tsNCharcb sExchange;
     tsNCharcb sProductCode;
     tsNCharcb sInstrumentType;

     int       iRatio;

     int clearHandles(int * aiCode);
     int dump(int * aiCode);

     private :
     };

/*   =====================================================================   */

class StrategyInfo
     {
     public :
     StrategyInfo();
     virtual ~StrategyInfo();

     tsNCharcb sTicker;
     tsNCharcb sExchange;
     tsNCharcb sProductCode;
     tsNCharcb sType;

     StrategyLegInfo * asLegArray;
     int               iLegArrayLen;

     void *    pContext;

     int       iRpCode;
     tsNCharcb sRpCode;

     int clearHandles(int * aiCode);
     int dump(int * aiCode);

     private :
     };

/*   =====================================================================   */

class StrategyListInfo
     {
     public :
     StrategyListInfo();
     virtual ~StrategyListInfo();

     virtual int clearHandles(int * aiCode);
     int dump(int * aiCode);

     tsNCharcb *   pExchangeArray;
     int           iExchangeArrayLen;
     tsNCharcb *   pProductCodeArray;
     int           iProductCodeArrayLen;
     tsNCharcb *   pStrategyTypeArray;
     int           iStrategyTypeArrayLen;
     tsNCharcb *   pExpirationCCYYMMArray;
     int           iExpirationCCYYMMArrayLen;
     tsNCharcb *   pTickerArray;
     int           iTickerArrayLen;

     int           iRpCode;
     tsNCharcb     sRpCode;

     void *        pContext;

     private :
     };

/*   =====================================================================   */

class TradeInfo
     {
     public :
     TradeInfo();
     virtual ~TradeInfo();

     tsNCharcb sTicker;
     tsNCharcb sExchange;
     double    dPrice;
     bool      bPriceFlag;

     long long llSize;

     double    dNetChange;
     bool      bNetChangeFlag;
     double    dPercentChange;
     bool      bPercentChangeFlag;

     tsNCharcb sAggressorSide;
     tsNCharcb sCondition;

     tsNCharcb sAggressorExchOrdId;
     tsNCharcb sExchOrdId;

     bool      bVolumeBoughtFlag;
     long long llVolumeBought;
     bool      bVolumeSoldFlag;
     long long llVolumeSold;

     double    dVwap;
     bool      bVwapFlag;
     double    dVwapLong;
     bool      bVwapLongFlag;

     int       iType;

     int       iConnId;

     int       iSourceSsboe;
     int       iSourceUsecs;
     int       iSourceNsecs;

     int       iJopSsboe;
     int       iJopNsecs;

     int       iSsboe;
     int       iUsecs;

     int dump(int * aiCode);

     private :
     };

/*   =====================================================================   */

class TradeReplayInfo
     {
     public :
     TradeReplayInfo();
     virtual ~TradeReplayInfo();

     tsNCharcb   sExchange;
     tsNCharcb   sTicker;

     int         iStartSsboe;
     int         iEndSsboe;

     int         iRpCode;
     tsNCharcb   sRpCode;

     int dump(int * aiCode);

     private :
     };

/*   =====================================================================   */

class TradeRouteInfo
     {
     public :
     TradeRouteInfo();
     virtual ~TradeRouteInfo();

     tsNCharcb sFcmId;
     tsNCharcb sIbId;
     tsNCharcb sExchange;
     tsNCharcb sTradeRoute;
     tsNCharcb sStatus;
     tsNCharcb sDefault;

     int iType;

     void * pContext;

     int clearHandles(int * aiCode);
     int dump(int * aiCode);

     private :
     };

/*   =====================================================================   */

class TradeRouteListInfo
     {
     public :
     TradeRouteListInfo();
     virtual ~TradeRouteListInfo();

     TradeRouteInfo * asTradeRouteInfoArray;
     int              iArrayLen;

     void * pContext;
     int    iRpCode;

     int dump(int * aiCode);

     private :
     };

/*   =====================================================================   */

class TradeVolumeInfo
     {
     public :
     TradeVolumeInfo();
     virtual ~TradeVolumeInfo();

     tsNCharcb sTicker;
     tsNCharcb sExchange;

     bool      bTotalVolumeFlag;
     long long llTotalVolume;

     int       iType;

     int       iSourceSsboe;
     int       iSourceUsecs;
     int       iSourceNsecs;

     int       iJopSsboe;
     int       iJopNsecs;

     int       iSsboe;
     int       iUsecs;

     int dump(int * aiCode);

     private :
     };

/*   =====================================================================   */

class UserInfo
     {
     public :
     UserInfo();
     virtual ~UserInfo();

     tsNCharcb sFcmId;
     tsNCharcb sIbId;
     tsNCharcb sUser;

     tsNCharcb sUserType;
     tsNCharcb sStatus;

     tsNCharcb sFirstName;
     tsNCharcb sLastName;
     tsNCharcb sAddress1;
     tsNCharcb sAddress2;
     tsNCharcb sCity;
     tsNCharcb sStateProvinceRegion;
     tsNCharcb sPostalCode;
     tsNCharcb sCountry;

     tsNCharcb sEmail;

     int       iCreationSsboe;
     int       iCreationUsecs;

     int       iFirstLoginSsboe;
     int       iFirstLoginUsecs;

     int       iLastLoginSsboe;
     int       iLastLoginUsecs;
     tsNCharcb sLastLoginResult;
     tsNCharcb sLastLoginRemarks;

     int       iDisableSsboe;

     int       iSsboe;
     int       iUsecs;

     tsNCharcb sAction;
     
     int       iCallbackType;

     void *    pContext;

     int dump(int * aiCode);

     private :
     };

/*   =====================================================================   */

class UserDefinedSpreadCreateInfo
     {
     public :
     UserDefinedSpreadCreateInfo();
     virtual ~UserDefinedSpreadCreateInfo();

     /*   ----------------------------------------------------------------   */

     tsNCharcb sText;
     tsNCharcb sTicker;
     int       iRpCode;

     void *    pContext;     

     int dump(int * aiCode);

     private :
     };

/*   =====================================================================   */

class UserListInfo
     {
     public :
     UserListInfo();
     virtual ~UserListInfo();

     tsNCharcb sIbId;
     tsNCharcb sUserType;

     int       iRpCode;

     void *    pContext;

     int dump(int * aiCode);

     private :
     };

/*   =====================================================================   */

class UserProfileInfo
     {
     public :
     UserProfileInfo();
     virtual ~UserProfileInfo();

     tsNCharcb sUser;
     tsNCharcb sUserType;

     tsNCharcb sFirstName;
     tsNCharcb sLastName;
     tsNCharcb sAddress1;
     tsNCharcb sAddress2;
     tsNCharcb sCity;
     tsNCharcb sStateProvinceRegion;
     tsNCharcb sPostalCode;
     tsNCharcb sCountry;

     tsNCharcb sResidencePhone;
     tsNCharcb sWorkPhone;
     tsNCharcb sMobilePhone;
     tsNCharcb sFaxPhone;
     tsNCharcb sEmail;

     int       iCurrentHistorySessionCount;
     int       iCurrentOrdersSessionCount;
     int       iCurrentPnlSessionCount;
     int       iCurrentRepositorySessionCount;
     int       iCurrentMarketDataSessionCount;

     int       iPeakHistorySessionCount;
     int       iPeakOrdersSessionCount;
     int       iPeakPnlSessionCount;
     int       iPeakRepositorySessionCount;
     int       iPeakMarketDataSessionCount;

     int       iMaxHistorySessionCount;
     int       iMaxOrdersSessionCount;
     int       iMaxPnlSessionCount;
     int       iMaxRepositorySessionCount;
     int       iMaxMarketDataSessionCount;

     int       iConnId;

     int       iRpCode;

     void *    pContext;

     int dump(int * aiCode);

     private :
     };

/*   =====================================================================   */

class VolumeAtPriceInfo
     {
     public :
     VolumeAtPriceInfo();
     virtual ~VolumeAtPriceInfo();

     tsNCharcb   sTicker;
     tsNCharcb   sExchange;

     void *      pContext;

     double *    adPriceArray;
     long long * allVolumeArray;
     int         iArrayLen;

     int         iSsboe;
     int         iUsecs;

     int         iRpCode;

     int dump(int * aiCode);

     private :
     };

/*   =====================================================================   */
/*   Reports                                                                 */
/*   =====================================================================   */

class SodReport
     {
     public :
     SodReport();
     virtual ~SodReport();

     AccountInfo oAccount;

     tsNCharcb   sTicker;
     tsNCharcb   sExchange;

     double      dPrevClosePrice;

     long long   llCarriedSize;

     int         iSsboe;

     tsNCharcb   sReportType;

     int clearHandles(int * aiCode);
     int dump(int * aiCode);
     };

/*   =====================================================================   */

class OrderReport
     {
     public :
     OrderReport();
     virtual ~OrderReport();

     AccountInfo oAccount;

     tsNCharcb   sTicker;
     tsNCharcb   sExchange;

     tsNCharcb   sOrderNum;
     tsNCharcb   sOriginalOrderNum;
     tsNCharcb   sInitialSequenceNumber;
     tsNCharcb   sCurrentSequenceNumber;
     tsNCharcb   sExchOrdId;
     tsNCharcb   sTickerPlantExchOrdId;

     tsNCharcb   sOmnibusAccount;
     tsNCharcb   sPositionDisposition;

     tsNCharcb   sOrderType;
     tsNCharcb   sOriginalOrderType;
     tsNCharcb   sBuySellType;
     tsNCharcb   sOrderDuration;
     tsNCharcb   sEntryType;
     tsNCharcb   sLinkedOrderNums;

     double      dPriceToFill;
     bool        bPriceToFillFlag;
     double      dTriggerPrice;
     bool        bTriggerPriceFlag;

     tsNCharcb   sTriggerTicker;
     tsNCharcb   sTriggerExchange;
     int         iTriggerCmpOperator;
     int         iTriggerPriceId;

     double      dFillPrice;
     bool        bFillPriceFlag;
     long long   llFillSize;

     long long   llTotalFilled;
     long long   llTotalUnfilled;

     double      dAvgFillPrice;
     bool        bAvgFillPriceFlag;

     tsNCharcb   sReportId;
     tsNCharcb   sReportType;
     tsNCharcb   sText;

     tsNCharcb   sWindowName;
     tsNCharcb   sOriginatorWindowName;

     tsNCharcb   sTag;
     tsNCharcb   sUserTag;
     tsNCharcb   sUserMsg;
     void *      pContext;

     tsNCharcb   sUser;

     int         iConnId;

     int         iGatewayBufferBytesBehind;                /* experimental   */
     int         iGatewaySsboe;                            /* experimental   */
     int         iGatewayUsecs;                            /* experimental   */

     int         iSourceSsboe;
     int         iSourceNsecs;

     int         iSsboe;
     int         iUsecs;

     int         iType;

     virtual int clearHandles(int * aiCode);
     virtual int dump(int * aiCode);

     private :
     };

class OrderBustReport : public OrderReport
     {
     public :
     OrderBustReport();
     virtual ~OrderBustReport();

     tsNCharcb sFillType;
     tsNCharcb sTradeDate;

     tsNCharcb sSettlementDate;
     tsNCharcb sSettlementCurrency;

     int dump(int * aiCode);
     };

class OrderCancelReport : public OrderReport
     {
     public :
     OrderCancelReport();
     virtual ~OrderCancelReport();

     long long llCancelledSize;

     int dump(int * aiCode);
     };

class OrderFailureReport : public OrderReport
     {
     public :
     OrderFailureReport();
     virtual ~OrderFailureReport();

     long long llCancelledSize;
     tsNCharcb sStatus;

     int dump(int * aiCode);
     };

class OrderFillReport : public OrderReport
     {
     public :
     OrderFillReport();
     virtual ~OrderFillReport();

     tsNCharcb sFillType;
     tsNCharcb sTradeDate;

     tsNCharcb sSettlementDate;
     tsNCharcb sSettlementCurrency;

     tsNCharcb sLiquidityIndicator;

     tsNCharcb sQuoteId;
     tsNCharcb sQuoteMsgId;

     int       iRpCode;
     tsNCharcb sRpCode;

     int clearHandles(int * aiCode);
     int dump(int * aiCode);
     };

class OrderModifyReport : public OrderReport
     {
     public :
     OrderModifyReport();
     virtual ~OrderModifyReport();

     long long llNewSize;

     int dump(int * aiCode);
     };

class OrderNotCancelledReport : public OrderReport
     {
     public :
     OrderNotCancelledReport();
     virtual ~OrderNotCancelledReport();

     long long llNotCancelledSize;

     int dump(int * aiCode);
     };

class OrderNotModifiedReport : public OrderReport
     {
     public :
     OrderNotModifiedReport();
     virtual ~OrderNotModifiedReport();

     long long llNotModifiedSize;

     int dump(int * aiCode);
     };

class OrderRejectReport : public OrderReport
     {
     public :
     OrderRejectReport();
     virtual ~OrderRejectReport();

     long long llRejectedSize;
     bool      bReplacementOrderToFollow;

     int dump(int * aiCode);
     };

class OrderStatusReport : public OrderReport
     {
     public :
     OrderStatusReport();
     virtual ~OrderStatusReport();

     long long llConfirmedSize;

     int dump(int * aiCode);
     };

class OrderTradeCorrectReport : public OrderReport
     {
     public :
     OrderTradeCorrectReport();
     virtual ~OrderTradeCorrectReport();

     tsNCharcb sFillType;
     tsNCharcb sTradeDate;

     tsNCharcb sSettlementDate;
     tsNCharcb sSettlementCurrency;

     int dump(int * aiCode);
     };

class OrderTriggerPulledReport : public OrderReport
     {
     public :
     OrderTriggerPulledReport();
     virtual ~OrderTriggerPulledReport();

     int dump(int * aiCode);
     };

class OrderTriggerReport : public OrderReport
     {
     public :
     OrderTriggerReport();
     virtual ~OrderTriggerReport();

     long long llConfirmedSize;

     int dump(int * aiCode);
     };

/*   =====================================================================   */
/*                       order parameter info classes                        */
/*   =====================================================================   */

class OrderParams
     {
     public :
     OrderParams();
     OrderParams(LimitOrderParams * pParamsIn);
     OrderParams(MarketOrderParams * pParamsIn);
     OrderParams(StopLimitOrderParams * pParamsIn);
     OrderParams(StopMarketOrderParams * pParamsIn);
     virtual ~OrderParams();

     int clearHandles(int * aiCode);

     AccountInfo * pAccount;

     tsNCharcb     sExchange;
     tsNCharcb     sTicker;

     tsNCharcb     sBuySellType;                     /* B/S                  */
     tsNCharcb     sOrderType;                       /* L/M/SLMT/STP/MIT/LIT */
     tsNCharcb     sDuration;                        /* DAY/FOK/GTC/OC       */

     tsNCharcb     sEntryType;                       /* A/M                  */
     tsNCharcb     sTradingAlgorithm;

     int           iQty;
     bool          bMinQty;
     int           iMinQty;
     bool          bMaxShowQty;
     int           iMaxShowQty;

     double        dPrice;                           /* L/SLMT/LIT           */
     double        dTriggerPrice;                    /* SLMT/STP/MIT/LIT     */

     bool          bTrailingStop;
     int           iTrailByTicks;                    /* SLMT/STP             */
     int           iTrailByPriceId;                  /* SLMT/STP             */

     tsNCharcb     sTriggerExchange;                 /* MIT/LIT              */
     tsNCharcb     sTriggerTicker;                   /* MIT/LIT              */
     int           iTriggerPriceId;                  /* MIT/LIT              */
     int           iTriggerCmpOperator;              /* MIT/LIT              */

     tsNCharcb     sTradeRoute;                      /*                      */
     tsNCharcb     sRoutingInstructions;             /*                      */

     tsNCharcb     sReleaseCommand;                  /* hold_order           */

     bool          bReleaseSsboe;
     int           iReleaseSsboe;
     bool          bReleaseUsecs;
     int           iReleaseUsecs;

     bool          bCancelSsboe;
     int           iCancelSsboe;
     bool          bCancelUsecs;
     int           iCancelUsecs;

     bool          bCancelAfter;
     int           iCancelAfter;

     bool          bToMarketAt;
     int           iToMarketAtSsboe;
     int           iToMarketAtUsecs;

     bool          bToMarketAfterPartialFillInSeconds;
     int           iToMarketAfterPartialFillInSeconds;

     tsNCharcb     sToMarketOnReject;
     tsNCharcb     sToMarketWhenLimitPriceTouched;

     bool   bReleaseCondition;                 /* enable release when        */
     int    iReleaseConditionPriceId;          /* BID_PRICE or ASK_PRICE     */
     int    iReleaseConditionPriceCmpOperator; /* gt, lt, etc                */
     double dReleaseConditionPrice;            /* price to compare           */
     int    iReleaseConditionSizeId;           /* BID_SIZE or ASK_SIZE       */
     int    iReleaseConditionSizeCmpOperator;  /* gt, lt, etc                */
     int    iReleaseConditionSize;             /* size to compare            */

     tsNCharcb     sTag;
     tsNCharcb     sUserTag;
     tsNCharcb     sUserMsg;
     tsNCharcb     sWindowName;
     void *        pContext;

     private :
     };

/*   =====================================================================   */

class LimitOrderParams
     {
     public :
     LimitOrderParams();
     virtual ~LimitOrderParams();

     AccountInfo * pAccount;

     tsNCharcb     sExchange;
     tsNCharcb     sTicker;

     tsNCharcb     sBuySellType;
     tsNCharcb     sDuration;

     tsNCharcb     sEntryType;
     tsNCharcb     sTradingAlgorithm;

     int           iQty;
     double        dPrice;

     tsNCharcb     sTradeRoute;
     tsNCharcb     sRoutingInstructions;

     tsNCharcb     sTag;
     tsNCharcb     sUserMsg;
     void *        pContext;

     int clearHandles(int * aiCode);

     private :
     };

/*   =====================================================================   */

class MarketOrderParams
     {
     public :
     MarketOrderParams();
     virtual ~MarketOrderParams();

     AccountInfo * pAccount;

     tsNCharcb     sExchange;
     tsNCharcb     sTicker;

     tsNCharcb     sBuySellType;
     tsNCharcb     sDuration;

     tsNCharcb     sEntryType;
     tsNCharcb     sTradingAlgorithm;

     int           iQty;

     tsNCharcb     sTradeRoute;
     tsNCharcb     sRoutingInstructions;

     tsNCharcb     sTag;
     tsNCharcb     sUserMsg;
     void *        pContext;

     int clearHandles(int * aiCode);

     private :
     };

/*   =====================================================================   */

class StopLimitOrderParams : public LimitOrderParams
     {
     public :
     StopLimitOrderParams();
     virtual ~StopLimitOrderParams();

     double    dTriggerPrice;

     int clearHandles(int * aiCode);

     private :
     };

/*   =====================================================================   */

class StopMarketOrderParams : public MarketOrderParams
     {
     public :
     StopMarketOrderParams();
     virtual ~StopMarketOrderParams();

     double    dTriggerPrice;

     int clearHandles(int * aiCode);

     private :
     };

/*   =====================================================================   */

class ModifyOrderParams
     {
     public :
     ModifyOrderParams();
     ModifyOrderParams(ModifyLimitOrderParams * pParamsIn);
     ModifyOrderParams(ModifyStopLimitOrderParams * pParamsIn);
     ModifyOrderParams(ModifyStopMarketOrderParams * pParamsIn);
     virtual ~ModifyOrderParams();

     int clearHandles(int * aiCode);

     AccountInfo * pAccount;

     tsNCharcb     sExchange;
     tsNCharcb     sTicker;
     tsNCharcb     sOrderNum;

     tsNCharcb     sOrderType;
     tsNCharcb     sDuration;
     tsNCharcb     sEntryType;
     tsNCharcb     sTradingAlgorithm;

     bool          bQty;
     int           iQty;
     bool          bMinQty;
     int           iMinQty;

     double        dPrice;
     bool          bPrice;
     double        dTriggerPrice;
     bool          bTriggerPrice;

     bool          bTrailByTicks;
     int           iTrailByTicks;

     tsNCharcb     sTriggerExchange;
     tsNCharcb     sTriggerTicker;

     tsNCharcb     sUserMsg;
     tsNCharcb     sWindowName;
     void *        pContext;

     private :
     };

/*   =====================================================================   */

class ModifyLimitOrderParams
     {
     public :
     ModifyLimitOrderParams();
     virtual ~ModifyLimitOrderParams();

     int clearHandles(int * aiCode);

     AccountInfo * pAccount;

     tsNCharcb     sExchange;
     tsNCharcb     sTicker;
     tsNCharcb     sOrderNum;

     tsNCharcb     sEntryType;
     tsNCharcb     sTradingAlgorithm;

     int           iQty;
     double        dPrice;

     tsNCharcb     sUserMsg;
     void *        pContext;

     private :
     };

/*   =====================================================================   */

class ModifyStopLimitOrderParams
     {
     public :
     ModifyStopLimitOrderParams();
     virtual ~ModifyStopLimitOrderParams();

     int clearHandles(int * aiCode);

     AccountInfo * pAccount;

     tsNCharcb     sExchange;
     tsNCharcb     sTicker;
     tsNCharcb     sOrderNum;

     tsNCharcb     sEntryType;
     tsNCharcb     sTradingAlgorithm;

     int           iQty;
     double        dPrice;
     double        dTriggerPrice;

     tsNCharcb     sUserMsg;
     void *        pContext;

     private :
     };

/*   =====================================================================   */

class ModifyStopMarketOrderParams
     {
     public :
     ModifyStopMarketOrderParams();
     virtual ~ModifyStopMarketOrderParams();

     int clearHandles(int * aiCode);

     AccountInfo * pAccount;

     tsNCharcb     sExchange;
     tsNCharcb     sTicker;
     tsNCharcb     sOrderNum;

     tsNCharcb     sEntryType;
     tsNCharcb     sTradingAlgorithm;

     int           iQty;
     double        dTriggerPrice;

     tsNCharcb     sUserMsg;
     void *        pContext;

     private :
     };

/*   =====================================================================   */

class BracketParams
     {
     public :
     BracketParams();
     virtual ~BracketParams();

     virtual int clearHandles(int * aiCode);

     /*   ----------------------------------------------------------------   */

     tsNCharcb     sBracketType;
     tsNCharcb     sOrderOperationType;

     /*   ----------------------------------------------------------------   */

     tsNCharcb     sTargetsToMarketWhenTouched;

     bool          bTargetsToMarketAt;
     int           iTargetsToMarketAtSsboe;
     int           iTargetsToMarketAtUsecs;

     bool          bTargetsToMarketAfterPartialFillInSeconds;
     int           iTargetsToMarketAfterPartialFillInSeconds;

     int           iTargetArrayLen;
     BracketTier * asTargetArray;

     /*   ----------------------------------------------------------------   */

     tsNCharcb     sStopsToMarketOnReject;

     bool          bStopsToMarketAt;
     int           iStopsToMarketAtSsboe;
     int           iStopsToMarketAtUsecs;

     bool          bTrailingStop;
     int           iTrailByPriceId;
     bool          bTrailingStopTriggerTicks;
     int           iTrailingStopTriggerTicks;

     int           iStopArrayLen;
     BracketTier * asStopArray;
     };

/*   =====================================================================   */

class QuoteParams
     {
     public :
     QuoteParams();
     virtual ~QuoteParams();

     int clearHandles(int * aiCode);

     AccountInfo * pAccount;

     tsNCharcb     sExchange;
     tsNCharcb     sTicker;

     bool          bBid;
     int           iBidQty; 
     double        dBidPrice;

     int           iAskQty;
     double        dAskPrice;

     tsNCharcb     sUserMsg;

     private :
     };

/*   =====================================================================   */

class QuoteCancelParams
     {
     public :
     QuoteCancelParams();
     virtual ~QuoteCancelParams();

     int clearHandles(int * aiCode);

     AccountInfo * pAccount;

     tsNCharcb     sExchange;
     tsNCharcb     sTicker;

     tsNCharcb     sUserMsg;

     private :
     };

/*   =====================================================================   */
/*                                     bar classes                           */
/*   =====================================================================   */

class BarParams
     {
     public :

     BarParams();
     virtual ~BarParams();

     tsNCharcb sExchange;
     tsNCharcb sTicker;

     int       iType;                       /* see BAR_TYPEs                 */
     double    dSpecifiedRange;
     int       iSpecifiedTicks;
     long long llSpecifiedVolume;
     int       iSpecifiedMinutes;
     int       iSpecifiedSeconds;

     bool      bCustomSession;
     int       iOpenSsm;                    /* seconds since midnight, GMT   */
     int       iCloseSsm;                   /* seconds since midnight, GMT   */

     bool      bProfile;                    /* if avail, inc profile data    */

     void *    pContext;
     };

class ReplayBarParams : public BarParams
     {
     public :

     ReplayBarParams();
     virtual ~ReplayBarParams();

     int       iStartSsboe;
     int       iStartUsecs;
     int       iEndSsboe;
     int       iEndUsecs;

     tsNCharcb sStartDate;     /* CCYYMMDD, only for daily and weekly bars   */
     tsNCharcb sEndDate;       /* CCYYMMDD, only for daily and weekly bars   */
     };

class BarInfo
     {
     public :
     BarInfo();
     virtual ~BarInfo();

     int dump(int * aiCode);

     tsNCharcb sExchange;
     tsNCharcb sTicker;

     int       iType;                       /* see BAR_TYPEs                 */
     double    dSpecifiedRange;
     int       iSpecifiedTicks;
     long long llSpecifiedVolume;
     int       iSpecifiedMinutes;
     int       iSpecifiedSeconds;
     tsNCharcb sSpecifiedDate;

     bool      bCustomSession;
     int       iOpenSsm;                    /* seconds since midnight, GMT   */
     int       iCloseSsm;                   /* seconds since midnight, GMT   */

     /* OHLC */
     double dOpenPrice;
     double dHighPrice;
     double dLowPrice;
     double dClosePrice;

     bool   bSettlementPrice;
     double dSettlementPrice;

     bool      bBuyerAsAggressorVolume;
     long long llBuyerAsAggressorVolume;

     bool      bSellerAsAggressorVolume;
     long long llSellerAsAggressorVolume;

     bool   bVwap;
     double dVwap;
     bool   bVwapLong;
     double dVwapLong;

     int iStartSsboe;
     int iStartUsecs;
     int iEndSsboe;
     int iEndUsecs;

     long long llVolume;                                    /* can be zero   */
     int iNumTrades;                                        /* can be zero   */

     /*   ----------------------------------------------------------------   */

     int         iProfileArrayLen;                          /* can be zero   */
     double *    adProfilePrices;                           /* can be NULL   */
     long long * allProfileNoAggressorVolumes;              /* can be NULL   */
     long long * allProfileBuyerAsAggressorVolumes;         /* can be NULL   */
     long long * allProfileSellerAsAggressorVolumes;        /* can be NULL   */
     int *       aiProfileNoAggressorNumTrades;             /* can be NULL   */
     int *       aiProfileBuyerAsAggressorNumTrades;        /* can be NULL   */
     int *       aiProfileSellerAsAggressorNumTrades;       /* can be NULL   */

     /*   ----------------------------------------------------------------   */

     void * pContext;
     };

/*   =====================================================================   */

class BarReplayInfo
     {
     public :
     BarReplayInfo();
     virtual ~BarReplayInfo();

     int dump(int * aiCode);

     tsNCharcb sExchange;
     tsNCharcb sTicker;

     int       iType;                       /* see BAR_TYPEs                 */
     double    dSpecifiedRange;
     int       iSpecifiedTicks;
     long long llSpecifiedVolume;
     int       iSpecifiedMinutes;
     int       iSpecifiedSeconds;

     bool      bCustomSession;
     int       iOpenSsm;                    /* seconds since midnight, GMT   */
     int       iCloseSsm;                   /* seconds since midnight, GMT   */

     void *    pContext;

     int       iStartSsboe;
     int       iStartUsecs;
     int       iEndSsboe;
     int       iEndUsecs;

     tsNCharcb sStartDate;
     tsNCharcb sEndDate;

     bool      bProfile;

     int       iRpCode;
     tsNCharcb sRpCode;

     private :
     };

/*   =====================================================================   */
/*                    engine and login parameter info classes                */
/*   =====================================================================   */

class REngineParams
     {
     public :

     REngineParams();
     virtual ~REngineParams();

     tsNCharcb sAppName;
     tsNCharcb sAppVersion;

     tsNCharcb sLogFilePath;
     char * *  envp;
     void *    pContext;

     AdmCallbacks * pAdmCallbacks;

     private :
     };

/*   =====================================================================   */

class LoginParams
     {
     public :

     LoginParams();
     virtual ~LoginParams();

     tsNCharcb sMdEnvKey;
     tsNCharcb sMdUser;
     tsNCharcb sMdPassword;
     tsNCharcb sMdCnnctPt;

     tsNCharcb sTsEnvKey;
     tsNCharcb sTsUser;
     tsNCharcb sTsPassword;
     tsNCharcb sTsCnnctPt;
     tsNCharcb sPnlCnnctPt;

     tsNCharcb sIhEnvKey;
     tsNCharcb sIhUser;
     tsNCharcb sIhPassword;
     tsNCharcb sIhCnnctPt;

     RCallbacks * pCallbacks;

     private :
     };

/*   =====================================================================   */
/*                                working classes                            */
/*   =====================================================================   */

class AdmCallbacks
     {
     public :
     AdmCallbacks();
     virtual ~AdmCallbacks();

     /*   ----------------------------------------------------------------   */

     virtual int Alert(AlertInfo * pInfo,
                       void *      pContext,
                       int *       aiCode) = 0;

     virtual int Environment(EnvironmentInfo * pInfo,
			     void *            pContext,
			     int *             aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     };

     virtual int EnvironmentList(EnvironmentListInfo * pInfo,
				 void *                pContext,
				 int *                 aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     };

     /*   ----------------------------------------------------------------   */

     private :
     };

/*   =====================================================================   */

class RCallbacks
     {
     public :
     virtual ~RCallbacks();

     /*   ----------------------------------------------------------------   */

     virtual int AccountList(AccountListInfo * pInfo,
                             void *            pContext,
                             int *             aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     };

     virtual int AccountUpdate(AccountUpdateInfo * pInfo,
			       void *              pContext,
			       int *               aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     };

     virtual int AgreementList(AgreementListInfo * pInfo,
			       void *              pContext,
			       int *               aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     };

     virtual int Aggregator(AggregatorInfo * pInfo,
			    void *           pContext,
			    int *            aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     };

     virtual int Alert(AlertInfo * pInfo,
                       void *      pContext,
                       int *       aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     };

     virtual int AskQuote(AskInfo * pInfo,
                          void *    pContext,
                          int *     aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     };

     virtual int AssignedUserList(AssignedUserListInfo * pInfo,
				  void *                 pContext,
				  int *                  aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     };

     virtual int AutoLiquidate(AutoLiquidateInfo * pInfo,
			       void *              pContext,
			       int *               aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     };

     virtual int AuxRefData(AuxRefDataInfo * pInfo,
			    void *           pContext,
			    int *            aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     };

     virtual int Bar(BarInfo * pInfo,
		     void *    pContext,
		     int *     aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     };

     virtual int BarReplay(BarReplayInfo * pInfo,
			   void *          pContext,
			   int *           aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     };

     virtual int BestAskQuote(AskInfo * pInfo,
                              void *    pContext,
                              int *     aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     };

     virtual int BestBidAskQuote(BidInfo * pBid,
				 AskInfo * pAsk,
				 void *    pContext,
				 int *     aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     };

     virtual int BestBidQuote(BidInfo * pInfo,
                              void *    pContext,
                              int *     aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     };

     virtual int BidQuote(BidInfo * pInfo,
                          void *    pContext,
                          int *     aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     };

     virtual int BinaryContractList(BinaryContractListInfo * pInfo,
				    void *                   pContext,
				    int *                    aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     };

     virtual int BracketReplay(BracketReplayInfo * pInfo,
			       void *              pContext,
			       int *               aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     };

     virtual int BracketTierModify(BracketTierModifyInfo * pInfo,
				   void *                  pContext,
				   int *                   aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     };

     virtual int BracketUpdate(BracketInfo * pInfo,
			       void *        pContext,
			       int *         aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     };

     virtual int BustReport(OrderBustReport * pReport,
                            void *            pContext,
                            int *             aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     };

     virtual int CancelReport(OrderCancelReport * pReport,
                              void *              pContext,
                              int *               aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     };

     virtual int CloseMidPrice(CloseMidPriceInfo * pInfo,
			       void *              pContext,
			       int *               aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     };

     virtual int ClosePrice(ClosePriceInfo * pInfo,
                            void *           pContext,
                            int *            aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     };

     virtual int ClosingIndicator(ClosingIndicatorInfo * pInfo,
                                  void *                 pContext,
                                  int *                  aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     };

     virtual int Dbo(DboInfo * pInfo,
		     void *    pContext,
		     int *     aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     };

     virtual int DboBookRebuild(DboBookRebuildInfo * pInfo,
				void *               pContext,
				int *                aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     };

     virtual int EasyToBorrow(EasyToBorrowInfo * pInfo,
			      void *             pContext,
			      int *              aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     };

     virtual int EasyToBorrowList(EasyToBorrowListInfo * pInfo,
				  void *                 pContext,
				  int *                  aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     };

     virtual int EndQuote(EndQuoteInfo * pInfo,
                          void *         pContext,
                          int *          aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     };

     virtual int EquityOptionStrategyList(EquityOptionStrategyListInfo * pInfo,
					  void *                         pContext,
					  int *                          aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     };

     virtual int ExchangeList(ExchangeListInfo * pInfo,
			      void *             pContext,
			      int *              aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     };

     virtual int ExecutionReplay(ExecutionReplayInfo * pInfo,
                                 void *                pContext,
                                 int *                 aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     };

     virtual int FailureReport(OrderFailureReport * pReport,
                               void *               pContext,
                               int *                aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     };

     virtual int FillReport(OrderFillReport * pReport,
                             void *           pContext,
                            int *             aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     };

     virtual int HighBidPrice(HighBidPriceInfo * pInfo,
			      void *             pContext,
			      int *              aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     };

     virtual int HighPrice(HighPriceInfo * pInfo,
                           void *          pContext,
                           int *           aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     };

     virtual int HighPriceLimit(HighPriceLimitInfo * pInfo,
				void *               pContext,
				int *                aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     };

     virtual int IbList(IbListInfo * pInfo,
			void *       pContext,
			int *        aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     };

     virtual int InstrumentByUnderlying(InstrumentByUnderlyingInfo * pInfo,
					void *                       pContext,
					int *                        aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     };

     virtual int InstrumentSearch(InstrumentSearchInfo * pInfo,
				  void *                 pContext,
				  int *                  aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     };

     virtual int LimitOrderBook(LimitOrderBookInfo * pInfo,
                                void *               pContext,
                                int *                aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     };

     virtual int LineUpdate(LineInfo * pInfo,
                            void *     pContext,
                            int *      aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     };

     virtual int LowAskPrice(LowAskPriceInfo * pInfo,
			     void *            pContext,
			     int *             aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     };

     virtual int LowPrice(LowPriceInfo * pInfo,
                          void *         pContext,
                          int *          aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     };

     virtual int LowPriceLimit(LowPriceLimitInfo * pInfo,
			       void *              pContext,
			       int *               aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     };

     virtual int MarketMode(MarketModeInfo * pInfo,
                            void *           pContext,
                            int *            aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     };

     virtual int MidPrice(MidPriceInfo * pInfo,
			  void *         pContext,
			  int *          aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     };

     virtual int ModifyReport(OrderModifyReport * pReport,
                              void *              pContext,
                              int *               aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     };

     virtual int NotCancelledReport(OrderNotCancelledReport * pReport,
                                    void *                    pContext,
                                    int *                     aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     };

     virtual int NotModifiedReport(OrderNotModifiedReport * pReport,
                                   void *                   pContext,
                                   int *                    aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     };

     virtual int OpenInterest(OpenInterestInfo * pInfo,
			      void *             pContext,
			      int *              aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     };

     virtual int OpenOrderReplay(OrderReplayInfo * pInfo,
                                 void *            pContext,
                                 int *             aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     };

     virtual int OpenPrice(OpenPriceInfo * pInfo,
                           void *          pContext,
                           int *           aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     };

     virtual int OpeningIndicator(OpeningIndicatorInfo * pInfo,
                                  void *                 pContext,
                                  int *                  aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     };

     virtual int OptionList(OptionListInfo * pInfo,
                            void *           pContext,
                            int *            aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     };

     virtual int OrderHistoryDates(OrderHistoryDatesInfo * pInfo,
				   void *                  pContext,
				   int *                   aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     };

     virtual int OrderReplay(OrderReplayInfo * pInfo,
                             void *            pContext,
                             int *             aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     };

     virtual int OtherReport(OrderReport * pReport,
                             void *        pContext,
                             int *         aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     };

     virtual int PasswordChange(PasswordChangeInfo * pInfo,
				void *               pContext,
				int *                aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     };

     virtual int Ping(PingInfo * pInfo,
		      void *     pContext,
		      int *      aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     };

     virtual int PnlReplay(PnlReplayInfo * pInfo,
                           void *          pContext,
                           int *           aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     };

     virtual int PnlUpdate(PnlInfo * pInfo,
                           void *    pContext,
                           int *     aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     };

     virtual int PositionExit(PositionExitInfo * pInfo,
			      void *             pContext,
			      int *              aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     };

     virtual int PriceIncrUpdate(PriceIncrInfo * pInfo,
                                 void *          pContext,
                                 int *           aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     };

     virtual int ProductRmsList(ProductRmsListInfo * pInfo,
				void *               pContext,
				int *                aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     };

     virtual int ProjectedSettlementPrice(ProjectedSettlementPriceInfo * pInfo,
					  void *                         pContext,
					  int *                          aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     };

     virtual int RefData(RefDataInfo * pInfo,
                         void *        pContext,
                         int *         aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     };

     virtual int RejectReport(OrderRejectReport * pReport,
                              void *              pContext,
                              int *               aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     };

     virtual int SettlementPrice(SettlementPriceInfo * pInfo,
                                 void *                pContext,
                                 int *                 aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     };

     virtual int SingleOrderReplay(SingleOrderReplayInfo * pInfo,
				   void *                  pContext,
				   int *                   aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     };

     virtual int SodUpdate(SodReport * pReport,
                           void *      pContext,
                           int *       aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     };

     virtual int StatusReport(OrderStatusReport * pReport,
                              void *              pContext,
                              int *               aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     };

     virtual int Strategy(StrategyInfo * pInfo,
                          void *         pContext,
                          int *          aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     };

     virtual int StrategyList(StrategyListInfo * pInfo,
                              void *             pContext,
                              int *              aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     };

     virtual int TradeReplay(TradeReplayInfo * pInfo,
                             void *            pContext,
                             int *             aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     };

     virtual int TradeRoute(TradeRouteInfo * pInfo,
			    void *           pContext,
			    int *            aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     };

     virtual int TradeRouteList(TradeRouteListInfo * pInfo,
				void *               pContext,
				int *                aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     };

     virtual int TriggerPulledReport(OrderTriggerPulledReport * pReport,
                                     void *                     pContext,
                                     int *                      aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     };

     virtual int TriggerReport(OrderTriggerReport * pReport,
                               void *               pContext,
                               int *                aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     };

     virtual int TradeCondition(TradeInfo * pInfo,
                                void *      pContext,
                                int *       aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     };

     virtual int TradeCorrectReport(OrderTradeCorrectReport * pReport,
                                    void *                    pContext,
                                    int *                     aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     };

     virtual int TradePrint(TradeInfo * pInfo,
                            void *      pContext,
                            int *       aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     };

     virtual int TradeVolume(TradeVolumeInfo * pInfo,
                             void *            pContext,
                             int *             aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     };

     virtual int User(UserInfo * pInfo,
		      void *     pContext,
		      int *      aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     };

     virtual int UserDefinedSpreadCreate(UserDefinedSpreadCreateInfo * pInfo,
					 void *                        pContext,
					 int *                         aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     };

     virtual int UserList(UserListInfo * pInfo,
			  void *         pContext,
			  int *          aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     };

     virtual int UserProfile(UserProfileInfo * pInfo,
			     void *            pContext,
			     int *             aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     };

     virtual int VolumeAtPrice(VolumeAtPriceInfo * pInfo,
			       void *              pContext,
			       int *               aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     };

     /*   ----------------------------------------------------------------   */

     virtual int Quote(QuoteReport * pReport,
		       void *        pContext,
		       int *         aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     };

     virtual int Quote(QuoteInfo * pInfo,
		       void *      pContext,
		       int *       aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     };

     virtual int QuoteReplay(QuoteReplayInfo * pInfo,
			     void *            pContext,
			     int *             aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     };

     /*   ----------------------------------------------------------------   */

     private :
     };

/*   =====================================================================   */

class REngine
     {
     public :
     REngine(REngineParams * pParams);
     virtual ~REngine();

     /*   ----------------------------------------------------------------   */

     int getAccounts(tsNCharcb * pStatus,
		     int *       aiCode);

     int getAdmCallbacks(AdmCallbacks * * apCallbacks,
                         int *            aiCode);

     int getAdminName(tsNCharcb * pAdminName,
                      int *       aiCode);

     int getAppName(tsNCharcb * pAppName,
                    int *       aiCode);

     int getAppVersion(tsNCharcb * pAppVersion,
                       int *       aiCode);

     int getCallbacks(RCallbacks * * apCallbacks,
                      int *          aiCode);

     int getContext(void * * apContext,
                    int *    aiCode);

     int getIhCnnctPt(tsNCharcb * pIhCnnctPt,
                      int *       aiCode);

     int getMdCnnctPt(tsNCharcb * pMdCnnctPt,
                      int *       aiCode);

     int getOmne(void * * apOmne,
                 int *    aiCode);

     int getOrderContext(tsNCharcb * pOrderNum,
                         void * *    apContext,
                         int *       aiCode);

     int getPassword(tsNCharcb * pPassword,
                     int *       aiCode);

     int getPendingInputSize(int                 iConnId,
			     unsigned long int * aulPendingBytes,
			     int *               aiCode);

     int getPnlCnnctPt(tsNCharcb * pPnlCnnctPt,
		       int *       aiCode);

     int getTsCnnctPt(tsNCharcb * pTsCnnctPt,
                      int *       aiCode);

     int getUser(tsNCharcb * pUser,
                 int *       aiCode);

     int getUserProfile(int    iConnId,
			void * pContext,
			int *  aiCode);

     static int getVersion(tsNCharcb * pVersion,
			   int *       aiCode);

     /*   ----------------------------------------------------------------   */

     int setContext(void * pContext,
                    int *  aiCode);

     int setOrderContext(tsNCharcb * pOrderNum,
                         void *      pContext,
                         int *       aiCode);

     /*   ----------------------------------------------------------------   */

     int addTimer(tsNCharcb * const                    pTimerName,
                  const OmneEngineSpace::OmneTimerCB * pTimerCB,
                  long                                 lFreq,
                  void *                               pContext,
                  int *                                aiCode);

     int cancelAllOrders(AccountInfo * pAccount,
			 tsNCharcb *   pEntryType,
			 tsNCharcb *   pTradingAlgorithm,
			 tsNCharcb *   pWindowName,
                         int *         aiCode);

     int cancelOrder(AccountInfo * pAccount,
                     tsNCharcb *   pOrderNum,
                     tsNCharcb *   pEntryType,
                     tsNCharcb *   pTradingAlgorithm,
                     tsNCharcb *   pUserMsg,
		     void *        pContext,
                     tsNCharcb *   pWindowName,
                     int *         aiCode);

     int cancelOrderList(AccountInfo * aoAccountArray,
			 tsNCharcb *   asOrderNumArray,
			 tsNCharcb *   asEntryTypeArray,
			 tsNCharcb *   asTradingAlgorithmArray,
			 tsNCharcb *   asUserMsgArray,
			 void * *      apContextArray,
			 int           iArrayLen,
			 tsNCharcb *   pWindowName,
			 int *         aiCode);

     int cancelQuoteList(QuoteCancelParams * pParams,
			 int                 iArrayLen,
			 int *               aiCode);

     int changePassword(tsNCharcb * pOldPassword,
			tsNCharcb * pNewPassword,
			int *       aiCode);

     int createUserDefinedSpread(AccountInfo * pAccount,
				 tsNCharcb *   pExchange,
				 tsNCharcb *   pTradeRoute,
				 tsNCharcb *   pEntryType,
				 tsNCharcb *   pStrategyType,
				 int           iNumLegs,
				 tsNCharcb *   asLegTickers,
				 int *         aiLegRatios,
				 void *        pContext,
				 int *         aiCode);

     int exitPosition(AccountInfo * pAccount,
		      tsNCharcb *   pExchange,
		      tsNCharcb *   pTicker,
		      tsNCharcb *   pEntryType,
		      tsNCharcb *   pTag,
		      tsNCharcb *   pTradingAlgorithm,
		      tsNCharcb *   pUserMsg, 
		      tsNCharcb *   pWindowName,
		      void *        pContext,
		      int *         aiCode);

     int getAuxRefData(tsNCharcb * pExchange,
		       tsNCharcb * pTicker,
		       int *       aiCode);

     int getEasyToBorrowList(void * pContext,
			     int *  aiCode);

     int getEnvironment(tsNCharcb * pKey,
			void *      pContext,
			int *       aiCode);

     int getEquityOptionStrategyList(tsNCharcb * pExchange,
				     tsNCharcb * pUnderlying,
				     tsNCharcb * pStrategyType,
				     tsNCharcb * pExpiration,
				     void *      pContext,
				     int *       aiCode);

     int getInstrumentByUnderlying(tsNCharcb * pUnderlying,
				   tsNCharcb * pExchange,
				   tsNCharcb * pExpiration,
				   void *      pContext,
				   int *       aiCode);

     int getOptionList(tsNCharcb * pExchange,
                       tsNCharcb * pProductCode,
                       tsNCharcb * pExpirationCCYYMM,
                       void *      pContext,
                       int *       aiCode);

     int getPriceIncrInfo(tsNCharcb * pExchange,
                          tsNCharcb * pTicker,
                          int *       aiCode);

     int getProductRmsInfo(AccountInfo * pAccount,
			   int *         aiCode);

     int getRefData(tsNCharcb * pExchange,
                    tsNCharcb * pTicker,
                    int *       aiCode);

     int getStrategyInfo(tsNCharcb * pExchange,
                         tsNCharcb * pTicker,
                         void *      pContext,
                         int *       aiCode);

     int getStrategyList(tsNCharcb * pExchange,
                         tsNCharcb * pProductCode,
                         tsNCharcb * pStrategyType,
                         tsNCharcb * pExpirationCCYYMM,
                         void *      pContext,
                         int *       aiCode);

     int getVolumeAtPrice(tsNCharcb * pExchange,
			  tsNCharcb * pTicker,
			  void *      pContext,
			  int *       aiCode);

     int isThereAnAggregator(int * aiCode);

     int linkOrders(AccountInfo * aoAccountArray,
		    tsNCharcb *   asOrderNumArray,
		    int           iArrayLen,
		    int *         aiCode);

     int listAgreements(bool   bAccepted,
			void * pContext,
			int *  aiCode);

     int listBinaryContracts(tsNCharcb * pExchange,
			     tsNCharcb * pProductCode,
			     void *      pContext,
			     int *       aiCode);

     int listEnvironments(void * pContext,
			  int *  aiCode);

     int listExchanges(void * pContext,
		       int *  aiCode);

     int listIbs(void * pContext,
		 int *  aiCode);

     int listOrderHistoryDates(void * pContext,
			       int *  aiCode);

     int listAssignedUsers(AccountInfo * pAccount,
			   void *        pContext,
			   int *         aiCode);

     int listTradeRoutes(void * pContext,
			 int *  aiCode);

     int listUsers(tsNCharcb * pIbId,
		   tsNCharcb * pUserType,
		   void *      pContext,
		   int *       aiCode);

     int lock(int * aiCode);

     int login(LoginParams * pParams,
               int *         aiCode);

     int loginRepository(tsNCharcb *  pEnvKey,
			 tsNCharcb *  pUser,
			 tsNCharcb *  pPassword,
			 tsNCharcb *  pCnnctPt,
			 RCallbacks * pCallbacks,
			 int *        aiCode);

     int logout(int * aiCode);

     int logoutRepository(int * aiCode);

     int modifyBracketTier(AccountInfo * pAccount,
			   tsNCharcb *   pOrderNum,
			   bool          bTarget,
			   int           iOldTickOffset,
			   int           iNewTickOffset,
			   void *        pContext,
			   int *         aiCode);

     int modifyOrder(ModifyLimitOrderParams * pParams,
                     int *                    aiCode);

     int modifyOrder(ModifyStopLimitOrderParams * pParams,
                     int *                        aiCode);

     int modifyOrder(ModifyStopMarketOrderParams * pParams,
                     int *                         aiCode);

     int modifyOrder(ModifyOrderParams * pParams,
                     int *               aiCode);

     int modifyOrderList(ModifyOrderParams * pParams,
			 int                 iArrayLen,
			 int *               aiCode);

     int modifyOrderRefData(AccountInfo * pAccount,
			    tsNCharcb *   pOrderNum,
			    tsNCharcb *   pUserMsg,
			    tsNCharcb *   pUserTag,
			    int *         aiCode);

     int ping(int    iConnId,
	      void * pContext,
	      int *  aiCode);

     int rebuildBook(tsNCharcb * pExchange,
                     tsNCharcb * pTicker,
                     int *       aiCode);

     int rebuildDboBook(tsNCharcb * pExchange,
			tsNCharcb * pTicker,
			bool        bPrice,
			double      dPrice,
			void *      pContext,
			int *       aiCode);

     int removeTimer(tsNCharcb * const pTimerName,
                     int *             aiCode);

     int replayAllOrders(AccountInfo * pAccount,
                         int           iStartSsboe,
                         int           iEndSsboe,
                         int *         aiCode);

     int replayBrackets(AccountInfo * pAccount,
			int *         aiCode);

     int replayExecutions(AccountInfo * pAccount,
                          int           iStartSsboe,
                          int           iEndSsboe,
                          int *         aiCode);

     int replayHistoricalOrders(AccountInfo * pAccount,
				tsNCharcb *   pDate,
				int *         aiCode);

     int replayOpenOrders(AccountInfo * pAccount,
                          int *         aiCode);

     int replayPnl(AccountInfo * pAccount,
                   int *         aiCode);

     int replayQuotes(AccountInfo * pAccount,
		      void *        pContext,
		      int *         aiCode);

     int replaySingleHistoricalOrder(AccountInfo * pAccount,
				     tsNCharcb *   pOrderNum,
				     tsNCharcb *   pDate,
				     void *        pContext,
				     int *         aiCode);

     int replaySingleOrder(AccountInfo * pAccount,
			   tsNCharcb *   pOrderNum,
			   void *        pContext,
			   int *         aiCode);

     int replayTrades(tsNCharcb * pExchange,
                      tsNCharcb * pTicker,
                      int         iStartSsboe,
                      int         iEndSsboe,
                      int *       aiCode);

     int resumeInput(int * aiCode);

     int searchInstrument(tsNCharcb *  pExchange,
			  SearchTerm * asTerms,
			  int          iNumTerms,
			  void *       pContext,
			  int *        aiCode);

     int sendBracketOrder(OrderParams * pEntry,
			  tsNCharcb *   pBracketType,
			  tsNCharcb *   pOrderOperationType,
			  bool          bTrailingStop,
			  int           iTrailByPriceId,
			  int           iTargetArrayLen,
			  int *         pTargetQty,
			  int *         pTargetTicks,
			  int           iStopArrayLen,
			  int *         pStopQty,
			  int *         pStopTicks,
			  int *         aiCode);

     int sendBracketOrder(OrderParams *   pEntry,
			  BracketParams * pBracketParams,
			  int *           aiCode);

     int sendOcaList(tsNCharcb *   pOcaType,
		     OrderParams * pParamsArray,
		     int           iArrayLen,
		     int *         aiCode);

     int sendOrder(LimitOrderParams * pParams,
                   int *              aiCode);

     int sendOrder(MarketOrderParams * pParams,
                   int *               aiCode);

     int sendOrder(StopLimitOrderParams * pParams,
                   int *                  aiCode);

     int sendOrder(StopMarketOrderParams * pParams,
                   int *                   aiCode);

     int sendOrderList(OrderParams * pParamsArray,
                       int           iArrayLen,
                       int *         aiCode);

     int setEnvironmentVariable(tsNCharcb * pKey,
				tsNCharcb * pVariable,
				tsNCharcb * pValue,
				int *       aiCode);

     int startTimer(tsNCharcb * const pTimerName,
                    int *             aiCode);

     int stopTimer(tsNCharcb * const pTimerName,
                   int *             aiCode);

     int submitQuoteList(QuoteParams * pParams,
			 int           iArrayLen,
			 int *         aiCode);

     int subscribe(tsNCharcb * pExchange,
                   tsNCharcb * pTicker,
                   int         iFlags,
                   int *       aiCode);

     int subscribeAutoLiquidate(AccountInfo * pAccount,
				int *         aiCode);

     int subscribeBracket(AccountInfo * pAccount,
			  int *         aiCode);

     int subscribeByUnderlying(tsNCharcb * pUnderlying,
			       tsNCharcb * pExchange,
			       tsNCharcb * pExpiration,
			       int         iFlags,
			       void *      pContext,
			       int *       aiCode);

     int subscribeDbo(tsNCharcb * pExchange,
		      tsNCharcb * pTicker,
		      bool        bPrice,
		      double      dPrice,
		      void *      pContext,
		      int *       aiCode);

     int subscribeEasyToBorrow(void * pContext,
			       int *  aiCode);

     int subscribeOrder(AccountInfo * pAccount,
			int *         aiCode);

     int subscribePnl(AccountInfo * pAccount,
		      int *         aiCode);

     int subscribeTradeRoute(tsNCharcb * pFcmId,
			     tsNCharcb * pIbId,
			     void *      pContext,
			     int *       aiCode);

     int subscribeUser(tsNCharcb * pIbId,
		       void *      pContext,
		       int *       aiCode);

     int suspendInput(int * aiCode);

     int tryLock(int * aiCode);

     int unlock(int * aiCode);

     int unsetEnvironmentVariable(tsNCharcb * pKey,
				  tsNCharcb * pVariable,
				  int *       aiCode);

     int unsubscribe(tsNCharcb * pExchange,
                     tsNCharcb * pTicker,
                     int *       aiCode);

     int unsubscribeAutoLiquidate(AccountInfo * pAccount,
				  int *         aiCode);

     int unsubscribeBracket(AccountInfo * pAccount,
			    int *         aiCode);

     int unsubscribeByUnderlying(tsNCharcb * pUnderlying,
				 tsNCharcb * pExchange,
				 tsNCharcb * pExpiration,
				 int *       aiCode);

     int unsubscribeDbo(tsNCharcb * pExchange,
			tsNCharcb * pTicker,
			bool        bPrice,
			double      dPrice,
			void *      pContext,
			int *       aiCode);

     int unsubscribeEasyToBorrow(int * aiCode);

     int unsubscribeOrder(AccountInfo * pAccount,
			  int *         aiCode);

     int unsubscribePnl(AccountInfo * pAccount,
			int *         aiCode);

     int unsubscribeTradeRoute(tsNCharcb * pFcmId,
			       tsNCharcb * pIbId,
			       int *       aiCode);

     int unsubscribeUser(tsNCharcb * pIbId,
			 int *       aiCode);

     /*   ----------------------------------------------------------------   */

     int replayBars(ReplayBarParams * pParams,
		    int *             aiCode);

     int subscribeBar(BarParams * pParams,
		      int *       aiCode);

     int unsubscribeBar(BarParams * pParams,
			int *       aiCode);

     /*   ----------------------------------------------------------------   */

     private :
     void * PRI_pIntHndl;
     };

/*   =====================================================================   */

}                                                        /* namespace RApi   */

#endif                                                   /* __RAPI_H_INCL    */

/*   =====================================================================   */
