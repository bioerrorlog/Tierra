/* beagleGUI_StatsWindow.c -- -*- C -*-
 * 
 *-  BEAGLE STATS AREA window.
 * 
 * Author          : YOSHIKAWA Tooru
 * Created On      : Mon Aug 26 17:52:24 1996
 * Last Modified By: YOSHIKAWA Tooru
 * Last Modified On: Thu Jun 17 15:41:40 1999
 * Update Count    : 220
 * Status          : 
 */

#ifndef lint
static char* sccsProgramId="@(#)beagleGUI_StatsWindow.c	1.16	06/17/99 15:41:57";
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
#include <X11/Xaw/Label.h>

#include "beagleMessageMgr.h"
#include "beagleGUI_Global.h"
#include "beagleGUI_StatsWindow.h"


/*
 *- SPECIAL VALIABLE NAME
 *- =====================
 *- This window show only one variable for another file using the file
 *- 'beagleGUI_Global.h'. It name and type a 'Widget statsWindow'.
 *- So you can open this window from every GUI file if need it.
 */

/*
 *- HERE ARE EXTERN FUNCTION DEFINEs.
 *- ================================
 *- This window (statsWindow) use here external function only.
 *- So this file need the file include here function fo linking.
 *- I tried to collect there functions in '../Bglclnt/beagleRoutines.c'.
 *- Another GUI functions are define in the 'beagleGUI_StatsWindow.h'.
 */

/*** NONE ***/

/*
 *- HERE ARE GLOBAL VARIABLE (EFFECTIVE IN THIS FILE ONLY) DEFINEs.
 *- ===============================================================
 *- So another files functions cannot search these.
 *- I want to avoid from linking trouble.
 */
static Widget	toplevel;	/* This file need to keep toplevel widget */
static Widget	statsInstExec_mDataLabel;
static Widget	statsInstExec_iDataLabel;
static Widget	statsNumCellsDataLabel;
static Widget	statsNumGenotypesDataLabel;
static Widget	statsNumSizeDataLabel;
static Widget	statsExtractedDataLabel;

/*
 *- HERE ARE STATIC FUNCTIONs PROTOTYPE DEFINEs.
 *- ============================================
 *- These functions call from in this file only.
 *- So another files functions cannot search these.
 *- I want to avoid from linking trouble.
 */

/*- Create windows.*/
static void	CreateMenuBar		P_((Widget));
static Widget	PutFileButton		P_((Widget));
static void	CreateStatsInfoArea	P_((Widget));
/*- Action definition.*/
static void	CloseButtonPress	P_((Widget));
/*- This windows process routines -*/


/*
 *- HERE ARE THIS WINDOW's SERVICE FUNCTIONs FOR CALL FROM ANOTHER FILEs.
 *- =====================================================================
 *- I wrote prototype define in 'beagleGUI_StatsWindow.h'.
 */
void	CreateStatsWindow(topWidget)
     Widget topWidget;
{
  Widget	form;

  toplevel = topWidget;
  
  statsWindow
    = XtVaCreatePopupShell("statsWindow",
			   topLevelShellWidgetClass, topWidget,
			   XtNallowShellResize,		True,
			   NULL);
  form
    = XtVaCreateManagedWidget("statsForm",
			      panedWidgetClass,	statsWindow,
			      XtNallowShellResize,	True,
			      NULL);
  CreateMenuBar(form);
  CreateStatsInfoArea(form);
}

void	UpdateStatsData()
{
  int test;
  BglStatsSt* data;
  static BglStatsSt lastData = {-1, 0, 0, 0, 0, 0, ""};
  Arg args[1];

  data = BglMM_PickStats();


  if (lastData.GeneBnker != data->GeneBnker){
    if (test = XtIsRealized(statsWindow)){
      XtDestroyWidget(statsWindow);
      CreateStatsWindow(toplevel);
    }
    if ((lastData.GeneBnker = data->GeneBnker) > 0){
      XtManageChild(statsNumGenotypesDataLabel);
      XtManageChild(statsNumSizeDataLabel);
      XtManageChild(statsExtractedDataLabel);
    }
    else{
      XtUnmanageChild(statsNumGenotypesDataLabel);
      XtUnmanageChild(statsNumSizeDataLabel);
      XtUnmanageChild(statsExtractedDataLabel);
    }
    if (test){
      XtPopup(statsWindow, XtGrabNone);
    }
 }

  if ( lastData.InstExe_m != data->InstExe_m ){
    lastData.InstExe_m = data->InstExe_m;
    tsprintf (bglStrBuf, "InstExe.m = %6d", (int)data->InstExe_m );
    XtSetArg(args[0], XtNlabel, bglStrBuf);
    XtSetValues(statsInstExec_mDataLabel, args, 1);
  }

  if ( lastData.InstExe_i != data->InstExe_i ){
    lastData.InstExe_i = data->InstExe_i;
    tsprintf (bglStrBuf, "InstExec.i = %6d", (int)data->InstExe_i );
    XtSetArg(args[0], XtNlabel, bglStrBuf);
    XtSetValues(statsInstExec_iDataLabel, args, 1);
  }

  if ( lastData.NumCells != data->NumCells ){
    lastData.NumCells = data->NumCells;
    tsprintf (bglStrBuf, "NumCreatures = %7d", (int)data->NumCells );
    XtSetArg(args[0], XtNlabel, bglStrBuf);
    XtSetValues(statsNumCellsDataLabel, args, 1);
  }
  
  if(data->GeneBnker > 0) {

    if ( lastData.NumGenotypes != data->NumGenotypes ){
      lastData.NumGenotypes = data->NumGenotypes;
      tsprintf (bglStrBuf, "NumGenotypes = %4d", (int)data->NumGenotypes );
      XtSetArg(args[0], XtNlabel, bglStrBuf);
      XtSetValues(statsNumGenotypesDataLabel, args, 1);
    }

    if ( lastData.NumSizes != data->NumSizes ){
      lastData.NumSizes = data->NumSizes;
      tsprintf (bglStrBuf, "NumSizes = %3d", (int)data->NumSizes );
      XtSetArg(args[0], XtNlabel, bglStrBuf);
      XtSetValues(statsNumSizeDataLabel, args, 1);
    }

    if ( strcmp(lastData.ExtrG_gen, data->ExtrG_gen) ){
      strcpy(lastData.ExtrG_gen, data->ExtrG_gen);
      tsprintf (bglStrBuf, "Extracted = %14s", data->ExtrG_gen);
      XtSetArg(args[0], XtNlabel, bglStrBuf);
      XtSetValues(statsExtractedDataLabel, args, 1);
    }
  }
  else{
    /* NOP */
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

static Widget	PutFileButton(w)
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
			      NULL);
  XtAddCallback(button,
		XtNcallback, (XtCallbackProc)CloseButtonPress,
		NULL);

  return openButton;
}

static void	CreateStatsInfoArea(w)
     Widget w;
{
  Widget viewport, infoArea;

  viewport
    = XtVaCreateManagedWidget("statsInfoViewport",
			      viewportWidgetClass, w,
			      XtNshowGrip,	False,
			      XtNallowHoriz,	False,
			      XtNallowVert,	True,
			      XtNallowResize,	True,
			      NULL);
  infoArea
    = XtVaCreateManagedWidget("statsInfoArea",
			      formWidgetClass, viewport,
			      NULL);
  statsInstExec_mDataLabel
    = XtVaCreateManagedWidget("statsInstExec_mDataLabel",
			      labelWidgetClass,	infoArea,
			      XtNlabel,	"InstExe.m = ------",
			      NULL);
  statsInstExec_iDataLabel
    = XtVaCreateManagedWidget("statsInstExe_iDataLabel",
			      labelWidgetClass,	infoArea,
			      XtNfromHoriz,	statsInstExec_mDataLabel,
			      XtNlabel,	"InstExec.i = ------",
			      NULL);
  statsNumCellsDataLabel
    = XtVaCreateManagedWidget("statsNumCellsDataLabel",
			      labelWidgetClass,	infoArea,
			      XtNfromHoriz,	statsInstExec_iDataLabel,
			      XtNlabel,	"NumCreatures = -------",
			      NULL);
  statsNumGenotypesDataLabel
    = XtVaCreateManagedWidget("statsNumGenotypesDataLabel",
			      labelWidgetClass, infoArea,
			      XtNfromVert,	statsInstExec_iDataLabel,
			      XtNlabel,	"NumGenotypes = ----",
			      NULL);
  statsNumSizeDataLabel
    = XtVaCreateManagedWidget("statsNumSizeDataLabel",
			      labelWidgetClass, infoArea,
			      XtNfromVert,	statsInstExec_iDataLabel,
			      XtNfromHoriz,	statsNumGenotypesDataLabel,
			      XtNlabel,	"NumSizes = ---",
			      NULL);
  statsExtractedDataLabel
    = XtVaCreateManagedWidget("statsExtractedDataLabel",
			      labelWidgetClass, infoArea,
			      XtNfromVert,	statsInstExec_iDataLabel,
			      XtNfromHoriz,	statsNumSizeDataLabel,
			      XtNlabel,	"Extracted = --------------",
			      NULL);
}


/*
 *- Action definition.
 */
static void	CloseButtonPress(w)
     Widget w;
{
  Arg args[1];
  XtPopdown(statsWindow);
  XtUnrealizeWidget(statsWindow);
  XtSetArg(args[0], XtNsensitive, True);
  XtSetValues(statsOpenButton, args, 1);
}


/*
 *- This windows process routines.
 */
