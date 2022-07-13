/* beagleGUI_OverviewWindow.c -- -*- C -*-
 * 
 *-  BEAGLE OVERVIEW window.
 * 
 * Author          : YOSHIKAWA Tooru
 * Created On      : Fri Aug 23 16:41:03 1996
 * Last Modified By: YOSHIKAWA Tooru
 * Last Modified On: Tue Nov 16 15:02:32 1999
 * Update Count    : 927
 * Status          : 
 */

#include "clnt.h"
#include <X11/Intrinsic.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include <X11/cursorfont.h>

#include <X11/Xaw/Cardinals.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/Box.h>
#include <X11/Xaw/Viewport.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Paned.h>
#include <X11/Xaw/MenuButton.h>
#include <X11/Xaw/SimpleMenu.h>
#include <X11/Xaw/SmeBSB.h>
#include <X11/Xaw/Porthole.h>

#include "beagleMessageMgr.h"
#include "beagleUTIL_OVMgr.h"

#include "beagleGUI_Global.h"
#include "beagleGUI_TopWindow.h"
#include "beagleGUI_KeyInWindow.h"
#include "beagleGUI_InfoWindows.h"
#include "beagleGUI_OverviewWindow.h"
#include "beagleGUI_OvInfoWindow.h"

#define	LEFT_MARGIN	30
#define	LABEL_WIDTH	50
#define	LEFT_OFFSET	(LEFT_MARGIN + LABEL_WIDTH)
#define	TOP_MARGIN	30
#define	TOP_OFFSET	(TOP_MARGIN)
#define	RIGHT_MARGIN	30
#define	BOTTOM_MARGIN	30
#define	FRAME_LINE_WIDTH	4
#define	NUM_OF_GENE_ON_LINE	1000
#define NUM_OF_GENE_ON_PAGE	400000
#define NUM_OF_LINE_ON_PAGE	(NUM_OF_GENE_ON_PAGE / NUM_OF_GENE_ON_LINE)
#define	SOUP_WIDTH	(NUM_OF_GENE_ON_LINE)
#define	LINE_TO_DRAW_Y(l)  (int)(TOP_MARGIN+(l)*(geneLineWidth+geneLineSkip))

#define	MOST_THIN_LINE	2
#define	INITIAL_WIDTH	(LEFT_OFFSET + SOUP_WIDTH + RIGHT_MARGIN)
#define	INITIAL_HEIGHT\
  (TOP_MARGIN + NUM_OF_LINE_ON_PAGE * (MOST_THIN_LINE * 2 + 2) + BOTTOM_MARGIN)

/*
 *- SPECIAL VALIABLE NAME
 *- =====================
 *- This window show only here variables for another file using the file
 *- 'beagleGUI_Global.h'. It name and type a 'Widget overviewWindow'.
 *- So you can open this window from every GUI file if need it.
 */
BglOvFrameSt	ovFrame;	/* Overview's frame information data.*/
int	colorAssignTable[NUM_OF_COLOR_VARIATION];
Cursor	OvCurs;

/*
 *- HERE ARE EXTERN FUNCTION DEFINEs.
 *- ================================
 *- This window (overviewWidow) use here external function only.
 *- So this file need the file include here function fo linking.
 *- I tried to collect there functions in '../Bglclnt/beagleRoutines.c'.
 *- Another GUI functions are define in the 'beagleGUI_OverviewWindow.h'.
 */
extern void	CleanInfoGeneColor();	/* beagleGUI_OvInfoWindow.c */
extern void	ChangeOvGInfoData();	/* beagleGUI_OvInfoWindow.c */
extern int	BglMM_IsOvSession_stop(); /* ../Bglcom/beagleMessage_Ov.c */

/*
 *- HERE ARE GLOBAL VARIABLE (EFFECTIVE IN THIS FILE ONLY) DEFINEs.
 *- ===============================================================
 *- So another files functions cannot search these.
 *- I want to avoid from linking trouble.
 */
static Widget	paned;
static Widget	memForm;
static Widget	stickylabel;
static Widget	pageUpButton, pageDownButton;
static Pixmap	memFormBackUpPixmap;

static int	memFormWidth  = INITIAL_WIDTH;
static int	memFormHeight = INITIAL_HEIGHT;
static int	geneLineWidth, geneLineSkip;
static int	numOfLine, numOfLineOnPage;
static I32s	topOfPage, endOfPage;

static int	maxLinesByACreature = 0;
static int	*drawPointsX, *drawPointsY;

static int	colorAssignNumber = 0;

static OvSizeData_t*	ovSizeDataRootP = (OvSizeData_t*)0;
static int	frameWasDrawnFlag = 0; /* 1: Drawn 0: Not yet */

typedef enum {nothing, request, selected} C_state;
static int	CursX, CursY;
static C_state	CursState;
static void	(*FuncCurs)(I32u);


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
static void	PutWindowButton		P_((Widget));
static void	PutModeButton		P_((Widget));
static void	PutPageUpButton		P_((Widget));
static void	PutPageDownButton	P_((Widget));
static void	CreateInfoArea		P_((Widget));
/*- Action definition.*/
static void	OpenInfoSubWindowButtonPress	P_((Widget));
static void	OpenInstSubWindowButtonPress	P_((Widget));
static void	CloseButtonPress	P_((Widget));
static void	Mode1ButtonPress	P_((Widget));
static void	Mode1Result		P_((I32u));
static void	Mode2ButtonPress	P_((Widget));
static void	PageUpButtonPress	P_((Widget));
static void	PageDownButtonPress	P_((Widget));
/*- This windows process routines -*/
static void	DrawOverviewMemFrame	P_((I32s));
static void	DrawOverviewMemFrameWindow	P_((I32s, Window));
static void	CreatureDraw		P_((char, I32s, I32s));
static void	ClearTableAndInfo	P_((void));
static void	CreatureBornDraw	P_((GC, I32s, I32s));
static void	CreatureBornDrawWindow	P_((Window, GC, I32s, I32s));
static void	CreatureDeathDraw	P_((I32s, I32s));
static void	CreatureDeathDrawWindow	P_((Window, I32s, I32s));
static int	StartAndLengthToPoints	P_((I32s, I32s));
static I32s	WhichAddress		P_((int, int));
static void	FormSizeDesign		P_((I32s));
static void	RedrawAllOvData		P_((void));
/*- about for SIZE DATA */
static int	AddSizeData		P_((I32s));
static void	DelSizeData		P_((I32s));
static void	ReAddSizeData		P_((OvSizeData_t*));
static void	ColorAssignCheck	P_((void));
static void	SizeDataClearAll	P_((void));
static void	AllCreaturesColorChange	P_((I32s,GC));
static int	SelectGcIndex		P_((void));
static int	ReleaseGcIndex		P_((int));

/*
 *- HERE ARE THIS WINDOW's SERVICE FUNCTIONs FOR CALL FROM ANOTHER FILEs.
 *- =====================================================================
 *- I wrote prototype define in 'beagleGUI_OverviewWindow.h'.
 */
void	CreateOverviewWindow(topWidget)
     Widget topWidget;
{
  Widget	form;

  overviewWindow
    = XtVaCreatePopupShell("overviewWindow",
			   topLevelShellWidgetClass, topWidget,
			   NULL);
  form
    = XtVaCreateManagedWidget("overviewForm",
			      panedWidgetClass,	overviewWindow,
			      XtNwidth,		screen->width - 130,
			      XtNheight,	screen->height - 30,
			      NULL);
  CreateMenuBar(form);
  CreateInfoArea(form);

  ClearTableAndInfo();
}


void	RedrawOverviewMemFrame(w, event, parms, num_parms)
     Widget w;
     XExposeEvent* event;
     String* parms;
     Cardinal* num_parms;
{
  if (XtIsRealized(overviewWindow))
    XCopyArea (display, memFormBackUpPixmap, XtWindow(memForm), default_gc,
	       event->x, event->y, event->width, event->height,
	       event->x, event->y);

}

int	Ov_MouseMapRequest(str, func)
     String str;
     void (*func)(I32u);
{
  Arg args[1];
  
  if (CursState == nothing){
    if (BglMM_OvSession_need()){
      XtSetArg(args[0], XtNsensitive, False);
      XtSetValues(overviewOpenButton, args, 1);
      XtPopup(overviewWindow, XtGrabNone);
    }
    else
      return 0;

    CursState = request;
    if (str){
      XtSetArg(args[0], XtNlabel, str);
      XtSetValues(stickylabel, args, 1);
    }
    FuncCurs = func;
    XtManageChild(stickylabel);
    return 1;
  }
  return 0;
}

void	Ov_MouseRedo(w, bevent, parms, num_parms)
     Widget w;
     XEvent* bevent;
     String* parms;
     Cardinal* num_parms;
{
  if (FuncCurs){
    CursState = request;
    BglMM_OvSession_need();
    Ov_EnterWindow(w, bevent, parms, num_parms);
    Ov_StickylabelUpdate(w, bevent, parms, num_parms);
  }
}

void	Ov_StickylabelUpdate(w, bevent, parms, num_parms)
     Widget w;
     XEvent* bevent;
     String* parms;
     Cardinal* num_parms;
{
  if (CursState == request)
    XtMoveWidget(stickylabel, bevent->xmotion.x + 10, bevent->xmotion.y + 10);
}

void	Ov_QuitWindow(w, bevent, parms, num_parms)
     Widget w;
     XEvent* bevent;
     String* parms;
     Cardinal* num_parms;
{
  if (CursState == request)
    XtUnmanageChild(stickylabel);
}

void	Ov_EnterWindow(w, bevent, parms, num_parms)
     Widget w;
     XEvent* bevent;
     String* parms;
     Cardinal* num_parms;
{
  if (CursState == request)
    XtManageChild(stickylabel);
}

void	Ov_PutCursorValue(w, bevent, parms, num_parms)
     Widget w;
     XEvent* bevent;
     String* parms;
     Cardinal* num_parms;
{
  if (CursState == request){
    CursX = bevent->xbutton.x;
    CursY = bevent->xbutton.y;
    CursState = selected;
    XtUnmanageChild(stickylabel);
  }
}

void	Ov_ConfirmCursorValue(w, bevent, parms, num_parms)
     Widget w;
     XEvent* bevent;
     String* parms;
     Cardinal* num_parms;
{
  I32s addr;
  Arg args[1];

  if ( (CursState == selected)
       && (abs(CursX - bevent->xbutton.x) < 2)
       && (abs(CursY - bevent->xbutton.y) < 2)){
    if ( (addr = WhichAddress(bevent->xbutton.x, bevent->xbutton.y)) >= 0)
      (*FuncCurs)(addr);
    else
      MessageMessage("Please click inside of soup.");

    BglMM_OvSession_stop();
    if (BglMM_IsOvSession_stop()){
      XtPopdown(overviewWindow);
      XtSetArg(args[0], XtNsensitive, True);
      XtSetValues(overviewOpenButton, args, 1);
    }
  }
  CursState = nothing;
}

void	UpdateOvFrameData(){
  BglOvFrameSt* data;

  if ( BglMM_HowManyOvFrame() ){
    data = BglMM_PickOvFrame();
    ovFrame.InstNum = data->InstNum;
    ovFrame.SoupSize = data->SoupSize;
    ovFrame.NumCells = data->NumCells;
    ovFrame.NumSizes = data->NumSizes;
    ovFrame.NumGenotypes = data->NumGenotypes;
    FormSizeDesign(ovFrame.SoupSize);
    ClearTableAndInfo();
    CleanInfoGeneColor();

    DrawOverviewMemFrame(ovFrame.SoupSize);
    frameWasDrawnFlag = 1;
  }
}

void	UpdateOvData(){
  BglOvMesSt* data;
  int	times;
  int	nOv;
  static int	lastNOv = 0;

  nOv = BglMM_HowManyOv();
  if (lastNOv != nOv){
    TopDisplayNOVChange( nOv );
    lastNOv = nOv;
  }

  times = 30;
  while( frameWasDrawnFlag && BglMM_HowManyOv() && times-- ){
    data = BglMM_PickOv();
    CreatureDraw(data->event, data->start, data->length);
  }
}

OvSizeData_t*	Ov_GetSizeRoot(){	return ovSizeDataRootP;	}

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
  PutWindowButton(menuBar);
  PutModeButton(menuBar);
  PutPageUpButton(menuBar);
  PutPageDownButton(menuBar);
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
			      smeBSBObjectClass, menu,
			      NULL);
  XtAddCallback(button,
		XtNcallback, (XtCallbackProc)CloseButtonPress,
		NULL);
}


static void	PutWindowButton(w)
     Widget w;
{
  Widget openButton, menu;

  openButton
    = XtVaCreateManagedWidget("windowButton",
			      menuButtonWidgetClass, w,
			      XtNmenuName,	"windowPullDownMenu",
			      NULL);
  menu
    = XtVaCreatePopupShell("windowPullDownMenu",
			   simpleMenuWidgetClass, openButton,
			   NULL);

  ovInstOpenButton
    = XtVaCreateManagedWidget("openInstSubWindowButton",
			      smeBSBObjectClass, menu,
			      XtNlabel,	"Instruction Map",
			      NULL);
  XtAddCallback(ovInstOpenButton,
		XtNcallback, (XtCallbackProc)OpenInstSubWindowButtonPress,
		NULL);
  ovInfoOpenButton
    = XtVaCreateManagedWidget("openInfoSubWindowButton",
			      smeBSBObjectClass, menu,
			      XtNlabel,	"Count Information",
			      NULL);
  XtAddCallback(ovInfoOpenButton,
		XtNcallback, (XtCallbackProc)OpenInfoSubWindowButtonPress,
		NULL);
}


static void	PutModeButton(w)
     Widget w;
{
  Widget openButton, menu, button;

  openButton
    = XtVaCreateManagedWidget("modeButton",
			      menuButtonWidgetClass, w,
			      XtNlabel,		"Gene",
			      XtNmenuName,	"modePullDownMenu",
			      NULL);
  menu
    =  XtVaCreatePopupShell("modePullDownMenu",
			    simpleMenuWidgetClass, openButton,
			    NULL);
  button
    = XtVaCreateManagedWidget("overviewMode1",
			      smeBSBObjectClass, menu,
			      XtNlabel,	"from address in Soup",
			      NULL);
  XtAddCallback(button,
		XtNcallback, (XtCallbackProc)Mode1ButtonPress,
		NULL);
  button
    = XtVaCreateManagedWidget("overviewMode2",
			      smeBSBObjectClass, menu,
			      XtNlabel,	"from Genome",
			      NULL);
  XtAddCallback(button,
		XtNcallback, (XtCallbackProc)Mode2ButtonPress,
		NULL);
}

static void	PutPageUpButton(w)
     Widget w;
{
  pageUpButton
    = XtVaCreateManagedWidget("pageUpButton",
			      commandWidgetClass, w,
			      XtNlabel,		"Page UP",
			      NULL);

  XtAddCallback(pageUpButton,
		XtNcallback, (XtCallbackProc)PageUpButtonPress,
		NULL);
}

static void	PutPageDownButton(w)
     Widget w;
{
  pageDownButton
    = XtVaCreateManagedWidget("pageDownButton",
			      commandWidgetClass, w,
			      XtNlabel,		"Page DOWN",
			      NULL);

  XtAddCallback(pageDownButton,
		XtNcallback, (XtCallbackProc)PageDownButtonPress,
		NULL);
}

static void	CreateInfoArea(w)
     Widget w;
{
  Widget		viewport;
  String		trans =  "<Expose>:	redrawOverviewMemFrame()\n\
				<Enter>:	ov_EnterWindow()\n\
				<Leave>:	ov_QuitWindow()\n\
				<Motion>:	ov_StickylabelUpdate()\n\
				<Btn1Down>:	ov_PutCursorValue()\n\
				<Btn1Up>:	ov_ConfirmCursorValue()\n\
				<Key>space:	ov_MouseRedo()";
  viewport
    = XtVaCreateManagedWidget("viewport",
			      viewportWidgetClass,	w,
			      XtNshowGrip,		False,
			      XtNallowHoriz,		True,
			      XtNallowVert,		True,
			      NULL);
  paned
    = XtVaCreateManagedWidget("paned",
			      panedWidgetClass,	viewport,
			      XtNcursor,	OvCurs,
			      NULL);
  memForm
    = XtVaCreateManagedWidget("memForm",
			      compositeWidgetClass, paned,
			      XtNtranslations,	XtParseTranslationTable(trans),
			      XtNwidth,		memFormWidth,
			      XtNheight,	memFormHeight,
			      XtNbackground,	BlackPixelOfScreen
			      ( DefaultScreenOfDisplay( display ) ),
			      NULL);
  stickylabel = XtVaCreateWidget("stickylabel",
				 labelWidgetClass,	memForm,
				 NULL);
  memFormBackUpPixmap
    = XCreatePixmap(display, DefaultRootWindow(display),
		    memFormWidth, memFormHeight,
		    DefaultDepth(display,DefaultScreen(display)));

  XFillRectangle(display, memFormBackUpPixmap, black_gc,
		 0, 0, memFormWidth, memFormHeight );
}


/*
 *- Action definition.
 */
static void	OpenInstSubWindowButtonPress(w)
     Widget w;
{
  Arg args[1];
  XtSetArg(args[0], XtNsensitive, False);
  XtSetValues(ovInstOpenButton, args, 1);
  XtPopup(ovInstWindow, XtGrabNone);
}

static void	OpenInfoSubWindowButtonPress(w)
     Widget w;
{
  Arg args[1];
  XtSetArg(args[0], XtNsensitive, False);
  XtSetValues(ovInfoOpenButton, args, 1);
  XtPopup(ovInfoWindow, XtGrabNone);
}

static void	CloseButtonPress(w)
     Widget w;
{
  Arg args[1];
  BglMM_OvSession_stop();
  ClearTableAndInfo();

  XtSetArg(args[0], XtNsensitive, True);

  XtPopdown(ovInfoWindow);
  XtSetValues(ovInfoOpenButton, args, 1);

  XtPopdown(ovInstWindow);
  XtSetValues(ovInstOpenButton, args, 1);

  XtPopdown(overviewWindow);
  XtSetValues(overviewOpenButton, args, 1);

  /*= Debug session sometime need OV data, then if following variable
   *= change to 0 then the function UpdateOvData() will never pick up
   *= OV data from Message Manager Buffer.
   *=*/
  frameWasDrawnFlag = 0;

  /*= Debug session sometime use OV data, then sometime called draw routine.
   *= Then we must keep following area and data for drawing routines.
   *=*/
  tfree(drawPointsX, maxLinesByACreature*2, 51001); drawPointsX = 0;
  tfree(drawPointsY, maxLinesByACreature,   51002); drawPointsY = 0;
  maxLinesByACreature = 0;
}


static void	Mode1ButtonPress(w)
     Widget w;
{
  Ov_MouseMapRequest("Click on the cell\n you want to see", &Mode1Result);
}

static void	Mode1Result(nbuf)
     I32u nbuf;
{
  char	numStr[30];
  OvData_t* p;

  if ( (p = UTIL_Ov_SearchByPoint(nbuf)) != 0 ){
    tsprintf( numStr, "%d\n", p->start );
    BglMM_GeneAddress_Need(numStr);
    XtPopup(ovGeneWindow, XtGrabNone);
  }
  else{
    MessageMessage("There is no Creature");
  }
  return;
}

static void	Mode2ButtonPress(w)
     Widget w;
{
  String sizeStr[] = {NULL};
  static String label = "Input Genemo (ex. 0080aaa)";

  StrKeyIn(sizeStr, 1, &label );
  if ( strlen(*sizeStr) ){
    BglMM_GeneSize_Need(*sizeStr);
    XtPopup(ovGeneWindow, XtGrabNone);
  }
  return;
}

static void	PageUpButtonPress(w)
     Widget w;
{
  Arg args[1];

  topOfPage -= (NUM_OF_GENE_ON_PAGE/2);
  endOfPage = topOfPage + NUM_OF_GENE_ON_PAGE;
  numOfLineOnPage = NUM_OF_LINE_ON_PAGE;
  if (topOfPage == 0){
    XtSetArg(args[0], XtNsensitive, False);
    XtSetValues(pageUpButton, args, 1);
  }
  XtSetArg(args[0], XtNsensitive, True);
  XtSetValues(pageDownButton, args, 1);

  RedrawAllOvData();
  return;
}

static void	PageDownButtonPress(w)
     Widget w;
{
  Arg args[1];

  topOfPage += (NUM_OF_GENE_ON_PAGE/2);
  endOfPage += (NUM_OF_GENE_ON_PAGE/2);
  if (ovFrame.SoupSize <= endOfPage){
    endOfPage = ovFrame.SoupSize;
    numOfLineOnPage = (ovFrame.SoupSize-topOfPage) / NUM_OF_GENE_ON_LINE;
    if (numOfLineOnPage * NUM_OF_GENE_ON_LINE < ovFrame.SoupSize){
      numOfLineOnPage += 1;
    }
    XtSetArg(args[0], XtNsensitive, False);
    XtSetValues(pageDownButton, args, 1);
  }
  XtSetArg(args[0], XtNsensitive, True);
  XtSetValues(pageUpButton, args, 1);

  RedrawAllOvData();
  return;
}


/*
 *- This windows process routines.
 */
static void	DrawOverviewMemFrame(soupSize)
     I32s soupSize;
{
  DrawOverviewMemFrameWindow(soupSize,memFormBackUpPixmap);
  if (XtIsRealized(overviewWindow))
    DrawOverviewMemFrameWindow(soupSize,XtWindow(memForm));
}


static void	DrawOverviewMemFrameWindow(soupSize, w)
     I32s soupSize;
     Window w;
{
  int	line, draw_y, height;
  static char	labelStr[50];

  XFillRectangle(display, w, black_gc,
		 0, 0, INITIAL_WIDTH, INITIAL_HEIGHT );

  height = numOfLineOnPage * (geneLineWidth + geneLineSkip);

  XFillRectangle(display, w, white_gc,
		 LEFT_OFFSET - FRAME_LINE_WIDTH, TOP_OFFSET - geneLineSkip,
		 FRAME_LINE_WIDTH, height + geneLineSkip );
  XFillRectangle(display, w, white_gc,
		 memFormWidth - RIGHT_MARGIN, TOP_OFFSET - geneLineSkip,
		 FRAME_LINE_WIDTH, height + geneLineSkip );

  for ( line = 0; line < numOfLineOnPage; line++ ){
    draw_y = LINE_TO_DRAW_Y(line);
    if ( !(line%5) ){
      tsprintf( labelStr, "%-20d", line * NUM_OF_GENE_ON_LINE + topOfPage);
      XDrawString(display,w,white_gc, 25, draw_y+5 ,labelStr,strlen(labelStr));
      XDrawLine(display, w, white_gc,
		LEFT_OFFSET - FRAME_LINE_WIDTH - 5, draw_y,
		LEFT_OFFSET, draw_y);
    }
    if ( line < (numOfLineOnPage-1) ){
      XFillRectangle(display, w, gray_gc,
		     LEFT_OFFSET, draw_y-(geneLineWidth/2),
		     SOUP_WIDTH, geneLineWidth);
    }
    else{
      XFillRectangle(display, w, gray_gc,
		     LEFT_OFFSET, draw_y-(geneLineWidth/2),
		     ( (soupSize-topOfPage) % NUM_OF_GENE_ON_LINE),
		     geneLineWidth);
    }
  }
}

static void	CreatureDraw(event, start, length)
     char event;
     I32s start;
     I32s length;
{
  int	gcIndex;

  if ( event == 'b' ){
    gcIndex = AddSizeData( length );
    CreatureBornDraw( colorGC[gcIndex], start, length );
    ChangeOvGInfoData();
  }
  else if ( event == 'd' ){
    CreatureDeathDraw( start, length );
    DelSizeData( length );
    ChangeOvGInfoData();
  }
  return;
}

static void	ClearTableAndInfo()
{
  int i;

  SizeDataClearAll();
  for ( i = 0; i < NUM_OF_COLOR_VARIATION; i++ ){
    colorAssignTable[i] = i; /* same as colrGC index */
  }
  colorAssignNumber = 0;
}

static void	CreatureBornDraw(gc, start, length)
     GC gc;
     I32s start;
     I32s length;
{
  CreatureBornDrawWindow(memFormBackUpPixmap, gc, start, length);
  if (XtIsRealized(overviewWindow))
    CreatureBornDrawWindow(XtWindow(memForm), gc, start, length);
}

static void	CreatureBornDrawWindow(w, gc, start, length)
     Window w;
     GC gc;
     I32s start;
     I32s length;
{
  int	line, lines;

  if (0 < (lines = StartAndLengthToPoints(start,length))){

    XDrawLine(display, w, white_gc,
	      drawPointsX[0], drawPointsY[0]-(geneLineWidth/2)-2,
	      drawPointsX[0], drawPointsY[0]+(geneLineWidth/2)+2 );

    XDrawLine(display, w, white_gc,
	      drawPointsX[lines*2-1]-1,
	      drawPointsY[lines-1]-(geneLineWidth/2)-2,
	      drawPointsX[lines*2-1]-1,
	      drawPointsY[lines-1]+(geneLineWidth/2)+2);

    for ( line = 0; line < lines; line++ ){
      XFillRectangle(display, w, gc,
		     drawPointsX[line*2+0],
		     drawPointsY[line]-(geneLineWidth/2),
		     drawPointsX[line*2+1]-drawPointsX[line*2+0],
		     geneLineWidth);
    }
  }
}


static void	CreatureDeathDraw(start, length)
     I32s start;
     I32s length;
{
  CreatureDeathDrawWindow(memFormBackUpPixmap, start, length);
  if (XtIsRealized(overviewWindow))
    CreatureDeathDrawWindow(  XtWindow(memForm), start, length);
}

static void	CreatureDeathDrawWindow(w, start, length)
     Window w;
     I32s start;
     I32s length;
{
  int	line, lines;

  if (0 < (lines = StartAndLengthToPoints(start,length))){

    XDrawLine(display, w, black_gc,
	      drawPointsX[0], drawPointsY[0]-(geneLineWidth/2)-2,
	      drawPointsX[0], drawPointsY[0]+(geneLineWidth/2)+2 );

    XDrawLine(display, w, black_gc,
	      drawPointsX[lines*2-1]-1,
	      drawPointsY[lines-1]-(geneLineWidth/2)-2,
	      drawPointsX[lines*2-1]-1,
	      drawPointsY[lines-1]+(geneLineWidth/2)+2);

    for ( line = 0; line < lines; line++ ){
      XFillRectangle(display, w, gray_gc,
		     drawPointsX[line*2+0],
		     drawPointsY[line]-(geneLineWidth/2),
		     drawPointsX[line*2+1]-drawPointsX[line*2+0],
		     geneLineWidth);
    }
  }
}


/*-
 *-   ret = StartAndLengthToPoints(start, length)
 *-
 *-   This function calculate drawing points
 *-   by CREATUREs start and length variable.
 *-   And return a value for indicate the line state.
 *-
 *-    drawPointsX[] and drawPointsY[] are allocate in this function
 *-       if need it.
 *-
 *-    "ret" is a number of lines
 *-
 *-    ret = 1 : There is one line (x[0],y[0])-(x[1],y[0]).
 *-    ret = 2 : There are tow lines (x[0],y[0])-(x[1],y[0])
 *-              and (x[2],y[1])-(x[3],y[1]).
 *-    ret = 3 : There are three lines (x[0],y[0])-(x[1],y[0])
 *-              and (x[2],y[1])-(x[3],y[1]) and (x[4],y[2])-(x[5],y[2]).
 *-           .......
 *-
 *-    ret = 0 : This line is not on this Page.
 *-    ret = negative value : There are some ERROR.
 *-
 */
static int	StartAndLengthToPoints(start, length)
     I32s start;
     I32s length;
{
  int	startLine;
  int	needLine;
  int	line;

  if ((start + length) < topOfPage)	return 0;	/* Out of this page */
  if (endOfPage < start)		return 0;	/* Out of this page */

  if (start < topOfPage){
    startLine = (int)((start-topOfPage-1) / NUM_OF_GENE_ON_LINE - 1) ;
  }
  else{
    startLine = (int)((start-topOfPage) / NUM_OF_GENE_ON_LINE);
  }

  needLine = length / NUM_OF_GENE_ON_LINE + 2;

  if ( needLine < maxLinesByACreature ){
    /* NOP */
  }
  else if ( !drawPointsX || !drawPointsY ){
    drawPointsX = (int*)tcalloc( needLine, sizeof(int)*2, 51001);
    drawPointsY = (int*)tcalloc( needLine, sizeof(int),   51002);
    maxLinesByACreature = needLine;
  }
  else{
    drawPointsX =
      (int*)trecalloc(drawPointsX, sizeof(int)*needLine*2,
		      sizeof(int)*maxLinesByACreature*2, 51001);
    drawPointsY =
      (int*)trecalloc(drawPointsY, sizeof(int)*needLine,
		      sizeof(int)*maxLinesByACreature,   51002);
    maxLinesByACreature = needLine;
  }
  
  line=0;
  drawPointsX[line*2+0] = (start % NUM_OF_GENE_ON_LINE) + LEFT_OFFSET;
  drawPointsY[line] = LINE_TO_DRAW_Y(startLine);
  drawPointsX[line*2+1] = drawPointsX[line*2+0] + length;

  while( drawPointsX[line*2+1] > (memFormWidth - RIGHT_MARGIN) ){
    line++;
    drawPointsX[line*2+1]
      = drawPointsX[line*2-1] - (memFormWidth-RIGHT_MARGIN) + LEFT_OFFSET;
    drawPointsX[line*2-1] = memFormWidth - RIGHT_MARGIN;
    drawPointsX[line*2+0] = LEFT_OFFSET;
    drawPointsY[line] = LINE_TO_DRAW_Y(startLine + line);
  }
  line++;

  return line;
}

static I32s	WhichAddress(x, y)
     int x, y;
{
  int lines, modulo, height;
  
  height = numOfLineOnPage * (geneLineWidth + geneLineSkip);

  if ((x > (LEFT_OFFSET + SOUP_WIDTH)) || (y > (TOP_MARGIN + height))
      || (x < (LEFT_OFFSET)) ||
      (y < (TOP_MARGIN - (geneLineWidth + geneLineSkip) / 2)))
    return -1;

  lines		= (y - TOP_MARGIN + (geneLineWidth + geneLineSkip) / 2)
    / (geneLineWidth + geneLineSkip);
  modulo	= (x - LEFT_OFFSET) * ( NUM_OF_GENE_ON_LINE / SOUP_WIDTH);
  return topOfPage + lines * NUM_OF_GENE_ON_LINE + modulo;
}

static void	FormSizeDesign(soupSize)
     I32s soupSize;
{
  Arg args[2];
  int soupHeight;

  numOfLine = soupSize / NUM_OF_GENE_ON_LINE;
  if (numOfLine * NUM_OF_GENE_ON_LINE < soupSize) numOfLine += 1;

  topOfPage = 0;
  endOfPage = NUM_OF_GENE_ON_PAGE;
  XtSetArg(args[0], XtNsensitive, False);
  XtSetValues(pageUpButton, args, 1);
  if (endOfPage < soupSize){
    numOfLineOnPage = NUM_OF_LINE_ON_PAGE;
    XtSetArg(args[0], XtNsensitive, True);
    XtSetValues(pageDownButton, args, 1);
  }
  else{
    endOfPage = soupSize;
    numOfLineOnPage = numOfLine;
    XtSetArg(args[0], XtNsensitive, False);
    XtSetValues(pageDownButton, args, 1);
  }

  geneLineWidth = 11;
  do{
    geneLineWidth--;
    geneLineSkip = geneLineWidth + 2;
    soupHeight = numOfLineOnPage * (geneLineWidth + geneLineSkip);
  }while( (800 < soupHeight)&&( MOST_THIN_LINE < geneLineWidth) );

  memFormHeight = TOP_OFFSET  + soupHeight + BOTTOM_MARGIN;
  XtSetArg(args[0], XtNwidth, memFormWidth);
  XtSetArg(args[1], XtNheight, memFormHeight);
  XtSetValues(paned, args, 2);
}

static void	RedrawAllOvData()
{
  OvData_t*	p;

  ClearTableAndInfo();
  CleanInfoGeneColor();
  DrawOverviewMemFrame(ovFrame.SoupSize);

  p = UTIL_Ov_GetRoot();
  while(p){
    CreatureDraw( 'b', p->start, p->length );
    p = p->next;
  }
  return;
}

/*--------------------------------------------
 *- static functions about for SIZE DATA
 *-
 */


static int	AddSizeData(length)
     I32s length;
{
  OvSizeData_t** p = &ovSizeDataRootP;
  OvSizeData_t* prevP = (OvSizeData_t*)0;
  OvSizeData_t* moveP;
  int	retColor;

  while( ((*p) != (OvSizeData_t*)0)&&((*p)->length != length) ){
    prevP = (*p);
    p = &((*p)->next);
  }

  if ( (*p) == (OvSizeData_t*)0 ){
    /* There is not a same length data */
    (*p) = (OvSizeData_t*)tcalloc(1,sizeof(OvSizeData_t),51000);
    (*p)->next = (OvSizeData_t*)0;
    (*p)->area = (*p)->length = length;
    (*p)->pop = 1;
    (*p)->color = SelectGcIndex();
    retColor = (*p)->color;
  }
  else{
    /* There is a same length data */
    (*p)->pop +=1;
    (*p)->area += length;
    retColor = (*p)->color;
    if ( ( prevP != (OvSizeData_t*)0 )&&( prevP->pop < (*p)->pop) ){
      moveP = (*p);
      (*p) = (*p)->next;
      ReAddSizeData(moveP);
      retColor = moveP->color;
    }
  }
  return retColor;
}


static void	DelSizeData(length)
     I32s length;
{
  OvSizeData_t** p = &ovSizeDataRootP;
  OvSizeData_t* moveP;
  OvSizeData_t* delP;

  while( ( (*p) != (OvSizeData_t*)0 )&&( (*p)->length != length ) ){
    p = &((*p)->next);
  }

  if ( (*p) == (OvSizeData_t*)0 ){
    DebugPrint1( "===  Bad death data: (l=%d)  ===\n", length );
    return;
  }

  (*p)->pop -= 1;
  (*p)->area -= length;

  if ( (*p)->pop == 0 ){
    ReleaseGcIndex((*p)->color);
    delP = (*p);
    (*p) = (*p)->next;
    tfree( delP, sizeof(OvSizeData_t), 51000);
    ColorAssignCheck();
  }
  else{
    if ( ( (*p)->next != (OvSizeData_t*)0 )&&( (*p)->next->pop > (*p)->pop ) ){
      moveP = (*p);
      (*p) = (*p)->next;
      ReAddSizeData(moveP);
    }
  }
}

static void	ReAddSizeData(moveP)
     OvSizeData_t* moveP;
{
  OvSizeData_t** p = &ovSizeDataRootP;
  OvSizeData_t* tmpP;

  while( ( (*p) != (OvSizeData_t*)0 )&&((*p)->pop > moveP->pop) ){
    p = &((*p)->next);
  }

  tmpP = (*p);
  (*p) = moveP;
  moveP->next = tmpP;
  ColorAssignCheck();
}

static void	ColorAssignCheck()
{
  OvSizeData_t* p = ovSizeDataRootP;
  OvSizeData_t* newP = (OvSizeData_t*)0;
  int	posI = 0;

  while( ( p != (OvSizeData_t*)0 )&&( posI < NUM_OF_COLOR_VARIATION-1) ){
    if ( p->color == colorAssignTable[NUM_OF_COLOR_VARIATION-1] ){
      newP = p;
    }
    posI++;
    p = p->next;
  }

  while( p != (OvSizeData_t*)0 ){
    if ( p->color != colorAssignTable[NUM_OF_COLOR_VARIATION-1] ){
      ReleaseGcIndex( p->color );
      p->color = colorAssignTable[NUM_OF_COLOR_VARIATION-1];
      AllCreaturesColorChange( p->length, colorGC[p->color]);
      break;
    }
    p = p->next;
  }
  if( newP ){
    newP->color = SelectGcIndex();
    AllCreaturesColorChange( newP->length, colorGC[newP->color]);
  }

  return;
}


static void	SizeDataClearAll()
{
  OvSizeData_t* p = ovSizeDataRootP;
  OvSizeData_t* nextP;

  while( p ){
    nextP = p->next;
    tfree( p, sizeof(OvSizeData_t), 51000);
    p = nextP;
  }
  ovSizeDataRootP = (OvSizeData_t*)0;
  return;
}


static void	AllCreaturesColorChange(length, gc)
     I32s length;
     GC gc;
{
  OvData_t*	p;

  p = UTIL_Ov_GetRoot();
  while(p){
    if ( p->length == length ){
      CreatureBornDraw( gc, p->start, p->length );
    }
    p = p->next;
  }
}


static int	SelectGcIndex()
{
  int	gcIndex;

  if ( colorAssignNumber < NUM_OF_COLOR_VARIATION-1 ){
    gcIndex = colorAssignTable[colorAssignNumber];
    colorAssignNumber++;
  }
  else{
    gcIndex = colorAssignTable[NUM_OF_COLOR_VARIATION-1];
  }
  return gcIndex;
}

static int	ReleaseGcIndex(gcIndex)
     int gcIndex;
{
  if ( gcIndex != colorAssignTable[NUM_OF_COLOR_VARIATION-1] ){
    colorAssignNumber--;
    colorAssignTable[colorAssignNumber] = gcIndex;
  }
  return 0;
}
