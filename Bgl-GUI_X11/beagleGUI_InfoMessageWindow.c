/*
 * beagleGUI_InfoMessageWindow.c -- 
 * 
 * This work has been done at ATR HIP
 * 
 * Author          : Marc Chaland
 * Created On      : Fri Sep 19 10:56:09 1997
 * Last Modified By: YOSHIKAWA Tooru
 * Last Modified On: Thu Jun 17 15:26:46 1999
 * Update Count    : 27
 * Status          : Unknown, Use with caution!
 */

#include "clnt.h"
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>

#include <X11/Xaw/List.h>
#include <X11/Xaw/Paned.h>
#include <X11/Xaw/Box.h>
#include <X11/Xaw/MenuButton.h>
#include <X11/Xaw/SimpleMenu.h>
#include <X11/Xaw/SmeBSB.h>
#include <X11/Xaw/Label.h>
#include <X11/Xaw/Viewport.h>

#include "beagleGUI_Global.h"
#include "beagleGUI_InfoMessageWindow.h"

/*
 *- SPECIAL VALIABLE NAME
 *- =====================
 *- This window show only one variable for another file using the file
 *- 'beagleGUI_Global.h'. It name and type a 'Widget infoMessageWindow'.
 *- So you can open this window from every GUI file if need it.
 */

/*
 *- HERE ARE EXTERN FUNCTION DEFINEs.
 *- ================================
 *- This window (infoMessageWindow) use here external function only.
 *- So this file need the file include here function fo linking.
 *- I tried to collect there functions in '../Bglclnt/beagleRoutines.c'.
 *- Another GUI functions are define in the 'beagleGUI_InfoMessageWindow.h'.
 */

/*** NONE ***/

/*
 *- HERE ARE GLOBAL VARIABLE (EFFECTIVE IN THIS FILE ONLY) DEFINEs.
 *- ===============================================================
 *- So another files functions cannot search these.
 *- I want to avoid from linking trouble.
 */
static Widget   infoMessageInfoArea;

#define	INFO_LIST_MAX	300

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
static void	CreateInfoMessageInfoArea	P_((Widget));
/*- Action definition.*/
static void	CloseButtonPress	P_((Widget));
/*- This windows process routines -*/


/*
 *- HERE ARE THIS WINDOW's SERVICE FUNCTIONs FOR CALL FROM ANOTHER FILEs.
 *- =====================================================================
 *- I wrote prototype define in 'beagleGUI_InfoMessageWindow.h'.
 */
void CreateInfoMessageWindow(topWidget)
     Widget topWidget;
{
  static Widget   form;

  infoMessageWindow
    = XtVaCreatePopupShell("infoMessageWindow",
			   topLevelShellWidgetClass, topWidget,
			   NULL);
  form
    = XtVaCreateManagedWidget("infoMessageForm",
			      panedWidgetClass,	infoMessageWindow,
			      XtNwidth,		500,
			      XtNheight,	200,
			      NULL);
  CreateMenuBar(form);
  CreateInfoMessageInfoArea(form);
}

void	InfoInfoWrite(d)
     String d;
{
  static int numOfListItem = 0;
  static String list[INFO_LIST_MAX];
  Arg args[1];

  tsprintf( bglStrBuf,"%s", d);
  if ( numOfListItem < INFO_LIST_MAX ){
    list[numOfListItem] = XtNewString(bglStrBuf);
    numOfListItem++;
  }
  else{
    int to;
    XtFree(list[0]);
    for (to=0; to<INFO_LIST_MAX-1; to++){
      list[to] = list[to+1];
    }
    list[to] = XtNewString(bglStrBuf);
  }
  XawListChange(infoMessageInfoArea, list, numOfListItem, 0, True);
  if (!XtIsRealized(infoMessageWindow)){
    XtSetArg(args[0], XtNsensitive, False);
    XtSetValues(infoMessageOpenButton, args, 1);
    XtPopup(infoMessageWindow, XtGrabNone);
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
  Widget openButton, menu, button;

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
			      smeBSBObjectClass,	 menu,
			      NULL);
  XtAddCallback(button,
		XtNcallback, (XtCallbackProc)CloseButtonPress,
		NULL);
}

static void	CreateInfoMessageInfoArea(w)
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
			      XtNlabel, "          Beagle Info Messages          ",
			      NULL);
  viewport
    = XtVaCreateManagedWidget("viewport",
			      viewportWidgetClass,	w,
			      XtNshowGrip,	False,
			      XtNforceBars,	True,
			      XtNallowVert,	True,
			      XtNwidth,		500,
			      XtNheight,	220,
			      NULL);

  infoMessageInfoArea
    = XtVaCreateManagedWidget("infoMessageInfoArea",
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
  XtPopdown(infoMessageWindow);
  XtSetArg(args[0], XtNsensitive, True);
  XtSetValues(infoMessageOpenButton, args, 1);
}

/*
 *- This windows process routines.
 */
