/* beagleGUI_MessageWindow.c -- -*- C -*-
 * 
 *-  BEAGLE MESSAGE AREA window.
 * 
 * Author          : YOSHIKAWA Tooru
 * Created On      : Mon Aug 26 17:53:30 1996
 * Last Modified By: YOSHIKAWA Tooru
 * Last Modified On: Thu Jun 17 15:29:58 1999
 * Update Count    : 135
 * Status          : 
 */

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
#include "beagleGUI_MessageWindow.h"


/*
 *- SPECIAL VALIABLE NAME
 *- =====================
 *- This window show only one variable for another file using the file
 *- 'beagleGUI_Global.h'. It name and type a 'Widget messageWindow'.
 *- So you can open this window from every GUI file if need it.
 */

/*
 *- HERE ARE EXTERN FUNCTION DEFINEs.
 *- ================================
 *- This window (messageWindow) use here external function only.
 *- So this file need the file include here function fo linking.
 *- I tried to collect there functions in '../Bglclnt/beagleRoutines.c'.
 *- Another GUI functions are define in the 'beagleGUI_MessageWindow.h'.
 */

/*** NONE ***/

/*
 *- HERE ARE GLOBAL VARIABLE (EFFECTIVE IN THIS FILE ONLY) DEFINEs.
 *- ===============================================================
 *- So another files functions cannot search these.
 *- I want to avoid from linking trouble.
 */
static Widget	messageInfoArea;


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
static void	CreateMessageInfoArea	P_((Widget));
/*- Action definition.*/
static void	CloseButtonPress	P_((Widget));
/*- This windows process routines -*/
static void	MessageInfoWrite	P_((char*));

/*
 *- HERE ARE THIS WINDOW's SERVICE FUNCTIONs FOR CALL FROM ANOTHER FILEs.
 *- =====================================================================
 *- I wrote prototype define in 'beagleGUI_MessageWindow.h'.
 */
void	CreateMessageWindow(topWidget)
     Widget topWidget;
{  
  Widget	form;

  messageWindow
    = XtVaCreatePopupShell("messageWindow",
			   topLevelShellWidgetClass, topWidget,
			   NULL);
  form
    = XtVaCreateManagedWidget("messageForm",
			      panedWidgetClass,	messageWindow,
			      XtNwidth,		500,
			      XtNheight,	200,
			      NULL);
  CreateMenuBar(form);
  CreateMessageInfoArea(form);
}

void	UpdateMessageData()
{
  BglMessageSt* data;
  Arg args[1];
  while( BglMM_HowManyMessage() ){
    data = BglMM_PickMessage();
    MessageInfoWrite(data->message);
  }

  if ( ! XtIsRealized(messageWindow)){
    XtSetArg(args[0], XtNsensitive, False);
    XtSetValues(messageOpenButton, args, 1);
    XtPopup(messageWindow, XtGrabNone);
  }
}

void	UpdateRepMessageData()
{
  BglReplyMesSt* d;
  Arg args[1];
  while( BglMM_HowManyReplyMes() ){
    d = BglMM_PickReplyMes();
    MessageInfoWrite(d->mes);
  }

  if ( ! XtIsRealized(messageWindow)){
    XtSetArg(args[0], XtNsensitive, False);
    XtSetValues(messageOpenButton, args, 1);
    XtPopup(messageWindow, XtGrabNone);
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
  Widget menuBar;

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
			      NULL);
  XtAddCallback(button,
		XtNcallback, (XtCallbackProc)CloseButtonPress,
		NULL);
}

static void	CreateMessageInfoArea(w)
     Widget w;
{
  Widget itemLabel, viewport;

  itemLabel
    = XtVaCreateManagedWidget("itemLabel",
			      labelWidgetClass,		w,
			      XtNshowGrip,		False,
			      XtNresize,		False,
			      XtNheight,		10,
			      XtNdefaultDistance,	0,
			      XtNhorizDistance,		0,
			      XtNvertDistance,		0,
			      XtNlabel, "          Tierra Info Messages          ",
			      NULL);
  viewport
    = XtVaCreateManagedWidget("viewport",
			      viewportWidgetClass,	w,
			      XtNshowGrip,	False,
			      /*XtNallowHoriz,	True,*/
			      XtNforceBars,	True,
			      XtNallowVert,	True,
			      XtNwidth,		500,
			      XtNheight,	220,
			      NULL);
  messageInfoArea
    = XtVaCreateManagedWidget("messageList",
			      listWidgetClass, viewport,
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
  XtPopdown(messageWindow);
  XtSetArg(args[0], XtNsensitive, True);
  XtSetValues(messageOpenButton, args, 1);
}


/*
 *- This windows process routines.
 */
#define	MESSAGE_LIST_MAX	100
static void	MessageInfoWrite(msg)
     char* msg;
{
  static int	i=0;
  static String list[MESSAGE_LIST_MAX];

  if ( i < MESSAGE_LIST_MAX ){
    list[i] = XtNewString(msg);
    i++;
  }
  else{
    int to;
    XtFree(list[0]);
    for (to=0; to<MESSAGE_LIST_MAX-1; to++){
      list[to] = list[to+1];
    }
    list[to] = XtNewString(msg);
  }
  XawListChange(messageInfoArea, list, i, 0, True);
}
#undef MESSAGE_LIST_MAX
