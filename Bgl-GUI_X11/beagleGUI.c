/* beagleGUI.c -- -*- C -*-
 * 
 *-  BEAGLE GUI part main program
 *-  There ara graphics preparation routins.
 * 
 * Author          : YOSHIKAWA Tooru
 * Created On      : Mon Aug 19 17:43:27 1996
 * Last Modified By: YOSHIKAWA Tooru
 * Last Modified On: Thu Oct 14 18:38:17 1999
 * Update Count    : 649
 * Status          : 
 */

#include "clnt.h"
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/cursorfont.h>

#include "beagleUI.h"
#include "beagleGUI.h"
#include "beagleUTIL_SockMgr.h"
#include "beagleRoutines.h"
#include "beagleMessageMgr.h"

#define	MAIN_DEF
#include "beagleGUI_Global.h"
#undef	MAIN_DEF

#include "beagleGUI_CommonActions.h"
#include "beagleGUI_TopWindow.h"
#include "beagleGUI_StatsWindow.h"
#include "beagleGUI_PlanWindow.h"
#include "beagleGUI_MessageWindow.h"
#include "beagleGUI_OverviewWindow.h"
#include "beagleGUI_OvInstWindow.h"
#include "beagleGUI_OvInfoWindow.h"
#include "beagleGUI_OvGeneWindow.h"
#include "beagleGUI_HistoWindow.h"
#include "beagleGUI_TPingSWindow.h"
#include "beagleGUI_TPingCWindow.h"
#include "beagleGUI_VarWindow.h"
#include "beagleGUI_DebugWindow.h"
#include "beagleGUI_DebugKeyWaitWindow.h"
#include "beagleGUI_MigrationWindow.h"
#include "beagleGUI_QueryWindow.h"
#include "beagleGUI_InfoWindows.h"
#include "beagleGUI_KeyInWindow.h"
#include "beagleGUI_InfoMessageWindow.h"



/*
 *- SPECIAL VALIABLE NAME
 *- =====================
 *- (If this window have some special variable then write here.)
 */

static XtAppContext	app_con;
static Widget		toplevel = 0;


/*
 *- HERE ARE EXTERN FUNCTION DEFINEs.
 *- ================================
 *- There are some routines that accessing to beagle core routines.
 *- And there are some routines for accessing GUI Services.
 */


/*
 *- HERE ARE GLOBAL VARIABLE (EFFECTIVE IN THIS FILE ONLY) DEFINEs.
 *- ===============================================================
 *- So another files functions cannot search these.
 *- I want to avoid from linking trouble.
 */
typedef struct {
  Pixel    creature00pixel;
  Pixel    creature01pixel;
  Pixel    creature02pixel;
  Pixel    creature03pixel;
  Pixel    creature04pixel;
  Pixel    creature05pixel;
  Pixel    creature06pixel;
  Pixel    creature07pixel;
  Pixel    creature08pixel;
  Pixel    creature09pixel;
} AppData, *AppDataPtr;

AppData app_data;

/* following color resource data are not active yet. */
static XtResource  resources[] = {
  {"creature00color", "Creature00Color", XtRPixel, sizeof(Pixel),
     XtOffsetOf(AppData, creature00pixel), XtRString, "red"
  }
  ,{"creature01color", "Creature01Color", XtRPixel, sizeof(Pixel),
      XtOffsetOf(AppData, creature01pixel), XtRString, "blue"
  }
  ,{"creature02color", "Creature02Color", XtRPixel, sizeof(Pixel),
      XtOffsetOf(AppData, creature02pixel), XtRString, "pink"
  }
  ,{"creature03color", "Creature03Color", XtRPixel, sizeof(Pixel),
      XtOffsetOf(AppData, creature03pixel), XtRString, "yellow"
  }
  ,{"creature04color", "Creature04Color", XtRPixel, sizeof(Pixel),
      XtOffsetOf(AppData, creature04pixel), XtRString, "cyan"
  }
  ,{"creature05color", "Creature05Color", XtRPixel, sizeof(Pixel),
      XtOffsetOf(AppData, creature05pixel), XtRString, "orange"
  }
  ,{"creature06color", "Creature06Color", XtRPixel, sizeof(Pixel),
      XtOffsetOf(AppData, creature06pixel), XtRString, "green"
  }
  ,{"creature07color", "Creature07Color", XtRPixel, sizeof(Pixel),
      XtOffsetOf(AppData, creature07pixel), XtRString, "brown"
  }
  ,{"creature08color", "Creature08Color", XtRPixel, sizeof(Pixel),
      XtOffsetOf(AppData, creature08pixel), XtRString, "violet"
  }
  ,{"creature09color", "Creature09Color", XtRPixel, sizeof(Pixel),
      XtOffsetOf(AppData, creature09pixel), XtRString, "sky blue"
  }
};


static XtActionsRec actions[] = {
  /*---  in beagleGUI_CommonActions.c  ---*/
  {"positionCascPullDownMenu",	(XtActionProc)PositionCascPullDownMenu},
  {"cascMenuPopup",		(XtActionProc)CascMenuPopup},
  {"cascMenuEnter",		(XtActionProc)CascMenuEnter},
  {"allCascMenuPopdown",	(XtActionProc)AllCascMenuPopdown},
  /*---  in beagleGUI_OverviewWindow.c  ---*/
  {"redrawOverviewMemFrame",	(XtActionProc)RedrawOverviewMemFrame},
  {"ov_PutCursorValue",		(XtActionProc)Ov_PutCursorValue},
  {"ov_ConfirmCursorValue",	(XtActionProc)Ov_ConfirmCursorValue},
  {"ov_QuitWindow",		(XtActionProc)Ov_QuitWindow},
  {"ov_EnterWindow",		(XtActionProc)Ov_EnterWindow},
  {"ov_StickylabelUpdate",	(XtActionProc)Ov_StickylabelUpdate},
  {"ov_MouseRedo",		(XtActionProc)Ov_MouseRedo},
  /*--- in beagleGUI_OvInfoWindow.c  ---*/
  {"redrawOvInfoArea",		(XtActionProc)RedrawOvInfoArea},
  /*---  in beagleGUI_KeyInWindow.c  ---*/
  {"activateText",		(XtActionProc)ActivateText},
  {"swapActiveText",		(XtActionProc)SwapActiveText},
  /*---  in beagleGUI_HistoWindow.c  ---*/
  {"redrawHistoInfoArea",	(XtActionProc)RedrawHistoInfoArea},
  /*---  in beagleGUI_TPingSWindow.c  ---*/
  {"tPingSInfoTextDecode",	(XtActionProc)TPingSInfoTextDecode},
  /*---  in beagleGUI_TPingWindow.c  ---*/
  {"tPingCInfoTextDecode",	(XtActionProc)TPingCInfoTextDecode}
};


static  String fallback_resources[] = {
  "*Font:			-adobe-*-*-*-*-*-12-*-*-*-*-*-*-*"
  ,"*confDialog*Font:		-adobe-*-*-*-*-*-16-*-*-*-*-*-*-*"
  ,"*infoDialog*Font:		-adobe-*-*-*-*-*-16-*-*-*-*-*-*-*"
  ,"*warningDialog*Font:	-adobe-*-*-*-*-*-16-*-*-*-*-*-*-*"
  ,"*errorDialog*Font:		-adobe-*-*-*-*-*-16-*-*-*-*-*-*-*"
  ,"*messageDialog*Font:	-adobe-*-*-*-*-*-16-*-*-*-*-*-*-*"
  ,"*menuBar.allowShellResize:	False"
  ,"*menuBar.resizable:		False"

  ,"*fileButton.label:	File"
  ,"*fileButton.resizable:	False"
  ,"*fileCloseButton.label:	Close this window"

  ,"*connectButton.label:	Connect"
  ,"*connectButton.resizable:	False"
  ,"*topNewConnectButton.label:	New Connection"

  ,"*windowButton.label:	Window"
  ,"*windowButton.resizable:	False"
  ,"*windowCloseButton.label:	Close This window"
  ,"*OKButton.label:		OK"
  ,"*YesButton.label:		Yes"
  ,"*NoButton.label:		No"
  ,"*CancelButton.label:       	Cancel"

  ,"*miscButton.label:	Misc"
  ,"*miscButton.resizable:	False"

  ,"*modeButton.label:	Mode"
  ,"*modeButton.resizable:	False"
  ,"*displayButton.label:	Display"
  ,"*modeButton.resizable:	False"
  ,"*arrow.label:	->"
  ,"*arrow.resizable:	False"
  ,NULL
};



/*
 *- HERE ARE STATIC FUNCTIONs PROTOTYPE DEFINEs.
 *- ============================================
 *- These functions call from in this file only.
 *- So another files functions cannot search these.
 *- I want to avoid from linking trouble.
 */
static void	SetupDefaultValues();


/*
 *- HERE ARE THIS files SERVICE FUNCTIONs FOR CALL FROM BEAGLE CORE.
 *- ================================================================
 *- I wrote prototype define in '../Bglclnt/beagleUI.h'.
 */
void BglSetupUI(sP, argc, argv)
     BglSockSt* sP;
     int* argc;
     char** argv;
{
  /* Create real base window */
  toplevel = XtVaAppInitialize(&app_con, "Beagle", NULL, 0,
			       argc, argv, fallback_resources,
			       NULL);
  XtGetApplicationResources(toplevel, (XtPointer)&app_data,
			    resources, XtNumber(resources), NULL, 0);

  XtAppAddActions(app_con, actions, XtNumber(actions));

  SetupDefaultValues();
}


/*
 *  Create and prepare the base frame and all widgets.
 */
void BglCreateBaseFrame(sP)
     BglSockSt* sP;
{  
  InfoWindowPutTop(toplevel);
  CreateConfirmationDialog(toplevel);
  CreateInformationDialog(toplevel);
  CreateWarningDialog(toplevel);
  CreateMessageDialog(toplevel);

  CreateStrKeyInDialog(toplevel);
  CreateSecretStrKeyInDialog(toplevel);

  CreateStatsWindow(toplevel);
  CreatePlanWindow(toplevel);
  CreateMessageWindow(toplevel);

  CreateOvInstWindow(toplevel);
  CreateOvInfoWindow(toplevel);
  CreateOvGeneWindow(toplevel);
  CreateOverviewWindow(toplevel);

  CreateHistoWindow(toplevel);

  CreateTPingSWindow(toplevel);
  CreateTPingCWindow(toplevel);

  CreateVarWindow(toplevel);

  CreateDebugKeyWaitWindow(toplevel);
  CreateDebugWindow(toplevel);

  CreateMigrationWindow(toplevel);

  CreateQueryWindow(toplevel);

  CreateInfoMessageWindow(toplevel);
  CreateTopWindow(toplevel);

  XtRealizeWidget(toplevel);
}



/*
 *  Infinite loop
 */
void BglEventLoop(sP)
     BglSockSt* sP;
{
  /* XtAppMainLoop(app_con); */
  while(1){
    if (XtAppPending(app_con)){
      XEvent event;
      XtAppNextEvent(app_con, &event);
      XtDispatchEvent(&event);
    }
    else{
      BglOneAction(sP);
    }
  }
}


/*
 *- HERE IS THIS files SERVICE FUNCTIONs FOR CALL FROM InformationWindows.
 *- ======================================================================
 *- I wrote prototype define in 'beagleGUI_InfoWindows.c'.
 */
int SelectWaitLoop(w, v)
     Widget w;
     int* v;
{
  *v = 0;
  XtPopup(w, XtGrabExclusive);

  while(*v == 0){
    if (XtAppPending(app_con)){
      XEvent event;
      XtAppNextEvent(app_con, &event);
      XtDispatchEvent(&event);
    }
    else{
      /*
       * BglOneAction (UTIL_Sock_GetCurrentSockStP());
       */
    }
  }

  return *v;
}


/*
 *- HERE ARE THIS files SERVICE FUNCTIONs FOR REQUEST CONVERSION
 *- from BEGLE CORE's UI request to X11 representation.
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
  sx, sy, sa, se; /* Not Use in this function */
  if ( toplevel && XtIsRealized(toplevel) ){
    switch(sc) {
    case BGL_ERR_SCR: InfoInfoWrite(str);	break;
    case BGL_MES_SCR: InfoInfoWrite(str);	break;
    case BGL_DIALOG_SCR: InformationMessage(str);  break;
    case BGL_ROOT_SCR: TopDisplayConnectionPrint(str); break;
    default:	       MessageMessage(str);	break;
    }
  }
  else{
/*
 * tfprintf(stdout, "scr=%d, scr_x=%d, scr_y=%d, scr_a=%d, slee=%d, str=%s\n",
 *	    (int)sc, (int)sx, (int)sy, (int)sa, (int)se, str);
 */
    switch(sc) {
    case BGL_ERR_SCR:
      (void)tfprintf(stderr, "%s\n", str);
      break;
    default:
      (void)tfprintf(stdout, "%s\n", str);
      break;
    }
  }
}

char*	BglUI_StrInput(msg)
     I8s* msg;
{
  static char	getStrBuf[100];
  static String p_getStrBuf[] = {NULL};

  if ( toplevel && XtIsRealized(toplevel) ){
    p_getStrBuf[0] = NULL;
    StrKeyIn(p_getStrBuf, 1, (String *)&msg);
    return (char *)*p_getStrBuf;
  }
  else{
    tfprintf( stdout, msg );
    tfgets( getStrBuf, 100, stdin );
    return getStrBuf;
  }
}

char*	BglUI_SecretStrInput(msg)
     I8s* msg;
{
  static char	getStrBuf[100];

  if ( toplevel && XtIsRealized(toplevel) ){
    strncpy( getStrBuf, SecretStrKeyIn(msg), 100 );
  }
  else{
    tfprintf( stdout, msg );
    BglTtyRaw( 0 );
    tfgets( getStrBuf, 100, stdin );
    BglTtyReset( 0 );
  }
  return getStrBuf;
}
void	BglUI_MainMenu(){ /* NOP */ }
void	BglUI_MicroDebuggerKeyWait(){	StartDebugKeyWait(); }
void	BglUI_Connection_Changed(){
  TopDisplayConnectionChange();
  TopDisplayResumeLifeLoop();
  DebugWindowResetMapCode();
}
void	BglUI_CommunicationCondition(str)
     char* str;
{
  TopDisplayCondition(str);
}
void	BglUI_SizeHist_Changed(){	UpdateSizeHistData(); }
void	BglUI_GeneHist_Changed(){	UpdateGeneHistData(); }
void	BglUI_MemHist_Changed(){	UpdateMemHistData(); }
void	BglUI_ESHist_Changed(){		UpdateESHistData(); }
void	BglUI_EGHist_Changed(){		UpdateEGHistData(); }
void	BglUI_ReplyMes_Changed(){	UpdateRepMessageData(); }
void	BglUI_Stats_Changed(){		UpdateStatsData(); }
void	BglUI_Plan_Changed(){		UpdatePlanData();}
void	BglUI_Message_Changed(){	UpdateMessageData(); }
void	BglUI_TPingSStatsToOn(){
  TopDisplayTPingSStatsToOn();
  TPingSAreaClean();
}
void	BglUI_TPingCStatsToOn(){
  TopDisplayTPingCStatsToOn();
  TPingCAreaClean();
}
void	BglUI_TPingSStatsToOff(){	TopDisplayTPingSStatsToOff(); }
void	BglUI_TPingCStatsToOff(){	TopDisplayTPingCStatsToOff(); }
void	BglUI_TPingS_Changed(){		UpdateTPingSInfo(); }
void	BglUI_TPingC_Changed(){		UpdateTPingCInfo(); }
void	BglUI_HistClear_Changed(){	HistoAreaClean(); }
void	BglUI_OvFrame_Changed(){	UpdateOvFrameData(); }
void	BglUI_Ov_Changed(){		UpdateOvData(); }
void	BglUI_OvGInst_Changed(){	UpdateOvGInstData(); }
void	BglUI_OvG_Changed(){		UpdateOvGeneData(); }
void	BglUI_SizeQuery_Changed(){	UpdateQuerySizeData(); }
void	BglUI_MCState_Changed(){	TopDisplayMCStateChange(); }
void	BglUI_MCSpy_Changed(){		UpdateDebugInfo(); }
void	BglUI_MicroDebuggerToKeypressMode(){
  TopDisplayMicroDebuggerToKeypressMode();
}
void	BglUI_MicroDebuggerToDelayMode(){
  TopDisplayMicroDebuggerToDelayMode();
}
void	BglUI_MicroDebuggerToOffMode(){
  TopDisplayMicroDebuggerToOffMode();
}
void	BglUI_Var_Changed(){		UpdateVarData(); }
void	BglUI_MigrationStatsToOn(){	SetMigrationDisplayToOn(); }
void	BglUI_MigrationStatsToOff(){	SetMigrationDisplayToOff();}
void	BglUI_Migration_Changed(){	UpdateMigrationInfo(); }
void	BglUI_SuspendLifeLoop(){	TopDisplaySuspendLifeLoop();}
void	BglUI_ResumeLifeLoop(){		TopDisplayResumeLifeLoop();}


 

/*
 *- HERE ARE STATIC FUNCTIONs DEFINITION FOR CALL FROM IN THIS FILE ONLY.
 *- =====================================================================
 *- So another files functions cannot search these.
 *- I want to avoid from linking trouble.
 */
static void
SetupDefaultValues()
{
  Colormap	cmap;
  XColor	colorcell_def, rgb_db_def;


  display = XtDisplay(toplevel);
  screen = XtScreen(toplevel);

  default_gc = XCreateGC(display, DefaultRootWindow(display),
			 (Drawable)NULL, NULL);


  cmap = DefaultColormap(display, DefaultScreen(display));

  text_gc    = XCreateGC(display, DefaultRootWindow(display),
			 (Drawable)NULL, NULL);
  black_gc   = XCreateGC(display, DefaultRootWindow(display),
			 (Drawable)NULL, NULL);
  XAllocNamedColor(display, cmap,	"black",
		   &colorcell_def, &rgb_db_def);
  XSetForeground(display, text_gc, colorcell_def.pixel);
  XSetForeground(display, black_gc, colorcell_def.pixel);


  white_gc   = XCreateGC(display, DefaultRootWindow(display),
			 (Drawable)NULL, NULL);
  XAllocNamedColor(display, cmap,	"white",
		   &colorcell_def, &rgb_db_def);
  XSetForeground(display, white_gc, colorcell_def.pixel);

  gray_gc    = XCreateGC(display, DefaultRootWindow(display),
			 (Drawable)NULL, NULL);
  XAllocNamedColor(display, cmap,	"DimGrey",
		   &colorcell_def, &rgb_db_def);
  XSetForeground(display, gray_gc, colorcell_def.pixel);


  colorGC[0] = XCreateGC(display, DefaultRootWindow(display),
			 (Drawable)NULL, NULL);
  XSetForeground(display, colorGC[0], app_data.creature00pixel);

  colorGC[1] = XCreateGC(display, DefaultRootWindow(display),
			 (Drawable)NULL, NULL);
  XSetForeground(display, colorGC[1], app_data.creature01pixel);

  colorGC[2] = XCreateGC(display, DefaultRootWindow(display),
			 (Drawable)NULL, NULL);
  XSetForeground(display, colorGC[2], app_data.creature02pixel);

  colorGC[3] = XCreateGC(display, DefaultRootWindow(display),
			 (Drawable)NULL, NULL);
  XSetForeground(display, colorGC[3], app_data.creature03pixel);

  colorGC[4] = XCreateGC(display, DefaultRootWindow(display),
			 (Drawable)NULL, NULL);
  XSetForeground(display, colorGC[4], app_data.creature04pixel);

  colorGC[5] = XCreateGC(display, DefaultRootWindow(display),
			 (Drawable)NULL, NULL);
  XSetForeground(display, colorGC[5], app_data.creature05pixel);

  colorGC[6] = XCreateGC(display, DefaultRootWindow(display),
			 (Drawable)NULL, NULL);
  XSetForeground(display, colorGC[6], app_data.creature06pixel);

  colorGC[7] = XCreateGC(display, DefaultRootWindow(display),
			 (Drawable)NULL, NULL);
  XSetForeground(display, colorGC[7], app_data.creature07pixel);

  colorGC[8] = XCreateGC(display, DefaultRootWindow(display),
			 (Drawable)NULL, NULL);
  XSetForeground(display, colorGC[8], app_data.creature08pixel);

  colorGC[9] = XCreateGC(display, DefaultRootWindow(display),
			 (Drawable)NULL, NULL);
  XSetForeground(display, colorGC[9], app_data.creature09pixel);

  OvCurs = XCreateFontCursor(display, XC_crosshair);

}
