/* beagleGUI_CommonActions.c -- -*- C -*-
 * 
 * This work has been done at ATR HIP
 * 
 * Author          : YOSHIKAWA Tooru
 * Created On      : Fri Oct 17 17:06:32 1997
 * Last Modified By: YOSHIKAWA Tooru
 * Last Modified On: Thu Nov 12 16:55:05 1998
 * Update Count    : 58
 * Status          : 
 */

#ifndef lint
static char* sccsProgramId="@(#)beagleGUI_CommonActions.c	1.9	05/10/99 15:55:01";
#endif /* lint */

#include "clnt.h"
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>

#include "beagleGUI_CommonActions.h"

/*
 *- HERE ARE GLOBAL VARIABLE (EFFECTIVE IN THIS FILE ONLY) DEFINEs.
 *- ===============================================================
 *- So another files functions cannot search these.
 *- I want to avoid from linking trouble.
 */
static Widget	popupMenuStack[10];
static int	popupMenuStackPointer = 0;


/*
 *- HERE ARE STATIC FUNCTIONs PROTOTYPE DEFINEs.
 *- ============================================
 *- These functions call from in this file only.
 *- So another files functions cannot search these.
 *- I want to avoid from linking trouble.
 */
static void	PushToPopupMenuStack	P_((Widget));
static Widget	PopFromPopupMenuStack	P_((void));
static void	PopupMenuStackEnterCheck	P_((Widget));


/*
 *- HERE ARE COMMON ACTION FUNCTIONs FOR CALL FROM ANOTHER FILEs.
 *- ==============================================================
 *- I wrote prototype define in 'beagleGUI_CommonActions.h'.
 */
void	PositionCascPullDownMenu(w, bevent, parms, num_parms)
     Widget w;
     XEvent* bevent;
     String* parms;
     Cardinal* num_parms;
{
  Arg	args[3];
  Position x, y, ax, ay;
  Dimension width;
  int i;
  Widget parent, cascmenu, menu;
  
  parent = XtParent(w);
  menu   = XtNameToWidget(parent, parms[0]);
  cascmenu = XtParent(parent);
  
  i = 0;
  XtSetArg(args[i], XtNx, &x);  i++;
  XtSetArg(args[i], XtNy, &y);  i++;
  XtSetArg(args[i], XtNwidth, &width);  i++;
  XtGetValues(w, args, i);
  i = 0;
  XtSetArg(args[i], XtNx, &ax); i++;
  XtSetArg(args[i], XtNy, &ay); i++;
  XtGetValues(cascmenu, args, i);

  if (strcmp(XtName(w), "arrow"))
    x  += width;
  ax += x;
  ay += y;
  
  i = 0;
  XtSetArg(args[i], XtNx, ax);  i++;
  XtSetArg(args[i], XtNy, ay);  i++;
  XtSetValues(menu, args, i);
}

void	CascMenuPopup(w, bevent, parms, num_parms)
     Widget w;
     XEvent* bevent;
     String* parms;
     Cardinal* num_parms;
{
  Widget menu;

  if (bevent->xcrossing.state & Button1Mask)
    {
      menu = XtNameToWidget(XtParent(w), parms[0]);
      XtPopup(menu, XtGrabNone);
      PushToPopupMenuStack(menu);
    }
}

void	CascMenuEnter(w, bevent, parms, num_parms)
     Widget w;
     XEvent* bevent;
     String* parms;
     Cardinal* num_parms;
{
  Widget menu;

  menu = XtNameToWidget(XtParent(w), parms[0]);
  PopupMenuStackEnterCheck(menu);
}

void	AllCascMenuPopdown(w, bevent, parms, num_parms)
     Widget w;
     XEvent* bevent;
     String* parms;
     Cardinal* num_parms;
{
  Widget menu;

  while(menu = PopFromPopupMenuStack()){
    XtPopdown(menu);
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

static void	PushToPopupMenuStack(w)
     Widget w;
{
  popupMenuStack[popupMenuStackPointer++] = w;
}

static Widget	PopFromPopupMenuStack()
{
  if ( ! popupMenuStackPointer ) return 0;
  else	return popupMenuStack[--popupMenuStackPointer];
}

static void	PopupMenuStackEnterCheck(w)
     Widget w;
{
  Widget	menu;

  while(menu = PopFromPopupMenuStack()){
    if ( w != menu ) XtPopdown(menu);
    else{
      PushToPopupMenuStack(menu);
      return;
    }
  }
}
