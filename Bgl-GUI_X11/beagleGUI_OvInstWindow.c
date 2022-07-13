/* beagleGUI_OvInstWindow.c -- -*- C -*-
 * 
 *-  BEAGLE OVERVIEW INSTRUCTION DATA window.
 * 
 * Author          : YOSHIKAWA Tooru
 * Created On      : Fri Aug 23 16:41:03 1996
 * Last Modified By: YOSHIKAWA Tooru
 * Last Modified On: Thu Jun 17 15:34:22 1999
 * Update Count    : 50
 * Status          : 
 */

#ifndef lint
static char* sccsProgramId="@(#)beagleGUI_OvInstWindow.c	1.17	06/17/99 15:34:50";
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
#include <X11/Xaw/AsciiText.h>

#include "beagleMessageMgr.h"
#include "beagleGUI_Global.h"
#include "beagleGUI_KeyInWindow.h"
#include "beagleGUI_OverviewWindow.h"

/*
 *- SPECIAL VALIABLE NAME
 *- =====================
 *- This window show only one variable for another file using the file
 *- 'beagleGUI_Global.h'. It name and type a 'Widget overviewWindow'.
 *- So you can open this window from every GUI file if need it.
 */

/*
 *- HERE ARE EXTERN FUNCTION DEFINEs.
 *- ================================
 *- This window (ovInstWindow) use here external function only.
 *- So this file need the file include here function fo linking.
 *- I tried to collect there functions in '../Bglclnt/beagleRoutines.c'.
 *- Another GUI functions are define in the 'beagleGUI_OvInstWindow.h'.
 */

/*** NONE ***/

/*
 *- HERE ARE GLOBAL VARIABLE (EFFECTIVE IN THIS FILE ONLY) DEFINEs.
 *- ===============================================================
 *- So another files functions cannot search these.
 *- I want to avoid from linking trouble.
 */
static Widget	instInfoArea;

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
static void	CreateInstInfoArea	P_((Widget));
/*- Action definition.*/
static void	CloseButtonPress	P_((Widget));
/*- This windows process routines -*/

/*
 *- HERE ARE THIS WINDOW's SERVICE FUNCTIONs FOR CALL FROM ANOTHER FILEs.
 *- =====================================================================
 *- I wrote prototype define in 'beagleGUI_OvInstWindow.h'.
 */
void	CreateOvInstWindow(topWidget)
     Widget topWidget;
{
  Widget	form;

  ovInstWindow
    = XtVaCreatePopupShell("ovInstWindow",
			   topLevelShellWidgetClass, topWidget,
			   NULL);
  form
    = XtVaCreateManagedWidget("instForm",
			      panedWidgetClass,	ovInstWindow,
			      XtNwidth,		150,
			      XtNheight,	screen->height - 30,
			      NULL);
  CreateMenuBar(form);
  CreateInstInfoArea(form);
}

void	UpdateOvGInstData()
{
  int		i, endi, j, k, l;
  static char	str[2000];
  char		list[500][81];
  BglOvGInstSt* data;
  Arg		args[2];
  
  i = 0;
  l = 0;
  while( BglMM_HowManyOvGInst() ){
    data	=  BglMM_PickOvGInst();
    tsprintf(list[i], "0x%02x: %s", data->op, data->mn);
    l		+= strlen(list[i]) + 1;
    i++;
  }
  endi = i;

  for (i = j = k = 0 ; i < endi ; i++){
    for (j = 0 ; list[i][j] - '\0' ; j++)	str[k++] = list[i][j];
    str[k++] = '\n';
  }
  str[k] = '\0';

  i = 0;
  XtSetArg(args[i], XtNstring, str);	i++;
  XtSetArg(args[i], XtNlength, l) ;	i++;
  XtSetValues(instInfoArea, args, i);  
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
    = XtVaCreateManagedWidget("fileCloseButton",
			      smeBSBObjectClass, menu,
			      NULL);
  XtAddCallback(button,
		XtNcallback, (XtCallbackProc)CloseButtonPress,
		NULL);
}

static void	CreateInstInfoArea(w)
     Widget w;
{
  static  XawTextSelectType sarray[] =
  {XawselectLine, XawselectAll, XawselectNull};

  instInfoArea
    = XtVaCreateManagedWidget("instInfoArea",
			      asciiTextWidgetClass,	w,
			      XtNshowGrip,		False,
			      XtNeditType,		XawtextRead,
			      XtNselectTypes,		sarray,
			      XtNwidth,			100,
			      XtNscrollVertical,	XawtextScrollAlways,
			      XtNdisplayCaret,		False,
			      NULL);
}

/*
 *- Action definition.
 */
static void	CloseButtonPress(w)
     Widget w;
{
  Arg args[1];
  XtPopdown(ovInstWindow);
  XtSetArg(args[0], XtNsensitive, True);
  XtSetValues(ovInstOpenButton, args, 1);
}

/*
 *- This windows process routines.
 */
