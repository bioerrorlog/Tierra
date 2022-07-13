/* bglstd_main.c -- -*- C -*-
 * 
 * This work has been done at ATR HIP
 * 
 * Author          : YOSHIKAWA Tooru
 * Created On      : Fri Aug 30 14:20:52 1996
 * Last Modified By: YOSHIKAWA Tooru
 * Last Modified On: Wed Mar 24 17:22:00 1999
 * Update Count    : 149
 * Status          : 
 */

#include "clnt.h"

#define	BGLSTD_GLOBAL_VALUE_DEFINE
#include "bglstd_global.h"
#undef	BGLSTD_GLOBAL_VALUE_DEFINE

#include "bglstd_prot.h"
#include "bglstd_show.h"
#include "beagleUI.h"
#include "beagleRoutines.h"


/*
 *- HERE ARE THIS files SERVICE FUNCTIONs FOR CALL FROM BEAGLE CORE.
 *- ================================================================
 *- I wrote prototype define in '../Bglclnt/beagleUI.h'.
 */
void BglCreateBaseFrame(s)
     BglSockSt* s;
{
  /* Empty !! */
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
void BglEventLoop(s)
     BglSockSt* s;
{
  while(1){
    if ( BglC_Menu )      BglStdMainMenu();
    BglOneAction (s);
  }
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
#ifdef __TURBOC__
int Bgl_sig_int(void)
#endif /* __TURBOC__ */
#ifdef __MWERKS__
void Bgl_sig_int(sig)
I32s sig;
#endif /* __MWERKS__ */

#ifdef __TURBOC__
int Bgl_sig_int(void)
#endif /* __TURBOC__ */
#ifdef __MWERKS__
void Bgl_sig_int(sig)
I32s sig;
#endif /* __MWERKS__ */
#ifdef unix
void Bgl_sig_int(sig)
int sig;
#endif /* unix */
#ifdef DECVAX
void Bgl_sig_int(int sig)
#endif /* DECVAX */

#ifdef DECVAX
void Bgl_sig_int(sig)
     int sig;
#endif /* DECVAX */
{
#ifdef SIGBLOCK
  sigset_t SigSaveSet;

  SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */

#ifdef DECVAX
  TieSignal(SIGINT, SIG_DFL);
  TieSignal(SIGINT, Bgl_sig_int);
#else /* DECVAX */
  TieSignal(SIGINT, Bgl_sig_int);
#endif /* __linux__ */

#ifdef SIGBLOCK
  TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */


  BglC_Menu = 1;
  DebugPrint1("Bgl_sig_int: BglC_Menu = %d\n", BglC_Menu);
}


/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
void BglSetupUI(s, argc, argv)
     BglSockSt* s;
     int* argc;
     char** argv;
{
  BglC_Menu = 0;  /* flag unix sigint, to do a menu */
  TieSignal(SIGINT, Bgl_sig_int);
}

/*
 *- HERE ARE THIS files SERVICE FUNCTIONs FOR REQUEST CONVERSION
 *- from BEGLE CORE's UI request to stdio representation.
 *- ================================================================
 *- I wrote prototype define in '../Bglclnt/beagleUI.h'.
 */
void	BglUI_Exit(eno)
     I32s eno;
{
  exit(eno);
}

void	BglUI_Printf(sc, sx, sy, sa, se, str)
     I32s sc;
     I32s sx, sy;
     I32s sa;
     I32s se;
     I8s* str;
{
  BglStdPrintf(sc, sx, sy, sa, se, str);
}

char*	BglUI_StrInput(msg)
     char* msg;
{
  static char	getStrBuf[100];
  tfprintf( stdout, msg );
  tfgets( getStrBuf, 100, stdin );
  return getStrBuf;
}

char*	BglUI_SecretStrInput(msg)
     char* msg;
{
  static char	getStrBuf[100];
  tfprintf( stdout, msg );
  BglTtyRaw( 0 );
  tfgets( getStrBuf, 100, stdin );
  BglTtyReset( 0 );
  return getStrBuf;
}

void	BglUI_MainMenu(){		BglStdMainMenu(); }
void	BglUI_MicroDebuggerKeyWait(){	BglStdMisc_mk();}
void	BglUI_Connection_Changed(){  /* NOP */}
void	BglUI_CommunicationCondition(char* str){	DebugPrint(str);}
void	BglUI_SizeHist_Changed(){	BglStdShowSizHist();}
void	BglUI_GeneHist_Changed(){	BglStdShowGenHist();}
void	BglUI_MemHist_Changed(){	BglStdShowMemHist();}
void	BglUI_ESHist_Changed(){		BglStdShowESHist();}
void	BglUI_EGHist_Changed(){		BglStdShowEGHist();}
void	BglUI_ReplyMes_Changed(){	BglStdShowReplyMes();}
void	BglUI_Stats_Changed(){		BglStdShowStats();}
void	BglUI_Plan_Changed(){		BglStdShowPlan();}
void	BglUI_Message_Changed(){	BglStdShowMessage();}
void	BglUI_TPingSStatsToOn(){	BglStdShowTPingSStatsToOn();}
void	BglUI_TPingCStatsToOn(){	BglStdShowTPingCStatsToOn();}
void	BglUI_TPingSStatsToOff(){	BglStdShowTPingSStatsToOff();}
void	BglUI_TPingCStatsToOff(){	BglStdShowTPingCStatsToOff();}
void	BglUI_TPingS_Changed(){		BglStdShowTPingS(); }
void	BglUI_TPingC_Changed(){		BglStdShowTPingC(); }
void	BglUI_HistClear_Changed(){	BglStdShowHistClear(); }
void	BglUI_OvFrame_Changed(){	BglStdShowOvFrame(); }
void	BglUI_Ov_Changed(){		BglStdShowOv(); }
void	BglUI_OvGInst_Changed(){	BglStdShowOvGInst(); }
void	BglUI_OvG_Changed(){		BglStdShowOvG(); }
void	BglUI_SizeQuery_Changed(){	BglStdShowSizQuery(); }
void	BglUI_MCState_Changed(){	BglStdShowMCState(); }
void	BglUI_MCSpy_Changed(){		BglStdShowMCSpy(); }
void	BglUI_MicroDebuggerToKeypressMode(){
  BglStdShowMicroDebuggerToKeypressMode();
}
void	BglUI_MicroDebuggerToDelayMode(){
  BglStdShowMicroDebuggerToDelayMode();
}
void	BglUI_MicroDebuggerToOffMode(){
  BglStdShowMicroDebuggerToOffMode();
}
void	BglUI_Var_Changed(){		BglStdShowVar();}
void	BglUI_MigrationStatsToOn(){	/* NOP */ }
void	BglUI_MigrationStatsToOff(){	/* NOP */ }
void	BglUI_Migration_Changed(){	BglStdShowMigrationTracking(); }
void	BglUI_SuspendLifeLoop(){	/* NOP */ }
void	BglUI_ResumeLifeLoop(){		/* NOP */ }
