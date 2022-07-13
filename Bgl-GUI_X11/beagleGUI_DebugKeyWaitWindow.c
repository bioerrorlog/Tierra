/* beagleGUI_DebugKeyWaitWindow.c -- -*- C -*-
 * 
 * This work has been done at ATR HIP
 * 
 * Author          : YOSHIKAWA Tooru
 * Created On      : Tue Oct 15 16:26:06 1996
 * Last Modified By: YOSHIKAWA Tooru
 * Last Modified On: Thu Nov 12 16:57:41 1998
 * Update Count    : 113
 * Status          : 
 */

#ifndef lint
static char* sccsProgramId="@(#)beagleGUI_DebugKeyWaitWindow.c	1.17	05/10/99 15:55:21";
#endif /* lint */

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

#include "beagleMessageMgr.h"
#include "beagleGUI_Global.h"
#include "beagleGUI_DebugKeyWaitWindow.h"


/*
 *- SPECIAL VALIABLE NAME
 *- =====================
 *- This window show only one variable for another file using the file
 *- 'beagleGUI_Global.h'. It name and type a 'Widget debugKeyWaitWindow'.
 *- So you can open this window from every GUI file if need it.
 */


/*
 *- HERE ARE EXTERN FUNCTION DEFINEs.
 *- ================================
 *- This window (debugKeyWaitWindow) use here external function only.
 *- So this file need the file include here function fo linking.
 *- I tried to collect there functions in '../Bglclnt/beagleRoutines.c'.
 *- Another GUI functions are define in the 'beagleGUI_DebugKeyWaitWindow.h'
 *- and 'beagleMessageMgr.h'.
 */

/*** NONE ***/

/*
 *- HERE ARE GLOBAL VARIABLE (EFFECTIVE IN THIS FILE ONLY) DEFINEs.
 *- ===============================================================
 *- So another files functions cannot search these.
 *- I want to avoid from linking trouble.
 */
static Widget	tcellButton;
static Widget	tcpuButton;

/*
 *- HERE ARE STATIC FUNCTIONs PROTOTYPE DEFINEs.
 *- ============================================
 *- These functions call from in this file only.
 *- So another files functions cannot search these.
 *- I want to avoid from linking trouble.
 */

/*- Create windows.*/
static void	CreateMenuBar	P_((Widget));
static void	PutFileButton	P_((Widget));
static void	CreateDebugKeyWaitInfoArea	P_((Widget));
/*- Action definition.*/
static void	CloseButtonPress	P_((Widget));
static void	CellTrackButtonPress	P_((Widget));
static void	CpuTrackButtonPress	P_((Widget));
static void	NewCpuButtonPress	P_((Widget));
static void	NextStepButtonPress	P_((Widget));
static void	NextBrkButtonPress	P_((Widget));
/*- This windows process routines -*/
/* NONE */

/*
 *- HERE ARE THIS WINDOW's SERVICE FUNCTIONs FOR CALL FROM ANOTHER FILEs.
 *- =====================================================================
 *- I wrote prototype define in 'beagleGUI_DebugKeyWaitWindow.h'.
 */
void	CreateDebugKeyWaitWindow(topWidget)
     Widget topWidget;
{
  Widget form;

  debugKeyWaitWindow
    = XtVaCreatePopupShell("debugKeyWaitWindow",
			   topLevelShellWidgetClass, topWidget,
			   NULL);
  form
    = XtVaCreateManagedWidget("debugKeyWaitForm",
			      panedWidgetClass,	debugKeyWaitWindow,
			      NULL);
  CreateMenuBar(form);
  CreateDebugKeyWaitInfoArea(form);
}

void	StartDebugKeyWait()
{
  Arg args[1];
  char	ansChar;
  static char	lastSlice = ' ';
  static char	lastCpu = ' ';

  ansChar = BglMM_WhatIsMCStateSlice();
  if ( ansChar != lastSlice ){
    lastSlice = ansChar;
    if ( ansChar == 't' ){
      XtSetArg(args[0], XtNlabel, "        Track Cell        ");
    }
    else{
      XtSetArg(args[0], XtNlabel, "       UnTrack Cell       ");
    }
    XtSetValues(tcellButton, args, 1);
  }

  ansChar = BglMM_WhatIsMCStateCpu();
  if ( ansChar != lastCpu ){
    lastCpu = ansChar;
    if ( (ansChar == 'u')){
      XtSetArg(args[0], XtNlabel, " Track CPU ");
    }
    else{
      XtSetArg(args[0], XtNlabel, "UnTrack CPU");
    }
    XtSetValues(tcpuButton, args, 1);
  }

}




/*
 *- HERE ARE STATIC FUNCTIONs DEFINITION FOR CALL FROM IN THIS FILE ONLY.
 *- =====================================================================
 *- So another files functions cannot search these.
 *- I want to avoid from linking trouble.
 *- -- First part are creating window style.
 *- -- Second part are window actions mechanism.
 */

/*
 *- Creating window style.
 *- -- This window has a 'menu bar' and 'information area'.
 */
static void	CreateMenuBar(w)
     Widget w;
{
  Widget	menuBar;

  menuBar
    = XtVaCreateManagedWidget("menuBar",
			      boxWidgetClass,	w,
			      XtNorientation,	"horizontal",
			      NULL);
  PutFileButton(menuBar);
}

static void	PutFileButton(w)
     Widget w;
{
  static Widget	openButton;
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
    = XtVaCreateManagedWidget("fileCloseButton",
			      smeBSBObjectClass, menu,
			      XtNlabel,	"Key Wait Quit and Close",
			      NULL);
  XtAddCallback(button,
		XtNcallback, (XtCallbackProc)CloseButtonPress,
		NULL);
}


static void	CreateDebugKeyWaitInfoArea(w)
     Widget w;
{
  Widget viewport, infoArea, ncpuButton, nextButton, nBrkButton;

  viewport
    = XtVaCreateManagedWidget("viewport",
			      viewportWidgetClass,	w,
			      XtNshowGrip,	False,
			      NULL);

  infoArea
    = XtVaCreateManagedWidget("debugKeyWaitInfoArea",
			      formWidgetClass, viewport,
			      NULL);
  tcellButton
    = XtVaCreateManagedWidget("tcellButton",
			      commandWidgetClass,	infoArea,
			      XtNlabel,		"---------------------------",
			      NULL);
  XtAddCallback(tcellButton,
		XtNcallback, (XtCallbackProc)CellTrackButtonPress,
		NULL);
  tcpuButton
    = XtVaCreateManagedWidget("tcpuButton",
			      commandWidgetClass,	infoArea,
			      XtNfromVert,	tcellButton,
			      XtNlabel,		"-----------",
			      NULL);
  XtAddCallback(tcpuButton,
		XtNcallback, (XtCallbackProc)CpuTrackButtonPress,
		NULL);
  ncpuButton
    = XtVaCreateManagedWidget("ncpuButton",
			      commandWidgetClass,	infoArea,
			      XtNfromVert,	tcellButton,
			      XtNfromHoriz,	tcpuButton,
			      XtNlabel,		"New Cpu Track",
			      NULL);
  XtAddCallback(ncpuButton,
		XtNcallback, (XtCallbackProc)NewCpuButtonPress,
		NULL);
  nBrkButton
    = XtVaCreateManagedWidget("nBrkButton",
			      commandWidgetClass,	infoArea,
			      XtNfromVert,	tcpuButton,
			      XtNborderWidth,	2,
			      XtNlabel,		"      Next breakpoint     ",
			      NULL);
  XtAddCallback(nBrkButton,
		XtNcallback, (XtCallbackProc)NextBrkButtonPress,
		NULL);
  nextButton
    = XtVaCreateManagedWidget("nextButton",
			      commandWidgetClass,	infoArea,
			      XtNfromVert,	nBrkButton,
			      XtNborderWidth,	2,
			      XtNlabel,		"         Next step        ",
			      NULL);
  XtAddCallback(nextButton,
		XtNcallback, (XtCallbackProc)NextStepButtonPress,
		NULL);
}

/*
 *- Action definition.
 */
static void	CloseButtonPress(w)
     Widget w;
{
  BglMM_Debug_Toggle();
  XtPopdown(debugKeyWaitWindow);
}

static void	CellTrackButtonPress(w)
     Widget w;
{
  BglMM_Debug_Slice_Toggle();
}

static void	CpuTrackButtonPress(w)
     Widget w;
{
  BglMM_Debug_Cpu_Toggle();
}

static void	NewCpuButtonPress(w)
     Widget w;
{
  BglMM_Debug_New_Cpu();
}

static void	NextBrkButtonPress(w)
     Widget w;
{
  BglMM_Debug_BrkRepeat();
}

static void	NextStepButtonPress(w)
     Widget w;
{
  BglMM_Debug_Next();
}


/*
 *- This windows process routines 
 */
