/* beagleGUI_VarWindow.c -- -*- C -*-
 * 
 *-  BEAGLE VAR window.
 * 
 * Author          : YOSHIKAWA Tooru
 * Created On      : Thu Oct 24 10:47:20 1996
 * Last Modified By: YOSHIKAWA Tooru
 * Last Modified On: Tue Oct 26 11:40:37 1999
 * Update Count    : 129
 * Status          : 
 */

#ifndef lint
static char* sccsProgramId="@(#)beagleGUI_VarWindow.c	1.25	10/26/99 11:41:25";
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
#include "beagleGUI_InfoWindows.h"
#include "beagleGUI_KeyInWindow.h"
#include "beagleGUI_VarWindow.h"

#define	VAR_LIST_MAX	500

/*
 *- SPECIAL VALIABLE NAME
 *- =====================
 *- This window show only one variable for another file using the file
 *- 'beagleGUI_Global.h'. It name and type a 'Widget varWindow'.
 *- So you can open this window from every GUI file if need it.
 */

/*
 *- HERE ARE EXTERN FUNCTION DEFINEs.
 *- ================================
 *- This window (varWindow) use here external function only.
 *- So this file need the file include here function fo linking.
 *- I tried to collect there functions in '../Bglclnt/beagleRoutines.c'.
 *- Another GUI functions are define in the 'beagleGUI_varWindow.h'.
 */

/* None */


/*
 *- HERE ARE GLOBAL VARIABLE (EFFECTIVE IN THIS FILE ONLY) DEFINEs.
 *- ===============================================================
 *- So another files functions cannot search these.
 *- I want to avoid from linking trouble.
 */
static Widget	varInfoArea;

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
static void	PutAlterButton		P_((Widget));
static void	CreateVarInfoArea	P_((Widget));
/*- Action definition.*/
static void	CloseButtonPress	P_((Widget));
static void	AlterStartButtonPress	P_((Widget));
/*- This windows process routines -*/


/*
 *- HERE ARE THIS WINDOW's SERVICE FUNCTIONs FOR CALL FROM ANOTHER FILEs.
 *- =====================================================================
 *- I wrote prototype define in 'beagleGUI_VarWindow.h'.
 */
void	CreateVarWindow(topWidget)
     Widget topWidget;
{
  Widget	form;

  varWindow
    = XtVaCreatePopupShell("varWindow",
			   topLevelShellWidgetClass, topWidget,
			   NULL);
  form
    = XtVaCreateManagedWidget("varForm",
			      panedWidgetClass,	varWindow,
			      XtNwidth,		600,
			      XtNheight,	600,
			      NULL);
  CreateMenuBar(form);
  CreateVarInfoArea(form);
}


void	UpdateVarData()
{
  static int last_i = 0;
  int	i;
  static String list[VAR_LIST_MAX];
  BglVarSt* data;

  for ( i = 0; i < last_i; i++ )   if ( list[i] ) XtFree(list[i]);
  XClearWindow(display, XtWindow(varInfoArea) );
  i = 0;
  while( BglMM_HowManyVar() ){
    data = BglMM_PickVar();
    if (((data->value[0] >= '0') && (data->value[0] <= '9'))
	|| (data->value[0] == '-'))
      tsprintf(bglStrBuf, "  %-18s   %13s  ", data->name, data->value);
    else
      tsprintf(bglStrBuf, "  %-18s   %-13s  ", data->name, data->value); 
    list[i] = XtNewString(bglStrBuf);
    i++;
    if ( i == VAR_LIST_MAX )  break;
  }
  XawListChange(varInfoArea, list, i, 0, True);
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
  PutAlterButton(menuBar);
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

static void	PutAlterButton(w)
     Widget w;
{
  Widget openButton;
  Widget menu, button;

  openButton
    = XtVaCreateManagedWidget("alterOpenButton",
			      menuButtonWidgetClass, w,
			      XtNlabel,		"Alter",
			      XtNmenuName,	"alterPullDownMenu",
			      NULL);
  menu
    = XtVaCreatePopupShell("alterPullDownMenu",
			   simpleMenuWidgetClass, openButton,
			   NULL);
  button
    = XtVaCreateManagedWidget("alterStartButton",
			      smeBSBObjectClass, menu,
			      XtNlabel,		"Alter Variables",
			      NULL);
  XtAddCallback(button,
		XtNcallback, (XtCallbackProc)AlterStartButtonPress,
		NULL);
}

static void	CreateVarInfoArea(w)
     Widget w;
{
  Widget	viewport;

  viewport
    = XtVaCreateManagedWidget("viewport",
			      viewportWidgetClass,	w,
			      XtNshowGrip,		False,
			      /*XtNallowHoriz,		True,*/
			      XtNallowVert,		True,
			      XtNwidth,		400,
			      XtNheight,	600,
			      NULL);
  varInfoArea
    = XtVaCreateManagedWidget("varInfoArea",
			      listWidgetClass,	viewport,
			      XtNwidth,		400,
			      XtNheight,	1000,
			      NULL);
}

/*
 *- Action definition.
 */
static void	CloseButtonPress(w)
     Widget w;
{
  Arg args[1];
  XtPopdown(varWindow);
  XtSetArg(args[0], XtNsensitive, True);
  XtSetValues(varOpenButton, args, 1);
}

static void	AlterStartButtonPress(w)
     Widget w;
{
  XawListReturnStruct *lselm;
  String input[]  = {NULL, NULL};
  String output[] = {"Enter name of variable",
		     "Enter value"};
  int str1, str2, i, j;

  lselm = XawListShowCurrent(varInfoArea);
  if (lselm->list_index != XAW_LIST_NONE){
    strcpy(bglStrBuf, lselm->string);
    for (i = 0 ; bglStrBuf[i] == ' ' ; i++);
    str1 = i;
    while (bglStrBuf[++i] != ' ');
    bglStrBuf[i++] = '\0';

    for (; (bglStrBuf[i] == '=') || (bglStrBuf[i] == ' ') ; i++);
    str2 = i;
    while (bglStrBuf[++i] != ' ');
    bglStrBuf[i++] = '\0';

    input[0] = bglStrBuf + str1;
    input[1] = bglStrBuf + str2;
    XtFree((char*)lselm);
  }
  
  StrKeyIn(input, 2, output);
  if ( strlen(*input) ){
    for( j = 0 ; ((bglStrBuf[j] = input[0][j]) != ' ') && input[0][j] ; j++);
    if (bglStrBuf[j] != ' ')
      bglStrBuf[j++] = ' ';
    bglStrBuf[j++] = '=';
    bglStrBuf[j++] = ' ';
    for( i = 0 ; (input[1][i] == ' ') ; i++);
    while (bglStrBuf[j++] = input[1][i++]);
    bglStrBuf[j] = '\0';

    BglMM_VarAlter_Need(bglStrBuf);
    BglMM_Var_modeReset();
    BglMM_Var_need();
  }
}

/*
 *- This windows process routines.
 */
