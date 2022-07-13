/* beagleGUI_OvInfoWindow.c -- -*- C -*-
 * 
 *-  BEAGLE OVERVIEW INFORMATION window.
 * 
 * Author          : YOSHIKAWA Tooru
 * Created On      : Fri Aug 23 16:41:03 1996
 * Last Modified By: YOSHIKAWA Tooru
 * Last Modified On: Thu Jun 17 15:33:00 1999
 * Update Count    : 120
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

#include "beagleMessageMgr.h"
#include "beagleGUI_Global.h"
#include "beagleGUI_KeyInWindow.h"
#include "beagleGUI_OvInfoWindow.h"
#include "beagleGUI_OverviewWindow.h"

#define	INFO_FRAME_WIDTH	700
#define	INFO_FRAME_HEIGHT	100


/*
 *- SPECIAL VALIABLE NAME
 *- =====================
 *- This window show only one variable for another file using the file
 *- 'beagleGUI_Global.h'. It name and type a 'Widget ovInfoWindow'.
 *- So you can open this window from every GUI file if need it.
 */

/*
 *- HERE ARE EXTERN FUNCTION DEFINEs.
 *- ================================
 *- This window (ovInfoWindow) use here external function only.
 *- So this file need the file include here function fo linking.
 *- I tried to collect there functions in '../Bglclnt/beagleRoutines.c'.
 *- Another GUI functions are define in the 'beagleGUI_OvInfoWindow.h'.
 */

/*** NONE ***/


/*
 *- HERE ARE GLOBAL VARIABLE (EFFECTIVE IN THIS FILE ONLY) DEFINEs.
 *- ===============================================================
 *- So another files functions cannot search these.
 *- I want to avoid from linking trouble.
 */
extern BglOvFrameSt	ovFrame; /* beagleGUI_OverviewWindow.c */
extern int	colorAssignTable[NUM_OF_COLOR_VARIATION];

static Widget	infoArea;

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
static void	CreateInfoArea	P_((Widget));
/*- Action definition.*/
static void	CloseButtonPress	P_((Widget));
/*- This windows process routines -*/


/*
 *- HERE ARE THIS WINDOW's SERVICE FUNCTIONs FOR CALL FROM ANOTHER FILEs.
 *- =====================================================================
 *- I wrote prototype define in 'beagleGUI_OvInfoWindow.h'.
 */
void	CreateOvInfoWindow(topWidget)
     Widget topWidget;
{
  Widget	form;

  ovInfoWindow
    = XtVaCreatePopupShell("ovInfoWindow",
			   topLevelShellWidgetClass, topWidget,
			   NULL);
  form
    = XtVaCreateManagedWidget("infoForm",
			      panedWidgetClass,	ovInfoWindow,
			      NULL);
  CreateMenuBar(form);
  CreateInfoArea(form);
}

void	CleanInfoGeneColor()
{
  if(XtIsRealized(infoArea) ) XClearWindow(display,XtWindow(infoArea));
}

void	ChangeOvGInfoData()
{
  OvSizeData_t*	p;
  int	i = 0;
  static char	labelStr1[50];
  static char	labelStr2[50];
  static char	labelStr3[50];
  Window w = XtWindow(infoArea);
  I32s	sumPop = 0;
  I32s	sumArea = 0;
 /*Window w = XtWindow(infoAreaBackUp);*/

  if( !XtIsRealized(infoArea) )  return;

  p = Ov_GetSizeRoot();
  for ( i = 0; i < NUM_OF_COLOR_VARIATION-1; i++ ){

    if (p){
      XFillRectangle(display, w, colorGC[p->color],
		     50+i*60, 40, 40, 4);
      tsprintf( labelStr1, "%8d", p->length );
      tsprintf( labelStr2, "%6.2f %%", (float)p->area*100.0/ovFrame.SoupSize );
      tsprintf( labelStr3, "%8d", p->pop );
      p = p->next;
    }
    else{
      XFillRectangle(display, w, colorGC[colorAssignTable[i]],
		     50+i*60, 40, 40, 4);
      tsprintf( labelStr1, "     N/A " );
      tsprintf( labelStr2, " ------- " );
      tsprintf( labelStr3, "   ------" );
    }
    XFillRectangle(display, w,   black_gc, 27+i*60, 50, 60, 30);
    XDrawString(display,w,white_gc,27+i*60,60, labelStr1, strlen(labelStr1));
    XDrawString(display,w,white_gc,34+i*60,70, labelStr2, strlen(labelStr2));
    XDrawString(display,w,white_gc,27+i*60,80, labelStr3, strlen(labelStr3));

  }

  XFillRectangle(display, w,
		 colorGC[colorAssignTable[NUM_OF_COLOR_VARIATION-1]],
		 50+i*60, 40, 40, 4);
  if (p){
    if ( p->next == (OvSizeData_t*)0 ){
      tsprintf( labelStr1, "%8d", p->length );
      tsprintf( labelStr2, "%6.2f %%", (float)p->area*100.0/ovFrame.SoupSize );
      tsprintf( labelStr3, "%8d", p->pop );
    }
    else{
      while(p){
	sumPop += p->pop;
	sumArea += p->area;
	p = p->next;
      }
      tsprintf( labelStr1, "    Other" );
      tsprintf( labelStr2, "%6.2f %%", (float)sumArea*100.0/ovFrame.SoupSize );
      tsprintf( labelStr3, "%8d", sumPop );
    }
  }
  else{
    tsprintf( labelStr1, "     N/A " );
    tsprintf( labelStr2, " ------- " );
    tsprintf( labelStr3, "   ------" );
  }
  XFillRectangle(display, w,   black_gc, 27+i*60, 50, 60, 30);
  XDrawString(display,w,white_gc,27+i*60,60, labelStr1, strlen(labelStr1));
  XDrawString(display,w,white_gc,34+i*60,70, labelStr2, strlen(labelStr2));
  XDrawString(display,w,white_gc,27+i*60,80, labelStr3, strlen(labelStr3));
}


void	RedrawOvInfoArea(w, event, parms, num_parms)
     Widget w;
     XExposeEvent* event;
     String* parms;
     Cardinal* num_parms;
{
  ChangeOvGInfoData();
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

static void	CreateInfoArea(w)
     Widget w;
{
  Widget	viewport;
  String trans =  "<Expose>:    redrawOvInfoArea()";

  viewport
    = XtVaCreateManagedWidget("viewport",
			      viewportWidgetClass,	w,
			      XtNshowGrip,		False,
			      XtNallowHoriz,		True,
			      XtNallowVert,		True,
			      NULL);
  infoArea
    = XtVaCreateManagedWidget("infoArea",
			      coreWidgetClass,	viewport,
			      XtNtranslations,
			      XtParseTranslationTable(trans),
			      XtNwidth,		INFO_FRAME_WIDTH,
			      XtNheight,		INFO_FRAME_HEIGHT,
			      XtNbackground,
			      BlackPixelOfScreen
			      ( DefaultScreenOfDisplay( display ) ),
			      NULL);
}

/*
 *- Action definition.
 */
static void	CloseButtonPress(w)
     Widget w;
{
  Arg args[1];
  XtPopdown(ovInfoWindow);
  XtSetArg(args[0], XtNsensitive, True);
  XtSetValues(ovInfoOpenButton, args, 1);
}

/*
 *- This windows process routines.
 */
