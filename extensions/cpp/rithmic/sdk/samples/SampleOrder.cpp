/*   =====================================================================

Copyright (c) 2025 by Omnesys Technologies, Inc.  All rights reserved.

Warning :
        This Software Product is protected by copyright law and international
        treaties.  Unauthorized use, reproduction or distribution of this
        Software Product (including its documentation), or any portion of it,
        may result in severe civil and criminal penalties, and will be
        prosecuted to the maximum extent possible under the law.

        Omnesys Technologies, Inc. will compensate individuals providing
        admissible evidence of any unauthorized use, reproduction, distribution
        or redistribution of this Software Product by any person, company or 
        organization.

This Software Product is licensed strictly in accordance with a separate
Software System License Agreement, granted by Omnesys Technologies, Inc., which
contains restrictions on use, reverse engineering, disclosure, confidentiality 
and other matters.

     =====================================================================   */
/*   =====================================================================
     Compile/link commands for linux and darwin using R | API+.  These should 
     work if your pwd is the ./samples directory.  You may need to change the 
     name of the RApi library if you are using one of the library variants, 
     like R | API or R | Diamond API.

     64-bit linux (2.6.32 kernel) :

     g++ -O3 -DLINUX -D_REENTRANT -Wall -Wno-sign-compare -Wno-write-strings -Wpointer-arith -Winline -Wno-deprecated -fno-strict-aliasing -I../include -o SampleOrder ../samples/SampleOrder.cpp -L../linux-gnu-2.6.32-x86_64/lib -lRApiPlus-optimize -lOmneStreamEngine-optimize -lOmneChannel-optimize -lOmneEngine-optimize -l_api-optimize -l_apipoll-stubs-optimize -l_kit-optimize -lssl -lcrypto -L/usr/lib64 -lz -L/usr/kerberos/lib -lkrb5 -lk5crypto -lcom_err -lresolv -lm -lpthread -lrt

     64-bit linux (3.10.0 kernel) :

     g++ -O3 -DLINUX -D_REENTRANT -Wall -Wno-sign-compare -Wno-write-strings -Wpointer-arith -Winline -Wno-deprecated -fno-strict-aliasing -I../include -o SampleOrder ../samples/SampleOrder.cpp -L../linux-gnu-3.10.0-x86_64/lib -lRApiPlus-optimize -lOmneStreamEngine-optimize -lOmneChannel-optimize -lOmneEngine-optimize -l_api-optimize -l_apipoll-stubs-optimize -l_kit-optimize -lssl -lcrypto -L/usr/lib64 -lz -lpthread -lrt -ldl

     64-bit linux (4.18 kernel) :

     g++ -O3 -DLINUX -D_REENTRANT -Wall -Wno-sign-compare -Wno-write-strings -Wpointer-arith -Winline -Wno-deprecated -fno-strict-aliasing -I../include -o SampleOrder ../samples/SampleOrder.cpp -L../linux-gnu-4.18-x86_64/lib -lRApiPlus-optimize -lOmneStreamEngine-optimize -lOmneChannel-optimize -lOmneEngine-optimize -l_api-optimize -l_apipoll-stubs-optimize -l_kit-optimize -lssl -lcrypto -L/usr/lib64 -lz -lpthread -lrt -ldl

     64-bit darwin :

     g++ -O3 -D_REENTRANT -Wall -Wno-sign-compare -fno-strict-aliasing -Wpointer-arith -Winline -Wno-deprecated -Wno-write-strings -I../include -o ./SampleOrder ../samples/SampleOrder.cpp -L../darwin-10/lib -lRApiPlus-optimize -lOmneStreamEngine-optimize -lOmneChannel-optimize -lOmneEngine-optimize -l_api-optimize -l_apipoll-stubs-optimize -l_kit-optimize -lssl -lcrypto -L/usr/lib -lz -Wl,-search_paths_first

     64-bit darwin arm64 :

     g++ -O3 -D_REENTRANT -Wall -Wno-sign-compare -fno-strict-aliasing -Wpointer-arith -Winline -Wno-deprecated -Wno-write-strings -I../include -o ./SampleOrder ../samples/SampleOrder.cpp -L../darwin-20.6-arm64/lib -lRApiPlus-optimize -lOmneStreamEngine-optimize -lOmneChannel-optimize -lOmneEngine-optimize -l_api-optimize -l_apipoll-stubs-optimize -l_kit-optimize -lssl -lcrypto -L/usr/lib -lz

     =====================================================================   */

#include "RApiPlus.h"

#include <stdio.h>
#include <string.h>
#ifndef WinOS
#include <unistd.h>
#else
#include <Windows.h>
#endif

#define GOOD 0
#define BAD  1

using namespace RApi;

/*   =====================================================================   */
/*   Use global variables to share between the callback thread and main      */
/*   thread.  The booleans are a primitive method of signaling state         */
/*   between the two threads.                                                */

bool        g_bTsLoginComplete = false;
bool        g_bRcvdAccount     = false;
bool        g_bRcvdPriceIncr   = false;
bool        g_bRcvdTradeRoutes = false;
bool        g_bDone            = false;

int         g_iToExchSsboe     = 0;
int         g_iToExchUsecs     = 0;
int         g_iFromExchSsboe   = 0;
int         g_iFromExchUsecs   = 0;

const int   g_iMAX_LEN         = 256;
char        g_cAccountId[g_iMAX_LEN];
char        g_cFcmId[g_iMAX_LEN];
char        g_cIbId[g_iMAX_LEN];
char        g_cExchange[g_iMAX_LEN];
char        g_cTradeRoute[g_iMAX_LEN];
AccountInfo g_oAccount;
tsNCharcb   g_sExchange;
tsNCharcb   g_sTradeRoute      = {(char *)NULL, 0};

REngine *   g_pEngine;

int main(int      argc,
         char * * argv,
         char * * envp);

/*   =====================================================================   */
/*                          class declarations                               */
/*   =====================================================================   */

class MyAdmCallbacks: public AdmCallbacks
     {
     public :
     MyAdmCallbacks()  {};
     ~MyAdmCallbacks() {};

     /*   ----------------------------------------------------------------   */

     virtual int Alert(AlertInfo * pInfo,
                       void *      pContext,
                       int *       aiCode);
     };

/*   =====================================================================   */

class MyCallbacks: public RCallbacks
     {
     public :
     MyCallbacks()  {};
     ~MyCallbacks() {};

     /*   ----------------------------------------------------------------   */

     virtual int Alert(AlertInfo * pInfo,
                       void *      pContext,
                       int *       aiCode);

     /*   ----------------------------------------------------------------   */

     virtual int AskQuote(AskInfo * pInfo,
                          void *    pContext,
                          int *     aiCode);

     virtual int BestAskQuote(AskInfo * pInfo,
                              void *    pContext,
                              int *     aiCode);

     virtual int BestBidAskQuote(BidInfo * pBid,
				 AskInfo * pAsk,
				 void *    pContext,
				 int *     aiCode);

     virtual int BestBidQuote(BidInfo * pInfo,
                              void *    pContext,
                              int *     aiCode);

     virtual int BidQuote(BidInfo * pInfo,
                          void *    pContext,
                          int *     aiCode);

     virtual int BinaryContractList(BinaryContractListInfo * pInfo,
				    void *                   pContext,
				    int *                    aiCode);

     virtual int ClosePrice(ClosePriceInfo * pInfo,
                            void *           pContext,
                            int *            aiCode);

     virtual int ClosingIndicator(ClosingIndicatorInfo * pInfo,
                                  void *                 pContext,
                                  int *                  aiCode);

     virtual int EndQuote(EndQuoteInfo * pInfo,
                          void *         pContext,
                          int *          aiCode);

     virtual int EquityOptionStrategyList(EquityOptionStrategyListInfo * pInfo,
					  void *                         pContext,
					  int *                          aiCode);

     virtual int HighPrice(HighPriceInfo * pInfo,
                           void *          pContext,
                           int *           aiCode);

     virtual int InstrumentByUnderlying(InstrumentByUnderlyingInfo * pInfo,
					void *                       pContext,
					int *                        aiCode);

     virtual int InstrumentSearch(InstrumentSearchInfo * pInfo,
				  void *                 pContext,
				  int *                  aiCode);

     virtual int LimitOrderBook(LimitOrderBookInfo * pInfo,
                                void *               pContext,
                                int *                aiCode);

     virtual int LowPrice(LowPriceInfo * pInfo,
                          void *         pContext,
                          int *          aiCode);

     virtual int MarketMode(MarketModeInfo * pInfo,
                            void *           pContext,
                            int *            aiCode);

     virtual int OpenInterest(OpenInterestInfo * pInfo,
			      void *             pContext,
			      int *              aiCode);

     virtual int OpenPrice(OpenPriceInfo * pInfo,
                           void *          pContext,
                           int *           aiCode);

     virtual int OpeningIndicator(OpeningIndicatorInfo * pInfo,
				  void *                 pContext,
				  int *                  aiCode);

     virtual int OptionList(OptionListInfo * pInfo,
                            void *           pContext,
                            int *            aiCode);

     virtual int RefData(RefDataInfo * pInfo,
                         void *        pContext,
                         int *         aiCode);

     virtual int SettlementPrice(SettlementPriceInfo * pInfo,
                                 void *                pContext,
                                 int *                 aiCode);

     virtual int Strategy(StrategyInfo * pInfo,
			  void *         pContext,
			  int *          aiCode);

     virtual int StrategyList(StrategyListInfo * pInfo,
			      void *             pContext,
			      int *              aiCode);

     virtual int TradeCondition(TradeInfo * pInfo,
                                void *      pContext,
                                int *       aiCode);

     virtual int TradePrint(TradeInfo * pInfo,
                            void *      pContext,
                            int *       aiCode);

     virtual int TradeReplay(TradeReplayInfo * pInfo,
			     void *            pContext,
			     int *             aiCode);

     virtual int TradeRoute(TradeRouteInfo * pInfo,
			    void *           pContext,
			    int *            aiCode);

     virtual int TradeRouteList(TradeRouteListInfo * pInfo,
				void *               pContext,
				int *                aiCode);

     virtual int TradeVolume(TradeVolumeInfo * pInfo,
                             void *            pContext,
                             int *             aiCode);

     /*   ----------------------------------------------------------------   */

     virtual int Bar(BarInfo * pInfo,
		     void *    pContext,
		     int *     aiCode);

     virtual int BarReplay(BarReplayInfo * pInfo,
			   void *          pContext,
			   int *           aiCode);

     /*   ----------------------------------------------------------------   */

     virtual int AccountList(AccountListInfo * pInfo,
                             void *            pContext,
                             int *             aiCode);

     virtual int PasswordChange(PasswordChangeInfo * pInfo,
				void *               pContext,
				int *                aiCode);

     /*   ----------------------------------------------------------------   */

     virtual int ExchangeList(ExchangeListInfo * pInfo,
			      void *             pContext,
			      int *              aiCode);

     virtual int ExecutionReplay(ExecutionReplayInfo * pInfo,
                                 void *                pContext,
                                 int *                 aiCode);

     virtual int LineUpdate(LineInfo * pInfo,
                            void *     pContext,
                            int *      aiCode);

     virtual int OpenOrderReplay(OrderReplayInfo * pInfo,
                                 void *            pContext,
                                 int *             aiCode);

     virtual int OrderReplay(OrderReplayInfo * pInfo,
                             void *            pContext,
                             int *             aiCode);

     virtual int PnlReplay(PnlReplayInfo * pInfo,
                           void *          pContext,
                           int *           aiCode);

     virtual int PnlUpdate(PnlInfo * pInfo,
                           void *    pContext,
                           int *     aiCode);

     virtual int PriceIncrUpdate(PriceIncrInfo * pInfo,
                                 void *          pContext,
                                 int *           aiCode);

     virtual int ProductRmsList(ProductRmsListInfo * pInfo,
				void *               pContext,
				int *                aiCode);

     virtual int SingleOrderReplay(SingleOrderReplayInfo * pInfo,
				   void *                  pContext,
				   int *                   aiCode);

     /*   ----------------------------------------------------------------   */

     virtual int BustReport(OrderBustReport * pReport,
                            void *            pContext,
                            int *             aiCode);

     virtual int CancelReport(OrderCancelReport * pReport,
                              void *              pContext,
                              int *               aiCode);

     virtual int FailureReport(OrderFailureReport * pReport,
                               void *               pContext,
                               int *                aiCode);

     virtual int FillReport(OrderFillReport * pReport,
                            void *            pContext,
                            int *             aiCode);

     virtual int ModifyReport(OrderModifyReport * pReport,
                              void *              pContext,
                              int *               aiCode);

     virtual int NotCancelledReport(OrderNotCancelledReport * pReport,
                                    void *                    pContext,
                                    int *                     aiCode);

     virtual int NotModifiedReport(OrderNotModifiedReport * pReport,
                                   void *                   pContext,
                                   int *                    aiCode);

     virtual int RejectReport(OrderRejectReport * pReport,
                              void *              pContext,
                              int *               aiCode);

     virtual int StatusReport(OrderStatusReport * pReport,
                              void *              pContext,
                              int *               aiCode);

     virtual int TradeCorrectReport(OrderTradeCorrectReport * pReport,
                                    void *                    pContext,
                                    int *                     aiCode);

     virtual int TriggerPulledReport(OrderTriggerPulledReport * pReport,
                                     void *                     pContext,
                                     int *                      aiCode);

     virtual int TriggerReport(OrderTriggerReport * pReport,
                               void *              pContext,
                               int *               aiCode);

     virtual int OtherReport(OrderReport * pReport,
                             void *        pContext,
                             int *         aiCode);

     /*   ----------------------------------------------------------------   */

     virtual int SodUpdate(SodReport * pReport,
                           void *      pContext,
                           int *       aiCode);

     /*   ----------------------------------------------------------------   */

     virtual int Quote(QuoteReport * pReport,
		       void *        pContext,
		       int *         aiCode);

     /*   ----------------------------------------------------------------   */

     private :
     };

/*   =====================================================================   */
/*                          class definitions                                */
/*   =====================================================================   */

int MyAdmCallbacks::Alert(AlertInfo * pInfo,
                          void *      pContext,
                          int *       aiCode)
     {
     int iIgnored;

     /*   ----------------------------------------------------------------   */

     printf("\n\n");
     if (!pInfo -> dump(&iIgnored))
          {
          printf("error in pInfo -> dump : %d", iIgnored);
          }

     /*   ----------------------------------------------------------------   */

     *aiCode = API_OK;
     return (OK);
     }

/*   =====================================================================   */

int MyCallbacks::AccountList(AccountListInfo * pInfo,
                             void *            pContext,
                             int *             aiCode)
     {
     AccountInfo * pAccount;
     int iCode;
     int iIgnored;

     /*   ----------------------------------------------------------------   */

     printf("\n\n");
     if (!pInfo -> dump(&iIgnored))
          {
          printf("error in pInfo -> dump : %d", iIgnored);
          }

     /*   ----------------------------------------------------------------   */

     if (pInfo -> iArrayLen > 0)
          {
          pAccount = &pInfo -> asAccountInfoArray[0];

          /* copy the first account */
          if ((pAccount -> sAccountId.iDataLen > g_iMAX_LEN) ||
              (pAccount -> sFcmId.iDataLen     > g_iMAX_LEN) ||
              (pAccount -> sIbId.iDataLen      > g_iMAX_LEN))
               {
               printf("one or more of the char arrays is too small.\n");
               }
          else
               {
               memcpy(&g_cAccountId, 
                      pAccount -> sAccountId.pData,
                      pAccount -> sAccountId.iDataLen);

               g_oAccount.sAccountId.pData    = g_cAccountId;
               g_oAccount.sAccountId.iDataLen = pAccount -> sAccountId.iDataLen;

               /*   ------------------------------------------------------   */

               memcpy(&g_cFcmId, 
                      pAccount -> sFcmId.pData,
                      pAccount -> sFcmId.iDataLen);
               g_oAccount.sFcmId.pData    = g_cFcmId;
               g_oAccount.sFcmId.iDataLen = pAccount -> sFcmId.iDataLen;

               /*   ------------------------------------------------------   */

               memcpy(&g_cIbId, 
                      pAccount -> sIbId.pData,
                      pAccount -> sIbId.iDataLen);
               g_oAccount.sIbId.pData    = g_cIbId;
               g_oAccount.sIbId.iDataLen = pAccount -> sIbId.iDataLen;

               if (!g_pEngine -> subscribeOrder(&g_oAccount, &iCode))
                    {
                    printf("REngine::subscribeOrder() error : %d\n", iCode);
                    }

               g_bRcvdAccount = true;
               }
          }

     /*   ----------------------------------------------------------------   */

     *aiCode = API_OK;
     return (OK);
     }

/*   =====================================================================   */

int MyCallbacks::PasswordChange(PasswordChangeInfo * pInfo,
				void *               pContext,
				int *                aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     }

/*   =====================================================================   */

int MyCallbacks::Alert(AlertInfo * pInfo,
                       void *      pContext,
                       int *       aiCode)
     {
     int iIgnored;

     /*   ----------------------------------------------------------------   */

     printf("\n\n");
     if (!pInfo -> dump(&iIgnored))
          {
          printf("error in pInfo -> dump : %d", iIgnored);
          }

     /*   ----------------------------------------------------------------   */
     /*   signal when the login to the trading system is complete */

     if (pInfo -> iAlertType == ALERT_LOGIN_COMPLETE &&
         pInfo -> iConnectionId == TRADING_SYSTEM_CONNECTION_ID)
          {
          g_bTsLoginComplete = true;
          }

     /*   ----------------------------------------------------------------   */

     *aiCode = API_OK;
     return (OK);
     }

/*   =====================================================================   */

int MyCallbacks::ExchangeList(ExchangeListInfo * pInfo,
			      void *             pContext,
			      int *              aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     }

/*   =====================================================================   */

int MyCallbacks::ExecutionReplay(ExecutionReplayInfo * pInfo,
                                 void *                pContext,
                                 int *                 aiCode)
     {
     *aiCode = API_OK;
     return(OK);
     }


/*   =====================================================================   */

int MyCallbacks::LineUpdate(LineInfo * pInfo,
                            void *     pContext,
                            int *      aiCode)
     {
     tsNCharcb sOrderSentToExch = {(char *)"order sent to exch",
                                   (int)strlen("order sent to exch")};
     int iIgnored;

     /*   ----------------------------------------------------------------   */

     printf("\n\n");
     if (!pInfo -> dump(&iIgnored))
          {
          printf("error in pInfo -> dump : %d", iIgnored);
          }

     /*   ----------------------------------------------------------------   */
     /*   record when the order was sent to the exchange... */

     if (pInfo -> sStatus.iDataLen == sOrderSentToExch.iDataLen &&
	 memcmp(pInfo -> sStatus.pData, 
		sOrderSentToExch.pData, 
		sOrderSentToExch.iDataLen) == 0)
	  {
	  g_iToExchSsboe = pInfo -> iSsboe;
	  g_iToExchUsecs = pInfo -> iUsecs;
	  }

     /*   ----------------------------------------------------------------   */
     /*   if there's a completion reason, the order is complete... */

     if (pInfo -> sCompletionReason.pData)
          {
          g_bDone = true;
          }

     /*   ----------------------------------------------------------------   */

     *aiCode = API_OK;
     return(OK);
     }


/*   =====================================================================   */

int MyCallbacks::OpenOrderReplay(OrderReplayInfo * pInfo,
                                 void *            pContext,
                                 int *             aiCode)
     {
     *aiCode = API_OK;
     return(OK);
     }


/*   =====================================================================   */

int MyCallbacks::OrderReplay(OrderReplayInfo * pInfo,
                             void *            pContext,
                             int *             aiCode)
     {
     *aiCode = API_OK;
     return(OK);
     }


/*   =====================================================================   */

int MyCallbacks::PnlReplay(PnlReplayInfo * pInfo,
                           void *          pContext,
                           int *           aiCode)
     {
     *aiCode = API_OK;
     return(OK);
     }

/*   =====================================================================   */

int MyCallbacks::PnlUpdate(PnlInfo * pInfo,
                           void *    pContext,
                           int *     aiCode)
     {
     *aiCode = API_OK;
     return(OK);
     }

/*   =====================================================================   */

int MyCallbacks::PriceIncrUpdate(PriceIncrInfo * pInfo,
                                 void *          pContext,
                                 int *           aiCode)
     {
     int iIgnored;

     /*   ----------------------------------------------------------------   */

     printf("\n\n");
     if (!pInfo -> dump(&iIgnored))
          {
          printf("error in pInfo -> dump : %d", iIgnored);
          }

     g_bRcvdPriceIncr = true;

     /*   ----------------------------------------------------------------   */

     *aiCode = API_OK;
     return(OK);
     }

/*   =====================================================================   */

int MyCallbacks::ProductRmsList(ProductRmsListInfo * pInfo,
				void *               pContext,
				int *                aiCode)
     {
     *aiCode = API_OK;
     return(OK);
     }

/*   =====================================================================   */

int MyCallbacks::SingleOrderReplay(SingleOrderReplayInfo * pInfo,
				   void *                  pContext,
				   int *                   aiCode)
     {
     *aiCode = API_OK;
     return(OK);
     }

/*   =====================================================================   */

int MyCallbacks::BustReport(OrderBustReport * pReport,
                            void *            pContext,
                            int *             aiCode)
     {
     int iIgnored;

     /*   ----------------------------------------------------------------   */

     printf("\n\nReceived Bust Report\n");
     pReport -> dump(&iIgnored);

     /*   ----------------------------------------------------------------   */

     *aiCode = API_OK;
     return(OK);
     }

/*   =====================================================================   */

int MyCallbacks::CancelReport(OrderCancelReport * pReport,
                              void *              pContext,
                              int *               aiCode)
     {
     int iIgnored;

     /*   ----------------------------------------------------------------   */

     printf("\n\nReceived Cancel Report\n");
     pReport -> dump(&iIgnored);

     /*   ----------------------------------------------------------------   */

     *aiCode = API_OK;
     return(OK);
     }

/*   =====================================================================   */

int MyCallbacks::FailureReport(OrderFailureReport * pReport,
                               void *               pContext,
                               int *                aiCode)
     {
     int iIgnored;

     /*   ----------------------------------------------------------------   */

     printf("\n\nReceived Failure Report\n");
     pReport -> dump(&iIgnored);

     /*   ----------------------------------------------------------------   */

     *aiCode = API_OK;
     return(OK);
     }

/*   =====================================================================   */

int MyCallbacks::FillReport(OrderFillReport * pReport,
                            void *            pContext,
                            int *             aiCode)
     {
     int iIgnored;

     /*   ----------------------------------------------------------------   */

     printf("\n\nReceived Fill Report\n");
     pReport -> dump(&iIgnored);

     /*   ----------------------------------------------------------------   */

     *aiCode = API_OK;
     return(OK);
     }

/*   =====================================================================   */

int MyCallbacks::ModifyReport(OrderModifyReport * pReport,
                              void *              pContext,
                              int *               aiCode)
     {
     int iIgnored;

     /*   ----------------------------------------------------------------   */

     printf("\n\nReceived Modify Report\n");
     pReport -> dump(&iIgnored);

     /*   ----------------------------------------------------------------   */

     *aiCode = API_OK;
     return(OK);
     }

/*   =====================================================================   */

int MyCallbacks::NotCancelledReport(OrderNotCancelledReport * pReport,
                                    void *                    pContext,
                                    int *                     aiCode)
     {
     int iIgnored;

     /*   ----------------------------------------------------------------   */

     printf("\n\nReceived NotCancelled Report\n");
     pReport -> dump(&iIgnored);

     /*   ----------------------------------------------------------------   */

     *aiCode = API_OK;
     return(OK);
     }

/*   =====================================================================   */

int MyCallbacks::NotModifiedReport(OrderNotModifiedReport * pReport,
                                   void *                   pContext,
                                   int *                    aiCode)
     {
     int iIgnored;

     /*   ----------------------------------------------------------------   */

     printf("\n\nReceived NotModified Report\n");
     pReport -> dump(&iIgnored);

     /*   ----------------------------------------------------------------   */

     *aiCode = API_OK;
     return(OK);
     }

/*   =====================================================================   */

int MyCallbacks::RejectReport(OrderRejectReport * pReport,
                              void *              pContext,
                              int *               aiCode)
     {
     int iIgnored;

     /*   ----------------------------------------------------------------   */

     printf("\n\nReceived Reject Report\n");
     pReport -> dump(&iIgnored);

     /*   ----------------------------------------------------------------   */
     /*   record when the order returned from the exchange... */

     g_iFromExchSsboe = pReport -> iSsboe;
     g_iFromExchUsecs = pReport -> iUsecs;

     /*   ----------------------------------------------------------------   */

     *aiCode = API_OK;
     return(OK);
     }

/*   =====================================================================   */

int MyCallbacks::StatusReport(OrderStatusReport * pReport,
                              void *              pContext,
                              int *               aiCode)
     {
     int iIgnored;

     /*   ----------------------------------------------------------------   */

     printf("\n\nReceived Status Report\n");
     pReport -> dump(&iIgnored);

     /*   ----------------------------------------------------------------   */
     /*   record when the order returned from the exchange... */

     g_iFromExchSsboe = pReport -> iSsboe;
     g_iFromExchUsecs = pReport -> iUsecs;

     /*   ----------------------------------------------------------------   */

     *aiCode = API_OK;
     return(OK);
     }

/*   =====================================================================   */

int MyCallbacks::TradeCorrectReport(OrderTradeCorrectReport * pReport,
                                    void *                    pContext,
                                    int *                     aiCode)
     {
     int iIgnored;

     /*   ----------------------------------------------------------------   */

     printf("\n\nReceived Trade Correct Report\n");
     pReport -> dump(&iIgnored);

     /*   ----------------------------------------------------------------   */

     *aiCode = API_OK;
     return(OK);
     }

/*   =====================================================================   */

int MyCallbacks::TriggerPulledReport(OrderTriggerPulledReport * pReport,
                                     void *                     pContext,
                                     int *                      aiCode)
     {
     int iIgnored;

     /*   ----------------------------------------------------------------   */

     printf("\n\nReceived Trigger Pulled Report\n");
     pReport -> dump(&iIgnored);

     /*   ----------------------------------------------------------------   */

     *aiCode = API_OK;
     return(OK);
     }

/*   =====================================================================   */

int MyCallbacks::TriggerReport(OrderTriggerReport * pReport,
                               void *               pContext,
                               int *                aiCode)
     {
     int iIgnored;

     /*   ----------------------------------------------------------------   */

     printf("\n\nReceived Trigger Report\n");
     pReport -> dump(&iIgnored);

     /*   ----------------------------------------------------------------   */

     *aiCode = API_OK;
     return(OK);
     }

/*   =====================================================================   */

int MyCallbacks::OtherReport(OrderReport * pReport,
                             void *        pContext,
                             int *         aiCode)
     {
     int iIgnored;

     /*   ----------------------------------------------------------------   */

     printf("\n\nReceived Other Report\n");
     pReport -> dump(&iIgnored);

     /*   ----------------------------------------------------------------   */

     *aiCode = API_OK;
     return(OK);
     }

/*   =====================================================================   */

int MyCallbacks::SodUpdate(SodReport * pReport,
                           void *      pContext,
                           int *       aiCode)
     {
     int iIgnored;

     /*   ----------------------------------------------------------------   */

     printf("\n\nReceived Sod Report\n");
     pReport -> dump(&iIgnored);

     /*   ----------------------------------------------------------------   */

     *aiCode = API_OK;
     return(OK);
     }

/*   =====================================================================   */

int MyCallbacks::Quote(QuoteReport * pReport,
		       void *        pContext,
		       int *         aiCode)
     {
     *aiCode = API_OK;
     return(OK);
     }

/*   =====================================================================   */

int MyCallbacks::AskQuote(AskInfo * pInfo,
                          void *    pContext,
                          int *     aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     }

/*   =====================================================================   */

int MyCallbacks::BestAskQuote(AskInfo * pInfo,
                              void *    pContext,
                              int *     aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     }

/*   =====================================================================   */

int MyCallbacks::BestBidAskQuote(BidInfo * pBid,
				 AskInfo * pAsk,
				 void *    pContext,
				 int *     aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     }

/*   =====================================================================   */

int MyCallbacks::BestBidQuote(BidInfo * pInfo,
                              void *    pContext,
                              int *     aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     }

/*   =====================================================================   */

int MyCallbacks::BidQuote(BidInfo * pInfo,
                          void *    pContext,
                          int *     aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     }

/*   =====================================================================   */

int MyCallbacks::BinaryContractList(BinaryContractListInfo * pInfo,
				    void *                   pContext,
				    int *                    aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     }

/*   =====================================================================   */

int MyCallbacks::ClosePrice(ClosePriceInfo * pInfo,
                            void *           pContext,
                            int *            aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     }

/*   =====================================================================   */

int MyCallbacks::ClosingIndicator(ClosingIndicatorInfo * pInfo,
                                  void *                 pContext,
                                  int *                  aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     }

/*   =====================================================================   */

int MyCallbacks::EndQuote(EndQuoteInfo * pInfo,
			  void *         pContext,
			  int *          aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     }

/*   =====================================================================   */

int MyCallbacks::EquityOptionStrategyList(EquityOptionStrategyListInfo * pInfo,
					  void *                         pContext,
					  int *                          aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     }

/*   =====================================================================   */

int MyCallbacks::HighPrice(HighPriceInfo * pInfo,
                           void *          pContext,
                           int *           aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     }

/*   =====================================================================   */

int MyCallbacks::InstrumentByUnderlying(InstrumentByUnderlyingInfo * pInfo,
					void *                       pContext,
					int *                        aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     }

/*   =====================================================================   */

int MyCallbacks::InstrumentSearch(InstrumentSearchInfo * pInfo,
				  void *                 pContext,
				  int *                  aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     }

/*   =====================================================================   */

int MyCallbacks::LimitOrderBook(LimitOrderBookInfo * pInfo,
                                void *               pContext,
                                int *                aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     }

/*   =====================================================================   */

int MyCallbacks::LowPrice(LowPriceInfo * pInfo,
                          void *         pContext,
                          int *          aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     }

/*   =====================================================================   */

int MyCallbacks::MarketMode(MarketModeInfo * pInfo,
                            void *           pContext,
                            int *            aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     }

/*   =====================================================================   */

int MyCallbacks::OpenInterest(OpenInterestInfo * pInfo,
			      void *             pContext,
			      int *              aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     }

/*   =====================================================================   */

int MyCallbacks::OpenPrice(OpenPriceInfo * pInfo,
                           void *          pContext,
                           int *           aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     }

/*   =====================================================================   */

int MyCallbacks::OpeningIndicator(OpeningIndicatorInfo * pInfo,
				  void *                 pContext,
				  int *                  aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     }

/*   =====================================================================   */

int MyCallbacks::OptionList(OptionListInfo * pInfo,
			    void *           pContext,
			    int *            aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     }

/*   =====================================================================   */

int MyCallbacks::RefData(RefDataInfo * pInfo,
                         void *        pContext,
                         int *         aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     }

/*   =====================================================================   */

int MyCallbacks::SettlementPrice(SettlementPriceInfo * pInfo,
                                 void *                pContext,
                                 int *                 aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     }

/*   =====================================================================   */

int MyCallbacks::Strategy(StrategyInfo * pInfo,
			  void *         pContext,
			  int *          aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     }

/*   =====================================================================   */

int MyCallbacks::StrategyList(StrategyListInfo * pInfo,
			      void *             pContext,
			      int *              aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     }

/*   =====================================================================   */

int MyCallbacks::TradeCondition(TradeInfo * pInfo,
                                void *      pContext,
                                int *       aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     }

/*   =====================================================================   */

int MyCallbacks::TradePrint(TradeInfo * pInfo,
                            void *      pContext,
                            int *       aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     }

/*   =====================================================================   */

int MyCallbacks::TradeReplay(TradeReplayInfo * pInfo,
			     void *            pContext,
			     int *             aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     }

/*   =====================================================================   */

int MyCallbacks::TradeRoute(TradeRouteInfo * pInfo,
			    void *           pContext,
			    int *            aiCode)
     {
     *aiCode = API_OK;
     return(OK);
     }

/*   =====================================================================   */

int MyCallbacks::TradeRouteList(TradeRouteListInfo * pInfo,
				void *               pContext,
				int *                aiCode)
     {
     printf("We are looking for a trade route for : %*.*s::%*.*s::%*.*s\n",
	    g_oAccount.sFcmId.iDataLen,
	    g_oAccount.sFcmId.iDataLen,
	    g_oAccount.sFcmId.pData,
	    g_oAccount.sIbId.iDataLen,
	    g_oAccount.sIbId.iDataLen,
	    g_oAccount.sIbId.pData,
	    g_sExchange.iDataLen,
	    g_sExchange.iDataLen,
	    g_sExchange.pData);

     tsNCharcb sFcmId;
     tsNCharcb sIbId;
     tsNCharcb sExchange;
     tsNCharcb sTradeRoute;
     tsNCharcb sStatus;

     for (int i = 0; i < pInfo -> iArrayLen; i++)
	  {
	  sFcmId      = pInfo -> asTradeRouteInfoArray[i].sFcmId;
	  sIbId       = pInfo -> asTradeRouteInfoArray[i].sIbId;
	  sExchange   = pInfo -> asTradeRouteInfoArray[i].sExchange;
	  sTradeRoute = pInfo -> asTradeRouteInfoArray[i].sTradeRoute;
	  sStatus     = pInfo -> asTradeRouteInfoArray[i].sStatus;

	  printf("%*.*s::%*.*s::%*.*s::%*.*s::%*.*s\n",
		 sFcmId.iDataLen,
		 sFcmId.iDataLen,
		 sFcmId.pData,
		 sIbId.iDataLen,
		 sIbId.iDataLen,
		 sIbId.pData,
		 sExchange.iDataLen,
		 sExchange.iDataLen,
		 sExchange.pData,
		 sTradeRoute.iDataLen,
		 sTradeRoute.iDataLen,
		 sTradeRoute.pData,
		 sStatus.iDataLen,
		 sStatus.iDataLen,
		 sStatus.pData);

	  /* use first trade route where fcm/ib/exch matches, and status is "UP" */
	  if (g_oAccount.sFcmId.iDataLen == sFcmId.iDataLen &&
	      (memcmp(g_oAccount.sFcmId.pData, 
		      sFcmId.pData, 
		      g_oAccount.sFcmId.iDataLen) == 0) &&

	      g_oAccount.sIbId.iDataLen == sIbId.iDataLen &&
	      (memcmp(g_oAccount.sIbId.pData, 
		      sIbId.pData, 
		      g_oAccount.sIbId.iDataLen) == 0) &&

	      g_sExchange.iDataLen == sExchange.iDataLen &&
	      (memcmp(g_sExchange.pData, 
		      sExchange.pData, 
		      g_sExchange.iDataLen) == 0) &&

	      sTRADE_ROUTE_STATUS_UP.iDataLen == sStatus.iDataLen &&
	      (memcmp(sTRADE_ROUTE_STATUS_UP.pData, 
		      sStatus.pData, 
		      sTRADE_ROUTE_STATUS_UP.iDataLen) == 0))
	       {
	       /*   copy memory into global trade route string */
	       memcpy(&g_cTradeRoute, 
		      sTradeRoute.pData,
		      sTradeRoute.iDataLen);
	       
	       g_sTradeRoute.pData    = g_cTradeRoute;
	       g_sTradeRoute.iDataLen = sTradeRoute.iDataLen;

	       break;
	       }

	  g_sTradeRoute.pData    = NULL;
	  g_sTradeRoute.iDataLen = 0;
	  }

     g_bRcvdTradeRoutes = true;

     *aiCode = API_OK;
     return(OK);
     }

/*   =====================================================================   */

int MyCallbacks::TradeVolume(TradeVolumeInfo * pInfo,
                             void *            pContext,
                             int *             aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     }

/*   =====================================================================   */

int MyCallbacks::Bar(BarInfo * pInfo,
		     void *    pContext,
		     int *     aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     }

/*   =====================================================================   */

int MyCallbacks::BarReplay(BarReplayInfo * pInfo,
			   void *          pContext,
			   int *           aiCode)
     {
     *aiCode = API_OK;
     return (OK);
     }
/*   =====================================================================   */

int main(int      argc,
         char * * argv,
         char * * envp)
     {
     char * USAGE = (char *)"SampleOrder user password exchange ticker [B|S]\n";

     MyAdmCallbacks *  pAdmCallbacks;
     RCallbacks *      pCallbacks;
     REngineParams     oParams;
     LoginParams       oLoginParams;
     MarketOrderParams oMktOrdParams;
     tsNCharcb         sExchange;
     tsNCharcb         sTicker;
     char *            fake_envp[9];
     int               iCode;

     /*   ----------------------------------------------------------------   */

     if (argc < 6)
          {
          printf("%s", USAGE);
          return (BAD);
          }

     /*   ----------------------------------------------------------------   */

     try
          {
          pAdmCallbacks = new MyAdmCallbacks();
          }
     catch (OmneException& oEx)
          {
          iCode = oEx.getErrorCode();
          printf("MyAdmCallbacks::MyAdmCallbacks() error : %d\n", iCode);
          return (BAD);
          }

     /*   ----------------------------------------------------------------   */
     /*   The following fake envp contains the settings for connecting to    */
     /*   Rithmic Test.  To connect to a different instance of the           */
     /*   Rithmic trading platform, consult appropriate connection params    */
     /*   document in your download directory after passing conformance.     */

     fake_envp[0] = "MML_DMN_SRVR_ADDR=rituz00100.00.rithmic.com:65000~rituz00100.00.rithmic.net:65000~rituz00100.00.theomne.net:65000~rituz00100.00.theomne.com:65000";
     fake_envp[1] = "MML_DOMAIN_NAME=rithmic_uat_dmz_domain";
     fake_envp[2] = "MML_LIC_SRVR_ADDR=rituz00100.00.rithmic.com:56000~rituz00100.00.rithmic.net:56000~rituz00100.00.theomne.net:56000~rituz00100.00.theomne.com:56000";
     fake_envp[3] = "MML_LOC_BROK_ADDR=rituz00100.00.rithmic.com:64100";
     fake_envp[4] = "MML_LOGGER_ADDR=rituz00100.00.rithmic.com:45454~rituz00100.00.rithmic.net:45454~rituz00100.00.theomne.com:45454~rituz00100.00.theomne.net:45454";
     fake_envp[5] = "MML_LOG_TYPE=log_net";

     /*   The SSL file is located in the ./<version>/etc directory           */
     /*   of the R | API package.  The settings below assume that this       */
     /*   file is in the current working directory.  Normally you should     */
     /*   specify the full path to the file.                              */
     fake_envp[6] = "MML_SSL_CLNT_AUTH_FILE=rithmic_ssl_cert_auth_params";

     fake_envp[7] = "USER=your_user_name";
     fake_envp[8] = NULL;

     /*   ----------------------------------------------------------------   */

     oParams.sAppName.pData        = "SampleOrder";
     oParams.sAppName.iDataLen     = (int)strlen(oParams.sAppName.pData);
     oParams.sAppVersion.pData     = "1.0.0.0";
     oParams.sAppVersion.iDataLen  = (int)strlen(oParams.sAppVersion.pData);
     oParams.envp                  = fake_envp;
     oParams.pAdmCallbacks         = pAdmCallbacks;
     oParams.sLogFilePath.pData    = "so.log";
     oParams.sLogFilePath.iDataLen = (int)strlen(oParams.sLogFilePath.pData);

     /*   ----------------------------------------------------------------   */

     try
          {
          g_pEngine = new REngine(&oParams);
          }
     catch (OmneException& oEx)
          {
          delete pAdmCallbacks;

          iCode = oEx.getErrorCode();
          printf("REngine::REngine() error : %d\n", iCode);
          return (BAD);
          }

     /*   ----------------------------------------------------------------   */
     /*   instantiate a callback object - prerequisite for logging in */
     try
          {
          pCallbacks = new MyCallbacks();
          }
     catch (OmneException& oEx)
          {
          delete g_pEngine;
          delete pAdmCallbacks;

          iCode = oEx.getErrorCode();
          printf("MyCallbacks::MyCallbacks() error : %d\n", iCode);
          return (BAD);
          }

     /*   ----------------------------------------------------------------   */
     /*   Set up parameters for logging in.  Again, the MdCnnctPt and        */
     /*   TsCnnctPt have values for Rithmic 01 Test.  Add values for other   */
     /*   members of LoginParams to log into other subsystems of the         */
     /*   infrastructure like like pnl and history.                          */

     oLoginParams.pCallbacks           = pCallbacks;

     oLoginParams.sMdUser.pData        = argv[1];
     oLoginParams.sMdUser.iDataLen     = (int)strlen(oLoginParams.sMdUser.pData);

     oLoginParams.sMdPassword.pData    = argv[2];
     oLoginParams.sMdPassword.iDataLen = (int)strlen(oLoginParams.sMdPassword.pData);

     oLoginParams.sMdCnnctPt.pData     = "login_agent_tpc";
     oLoginParams.sMdCnnctPt.iDataLen  = (int)strlen(oLoginParams.sMdCnnctPt.pData);

     oLoginParams.sTsUser.pData        = argv[1];
     oLoginParams.sTsUser.iDataLen     = (int)strlen(oLoginParams.sTsUser.pData);

     oLoginParams.sTsPassword.pData    = argv[2];
     oLoginParams.sTsPassword.iDataLen = (int)strlen(oLoginParams.sTsPassword.pData);

     oLoginParams.sTsCnnctPt.pData     = "login_agent_opc";
     oLoginParams.sTsCnnctPt.iDataLen  = (int)strlen(oLoginParams.sTsCnnctPt.pData);

     /*   ----------------------------------------------------------------   */

     if (!g_pEngine -> login(&oLoginParams, &iCode))
          {
          printf("REngine::login() error : %d\n", iCode);

          delete g_pEngine;
          delete pCallbacks;
          delete pAdmCallbacks;

          return (BAD);
          }

     /*   ----------------------------------------------------------------   */
     /*   After calling REngine::login, RCallbacks::Alert will be called a   */
     /*   number of times.  Wait for when the login to the TsCnnctPt is      */
     /*   complete.  (See MyCallbacks::Alert() for details).                 */

     while (!g_bTsLoginComplete)
          {
#ifndef WinOS
          sleep(1);
#else
          Sleep(1000);
#endif
          }

     /*   ----------------------------------------------------------------   */
     /*   Once logged in, we request price increment info for the instrument */
     /*   that we want to trade.  This call will return price increment      */
     /*   information as well as set up internal instrument-specific data.   */

     sExchange.pData    = argv[3];
     sExchange.iDataLen = (int)strlen(sExchange.pData);
     sTicker.pData      = argv[4];
     sTicker.iDataLen   = (int)strlen(sTicker.pData);

     if (!g_pEngine -> getPriceIncrInfo(&sExchange, &sTicker, &iCode))
          {
          printf("REngine::getPriceIncrInfo() error : %d\n", iCode);

          delete g_pEngine;
          delete pCallbacks;
          delete pAdmCallbacks;

          return (BAD);
          }

     /*   ----------------------------------------------------------------   */
     /*   Use the global boolean as the signaller of when the callback has   */
     /*   been fired (and our internal instrument-specific data is ready.)   */

     while (!g_bRcvdPriceIncr)
          {
#ifndef WinOS
          sleep(1);
#else
          Sleep(1000);
#endif
          }

     /*   ----------------------------------------------------------------   */
     /*   Placing an order requires an account for the order.  Wait for      */
     /*   an account to be received via RCallbacks::AccountList().           */

     while (!g_bRcvdAccount)
          {
#ifndef WinOS
          sleep(1);
#else
          Sleep(1000);
#endif
          }

     /*   ----------------------------------------------------------------   */
     /*   Placing an order requires a trade route to be specified.  Based    */
     /*   on your FCM and/or IB, you may have zero to many trade routes      */
     /*   for a given exchange.  Store the exchange where the callback can   */
     /*   see it, and then request the list.                                 */

     memcpy(&g_cExchange, 
	    sExchange.pData,
	    sExchange.iDataLen);

     g_sExchange.pData    = g_cExchange;
     g_sExchange.iDataLen = sExchange.iDataLen;

     /*   ----------------------------------------------------------------   */

     if (!g_pEngine -> listTradeRoutes(NULL, &iCode))
	  {
          printf("REngine::listTradeRoutes() error : %d\n", iCode);

          delete g_pEngine;
          delete pCallbacks;
          delete pAdmCallbacks;

          return (BAD);
	  }

     while (!g_bRcvdTradeRoutes)
          {
#ifndef WinOS
          sleep(1);
#else
          Sleep(1000);
#endif
          }

     if (g_sTradeRoute.iDataLen == 0)
	  {
	  printf("No available trade routes for : %*.*s::%*.*s::%*.*s\n",
		 g_oAccount.sFcmId.iDataLen,
		 g_oAccount.sFcmId.iDataLen,
		 g_oAccount.sFcmId.pData,
		 g_oAccount.sIbId.iDataLen,
		 g_oAccount.sIbId.iDataLen,
		 g_oAccount.sIbId.pData,
		 g_sExchange.iDataLen,
		 g_sExchange.iDataLen,
		 g_sExchange.pData);

          delete g_pEngine;
          delete pCallbacks;
          delete pAdmCallbacks;

          return (BAD);
	  }

     /*   ----------------------------------------------------------------   */
     /*   send the order */

     oMktOrdParams.sExchange             = sExchange;
     oMktOrdParams.sTicker               = sTicker;
     oMktOrdParams.pAccount              = &g_oAccount;
     oMktOrdParams.iQty                  = 1;
     oMktOrdParams.sBuySellType.pData    = argv[5];
     oMktOrdParams.sBuySellType.iDataLen = (int)strlen(oMktOrdParams.sBuySellType.pData);
     oMktOrdParams.sDuration             = sORDER_DURATION_DAY;
     oMktOrdParams.sEntryType            = sORDER_ENTRY_TYPE_MANUAL;
     oMktOrdParams.sTradeRoute           = g_sTradeRoute;

     /*   ----------------------------------------------------------------   */

     if (!g_pEngine -> sendOrder(&oMktOrdParams, &iCode))
          {
          printf("REngine::sendOrder() error : %d\n", iCode);

          delete g_pEngine;
          delete pCallbacks;
          delete pAdmCallbacks;

          return (BAD);
          }

     /*   ----------------------------------------------------------------   */
     /*   A number of Order*Report and LineInfo callbacks will be fired.     */
     /*   Wait for the order to complete (see MyCallbacks::LineUpdate()      */
     /*   for details.                                                       */

     while (!g_bDone)
          {
#ifndef WinOS
          sleep(1);
#else
          Sleep(1000);
#endif
          }

     /*   ----------------------------------------------------------------   */
     /*   The order is complete.  Clean up and exit.                         */

     delete g_pEngine;
     delete pCallbacks;
     delete pAdmCallbacks;

     /*   ----------------------------------------------------------------   */

     int iUsecs = ((g_iFromExchSsboe - g_iToExchSsboe) * 1000 * 1000) +
                  (g_iFromExchUsecs - g_iToExchUsecs);
     printf("\nusecs at exchange : %d\n\n", iUsecs);

     /*   ----------------------------------------------------------------   */

     return (GOOD);
     }

/*   =====================================================================   */
