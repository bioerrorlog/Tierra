/* beagleGUI_TopWindow.c -- -*- C -*-
 * 
 *-  BEAGLE first window.
 *-  You can open another information window from "Window" menu.
 *-  This window hove a information area and some some information
 *-  (about for this program) write there.
 * 
 * Author          : YOSHIKAWA Tooru
 * Created On      : Wed Aug 21 14:13:49 1996
 * Last Modified By: YOSHIKAWA Tooru
 * Last Modified On: Mon Nov 22 09:43:22 1999
 * Update Count    : 1226
 * Status          : 
 */

#include <stdio.h>

#include "clnt.h"
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>

#include <X11/Xaw/Cardinals.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/Box.h>
#include <X11/Xaw/Viewport.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Paned.h>
#include <X11/Xaw/MenuButton.h>
#include <X11/Xaw/SimpleMenu.h>
#include <X11/Xaw/SmeBSB.h>
#include <X11/Xaw/SmeLine.h>
#include <X11/Xaw/Label.h>
#include <X11/Xaw/List.h>
#include <X11/Xaw/AsciiText.h>

#include "beagleMessageMgr.h"
#include "beagleUTIL_SockMgr.h"
#include "beagleGUI_Global.h"
#include "beagleGUI_TopWindow.h"
#include "beagleGUI_InfoWindows.h"
#include "beagleGUI_KeyInWindow.h"
#include "beagleGUI_OverviewWindow.h"

/*
 *- SPECIAL VALIABLE NAME
 *- =====================
 *- This window show only one variable for another file using the file
 *- 'beagleGUI_Global.h'. It name and type a 'Widget topWindow'.
 *- So you can open this window from every GUI file if need it.
 */


/*
 *- HERE ARE EXTERN FUNCTION DEFINEs.
 *- ================================
 *- This window (topWindow) use here external function only.
 *- So this file need the file include here function fo linking.
 *- I tried to collect there functions in '../Bglclnt/beagleRoutines.c'.
 *- Another GUI functions are define in the 'beagleGUI_TopWindow.h' and
 *- 'beagleMessageMgr.h'.
 */

/*
 *- HERE ARE GLOBAL VARIABLE (EFFECTIVE IN THIS FILE ONLY) DEFINEs.
 *- ===============================================================
 *- So another files functions cannot search these.
 *- I want to avoid from linking trouble.
 */
static Widget	connectOpenButton;
static Widget	topHostNameLabel;
static Widget	topConnectionModeLabel;
static Widget	topConditionLabel;
static char	nowConditionChar=' ';
static Widget	topTPingSLabel;
static Widget	topTPingCLabel;
static Widget	topMigrationLabel;
static Widget	topMCStepLabel;
static Widget	topSliceLabel;
static Widget	topCpuLabel;
static Widget   topConnectLabel;
static Widget	topNOVLabel;
static Widget	topMiscSuspendButton;
static Widget	topMiscResumeButton;

static XtTranslations	buttTransTable;
static String	buttTrans = "#override\n\
<EnterWindow>: set()\n\
<LeaveWindow>: unset()";

static XtTranslations	menuTransTable;
static String menuTrans ="#override\n\
<LeaveWindow>: unhighlight()";

static XtTranslations	connectCascMenuTransTable;
static String	connectCascMenuTrans = "#override\n\
<BtnUp>: allCascMenuPopdown() MenuPopdown(connectPullDownMenu)\n\
<EnterWindow>: cascMenuEnter(connectPullDownMenu)";

static XtTranslations	connect_ArrowTransTable;
static String	connect_ArrowTrans ="#replace\n\
<EnterWindow>: positionCascPullDownMenu(connect_SH_Menu) cascMenuPopup(connect_SH_Menu)";

static char	nameStr[30];
static char	numStr[30];
static Arg	args[1];


/*
 *- HERE ARE STATIC FUNCTIONs PROTOTYPE DEFINEs.
 *- ============================================
 *- These functions call from in this file only.
 *- So another files functions cannot search these.
 *- I want to avoid from linking trouble.
 */

/*- Create windows.*/
static void	CreateMenuBar		P_((void));
static void	PutFileButton		P_((Widget));
static void	PutConnectButton	P_((Widget));
static void	PutWindowButton		P_((Widget));
static void	Create_tPing_Menu	P_((Widget));
static void	Create_message_Menu	P_((Widget));
static void	PutMiscButton		P_((Widget));
static void	PutOptionButton		P_((Widget));
static void	CreateTopInfoArea	P_((void));
/*- Action definition.*/
static void	TopFileSaveButtonPress		P_((Widget));
static void	TopFileSaveAndQuitButtonPress	P_((Widget));
static void	TopFileTierraQuitButtonPress	P_((Widget));
static void	TopFileBeagleExitButtonPress	P_((Widget));
static void	TopNewConnectByScriptReq	P_((Widget));
static void	TopNewConnectByHostNameReq	P_((Widget));
static void	TopNewConnectButtonPress	P_((Widget,int));
static void	TopCloseConnectButtonPress	P_((Widget));
static void	StatsOpenReq		P_((Widget));
static void	PlanOpenReq		P_((Widget));
static void	MessageOpenReq		P_((Widget));
static void	OverviewOpenReq		P_((Widget));
static void	QueryOpenReq		P_((Widget));
static void	HistoOpenReq		P_((Widget));
static void	TPingSOpenReq		P_((Widget));
static void	TPingCOpenReq		P_((Widget));
static void	VarOpenReq		P_((Widget));
static void	DebugOpenReq		P_((Widget));
static void	MigrationOpenReq	P_((Widget));
static void	InfoOpenReq		P_((Widget));
static void	InjectOpenReq		P_((Widget));
static void	InjectOpenReqSend	P_((I32u));
static void	HistLoggingToggleReq	P_((Widget));
static void	SuspendButtonPress	P_((Widget));
static void	ResumeButtonPress	P_((Widget));
static void	ContButtonPress		P_((Widget));
static void	AckTimeOutButtonPress	P_((Widget));
static void	PollingTimeButtonPress	P_((Widget));
static void	TrackingTimeout		P_((Widget));
static void	XDRBufMaxSizeChange	P_((Widget));
/*- This windows process routines -*/
static int	ReMakeConnectionPullDownMenu	P_((void));
static void	CreateConnect_SH_Menu		P_((Widget));
static int	AddOldConnectButtons		P_((Widget,Widget));



/*
 *- HERE ARE THIS WINDOW's SERVICE FUNCTIONs FOR CALL FROM ANOTHER FILEs.
 *- =====================================================================
 *- I wrote prototype define in 'beagleGUI_TopWindow.h'.
 */
void	CreateTopWindow(topWidget)
     Widget topWidget;
{
  topWindow
    = XtVaCreateManagedWidget("topWindow",
			      panedWidgetClass, topWidget,
			      NULL);

  buttTransTable            = XtParseTranslationTable(buttTrans);
  menuTransTable            = XtParseTranslationTable(menuTrans);

  CreateMenuBar();
  CreateTopInfoArea();
}

void	TopDisplayNOVChange(n)
     int n;
{
  tsprintf(bglStrBuf, "n_OV = %d", n);
  XtSetArg(args[0], XtNlabel, bglStrBuf);
  XtSetValues(topNOVLabel, args,1);
}

void	TopDisplayConnectionChange()
{
  tsprintf(bglStrBuf, "HOST = %-32s",
	   UTIL_Sock_GetCurrent_Connection()->addr.host );
  XtSetArg(args[0], XtNlabel, bglStrBuf);
  XtSetValues(topHostNameLabel, args, 1);

  tsprintf(bglStrBuf, "%s",
	   (UTIL_Sock_GetCurrent_Connection()->user.priv == BGL_PRIV_S)?
	   "   SU Mode" : "NonSU Mode" );
  XtSetArg(args[0], XtNlabel, bglStrBuf);
  XtSetValues(topConnectionModeLabel, args, 1);
  (void)ReMakeConnectionPullDownMenu();
}


void	TopDisplayCondition(c)
     char* c;
{
  if ( nowConditionChar != c[0] ){
    nowConditionChar = c[0];
    if (c[0] == '.'){
      XtSetArg(args[0], XtNlabel, "Condition OK");
    }
    else if ( c[0] == '?' ){
      XtSetArg(args[0], XtNlabel, "Condition ??");
    }
    else{
      XtSetArg(args[0], XtNlabel, "Condition   ");
    }
    XtSetValues(topConditionLabel, args, 1);
  }
}

void	TopDisplayTPingSStatsToOn()
{
  XtSetArg(args[0], XtNlabel, "(Server)  Tping information On         ");
  XtSetValues(topTPingSLabel, args, 1);
}

void	TopDisplayTPingCStatsToOn()
{
  XtSetArg(args[0], XtNlabel, "(Cluster) Tping information On         ");
  XtSetValues(topTPingCLabel, args, 1);
}

void	TopDisplayTPingSStatsToOff()
{
  XtSetArg(args[0], XtNlabel, "(Server)  Tping information Off        ");
  XtSetValues(topTPingSLabel, args, 1);
}

void	TopDisplayTPingCStatsToOff()
{
  XtSetArg(args[0], XtNlabel, "(Cluster) Tping information Off        ");
  XtSetValues(topTPingCLabel, args, 1);
}

void	TopDisplayMigrationStatsToOn()
{
  XtSetArg(args[0], XtNlabel, "Migration monitoring On ");
  XtSetValues(topMigrationLabel, args, 1);
}

void	TopDisplayMigrationStatsToOff()
{
  XtSetArg(args[0], XtNlabel, "Migration monitoring Off");
  XtSetValues(topMigrationLabel, args, 1);
}

void	TopDisplayConnectionPrint(str)
     char* str;
{
  XtSetArg(args[0], XtNlabel, str);
  XtSetValues(topConnectLabel, args, 1);
}


void	TopDisplayMCStateChange()
{
  BglMCStateSt* data;
  static I8s	lastMCstep;
  static I8s	lastSlice;
  static I8s	lastCpu;

  while( BglMM_HowManyMCState() ){
    data = BglMM_PickMCState();

    if ( lastMCstep != data->MC_step ){
      lastMCstep = data->MC_step;
      if ( lastMCstep == 'k' )      TopDisplayMicroDebuggerToKeypressMode();
      else if ( lastMCstep == 'd' ) TopDisplayMicroDebuggerToDelayMode();
      else			    TopDisplayMicroDebuggerToOffMode();
    }

    if ( lastSlice != data->Slice ){
      lastSlice = data->Slice;
      tsprintf(bglStrBuf, "Slice = %s",
	       (lastSlice == 'T')?"  Track Cell":"UnTrack Cell");
      XtSetArg(args[0], XtNlabel, bglStrBuf);
      XtSetValues(topSliceLabel, args,1);
    }

    if ( lastCpu != data->Cpu ){
      lastCpu = data->Cpu;
      tsprintf(bglStrBuf, "Cpu = %s",
	       (lastCpu == 'u')?"UnTrack CPU" : "  Track CPU");
      XtSetArg(args[0], XtNlabel, bglStrBuf);
      XtSetValues(topCpuLabel, args,1);
    }
  }
}

void	TopDisplayMicroDebuggerToKeypressMode(){
  XtSetArg(args[0], XtNlabel, "MICRO Debugger Keypress-mode");
  XtSetValues(topMCStepLabel, args,1);
  XtPopup(debugKeyWaitWindow, XtGrabNone);
}

void	TopDisplayMicroDebuggerToDelayMode(){
  XtSetArg(args[0], XtNlabel, "MICRO Debugger Delay-mode");
  XtSetValues(topMCStepLabel, args,1);
  XtPopdown(debugKeyWaitWindow);
}

void	TopDisplayMicroDebuggerToOffMode(){
  XtSetArg(args[0], XtNlabel, "MICRO Debugger Off");
  XtSetValues(topMCStepLabel, args,1);
  XtPopdown(debugKeyWaitWindow);
}

void	TopDisplaySuspendLifeLoop(){
  XtSetArg(args[0], XtNsensitive, False);
  XtSetValues(topMiscSuspendButton, args, 1);
  XtSetValues(topConditionLabel, args, 1);

  XtSetArg(args[0], XtNsensitive, True);
  XtSetValues(topMiscResumeButton, args, 1);
}

void	TopDisplayResumeLifeLoop(){
  XtSetArg(args[0], XtNsensitive, True);
  XtSetValues(topMiscSuspendButton, args, 1);
  XtSetValues(topConditionLabel, args, 1);

  XtSetArg(args[0], XtNsensitive, False);
  XtSetValues(topMiscResumeButton, args, 1);
}


/*
 *- HERE ARE STATIC FUNCTIONs DEFINITION FOR CALL FROM IN THIS FILE ONLY.
 *- =====================================================================
 *- So another files functions cannot search these.
 *- I want to avoid from linking trouble.
 *- -- First part are creating window style.
 *- -- Second part are window actions mechanism.
 *- -- Third part are this windows process routines.
 */

/*
 *- Creating window style.
 *- -- This window has 'dialog', 'menu bar' and 'information area'.
 */
static void	CreateMenuBar()
{
  Widget	menuBar;

  menuBar
    = XtVaCreateManagedWidget("menuBar",
			      boxWidgetClass,	topWindow,
			      XtNorientation,	"horizontal",
			      NULL);
  PutFileButton(menuBar);
  PutConnectButton(menuBar);
  PutWindowButton(menuBar);
  PutMiscButton(menuBar);
  PutOptionButton(menuBar);
}


static void	PutFileButton(w)
     Widget w;
{
  Widget openButton;
  Widget menu, button;

  openButton
    = XtVaCreateManagedWidget("fileButton",
			      menuButtonWidgetClass, w,
			      XtNmenuName,	"filePullDownMenu",
			      NULL);
  menu
    = XtVaCreatePopupShell("filePullDownMenu",
			   simpleMenuWidgetClass, openButton,
			   NULL);
  button
    = XtVaCreateManagedWidget("topFileSaveButton",
			      smeBSBObjectClass,	menu,
			      XtNlabel,		"Save soup",
			      NULL);
  XtAddCallback(button,
		XtNcallback, (XtCallbackProc)TopFileSaveButtonPress,
		NULL);
  button
    = XtVaCreateManagedWidget("topFileSaveAndQuitButton",
			      smeBSBObjectClass,	menu,
			      XtNlabel,		"Save soup and Quit Tierra",
			      NULL);
  XtAddCallback(button,
		XtNcallback, (XtCallbackProc)TopFileSaveAndQuitButtonPress,
		NULL);
  button
    = XtVaCreateManagedWidget("topFileTierraQuitButton",
			      smeBSBObjectClass,	menu,
			      XtNlabel,		"Quit Tierra",
			      NULL);
  XtAddCallback(button,
		XtNcallback, (XtCallbackProc)TopFileTierraQuitButtonPress,
		NULL);
  button
    = XtVaCreateManagedWidget("topFileBeagleExitButton",
			      smeBSBObjectClass,	 menu,
			      XtNlabel,		"Exit Beagle",
			      NULL);
  XtAddCallback(button,
		XtNcallback, (XtCallbackProc)TopFileBeagleExitButtonPress,
		NULL);
}


static void	PutConnectButton(w)
     Widget w;
{
  connectOpenButton
    = XtVaCreateManagedWidget("connectButton",
			      menuButtonWidgetClass, w,
			      XtNmenuName,	"connectPullDownMenu",
			      NULL);

  connectCascMenuTransTable = XtParseTranslationTable(connectCascMenuTrans);
  connect_ArrowTransTable   = XtParseTranslationTable(connect_ArrowTrans);

  (void)ReMakeConnectionPullDownMenu();
}

static void	PutWindowButton(w)
     Widget w;
{
  Widget	openButton, menu, pullDownForm;
  Widget	tPingButton, messageButton;
  String	windowCascMenuTrans = "#override\n\
<BtnUp>: allCascMenuPopdown() MenuPopdown(windowPullDownMenu)\n\
<EnterWindow>: cascMenuEnter(windowPullDownMenu)";
  String	tPing_ArrowTrans ="#replace\n\
<EnterWindow>: positionCascPullDownMenu(tPing_Menu) cascMenuPopup(tPing_Menu)";
  String	message_ArrowTrans ="#replace\n\
<EnterWindow>: positionCascPullDownMenu(message_Menu) cascMenuPopup(message_Menu)";

  openButton
    = XtVaCreateManagedWidget("windowButton",
			      menuButtonWidgetClass, w,
			      XtNmenuName,	"windowPullDownMenu",
			      NULL);
  menu
    = XtVaCreatePopupShell("windowPullDownMenu",
			   overrideShellWidgetClass,	openButton,
			   XtNtranslations,
			   XtParseTranslationTable(windowCascMenuTrans),
			   NULL);

  pullDownForm
    = XtVaCreateManagedWidget("windowPullDownMenuForm",
			      formWidgetClass,		menu,
			      XtNdefaultDistance,	0,
			      NULL);

  statsOpenButton
    = XtVaCreateManagedWidget("statsOpenButton",
			      commandWidgetClass,	pullDownForm,
			      XtNlabel,			"Status              ",
			      XtNborderWidth,		0,
			      XtNtranslations,		buttTransTable,
			      NULL);
  XtAddCallback(statsOpenButton,
		XtNcallback, (XtCallbackProc)StatsOpenReq,
		NULL);

  planOpenButton
    = XtVaCreateManagedWidget("planOpenButton",
			      commandWidgetClass,	pullDownForm,
			      XtNlabel,			"Plan                ",
			      XtNfromVert,		statsOpenButton,
			      XtNborderWidth,		0,
			      XtNtranslations,		buttTransTable,
			      NULL);
  XtAddCallback(planOpenButton,
		XtNcallback, (XtCallbackProc)PlanOpenReq,
		NULL);

  histoOpenButton
    = XtVaCreateManagedWidget("histoOpenButton",
			      commandWidgetClass,	pullDownForm,
			      XtNlabel,			"Histogram           ",
			      XtNfromVert,		planOpenButton,
			      XtNborderWidth,		0,
			      XtNtranslations,		buttTransTable,
			      NULL);
  XtAddCallback(histoOpenButton,
		XtNcallback, (XtCallbackProc)HistoOpenReq,
		NULL);

  queryOpenButton
    = XtVaCreateManagedWidget("queryOpenButton",
			      commandWidgetClass,	pullDownForm,
			      XtNlabel,			"Size Query          ",
			      XtNfromVert,		histoOpenButton,
			      XtNborderWidth,		0,
			      XtNtranslations,		buttTransTable,
			      NULL);
  XtAddCallback(queryOpenButton,
		XtNcallback, (XtCallbackProc)QueryOpenReq,
		NULL);

  Create_tPing_Menu(pullDownForm);
  tPingButton
    = XtVaCreateManagedWidget("tPingCascButton",
			      commandWidgetClass,	pullDownForm,
			      XtNlabel,			"TPing information",
			      XtNfromVert,		queryOpenButton,
			      XtNborderWidth,		0,
			      XtNtranslations,		buttTransTable,
			      NULL);
  (void)XtVaCreateManagedWidget("arrow",
				commandWidgetClass,	pullDownForm,
				XtNfromVert,		queryOpenButton,
				XtNfromHoriz,		tPingButton,
				XtNborderWidth,		0,
				XtNtranslations,
				XtParseTranslationTable(tPing_ArrowTrans),
				NULL);

  varOpenButton
    = XtVaCreateManagedWidget("varOpenButton",
			      commandWidgetClass,	pullDownForm,
			      XtNlabel,			"Variable            ",
			      XtNfromVert,		tPingButton,
			      XtNborderWidth,		0,
			      XtNtranslations,		buttTransTable,
			      NULL);
  XtAddCallback(varOpenButton,
		XtNcallback, (XtCallbackProc)VarOpenReq,
		NULL);

  overviewOpenButton
    = XtVaCreateManagedWidget("overviewOpenButton",
			      commandWidgetClass,	pullDownForm,
			      XtNlabel,			"Overview            ",
			      XtNfromVert,		varOpenButton,
			      XtNborderWidth,		0,
			      XtNtranslations,		buttTransTable,
			      NULL);
  XtAddCallback(overviewOpenButton,
		XtNcallback, (XtCallbackProc)OverviewOpenReq,
		NULL);

  debugOpenButton
    = XtVaCreateManagedWidget("debugOpenButton",
			      commandWidgetClass,	pullDownForm,
			      XtNlabel,			"Debug               ",
			      XtNfromVert,		overviewOpenButton,
			      XtNborderWidth,		0,
			      XtNtranslations,		buttTransTable,
			      NULL);
  XtAddCallback(debugOpenButton,
		XtNcallback, (XtCallbackProc)DebugOpenReq,
		NULL);

  migrationOpenButton
    = XtVaCreateManagedWidget("migrationOpenButton",
			      commandWidgetClass,	pullDownForm,
			      XtNlabel,			"Migration           ",
			      XtNfromVert,		debugOpenButton,
			      XtNborderWidth,		0,
			      XtNtranslations,		buttTransTable,
			      NULL);
  XtAddCallback(migrationOpenButton,
		XtNcallback, (XtCallbackProc)MigrationOpenReq,
		NULL);

  Create_message_Menu(pullDownForm);
  messageButton
    = XtVaCreateManagedWidget("messageCascButton",
			      commandWidgetClass,	pullDownForm,
			      XtNlabel,			"Messages         ",
			      XtNfromVert,		migrationOpenButton,
			      XtNborderWidth,		0,
			      XtNtranslations,		buttTransTable,
			      NULL);
  (void)XtVaCreateManagedWidget("arrow",
				commandWidgetClass,	pullDownForm,
				XtNfromVert,		migrationOpenButton,
				XtNfromHoriz,		messageButton,
				XtNborderWidth,		0,
				XtNtranslations,
				XtParseTranslationTable(message_ArrowTrans),
				NULL);

}


static void	Create_tPing_Menu(w)
     Widget w;
{
  Widget menu;

  menu
    = XtVaCreatePopupShell("tPing_Menu",
			   simpleMenuWidgetClass,	w,
			   XtNtranslations,		menuTransTable,
			   NULL);

  tPingSOpenButton
    = XtVaCreateManagedWidget("tPingSOpenButton",
			      smeBSBObjectClass,	menu,
			      XtNlabel,		"Cluster Server Map",
			      NULL);
  XtAddCallback(tPingSOpenButton,
		XtNcallback, (XtCallbackProc)TPingSOpenReq,
		NULL);

  tPingCOpenButton
    = XtVaCreateManagedWidget("tPingCOpenButton",
			      smeBSBObjectClass,	 menu,
			      XtNlabel,		"Local Cluster Map",
			      NULL);
  XtAddCallback(tPingCOpenButton,
		XtNcallback, (XtCallbackProc)TPingCOpenReq,
		NULL);
}


static void	Create_message_Menu(w)
     Widget w;
{
  Widget menu;

  menu
    = XtVaCreatePopupShell("message_Menu",
			   simpleMenuWidgetClass,	w,
			   XtNtranslations,		menuTransTable,
			   NULL);

  messageOpenButton
    = XtVaCreateManagedWidget("messageOpenButton",
			      smeBSBObjectClass, menu,
			      XtNlabel,		"from Tierra",
			      NULL);
  XtAddCallback(messageOpenButton,
		XtNcallback, (XtCallbackProc)MessageOpenReq,
		NULL);
  infoMessageOpenButton
    = XtVaCreateManagedWidget("infoOpenButton",
			      smeBSBObjectClass, menu,
			      XtNlabel,		"form Beagle",
			      NULL);
  XtAddCallback(infoMessageOpenButton,
		XtNcallback, (XtCallbackProc)InfoOpenReq,
		NULL);
}


static void	PutMiscButton(w)
     Widget w;
{
  Widget	openButton, menu;
  Widget	button;

  openButton
    = XtVaCreateManagedWidget("miscButton",
			      menuButtonWidgetClass, w,
			      XtNmenuName,	"miscPullDownMenu",
			      NULL);
  menu
    = XtVaCreatePopupShell("miscPullDownMenu",
			   simpleMenuWidgetClass, openButton,
			   NULL);
  button
    = XtVaCreateManagedWidget("injectOpenButton",
			      smeBSBObjectClass,	menu,
			      XtNlabel,			"Inject",
			      NULL);
  XtAddCallback(button,
		XtNcallback, (XtCallbackProc)InjectOpenReq,
		NULL);
  button
    = XtVaCreateManagedWidget("histLoggingToggleButton",
			      smeBSBObjectClass,	menu,
			      XtNlabel,			"Hist logging on/off",
			      NULL);
  XtAddCallback(button,
		XtNcallback, (XtCallbackProc)HistLoggingToggleReq,
		NULL);
  (void)XtVaCreateManagedWidget("holizontalLine1",
				smeLineObjectClass,	menu,
				NULL);
  topMiscSuspendButton
    = XtVaCreateManagedWidget("suspendButton",
			      smeBSBObjectClass,	menu,
			      XtNlabel,			"Suspend Life Loop",
			      NULL);
  XtAddCallback(topMiscSuspendButton,
		XtNcallback, (XtCallbackProc)SuspendButtonPress,
		NULL);
  topMiscResumeButton
    = XtVaCreateManagedWidget("resumeButton",
			      smeBSBObjectClass,	menu,
			      XtNlabel,			"Resume Life Loop",
			      XtNsensitive,		 False,
			      NULL);
  XtAddCallback(topMiscResumeButton,
		XtNcallback, (XtCallbackProc)ResumeButtonPress,
		NULL);
  (void)XtVaCreateManagedWidget("holizontalLine2",
				smeLineObjectClass,	menu,
				NULL);
  button
    = XtVaCreateManagedWidget("contButton",
			      smeBSBObjectClass,	menu,
			      XtNlabel,			"Continue",
			      NULL);
  XtAddCallback(button,
		XtNcallback, (XtCallbackProc)ContButtonPress,
		NULL);
}


static void	PutOptionButton(w)
     Widget w;
{
  Widget	openButton, menu;
  Widget	button;

  openButton
    = XtVaCreateManagedWidget("optionButton",
			      menuButtonWidgetClass, w,
			      XtNlabel,			"Option",
			      XtNmenuName,	"optionPullDownMenu",
			      NULL);
  menu
    = XtVaCreatePopupShell("optionPullDownMenu",
			   simpleMenuWidgetClass, openButton,
			   NULL);
  button
    = XtVaCreateManagedWidget("ackWaitTimeOutButton",
			      smeBSBObjectClass,	menu,
			      XtNlabel,
			      "ACK wait time out",
			      NULL);
  XtAddCallback(button,
		XtNcallback, (XtCallbackProc)AckTimeOutButtonPress,
		NULL);
  button
    = XtVaCreateManagedWidget("pollingTimeButton",
			      smeBSBObjectClass,	menu,
			      XtNlabel,
			      "Polling interval time",
			      NULL);
  XtAddCallback(button,
		XtNcallback, (XtCallbackProc)PollingTimeButtonPress,
		NULL);
  button
    = XtVaCreateManagedWidget("trackingTimeoutButton",
			      smeBSBObjectClass,	menu,
			      XtNlabel,
			      "Migration tracking time out",
			      NULL);
  XtAddCallback(button,
		XtNcallback, (XtCallbackProc)TrackingTimeout,
		NULL);
  (void)XtVaCreateManagedWidget("holizontalLine3",
				smeLineObjectClass,	menu,
				NULL);
  button
    = XtVaCreateManagedWidget("xdrBufMaxSizeButton",
			      smeBSBObjectClass,	menu,
			      XtNlabel,
			      "max XDR encode/decode buffer size",
			      NULL);
  XtAddCallback(button,
		XtNcallback, (XtCallbackProc)XDRBufMaxSizeChange,
		NULL);
}


static void	CreateTopInfoArea()
{
  Widget viewport;
  Widget infoArea;

  viewport
    = XtVaCreateManagedWidget("viewport",
			      viewportWidgetClass,	topWindow,
			      XtNshowGrip,	False,
			      XtNallowHoriz,	True,
			      XtNallowVert,	True,
			      NULL);
  infoArea
    = XtVaCreateManagedWidget("topInfoArea",
			      formWidgetClass,	viewport,
			      NULL);

  topHostNameLabel
    = XtVaCreateManagedWidget("topHostNameLabel",
			      labelWidgetClass,	infoArea,
			      XtNlabel,
			      "HOST = --------------------------------",
			      NULL);

  topConnectionModeLabel
    = XtVaCreateManagedWidget("topConnectionModeLabel",
			      labelWidgetClass,	infoArea,
			      XtNfromVert,	topHostNameLabel,
			      XtNlabel,		"----------",
			      NULL);
  topConditionLabel
    = XtVaCreateManagedWidget("topConditionLabel",
			      labelWidgetClass, infoArea,
			      XtNfromVert,	topHostNameLabel,
			      XtNfromHoriz,	topConnectionModeLabel,
			      XtNlabel,		"Condition ??",
			      NULL);
  topNOVLabel
    = XtVaCreateManagedWidget("topNOVLabel",
			      labelWidgetClass, infoArea,
			      XtNfromVert,	topHostNameLabel,
			      XtNfromHoriz,	topConditionLabel,
			      XtNlabel,		"n_OV = ??????",
			      NULL);
  topTPingSLabel
    = XtVaCreateManagedWidget("topTPingSLabel",
			      labelWidgetClass, infoArea,
			      XtNfromVert,	topConditionLabel,
			      XtNlabel,		
			      "(Server)  TPing information Off        ",
			      NULL);
  topTPingCLabel
    = XtVaCreateManagedWidget("topTPingCLabel",
			      labelWidgetClass, infoArea,
			      XtNfromVert,	topTPingSLabel,
			      XtNlabel,		
			      "(Cluster) TPing information Off        ",
			      NULL);
  topMigrationLabel
    = XtVaCreateManagedWidget("topMigrationLabel",
			      labelWidgetClass, infoArea,
			      XtNfromVert,	topTPingCLabel,
			      XtNlabel,
			      "       Migration monitoring Off        ",
			      NULL);
  topMCStepLabel
    = XtVaCreateManagedWidget("topMCStepLabel",
			      labelWidgetClass, infoArea,
			      XtNfromVert,	topMigrationLabel,
			      XtNlabel,		
			      "MC_step = MICRO Debugger  ?????????????",
			      NULL);
  topSliceLabel
    = XtVaCreateManagedWidget("topSliceLabel",
			      labelWidgetClass, infoArea,
			      XtNfromVert,	topMCStepLabel,
			      XtNlabel,		"Slice = ????????????",
			      NULL);
  topCpuLabel
    = XtVaCreateManagedWidget("topCpuLabel",
			      labelWidgetClass, infoArea,
			      XtNfromVert,	topMCStepLabel,
			      XtNfromHoriz,	topSliceLabel,
			      XtNlabel,		"Cpu = ???????????",
			      NULL);
  topConnectLabel
    = XtVaCreateManagedWidget("topConnectLabel",
			      labelWidgetClass, infoArea,
			      XtNfromVert,	topSliceLabel,
			      XtNlabel,		
			      "           Connection info...          \n ",
			      NULL);
}


/*
 *- Action definition.
 */
static void	TopFileSaveButtonPress(w)
     Widget w;
{
  if ( ConfirmationMessage
      ("Soup will be saved. Are you sure ?")
      == 1 ){/* YES */
    BglMM_File_Save();
  }
}

static void	TopFileSaveAndQuitButtonPress(w)
     Widget w;
{
  if ( ConfirmationMessage
      ("Soup will be saved and Tierra will go down. Are you sure ?")
      == 1 ){/* YES */
    BglMM_File_SaveAndQuit();
  }
}

static void	TopFileTierraQuitButtonPress(w)
     Widget w;
{
  if ( ConfirmationMessage
      ("Tierra will be going down. Are you sure ?")
      == 1 ){/* YES */
    BglMM_File_Quit();
  }
}

static void	TopFileBeagleExitButtonPress(w)
     Widget w;
{
  if ( ConfirmationMessage
      ("Beagle will be going down. Are you sure ?")
      == 1 ){/* YES */
    BglMM_File_Exit();
  }
}

static void	TopNewConnectByScriptReq(w)
     Widget w;
{
  String label[1], out[1];

  label[0] = "Please Key in connection script file name";
  out[0] = selectedHostName;
  StrKeyIn(out,1,label);

  if ( out[0][0] ){	/* There are some data */
    BglMM_Connect_Open(out[0], 0);
  }
}

static void	TopNewConnectByHostNameReq(w)
     Widget w;
{
  char urlStyleString[200];
  String label[4], out[4];

  label[0] = "Machine and domain strings";
  out[0] = selectedHostName;
  label[1] = "Short communication port number";
  out[1] = UTIL_Sock_GetCurrent_Connection()->addr.sPortService;
  label[2] = "Normal communication port number";
  out[2] = UTIL_Sock_GetCurrent_Connection()->addr.service;
  label[3] = "If you want to SU mode connection then select here.";
  out[3] = "yes";
  StrKeyIn(out,4,label);

  if ( out[0][0] ){	/* There are some data */
    tsprintf(urlStyleString,"bgl%c://%s:%s:%s",
	     ( !strcmp(out[3],"yes") )? BGL_PRIV_S : BGL_PRIV_N,
	     out[0],
	     out[2],
	     out[1]
	     );
    BglMM_Connect_Open(urlStyleString, 0);
  }
}

static void	TopNewConnectButtonPress(w, n)
     Widget w;
     int n;
{
  BglMM_Connect_ReOpen(n);
}

static void	TopCloseConnectButtonPress(w)
     Widget w;
{
  if ( ConfirmationMessage
      ("Current connection will be close. Are you sure ?")
      == 1 ){/* YES */
    BglMM_Connect_Close();
  }
}

static void	StatsOpenReq(w)
     Widget w;
{
  XtSetArg(args[0], XtNsensitive, False);
  XtSetValues(statsOpenButton, args, 1);
  XtPopup(statsWindow, XtGrabNone);
}

static void	PlanOpenReq(w)
     Widget w;
{
  XtSetArg(args[0], XtNsensitive, False);
  XtSetValues(planOpenButton, args, 1);
  BglMM_Plan_need();
  XtPopup(planWindow, XtGrabNone);
  return;
}

static void	MessageOpenReq(w)
     Widget w;
{
  XtSetArg(args[0], XtNsensitive, False);
  XtSetValues(messageOpenButton, args, 1);
  XtPopup(messageWindow, XtGrabNone);
}

static void	OverviewOpenReq(w)
     Widget w;
{
  if ( BglMM_OvSession_need() ){
    XtSetArg(args[0], XtNsensitive, False);
    XtSetValues(overviewOpenButton, args, 1);
    XtPopup(overviewWindow, XtGrabNone);
  }
  else{	MessageMessage("Request (overviewWindow open) was refused.");  }
}

static void	QueryOpenReq(w)
     Widget w;
{
  XtSetArg(args[0], XtNsensitive, False);
  XtSetValues(queryOpenButton, args, 1);
  XtPopup(queryWindow, XtGrabNone);
}

static void	HistoOpenReq(w)
     Widget w;
{
  XtSetArg(args[0], XtNsensitive, False);
  XtSetValues(histoOpenButton, args, 1);
  XtPopup(histoWindow, XtGrabNone);
}

static void	TPingSOpenReq(w)
     Widget w;
{
  XtSetArg(args[0], XtNsensitive, False);
  XtSetValues(tPingSOpenButton, args, 1);
  XtPopup(tPingSWindow, XtGrabNone);
}

static void	TPingCOpenReq(w)
     Widget w;
{
  XtSetArg(args[0], XtNsensitive, False);
  XtSetValues(tPingCOpenButton, args, 1);
  XtPopup(tPingCWindow, XtGrabNone);
}

static void	VarOpenReq(w)
     Widget w;
{
  XtSetArg(args[0], XtNsensitive, False);
  XtSetValues(varOpenButton, args, 1);
  BglMM_Var_modeReset();
  BglMM_Var_need();
  XtPopup(varWindow, XtGrabNone);
}

static void	DebugOpenReq(w)
     Widget w;
{
  XtSetArg(args[0], XtNsensitive, False);
  XtSetValues(debugOpenButton, args, 1);
  XtPopup(debugWindow, XtGrabNone);
}

static void	MigrationOpenReq(w)
     Widget w;
{
  XtSetArg(args[0], XtNsensitive, False);
  XtSetValues(migrationOpenButton, args, 1);
  XtPopup(migrationWindow, XtGrabNone);
}

static void	InfoOpenReq(w)
     Widget w;
{
  XtSetArg(args[0], XtNsensitive, False);
  XtSetValues(infoMessageOpenButton, args, 1);
  XtPopup(infoMessageWindow, XtGrabNone);
}

static void	InjectOpenReq(w)
     Widget w;
{
  int	i;
  char	addrStr[30];
  String labels[] = { "Enter genome to be injected (eg 0080aaa)",
		      "Enter number of creatures to inject",
		      "Enter address (\"o\" if you want overview)"};
  String strs[] = {NULL, NULL, NULL};
  
  StrKeyIn(strs, 3, labels);
  strcpy(nameStr, strs[0]);
  if (!strlen(nameStr))		return;
  strcpy(numStr, strs[1]);
  if (!strlen(numStr))		return;
  strcpy(addrStr, strs[2]);
  if (!strlen(addrStr))		return;

  for ( i = 0 ; ((addrStr[i] < '0') || (addrStr[i] > '9'))
       && (addrStr[i] - 'o')
       && (addrStr[i] - '\0') ; i++);
  if (addrStr[i] - 'o')
    BglMM_Inject_Need( nameStr, numStr, addrStr );
  else
    Ov_MouseMapRequest("Select position\n for insertion", InjectOpenReqSend);

  return;
}

static void	InjectOpenReqSend(address)
     I32u address;
{
  char addrStr[11];
  
  tsprintf(addrStr, "%d", address);
  BglMM_Inject_Need( nameStr, numStr, addrStr );
}

static void	HistLoggingToggleReq(w)
     Widget w;
{
  BglMM_Misc_h_need();
}

static void	SuspendButtonPress(w)
     Widget w;
{
  BglMM_Suspend_Need();
}

static void	ResumeButtonPress(w)
     Widget w;
{
  BglMM_Resume_Need();
}

static void	ContButtonPress(w)
     Widget w;
{
  BglMM_B_OneCellCont();
  if ( nowConditionChar != '.' ){
    BglMM_B_Cont();
  }
}

static void	AckTimeOutButtonPress(w)
     Widget w;
{
  String ansStr[] = {NULL};
  String tmp;
  double t;

  tsprintf(bglStrBuf,
	   "Ack wait timeout (current %8.2f sec.)",
	   Bgl_wait0 );
  tmp = bglStrBuf;
  StrKeyIn( ansStr, 1, &tmp );
  if ( strlen(*ansStr)>0 ){
    t = atof(*ansStr);
    BglMM_oPt_w('0',t);
  }
}

static void	PollingTimeButtonPress(w)
     Widget w;
{
  String ansStr[] = {NULL};
  String tmp;
  double t;

  tsprintf(bglStrBuf,
	   "Polling interval time (current %5.4f sec.)",
	   Bgl_wait1 );
  tmp = bglStrBuf;
  StrKeyIn( ansStr, 1, &tmp );
  if ( strlen(*ansStr)>0 ){
    t = atof(*ansStr);
    BglMM_oPt_w('1',t);
  }
}

static void	TrackingTimeout(w)
     Widget w;
{
  String ansStr[] = {NULL};
  String tmp;
  double t;

  tsprintf(bglStrBuf,
	   "Migration tracking timeout (current %8.2f sec.)",
	   Bgl_wait2 );
  tmp = bglStrBuf;
  StrKeyIn( ansStr, 1, &tmp );
  if ( strlen(*ansStr)>0 ){
    t = atof(*ansStr);
    BglMM_oPt_w('2',t);
  }
}


static void	XDRBufMaxSizeChange(w)
     Widget w;
{
  String ansStr[] = {NULL};
  String tmp;
  I32s size;

  tsprintf(bglStrBuf,
	   "max XDR encode/decode buffer size (current %d.)",
	   XDRBufMaxSize );
  tmp = bglStrBuf;
  StrKeyIn( ansStr, 1, &tmp );
  if ( strlen(*ansStr)>0 ){
    size = atoi(*ansStr);
    XDRBufMaxSize = size;
  }
}


/*
 *- This windows process routines.
 */
static int	ReMakeConnectionPullDownMenu()
{
  static Widget pullDownMenu = 0;
  Widget pullDownForm;
  Widget button, connectButton;

  if (pullDownMenu)	XtDestroyWidget(pullDownMenu);

  pullDownMenu
    = XtVaCreatePopupShell("connectPullDownMenu",
			   overrideShellWidgetClass,	connectOpenButton,
			   XtNtranslations,	connectCascMenuTransTable,
			   NULL);
  pullDownForm
    = XtVaCreateManagedWidget("connectPullDownMenuForm",
			      formWidgetClass,		pullDownMenu,
			      XtNdefaultDistance,	0,
			      NULL);

  CreateConnect_SH_Menu( pullDownForm );

  connectButton
    = XtVaCreateManagedWidget("topNewConnectButton",
			      commandWidgetClass,	pullDownForm,
			      XtNlabel,		"Create New Connection",
			      XtNborderWidth,	0,
			      XtNtranslations,		buttTransTable,
			      NULL);
  (void)XtVaCreateManagedWidget("arrow",
				commandWidgetClass,	pullDownForm,
				XtNfromHoriz,		connectButton,
				XtNborderWidth,	0,
				XtNtranslations,   connect_ArrowTransTable,
				NULL);
  button
    = XtVaCreateManagedWidget("topCloseConnectButton",
			      commandWidgetClass,	pullDownForm,
			      XtNlabel,		"Close Connection        ",
			      XtNfromVert,	connectButton,
			      XtNborderWidth,	0,
			      XtNtranslations,		buttTransTable,
			      NULL);
  XtAddCallback(button,
		XtNcallback, (XtCallbackProc)TopCloseConnectButtonPress,
		NULL);

  return AddOldConnectButtons(pullDownForm, button);
}


static void	CreateConnect_SH_Menu(w)
     Widget w;
{
  Widget menu, button;

  menu
    = XtVaCreatePopupShell("connect_SH_Menu",
			   simpleMenuWidgetClass, w,
			   XtNtranslations,		menuTransTable,
			   NULL);
  button
    = XtVaCreateManagedWidget("connect_S_Button",
			      smeBSBObjectClass,	menu,
			      XtNlabel,	"by Script",
			      NULL);
  XtAddCallback(button,
		XtNcallback, (XtCallbackProc)TopNewConnectByScriptReq,
		NULL);
  button
    = XtVaCreateManagedWidget("connect_H_Button",
			      smeBSBObjectClass,	menu,
			      XtNlabel,	"by Host name",
			      NULL);
  XtAddCallback(button,
		XtNcallback, (XtCallbackProc)TopNewConnectByHostNameReq,
		NULL);
}

static int	AddOldConnectButtons(pW, uW)
     Widget pW, uW;
{
  Widget line = NULL;
  Widget button, upperButton, connectProperty, connectPropForm;
  BglSockSt*	sP;
  int		n = 0;
  static XtTranslations	propButtTransTable = 0;

  while( sP =  UTIL_Sock_Get_Connection(++n)){
    if ( !line ){
      line
	= XtVaCreateManagedWidget("separatorLine",
				  labelWidgetClass,	pW,
				  XtNfromVert,		uW,
				  XtNlabel,	"------------------------",
				  XtNborderWidth,	0,
				  NULL);
      upperButton = line;
    }

    tsprintf(bglStrBuf, "#override\n\
<EnterWindow>: set() positionCascPullDownMenu(property%-d) cascMenuPopup(property%-d)\n\
<LeaveWindow>: unset() cascMenuEnter(connectPullDownMenuForm)", n, n);
    propButtTransTable  = XtParseTranslationTable((String)bglStrBuf);

    tsprintf(bglStrBuf,  "%-23s", sP->addr.host);
    bglStrBuf[24] = '\0';
    bglStrBuf[23] = '.';
    bglStrBuf[22] = '.';
    bglStrBuf[21] = '.';
    bglStrBuf[20] = ' ';

    button
      = XtVaCreateManagedWidget("oldConnectButton",
				commandWidgetClass,	pW,
				XtNlabel,		bglStrBuf,
				XtNfromVert,		upperButton,
				XtNborderWidth,		0,
				XtNtranslations,	propButtTransTable,
				NULL);
    XtAddCallback(button,
		  XtNcallback, (XtCallbackProc)TopNewConnectButtonPress,
		  (XtPointer)n);

    tsprintf(bglStrBuf, "property%-d", n);
    connectProperty
      = XtVaCreatePopupShell(bglStrBuf,
			     overrideShellWidgetClass,	pW,
			     NULL);

    connectPropForm
      = XtVaCreateManagedWidget("connectPropForm",
				formWidgetClass,	connectProperty,
				XtNdefaultDistance,	0,
				NULL);

    tsprintf(bglStrBuf,  "%s\n%s mode\nShort commPort=%s\nNorm  commPort=%5s",
	     sP->addr.host,
	     (sP->user.priv == BGL_PRIV_S)? "   SU" : "NonSU",
	     sP->addr.sPortService,
	     sP->addr.service);

    (void)XtVaCreateManagedWidget("topHostNameLabel",
				  labelWidgetClass,	connectPropForm,
				  XtNlabel,		bglStrBuf,
				  NULL);

    upperButton = button;
  }

  return n-1;
}
