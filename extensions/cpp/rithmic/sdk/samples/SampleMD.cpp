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

     g++ -O3 -DLINUX -D_REENTRANT -Wall -Wno-sign-compare -Wno-write-strings -Wpointer-arith -Winline -Wno-deprecated -fno-strict-aliasing -I../include -o SampleMD ../samples/SampleMD.cpp -L../linux-gnu-2.6.32-x86_64/lib -lRApiPlus-optimize -lOmneStreamEngine-optimize -lOmneChannel-optimize -lOmneEngine-optimize -l_api-optimize -l_apipoll-stubs-optimize -l_kit-optimize -lssl -lcrypto -L/usr/lib64 -lz -L/usr/kerberos/lib -lkrb5 -lk5crypto -lcom_err -lresolv -lm -lpthread -lrt

     64-bit linux (3.10.0 kernel) :

     g++ -O3 -DLINUX -D_REENTRANT -Wall -Wno-sign-compare -Wno-write-strings -Wpointer-arith -Winline -Wno-deprecated -fno-strict-aliasing -I../include -o SampleMD ../samples/SampleMD.cpp -L../linux-gnu-3.10.0-x86_64/lib -lRApiPlus-optimize -lOmneStreamEngine-optimize -lOmneChannel-optimize -lOmneEngine-optimize -l_api-optimize -l_apipoll-stubs-optimize -l_kit-optimize -lssl -lcrypto -L/usr/lib64 -lz -lpthread -lrt -ldl

     64-bit linux (4.18 kernel) :

     g++ -O3 -DLINUX -D_REENTRANT -Wall -Wno-sign-compare -Wno-write-strings -Wpointer-arith -Winline -Wno-deprecated -fno-strict-aliasing -I../include -o SampleMD ../samples/SampleMD.cpp -L../linux-gnu-4.18-x86_64/lib -lRApiPlus-optimize -lOmneStreamEngine-optimize -lOmneChannel-optimize -lOmneEngine-optimize -l_api-optimize -l_apipoll-stubs-optimize -l_kit-optimize -lssl -lcrypto -L/usr/lib64 -lz -lpthread -lrt -ldl

     64-bit darwin :

     g++ -O3 -D_REENTRANT -Wall -Wno-sign-compare -fno-strict-aliasing -Wpointer-arith -Winline -Wno-deprecated -Wno-write-strings -I../include -o ./SampleMD ../samples/SampleMD.cpp -L../darwin-10/lib -lRApiPlus-optimize -lOmneStreamEngine-optimize -lOmneChannel-optimize -lOmneEngine-optimize -l_api-optimize -l_apipoll-stubs-optimize -l_kit-optimize -lssl -lcrypto -L/usr/lib -lz -Wl,-search_paths_first

     64-bit darwin arm64 :

     g++ -O3 -D_REENTRANT -Wall -Wno-sign-compare -fno-strict-aliasing -Wpointer-arith -Winline -Wno-deprecated -Wno-write-strings -I../include -o ./SampleMD ../samples/SampleMD.cpp -L../darwin-20.6-arm64/lib -lRApiPlus-optimize -lOmneStreamEngine-optimize -lOmneChannel-optimize -lOmneEngine-optimize -l_api-optimize -l_apipoll-stubs-optimize -l_kit-optimize -lssl -lcrypto -L/usr/lib -lz

     =====================================================================   */

#include "RApiPlus.h"

#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifndef WinOS
#include <unistd.h>
#else
#include <Windows.h>
#endif

#define GOOD 0
#define BAD  1

using namespace std;
using namespace RApi;

/*   =====================================================================   */

int LoginStatus_NotLoggedIn     = 0;
int LoginStatus_AwaitingResults = 1;
int LoginStatus_Failed          = 2;
int LoginStatus_Complete        = 3;

/*   =====================================================================   */

int  g_iRepLoginStatus                = LoginStatus_NotLoggedIn;
bool g_bRcvdUnacceptedAgreements      = false;
int  g_iUnacceptedMandatoryAgreements = 0;

int  g_iMdLoginStatus                 = LoginStatus_NotLoggedIn;

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

     virtual int AgreementList(AgreementListInfo * pInfo,
			       void *              pContext,
			       int *               aiCode);

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

     cout << endl << endl;
     if (!pInfo -> dump(&iIgnored))
          {
          cout << "error in pInfo -> dump : " << iIgnored << endl;
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

     cout << endl << endl;
     if (!pInfo -> dump(&iIgnored))
          {
          cout << "error in pInfo -> dump : " << iIgnored << endl;
          }

     /*   ----------------------------------------------------------------   */
     /*   Signal when the login to the repository sub-system (agreements)    */
     /*   is complete, and what the results are.                             */

     if (pInfo -> iConnectionId == REPOSITORY_CONNECTION_ID)
          {
	  if (pInfo -> iAlertType == ALERT_LOGIN_COMPLETE)
	       {
	       g_iRepLoginStatus = LoginStatus_Complete;
	       }
	  else if (pInfo -> iAlertType == ALERT_LOGIN_FAILED)
	       {
	       g_iRepLoginStatus = LoginStatus_Failed;
	       }
          }

     /*   ----------------------------------------------------------------   */
     /*   Signal when the login to the market data system (ticker plant)     */
     /*   is complete, and what the results are.                             */

     if (pInfo -> iConnectionId == MARKET_DATA_CONNECTION_ID)
          {
	  if (pInfo -> iAlertType == ALERT_LOGIN_COMPLETE)
	       {
	       g_iMdLoginStatus = LoginStatus_Complete;
	       }
	  else if (pInfo -> iAlertType == ALERT_LOGIN_FAILED)
	       {
	       g_iMdLoginStatus = LoginStatus_Failed;
	       }
          }

     /*   ----------------------------------------------------------------   */

     *aiCode = API_OK;
     return (OK);
     }

/*   =====================================================================   */

int MyCallbacks::AgreementList(AgreementListInfo * pInfo,
			       void *              pContext,
			       int *               aiCode)
     {
     int iIgnored;

     /*   ----------------------------------------------------------------   */

     cout << endl << endl;

     /*   ----------------------------------------------------------------   */

     if (!pInfo -> dump(&iIgnored))
	  {
	  ; // do nothing ...
	  }

     /*   ----------------------------------------------------------------   */

     if (!pInfo -> bAccepted)
	  {
	  for (int i = 0; i < pInfo -> iArrayLen; i++)
	       {
	       AgreementInfo oAg     = pInfo -> asAgreementInfoArray[i];
	       tsNCharcb     sActive = {"active", 6};
	       bool          bActive = false;

	       if (oAg.sStatus.iDataLen == sActive.iDataLen &&
		   (memcmp(oAg.sStatus.pData, 
			   sActive.pData, 
			   oAg.sStatus.iDataLen) == 0))
		    {
		    bActive = true;
		    }

	       if (oAg.bMandatory && bActive)
		    {
		    g_iUnacceptedMandatoryAgreements++;
		    }
	       }

	  g_bRcvdUnacceptedAgreements = true;
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

     cout << endl << endl;
     if (!pInfo -> dump(&iIgnored))
          {
          cout << "error in pInfo -> dump : " << iIgnored << endl;
          }

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
     *aiCode = API_OK;
     return(OK);
     }

/*   =====================================================================   */

int MyCallbacks::CancelReport(OrderCancelReport * pReport,
                              void *              pContext,
                              int *               aiCode)
     {
     *aiCode = API_OK;
     return(OK);
     }

/*   =====================================================================   */

int MyCallbacks::FailureReport(OrderFailureReport * pReport,
                               void *               pContext,
                               int *                aiCode)
     {
     *aiCode = API_OK;
     return(OK);
     }

/*   =====================================================================   */

int MyCallbacks::FillReport(OrderFillReport * pReport,
                            void *            pContext,
                            int *             aiCode)
     {
     *aiCode = API_OK;
     return(OK);
     }

/*   =====================================================================   */

int MyCallbacks::ModifyReport(OrderModifyReport * pReport,
                              void *              pContext,
                              int *               aiCode)
     {
     *aiCode = API_OK;
     return(OK);
     }

/*   =====================================================================   */

int MyCallbacks::NotCancelledReport(OrderNotCancelledReport * pReport,
                                    void *                    pContext,
                                    int *                     aiCode)
     {
     *aiCode = API_OK;
     return(OK);
     }

/*   =====================================================================   */

int MyCallbacks::NotModifiedReport(OrderNotModifiedReport * pReport,
                                   void *                   pContext,
                                   int *                    aiCode)
     {
     *aiCode = API_OK;
     return(OK);
     }

/*   =====================================================================   */

int MyCallbacks::RejectReport(OrderRejectReport * pReport,
                              void *              pContext,
                              int *               aiCode)
     {
     *aiCode = API_OK;
     return(OK);
     }

/*   =====================================================================   */

int MyCallbacks::StatusReport(OrderStatusReport * pReport,
                              void *              pContext,
                              int *               aiCode)
     {
     *aiCode = API_OK;
     return(OK);
     }

/*   =====================================================================   */

int MyCallbacks::TradeCorrectReport(OrderTradeCorrectReport * pReport,
                                    void *                    pContext,
                                    int *                     aiCode)
     {
     *aiCode = API_OK;
     return(OK);
     }

/*   =====================================================================   */

int MyCallbacks::TriggerPulledReport(OrderTriggerPulledReport * pReport,
                                     void *                     pContext,
                                     int *                      aiCode)
     {
     *aiCode = API_OK;
     return(OK);
     }

/*   =====================================================================   */

int MyCallbacks::TriggerReport(OrderTriggerReport * pReport,
                               void *               pContext,
                               int *                aiCode)
     {
     *aiCode = API_OK;
     return(OK);
     }

/*   =====================================================================   */

int MyCallbacks::OtherReport(OrderReport * pReport,
                             void *        pContext,
                             int *         aiCode)
     {
     *aiCode = API_OK;
     return(OK);
     }

/*   =====================================================================   */

int MyCallbacks::SodUpdate(SodReport * pReport,
                           void *      pContext,
                           int *       aiCode)
     {
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
     int iIgnored;

     /*   ----------------------------------------------------------------   */

     cout << endl << endl;
     if (!pInfo -> dump(&iIgnored))
          {
          cout << "error in pInfo -> dump : " << iIgnored << endl;
          }

     /*   ----------------------------------------------------------------   */

     *aiCode = API_OK;
     return (OK);
     }

/*   =====================================================================   */

int MyCallbacks::BestAskQuote(AskInfo * pInfo,
                              void *    pContext,
                              int *     aiCode)
     {
     int iIgnored;

     /*   ----------------------------------------------------------------   */

     cout << endl << endl << "Best";
     if (!pInfo -> dump(&iIgnored))
          {
          cout << "error in pInfo -> dump : " << iIgnored << endl;
          }

     /*   ----------------------------------------------------------------   */

     *aiCode = API_OK;
     return (OK);
     }

/*   =====================================================================   */

int MyCallbacks::BestBidAskQuote(BidInfo * pBid,
				 AskInfo * pAsk,
				 void *    pContext,
				 int *     aiCode)
     {
     int iIgnored;

     /*   ----------------------------------------------------------------   */

     cout << endl << endl << "Best Bid/Ask";
     if (!pBid -> dump(&iIgnored))
          {
          cout << "error in pBid -> dump : " << iIgnored << endl;
          }

     if (!pAsk -> dump(&iIgnored))
          {
          cout << "error in pAsk -> dump : " << iIgnored << endl;
          }

     /*   ----------------------------------------------------------------   */

     *aiCode = API_OK;
     return (OK);
     }

/*   =====================================================================   */

int MyCallbacks::BestBidQuote(BidInfo * pInfo,
                              void *    pContext,
                              int *     aiCode)
     {
     int iIgnored;

     /*   ----------------------------------------------------------------   */

     cout << endl << endl << "Best";
     if (!pInfo -> dump(&iIgnored))
          {
          cout << "error in pInfo -> dump : " << iIgnored << endl;
          }

     /*   ----------------------------------------------------------------   */

     *aiCode = API_OK;
     return (OK);
     }

/*   =====================================================================   */

int MyCallbacks::BidQuote(BidInfo * pInfo,
                          void *    pContext,
                          int *     aiCode)
     {
     int iIgnored;

     /*   ----------------------------------------------------------------   */

     cout << endl << endl;
     if (!pInfo -> dump(&iIgnored))
          {
          cout << "error in pInfo -> dump : " << iIgnored << endl;
          }

     /*   ----------------------------------------------------------------   */

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
     int iIgnored;

     /*   ----------------------------------------------------------------   */

     cout << endl << endl;
     if (!pInfo -> dump(&iIgnored))
          {
          cout << "error in pInfo -> dump : " << iIgnored << endl;
          }

     /*   ----------------------------------------------------------------   */

     *aiCode = API_OK;
     return (OK);
     }

/*   =====================================================================   */

int MyCallbacks::ClosingIndicator(ClosingIndicatorInfo * pInfo,
                                  void *                 pContext,
                                  int *                  aiCode)
     {
     int iIgnored;

     /*   ----------------------------------------------------------------   */

     cout << endl << endl;
     if (!pInfo -> dump(&iIgnored))
          {
          cout << "error in pInfo -> dump : " << iIgnored << endl;
          }

     /*   ----------------------------------------------------------------   */

     *aiCode = API_OK;
     return (OK);
     }

/*   =====================================================================   */

int MyCallbacks::EndQuote(EndQuoteInfo * pInfo,
			  void *         pContext,
			  int *          aiCode)
     {
     int iIgnored;

     /*   ----------------------------------------------------------------   */

     cout << endl << endl;
     if (!pInfo -> dump(&iIgnored))
          {
          cout << "error in pInfo -> dump : " << iIgnored << endl;
          }

     /*   ----------------------------------------------------------------   */

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
     int iIgnored;

     /*   ----------------------------------------------------------------   */

     cout << endl << endl;
     if (!pInfo -> dump(&iIgnored))
          {
          cout << "error in pInfo -> dump : " << iIgnored << endl;
          }

     /*   ----------------------------------------------------------------   */

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
     int iIgnored;

     /*   ----------------------------------------------------------------   */

     cout << endl << endl;
     if (!pInfo -> dump(&iIgnored))
          {
          cout << "error in pInfo -> dump : " << iIgnored << endl;
          }

     /*   ----------------------------------------------------------------   */

     *aiCode = API_OK;
     return (OK);
     }

/*   =====================================================================   */

int MyCallbacks::LowPrice(LowPriceInfo * pInfo,
                          void *         pContext,
                          int *          aiCode)
     {
     int iIgnored;

     /*   ----------------------------------------------------------------   */

     cout << endl << endl;
     if (!pInfo -> dump(&iIgnored))
          {
          cout << "error in pInfo -> dump : " << iIgnored << endl;
          }

     /*   ----------------------------------------------------------------   */

     *aiCode = API_OK;
     return (OK);
     }

/*   =====================================================================   */

int MyCallbacks::MarketMode(MarketModeInfo * pInfo,
                            void *           pContext,
                            int *            aiCode)
     {
     int iIgnored;

     /*   ----------------------------------------------------------------   */

     cout << endl << endl;
     if (!pInfo -> dump(&iIgnored))
          {
          cout << "error in pInfo -> dump : " << iIgnored << endl;
          }

     /*   ----------------------------------------------------------------   */

     *aiCode = API_OK;
     return (OK);
     }

/*   =====================================================================   */

int MyCallbacks::OpenInterest(OpenInterestInfo * pInfo,
			      void *             pContext,
			      int *              aiCode)
     {
     int iIgnored;

     /*   ----------------------------------------------------------------   */

     cout << endl << endl;
     if (!pInfo -> dump(&iIgnored))
          {
          cout << "error in pInfo -> dump : " << iIgnored << endl;
          }

     /*   ----------------------------------------------------------------   */

     *aiCode = API_OK;
     return (OK);
     }

/*   =====================================================================   */

int MyCallbacks::OpenPrice(OpenPriceInfo * pInfo,
                           void *          pContext,
                           int *           aiCode)
     {
     int iIgnored;

     /*   ----------------------------------------------------------------   */

     cout << endl << endl;
     if (!pInfo -> dump(&iIgnored))
          {
          cout << "error in pInfo -> dump : " << iIgnored << endl;
          }

     /*   ----------------------------------------------------------------   */

     *aiCode = API_OK;
     return (OK);
     }

/*   =====================================================================   */

int MyCallbacks::OpeningIndicator(OpeningIndicatorInfo * pInfo,
				  void *                 pContext,
				  int *                  aiCode)
     {
     int iIgnored;

     /*   ----------------------------------------------------------------   */

     cout << endl << endl;
     if (!pInfo -> dump(&iIgnored))
          {
          cout << "error in pInfo -> dump : " << iIgnored << endl;
          }

     /*   ----------------------------------------------------------------   */

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
     int iIgnored;

     /*   ----------------------------------------------------------------   */

     cout << endl << endl;
     if (!pInfo -> dump(&iIgnored))
          {
          cout << "error in pInfo -> dump : " << iIgnored << endl;
          }

     /*   ----------------------------------------------------------------   */

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
     int iIgnored;

     /*   ----------------------------------------------------------------   */

     cout << endl << endl;
     if (!pInfo -> dump(&iIgnored))
          {
          cout << "error in pInfo -> dump : " << iIgnored << endl;
          }

     /*   ----------------------------------------------------------------   */

     *aiCode = API_OK;
     return (OK);
     }

/*   =====================================================================   */

int MyCallbacks::TradePrint(TradeInfo * pInfo,
                            void *      pContext,
                            int *       aiCode)
     {
     int iIgnored;

     /*   ----------------------------------------------------------------   */

     cout << endl << endl;
     if (!pInfo -> dump(&iIgnored))
          {
          cout << "error in pInfo -> dump : " << iIgnored << endl;
          }

     /*   ----------------------------------------------------------------   */

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
     *aiCode = API_OK;
     return(OK);
     }

/*   =====================================================================   */

int MyCallbacks::TradeVolume(TradeVolumeInfo * pInfo,
                             void *            pContext,
                             int *             aiCode)
     {
     int iIgnored;

     /*   ----------------------------------------------------------------   */

     cout << endl << endl;
     if (!pInfo -> dump(&iIgnored))
          {
          cout << "error in pInfo -> dump : " << iIgnored << endl;
          }

     /*   ----------------------------------------------------------------   */

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
     char * USAGE = (char *)"SampleMD user password exchange ticker";

     REngine *        pEngine;
     MyAdmCallbacks * pAdmCallbacks;
     RCallbacks *     pCallbacks;
     REngineParams    oParams;
     LoginParams      oLoginParams;
     tsNCharcb        sExchange;
     tsNCharcb        sTicker;
     char *           fake_envp[9];
     int              iFlags;
     int              iCode;

     /*   ----------------------------------------------------------------   */

     if (argc < 5)
          {
          cout << USAGE << endl;
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
          cout << "MyAdmCallbacks::MyAdmCallbacks() error : " << iCode << endl;
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
     /*   specify the full path to the file.                                 */
     fake_envp[6] = "MML_SSL_CLNT_AUTH_FILE=rithmic_ssl_cert_auth_params";

     fake_envp[7] = "USER=your_user_name";
     fake_envp[8] = NULL;

     /*   ----------------------------------------------------------------   */

     oParams.sAppName.pData        = "SampleMD";
     oParams.sAppName.iDataLen     = (int)strlen(oParams.sAppName.pData);
     oParams.sAppVersion.pData     = "1.0.0.0";
     oParams.sAppVersion.iDataLen  = (int)strlen(oParams.sAppVersion.pData);
     oParams.envp                  = fake_envp;
     oParams.pAdmCallbacks         = pAdmCallbacks;
     oParams.sLogFilePath.pData    = "smd.log";
     oParams.sLogFilePath.iDataLen = (int)strlen(oParams.sLogFilePath.pData);

     /*   ----------------------------------------------------------------   */

     try
          {
          pEngine = new REngine(&oParams);
          }
     catch (OmneException& oEx)
          {
          delete pAdmCallbacks;

          iCode = oEx.getErrorCode();
          cout << "REngine::REngine() error : " << iCode << endl;
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
          delete pEngine;
          delete pAdmCallbacks;

          iCode = oEx.getErrorCode();
          cout << "MyCallbacks::MyCallbacks() error : " << iCode << endl;
          return (BAD);
          }

     /*   ----------------------------------------------------------------   */
     /*   First, log in to the repository to check agreements.               */
     /*   (See the FAQ for more details on agreements.)                      */
     /*   ----------------------------------------------------------------   */

     tsNCharcb sRepEnvKey;
     tsNCharcb sRepUser;
     tsNCharcb sRepPassword;
     tsNCharcb sRepCnnctPt;

     sRepEnvKey.pData      = "system";
     sRepEnvKey.iDataLen   = (int)strlen(sRepEnvKey.pData);

     sRepUser.pData        = argv[1];
     sRepUser.iDataLen     = (int)strlen(sRepUser.pData);

     sRepPassword.pData    = argv[2];
     sRepPassword.iDataLen = (int)strlen(sRepPassword.pData);

     sRepCnnctPt.pData     = "login_agent_repositoryc";
     sRepCnnctPt.iDataLen  = (int)strlen(sRepCnnctPt.pData);

     if (!pEngine -> loginRepository(&sRepEnvKey,
				     &sRepUser,
				     &sRepPassword,
				     &sRepCnnctPt,
				     pCallbacks,
				     &iCode))
          {
          cout << "REngine::loginRepository() error : " << iCode << endl;

          delete pEngine;
          delete pCallbacks;
          delete pAdmCallbacks;

          return (BAD);
          }

     /*   ----------------------------------------------------------------   */
     /*   After calling REngine::loginRepository, RCallbacks::Alert will     */
     /*   be called a number of times.  Wait until the login succeeds or     */
     /*   fails.                                                             */
     /*   ----------------------------------------------------------------   */

     while (g_iRepLoginStatus != LoginStatus_Complete &&
	    g_iRepLoginStatus != LoginStatus_Failed)
          {
#ifndef WinOS
          sleep(1);
#else
          Sleep(1000);
#endif
          }

     if (g_iRepLoginStatus == LoginStatus_Failed)
	  {
	  cout << endl << endl;
          cout << "Please make sure you entered the username and password "
	       << "correctly.  Also, please make sure your credentials match "
	       << "the system you are trying to log in to." << endl;
	  cout << endl << endl;

          delete pEngine;
          delete pCallbacks;
          delete pAdmCallbacks;

          return (BAD);
	  }

     /*   ----------------------------------------------------------------   */
     /*   Once logged in to the repository, we can request a list of         */
     /*   unaccepted agreements.                                             */
     /*   ----------------------------------------------------------------   */

     if (!pEngine -> listAgreements(false, NULL, &iCode))
          {
          cout << "REngine::listAgreements() error : " << iCode << endl;

	  int iIgnored;
	  pEngine -> logoutRepository(&iIgnored);

          delete pEngine;
          delete pCallbacks;
          delete pAdmCallbacks;

          return (BAD);
          }

     /*   ----------------------------------------------------------------   */
     /*   Wait for the list to arrive from the infrastructure.               */
     /*   ----------------------------------------------------------------   */

     while (!g_bRcvdUnacceptedAgreements)
          {
#ifndef WinOS
          sleep(1);
#else
          Sleep(1000);
#endif
          }

     /*   ----------------------------------------------------------------   */
     /*   If there are any unaccepted mandatory agreements, you must log     */
     /*   in using R | Trader (Pro) and accept them before being able to     */
     /*   log in to the trading platform.                                    */
     /*   ----------------------------------------------------------------   */

     if (g_iUnacceptedMandatoryAgreements > 0)
	  {
	  cout << endl << endl;
          cout << "Please log in using R | Trader or R | Trader Pro" 
	       << " and sign the agreements." << endl;
	  cout << endl << endl;

	  int iIgnored;

	  pEngine -> logoutRepository(&iIgnored);

          delete pEngine;
          delete pCallbacks;
          delete pAdmCallbacks;

          return (BAD);
	  }
     
     /*   ----------------------------------------------------------------   */
     /*   Log out from the repository.  We are done with it ...              */
     /*   ----------------------------------------------------------------   */

     if (!pEngine -> logoutRepository(&iCode))
          {
          cout << "REngine::logoutRepository() error : " << iCode << endl;

          delete pEngine;
          delete pCallbacks;
          delete pAdmCallbacks;

          return (BAD);
          }

     /*   ----------------------------------------------------------------   */
     /*   Set up parameters for logging in.  Again, the MdCnnctPt and        */
     /*   TsCnnctPt have values for Rithmic 01 Test.  Add values for other   */
     /*   members of LoginParams to log into other subsystems of the         */
     /*   infrastructure like pnl and history.                               */

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

     if (!pEngine -> login(&oLoginParams, &iCode))
          {
          cout << "REngine::login() error : " << iCode << endl;

          delete pEngine;
          delete pCallbacks;
          delete pAdmCallbacks;

          return (BAD);
          }

     /*   ----------------------------------------------------------------   */
     /*   After calling REngine::login, RCallbacks::Alert may be called a    */
     /*   number of times.  Wait for when the login to the MdCnnctPt is      */
     /*   complete.  (See MyCallbacks::Alert() for details).                 */

     while (g_iMdLoginStatus != LoginStatus_Complete &&
	    g_iMdLoginStatus != LoginStatus_Failed)
          {
#ifndef WinOS
          sleep(1);
#else
          Sleep(1000);
#endif
          }

     if (g_iMdLoginStatus == LoginStatus_Failed)
	  {
	  cout << endl << endl;
          cout << "Please make sure you entered the username and password "
	       << "correctly.  Also, please make sure your credentials match "
	       << "the system you are trying to log in to." << endl;
	  cout << endl << endl;

          delete pEngine;
          delete pCallbacks;
          delete pAdmCallbacks;

          return (BAD);
	  }

     /*   ----------------------------------------------------------------   */

     sExchange.pData    = argv[3];
     sExchange.iDataLen = (int)strlen(sExchange.pData);

     /*   ----------------------------------------------------------------   */

     sTicker.pData    = argv[4];
     sTicker.iDataLen = (int)strlen(sTicker.pData);

     /*   ----------------------------------------------------------------   */
     /*   Subscription flags are OR'd.  Add more flags to get more data.     */

     iFlags = (MD_PRINTS | MD_BEST);

     /*   ----------------------------------------------------------------   */

     if (!pEngine -> subscribe(&sExchange, &sTicker, iFlags, &iCode))
          {
          cout << "REngine::subscribe() error : " << iCode << endl;

          delete pEngine;
          delete pCallbacks;
          delete pAdmCallbacks;

          return (BAD);
          }

     /*   ----------------------------------------------------------------   */
     /*   press 'enter' to quit... */

     fgetc(stdin);

     /*   ----------------------------------------------------------------   */

     delete pEngine;
     delete pCallbacks;
     delete pAdmCallbacks;

     /*   ----------------------------------------------------------------   */

     return (GOOD);
     }

/*   =====================================================================   */
