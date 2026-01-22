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

     g++ -O3 -DLINUX -D_REENTRANT -Wall -Wno-sign-compare -Wno-write-strings -Wpointer-arith -Winline -Wno-deprecated -fno-strict-aliasing -I../include -o SampleBar ../samples/SampleBar.cpp -L../linux-gnu-2.6.32-x86_64/lib -lRApiPlus-optimize -lOmneStreamEngine-optimize -lOmneChannel-optimize -lOmneEngine-optimize -l_api-optimize -l_apipoll-stubs-optimize -l_kit-optimize -lssl -lcrypto -L/usr/lib64 -lz -L/usr/kerberos/lib -lkrb5 -lk5crypto -lcom_err -lresolv -lm -lpthread -lrt

     64-bit linux (3.10.0 kernel) :

     g++ -O3 -DLINUX -D_REENTRANT -Wall -Wno-sign-compare -Wno-write-strings -Wpointer-arith -Winline -Wno-deprecated -fno-strict-aliasing -I../include -o SampleBar ../samples/SampleBar.cpp -L../linux-gnu-3.10.0-x86_64/lib -lRApiPlus-optimize -lOmneStreamEngine-optimize -lOmneChannel-optimize -lOmneEngine-optimize -l_api-optimize -l_apipoll-stubs-optimize -l_kit-optimize -lssl -lcrypto -L/usr/lib64 -lz -lpthread -lrt -ldl

     64-bit linux (4.18 kernel) :

     g++ -O3 -DLINUX -D_REENTRANT -Wall -Wno-sign-compare -Wno-write-strings -Wpointer-arith -Winline -Wno-deprecated -fno-strict-aliasing -I../include -o SampleBar ../samples/SampleBar.cpp -L../linux-gnu-4.18-x86_64/lib -lRApiPlus-optimize -lOmneStreamEngine-optimize -lOmneChannel-optimize -lOmneEngine-optimize -l_api-optimize -l_apipoll-stubs-optimize -l_kit-optimize -lssl -lcrypto -L/usr/lib64 -lz -lpthread -lrt -ldl

     64-bit darwin :

     g++ -O3 -D_REENTRANT -Wall -Wno-sign-compare -fno-strict-aliasing -Wpointer-arith -Winline -Wno-deprecated -Wno-write-strings -I../include -o ./SampleMD ../samples/SampleMD.cpp -L../darwin-10/lib -lRApiPlus-optimize -lOmneStreamEngine-optimize -lOmneChannel-optimize -lOmneEngine-optimize -l_api-optimize -l_apipoll-stubs-optimize -l_kit-optimize -lssl -lcrypto -L/usr/lib -lz -Wl,-search_paths_first

     64-bit darwin arm64 :

     g++ -O3 -D_REENTRANT -Wall -Wno-sign-compare -fno-strict-aliasing -Wpointer-arith -Winline -Wno-deprecated -Wno-write-strings -I../include -o ./SampleBar ../samples/SampleBar.cpp -L../darwin-20.6-arm64/lib -lRApiPlus-optimize -lOmneStreamEngine-optimize -lOmneChannel-optimize -lOmneEngine-optimize -l_api-optimize -l_apipoll-stubs-optimize -l_kit-optimize -lssl -lcrypto -L/usr/lib -lz

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
#include <time.h>
#endif

/*   =====================================================================   */

#define GOOD 0
#define BAD  1

/*   =====================================================================   */

using namespace std;
using namespace RApi;

/*   =====================================================================   */

int LoginStatus_NotLoggedIn     = 0;
int LoginStatus_AwaitingResults = 1;
int LoginStatus_Failed          = 2;
int LoginStatus_Complete        = 3;

/*   =====================================================================   */

int  g_iMdLoginStatus = LoginStatus_NotLoggedIn;
int  g_iIhLoginStatus = LoginStatus_NotLoggedIn;

/*   =====================================================================   */

bool g_bBarsDone = false;

/*   =====================================================================   */

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

     virtual int Bar(BarInfo * pInfo,
		     void *    pContext,
		     int *     aiCode);

     virtual int BarReplay(BarReplayInfo * pInfo,
			   void *          pContext,
			   int *           aiCode);

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
     printf("AdmAlert : %*.*s\n", 
	    pInfo -> sMessage.iDataLen,
	    pInfo -> sMessage.iDataLen,
	    pInfo -> sMessage.pData);

     /*   ----------------------------------------------------------------   */

     *aiCode = API_OK;
     return (OK);
     }

/*   =====================================================================   */

int MyCallbacks::Alert(AlertInfo * pInfo,
                       void *      pContext,
                       int *       aiCode)
     {
     printf("Alert : %*.*s\n", 
	    pInfo -> sMessage.iDataLen,
	    pInfo -> sMessage.iDataLen,
	    pInfo -> sMessage.pData);

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

     if (pInfo -> iConnectionId == INTRADAY_HISTORY_CONNECTION_ID)
          {
	  if (pInfo -> iAlertType == ALERT_LOGIN_COMPLETE)
	       {
	       g_iIhLoginStatus = LoginStatus_Complete;
	       }
	  else if (pInfo -> iAlertType == ALERT_LOGIN_FAILED)
	       {
	       g_iIhLoginStatus = LoginStatus_Failed;
	       }
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
     cout << endl;

     /*   ----------------------------------------------------------------   */

     int iIgnored;
     if (!pInfo -> dump(&iIgnored))
	  {
          cout << "error in pInfo -> dump : " << iIgnored << endl;
	  }

     /*   ----------------------------------------------------------------   */

     *aiCode = API_OK;
     return (OK);
     }

/*   =====================================================================   */

int MyCallbacks::BarReplay(BarReplayInfo * pInfo,
			   void *          pContext,
			   int *           aiCode)
     {
     cout << endl;

     /*   ----------------------------------------------------------------   */

     int iIgnored;
     if (!pInfo -> dump(&iIgnored))
	  {
          cout << "error in pInfo -> dump : " << iIgnored << endl;
	  }

     /*   ----------------------------------------------------------------   */

     g_bBarsDone = true;

     /*   ----------------------------------------------------------------   */

     *aiCode = API_OK;
     return (OK);
     }

/*   =====================================================================   */

int main(int      argc,
         char * * argv,
         char * * envp)
     {
     char * USAGE = (char *)("SampleBar user password "
			     "exchange ticker "
			     "bar_type=[r|t|v|s|m|d|w] type_specifier\n");

     /*   ----------------------------------------------------------------   */

     if (argc < 7)
          {
          cout << USAGE << endl;
          return (BAD);
          }

     /*   ----------------------------------------------------------------   */

     REngine *        pEngine       = (REngine *)NULL;
     MyAdmCallbacks * pAdmCallbacks = (MyAdmCallbacks *)NULL;
     RCallbacks *     pCallbacks    = (RCallbacks *)NULL;
     REngineParams    oReParams;
     LoginParams      oLoginParams;
     int              iCode;

     /*   ----------------------------------------------------------------   */

     string sUser          = string(argv[1]);
     string sPassword      = string(argv[2]);
     string sExchange      = string(argv[3]);
     string sTicker        = string(argv[4]);
     string sType          = string(argv[5]);
     string sTypeSpecifier = string(argv[6]);

     char * fake_envp[9];

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

     oReParams.sAppName.pData        = "SampleBar";
     oReParams.sAppName.iDataLen     = (int)strlen(oReParams.sAppName.pData);
     oReParams.sAppVersion.pData     = "1.0.0.0";
     oReParams.sAppVersion.iDataLen  = (int)strlen(oReParams.sAppVersion.pData);
     oReParams.envp                  = fake_envp;
     oReParams.pAdmCallbacks         = pAdmCallbacks;
     oReParams.sLogFilePath.pData    = "sb.log";
     oReParams.sLogFilePath.iDataLen = (int)strlen(oReParams.sLogFilePath.pData);

     /*   ----------------------------------------------------------------   */

     try
          {
          pEngine = new REngine(&oReParams);
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
     /*   Set up parameters for logging in.  The MdCnnctPt and IhCnnctPt     */
     /*   have values for Rithmic Test.   Both are necessary to retrieve     */
     /*   bars.  Sometimes, users have different credentials for market      */
     /*   data and history connect points, but this is a more unusual case.  */
     /*   The code below uses the same credentials for both.                 */

     oLoginParams.pCallbacks           = pCallbacks;

     oLoginParams.sMdCnnctPt.pData     = "login_agent_tpc";
     oLoginParams.sMdCnnctPt.iDataLen  = (int)strlen(oLoginParams.sMdCnnctPt.pData);

     oLoginParams.sMdUser.pData        = const_cast<char*>(sUser.data());
     oLoginParams.sMdUser.iDataLen     = (int)strlen(oLoginParams.sMdUser.pData);

     oLoginParams.sMdPassword.pData    = const_cast<char*>(sPassword.data());
     oLoginParams.sMdPassword.iDataLen = (int)strlen(oLoginParams.sMdPassword.pData);

     oLoginParams.sIhCnnctPt.pData     = "login_agent_historyc";
     oLoginParams.sIhCnnctPt.iDataLen  = (int)strlen(oLoginParams.sIhCnnctPt.pData);

     oLoginParams.sIhUser              = oLoginParams.sMdUser;
     oLoginParams.sIhPassword          = oLoginParams.sMdPassword;

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

     while (g_iIhLoginStatus != LoginStatus_Complete &&
	    g_iIhLoginStatus != LoginStatus_Failed)
          {
#ifndef WinOS
          sleep(1);
#else
          Sleep(1000);
#endif
          }

     if (g_iIhLoginStatus == LoginStatus_Failed)
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

     ReplayBarParams oRbParams;

     /*   ----------------------------------------------------------------   */

     oRbParams.sExchange.pData    = const_cast<char*>(sExchange.data());
     oRbParams.sExchange.iDataLen = (int)strlen(oRbParams.sExchange.pData);

     oRbParams.sTicker.pData      = const_cast<char*>(sTicker.data());
     oRbParams.sTicker.iDataLen   = (int)strlen(oRbParams.sTicker.pData);

     /*   ----------------------------------------------------------------   */
     //   bar type and type specifier

     if (sType[0] == 'r' || sType[0] == 'R')
	  {
	  oRbParams.iType           = BAR_TYPE_RANGE;
	  oRbParams.dSpecifiedRange = atof(sTypeSpecifier.data());
	  }
     else if (sType[0] == 't' || sType[0] == 'T')
	  {
	  oRbParams.iType           = BAR_TYPE_TICK;
	  oRbParams.iSpecifiedTicks = atoi(sTypeSpecifier.data());
	  }
     else if (sType[0] == 'v' || sType[0] == 'V')
	  {
	  oRbParams.iType             = BAR_TYPE_VOLUME;
	  oRbParams.llSpecifiedVolume = (long)atoi(sTypeSpecifier.data());
	  }
     else if (sType[0] == 's' || sType[0] == 'S')
	  {
	  oRbParams.iType             = BAR_TYPE_SECOND;
	  oRbParams.iSpecifiedSeconds = atoi(sTypeSpecifier.data());
	  }
     else if (sType[0] == 'm' || sType[0] == 'M')
	  {
	  oRbParams.iType             = BAR_TYPE_MINUTE;
	  oRbParams.iSpecifiedMinutes = atoi(sTypeSpecifier.data());
	  }
     else if (sType[0] == 'd' || sType[0] == 'D')
	  {
	  // daily bars do not support <N> day bars
	  oRbParams.iType = BAR_TYPE_DAILY;
	  }
     else if (sType[0] == 'w' || sType[0] == 'W')
	  {
	  // weekly bars do not support <N> week bars
	  oRbParams.iType = BAR_TYPE_WEEKLY;
	  }
     else
	  {
	  printf("%s", USAGE);
	  return (BAD);
	  }

     /*   ----------------------------------------------------------------   */

     //   oRbParams.bCustomSession : indicates whether the actual trading
     //   session or a custom session should be used to calculate bars.  A
     //   custom session is specified in seconds-since-midnight (ssm).
     //   For more information, see documentation on :
     //      ReplayBarParams::bCustomSession
     //      ReplayBarParams::iOpenSsm
     //      ReplayBarParams::iCloseSsm

     /*   ----------------------------------------------------------------   */

     //   oRbParams.bProfile : set to true if you want to retrieve volume
     //   profile metrics with minute bars.

     /*   ----------------------------------------------------------------   */
     //   This section specifies the time period over which bars are being
     //   requested.  Daily and weekly bars use date strings in the form 
     //   CCYYMMDD to specify the date range over which bars are being 
     //   requested.  (See ReplayBarParams::sStart/EndDate).  Other bar types
     //   use the unix time seconds-since-the-beginning-of-epoch 
     //   format.  (See https://en.wikipedia.org/wiki/Unix_time for more info.)
     //
     //   NOTE : You may wish to use more modern C++ for your own date/time
     //          operations, but this sample code is written for a wide range
     //          of compilers and versions.

     if (oRbParams.iType == BAR_TYPE_DAILY ||
	 oRbParams.iType == BAR_TYPE_WEEKLY)
	  {
	  time_t ttNow = time(NULL);
	  tm *   tmNow = localtime(const_cast<time_t*>(&ttNow));

	  char cStartDate[64];  // 8 chars for date, 1 for null terminator
	  char cEndDate[64];    // 8 chars for date, 1 for null terminator

	  sprintf(cEndDate, 
		  "%4d%02d%02d",
		  tmNow -> tm_year + 1900,      // 1900 based
		  tmNow -> tm_mon + 1,          // zero based
		  tmNow -> tm_mday);            // not zero based

	  sprintf(cStartDate, 
		  "%4d%02d%02d",
		  tmNow -> tm_year + 1900 - 1,  // for one year of bars
		  tmNow -> tm_mon + 1,
		  tmNow -> tm_mday);

	  oRbParams.sStartDate.pData    = (char *)&cStartDate;
	  oRbParams.sStartDate.iDataLen = (int)strlen(oRbParams.sStartDate.pData);
	  oRbParams.sEndDate.pData    = (char *)&cEndDate;
	  oRbParams.sEndDate.iDataLen = (int)strlen(oRbParams.sEndDate.pData);

	  printf("start : %*.*s\n",
		 oRbParams.sStartDate.iDataLen,
		 oRbParams.sStartDate.iDataLen,
		 oRbParams.sStartDate.pData);
	  printf("  end : %*.*s\n",
		 oRbParams.sEndDate.iDataLen,
		 oRbParams.sEndDate.iDataLen,
		 oRbParams.sEndDate.pData);
	  }
     else
	  {
	  // For the purposes of this sample, use the most recent 24 hours, 
	  // in whole seconds.

	  // Get current system time in ssboe
	  oRbParams.iEndSsboe = (int)time(NULL);
	  oRbParams.iEndUsecs = 0;

	  // subtract 24 hrs * 60 min * 60 sec
	  oRbParams.iStartSsboe = oRbParams.iEndSsboe - (24 * 60 * 60);
	  oRbParams.iStartUsecs = 0;
	  }

     /*   ----------------------------------------------------------------   */

     oRbParams.pContext = (void *)&oRbParams;
 
     /*   ----------------------------------------------------------------   */

     if (!pEngine -> replayBars(&oRbParams, &iCode))
          {
          cout << "REngine::replayBars() error : " << iCode << endl;

          delete pEngine;
          delete pCallbacks;
          delete pAdmCallbacks;

          return (BAD);
          }

     /*   ----------------------------------------------------------------   */
     /*   A number of bars will be returned via RCallbacks::Bar().  When     */
     /*   the bars are done, RCallbacks::BarReplay() will be invoked,        */
     /*   indicating that the request is complete, and conveying status      */
     /*   info.                                                              */

     while (!g_bBarsDone)
          {
#ifndef WinOS
          sleep(1);
#else
          Sleep(1000);
#endif
          }

     /*   ----------------------------------------------------------------   */

     delete pEngine;
     delete pCallbacks;
     delete pAdmCallbacks;

     /*   ----------------------------------------------------------------   */

     return (GOOD);
     }

/*   =====================================================================   */

