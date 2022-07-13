/* beagleGUI_QueryWindow.c -- -*- C -*-
 * 
 *-  BEAGLE QUERY SIZE  window.
 * 
 * Author          : YOSHIKAWA Tooru
 * Created On      : Mon Sep  9 15:36:47 1996
 * Last Modified By: YOSHIKAWA Tooru
 * Last Modified On: Thu Jun 17 15:40:17 1999
 * Update Count    : 131
 * Status          : 
 */

#ifndef lint
static char* sccsProgramId="@(#)beagleGUI_QueryWindow.c	1.19	06/17/99 15:40:38";
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
#include <X11/Xaw/List.h>

#include "beagleMessageMgr.h"
#include "beagleGUI_Global.h"
#include "beagleGUI_KeyInWindow.h"
#include "beagleGUI_QueryWindow.h"

#define	QUERY_LIST_MAX	500

/*
 *- SPECIAL VALIABLE NAME
 *- =====================
 *- This window show only one variable for another file using the file
 *- 'beagleGUI_Global.h'. It name and type a 'Widget queryWindow'.
 *- So you can open this window from every GUI file if need it.
 */

/*
 *- HERE ARE EXTERN FUNCTION DEFINEs.
 *- ================================
 *- This window (queryWindow) use here external function only.
 *- So this file need the file include here function fo linking.
 *- I tried to collect there functions in '../Bglclnt/beagleRoutines.c'.
 *- Another GUI functions are define in the 'beagleGUI_QueryWindow.h'.
 */

/*** NONE ***/

/*
 *- HERE ARE GLOBAL VARIABLE (EFFECTIVE IN THIS FILE ONLY) DEFINEs.
 *- ===============================================================
 *- So another files functions cannot search these.
 *- I want to avoid from linking trouble.
 */
static Widget	queryInfoArea;

/*
 *- HERE ARE STATIC FUNCTIONs PROTOTYPE DEFINEs.
 *- ============================================
 *- These functions call from in this file only.
 *- So another files functions cannot search these.
 *- I want to avoid from linking trouble.
 */

/*- Create windows.*/
static void	CreateMenuBar		P_((Widget));
static void	PutFileButton		P_((Widget));
static void	PutSizeButton		P_((Widget));
static void	CreateQueryInfoArea	P_((Widget));
/*- Action definition.*/
static void	CloseButtonPress	P_((Widget));
static void	QuerySizeButtonPress	P_((Widget));
/*- This windows process routines -*/

/*
 *- HERE ARE THIS WINDOW's SERVICE FUNCTIONs FOR CALL FROM ANOTHER FILEs.
 *- =====================================================================
 *- I wrote prototype define in 'beagleGUI_QueryWindow.h'.
 */
void	CreateQueryWindow(topWidget)
     Widget topWidget;
{
  Widget	form;

  queryWindow
    = XtVaCreatePopupShell("queryWindow",
			   topLevelShellWidgetClass, topWidget,
			   NULL);
  form
    = XtVaCreateManagedWidget("queryForm",
			      panedWidgetClass,	queryWindow,
			      XtNwidth,		550,
			      XtNheight,	200,
			      NULL);
  CreateMenuBar(form);
  CreateQueryInfoArea(form);
}

void	UpdateQuerySizeData()
{
  static int last_i = 0;
  int	i;
  static String list[QUERY_LIST_MAX];
  BglSizQueryOutputSt* data;

  for ( i = 0; i < last_i; i++ ){
    if ( list[i] ){
      XtFree(list[i]);
      list[i] = NULL;
    }
  }

  i = 0;
  while( BglMM_HowManySizeQuery() ){
    data = BglMM_PickSizeQuery();
    tsprintf(bglStrBuf, "%5d %3.3s %5d %2d %2d %4d %s",
	     data->count, data->lbl, data->moves, data->flags, 
	     data->bits, data->csS, data->WatchBits); 
    list[i] = XtNewString(bglStrBuf);
    i++;
    if ( i >= QUERY_LIST_MAX ) break;
  }
  XawListChange(queryInfoArea, list, i, 0, True);
  last_i = i;
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
  PutSizeButton(menuBar);
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
			      NULL);
  XtAddCallback(button,
		XtNcallback, (XtCallbackProc)CloseButtonPress,
		NULL);
}


static void	PutSizeButton(w)
     Widget w;
{
  static Widget	openButton;
  Widget menu, button;

  openButton
    = XtVaCreateManagedWidget("sizeButton",
			      menuButtonWidgetClass, w,
			      XtNmenuName,	"sizePullDownMenu",
			      NULL);
  menu
    = XtVaCreatePopupShell("sizePullDownMenu",
			   simpleMenuWidgetClass, openButton,
			   NULL);
  button
    = XtVaCreateManagedWidget("querySizeButton",
			      smeBSBObjectClass, menu,
			      XtNlabel,		"Size Query",
			      NULL);
  XtAddCallback(button,
		XtNcallback, (XtCallbackProc)QuerySizeButtonPress,
		NULL);
}


static void	CreateQueryInfoArea(w)
     Widget w;
{
  Widget itemLabel, viewport;

  itemLabel
    = XtVaCreateManagedWidget("itemLabel",
			      labelWidgetClass, w,
			      XtNshowGrip,	False,
			      XtNresize,		False,
			      XtNheight,		10,
			      XtNdefaultDistance,	0,
			      XtNhorizDistance,		0,
			      XtNvertDistance,		0,
			      XtNlabel,
"  Count Label Mov  F  B  CsS WatchBits                                           ",
			      NULL);

  viewport
    = XtVaCreateManagedWidget("viewport",
			      viewportWidgetClass,	w,
			      XtNfromVert,	itemLabel,
			      XtNshowGrip,	False,
			      XtNforceBars,	True,
			      XtNallowVert,	True,
			      XtNwidth,		550,
			      XtNheight,	200,
			      NULL);

  queryInfoArea
    = XtVaCreateManagedWidget("queryInfoList",
			      listWidgetClass,	viewport,
			      XtNdefaultColumns, 1,
			      XtNforceColumns,	True,
			      XtNpasteBuffer,	True,
			      XtNnumberStrings,	0,
			      NULL);
}


/*
 *- Action definition.
 */
static void	CloseButtonPress(w)
     Widget w;
{
  Arg args[1];
  XtPopdown(queryWindow);
  XtSetArg(args[0], XtNsensitive, True);
  XtSetValues(queryOpenButton, args, 1);
}

static void	QuerySizeButtonPress(w)
     Widget w;
{
  String sizeStr[] = {NULL};
  String label = "Enter a size class ( eg: 80 ) to examine";

  StrKeyIn( sizeStr, 1, &label);
  if ( strlen(*sizeStr) ){
    BglMM_SizeQuery_Need(*sizeStr);
  }
}


/*
 *- This windows process routines.
 */
