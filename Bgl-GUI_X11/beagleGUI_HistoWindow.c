/* beagleGUI_HistoWindow.c -- -*- C -*-
 * 
 *-  BEAGLE HISTOGRAM window.
 * 
 * Author          : YOSHIKAWA Tooru
 * Created On      : Wed Aug 28 10:51:56 1996
 * Last Modified By: YOSHIKAWA Tooru
 * Last Modified On: Thu Jun 17 15:25:00 1999
 * Update Count    : 520
 * Status          : 
 */

#include <stdio.h>

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
#include "beagleGUI_InfoWindows.h"
#include "beagleGUI_HistoWindow.h"

#define	PIXMAP_WIDTH	1500
#define	PIXMAP_HEIGHT	1820

/*
 *- SPECIAL VALIABLE NAME
 *- =====================
 *- This window show only one variable for another file using the file
 *- 'beagleGUI_Global.h'. It name and type a 'Widget histoWindow'.
 *- So you can open this window from every GUI file if need it.
 */

/*
 *- HERE ARE EXTERN FUNCTION DEFINEs.
 *- ================================
 *- This window (histoWindow) use here external function except for message
 *- access.
 */

/*** NONE ***/

/*
 *- HERE ARE GLOBAL VARIABLE (EFFECTIVE IN THIS FILE ONLY) DEFINEs.
 *- ===============================================================
 *- So another files functions cannot search these.
 *- I want to avoid from linking trouble.
 */
static Widget	histoInfoArea;
static Pixmap	histoInfoAreaBackUpPixmap;

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
static void	PutModeButton		P_((Widget));
static void	CreateSize_FS_Menu	P_((Widget));
static void	CreateGene_FG_Menu	P_((Widget));
static void	CreateMem_FM_Menu	P_((Widget));
static void	CreateEffic_SG_Menu	P_((Widget));
static void	CreateES_12_Menu	P_((Widget));
static void	CreateEG_12_Menu	P_((Widget));
static void	CreateES1_FS_Menu	P_((Widget));
static void	CreateES2_FS_Menu	P_((Widget));
static void	CreateEG1_FS_Menu	P_((Widget));
static void	CreateEG2_FS_Menu	P_((Widget));
static void	CreateHistoInfoArea	P_((Widget));
/*- Action definition.*/
static void	CloseButtonPress	P_((Widget));
static void	HistModeSize_f_Req	P_((Widget));
static void	HistModeSize_s_Req	P_((Widget));
static void	HistModeSizeReq		P_((void));
static void	HistModeGene_f_Req	P_((Widget));
static void	HistModeGene_g_Req	P_((Widget));
static void	HistModeGeneReq		P_((void));
static void	HistModeMem_f_Req	P_((Widget));
static void	HistModeMem_m_Req	P_((Widget));
static void	HistModeMemReq		P_((void));
static void	HistModeES_1f_Req	P_((Widget));
static void	HistModeES_1s_Req	P_((Widget));
static void	HistModeES_2f_Req	P_((Widget));
static void	HistModeES_2s_Req	P_((Widget));
static void	HistModeESReq		P_((void));
static void	HistModeEG_1f_Req	P_((Widget));
static void	HistModeEG_1g_Req	P_((Widget));
static void	HistModeEG_2f_Req	P_((Widget));
static void	HistModeEG_2g_Req	P_((Widget));
static void	HistModeEGReq		P_((void));
/*- This windows process routines -*/
static void	InfoWriteStr		P_((int, char*));
static void	InfoBarDraw		P_((int, int));


/*
 *- HERE ARE THIS WINDOW's SERVICE FUNCTIONs FOR CALL FROM ANOTHER FILEs.
 *- =====================================================================
 *- I wrote prototype define in 'beagleGUI_HistoWindow.h'.
 */
void	CreateHistoWindow(topWidget)
     Widget topWidget;
{
  Widget	form;

  histoWindow
    = XtVaCreatePopupShell("histoWindow",
			   topLevelShellWidgetClass, topWidget,
			   NULL);
  form
    = XtVaCreateManagedWidget("histoForm",
			      panedWidgetClass,	histoWindow,
			      NULL);
  CreateMenuBar(form);
  CreateHistoInfoArea(form);
}

void	RedrawHistoInfoArea(w, event, parms, num_parms)
     Widget w;
     XExposeEvent* event;
     String* parms;
     Cardinal* num_parms;
{
  XCopyArea (display,
	     histoInfoAreaBackUpPixmap,
	     XtWindow(histoInfoArea),
	     text_gc,
	     event->x, event->y, event->width, event->height,
	     event->x, event->y);
}

void	HistoAreaClean()
{
  XClearWindow(display, XtWindow(histoInfoArea));
  XFillRectangle(display,
		 histoInfoAreaBackUpPixmap,
		 white_gc,
		 0, 0, PIXMAP_WIDTH, PIXMAP_HEIGHT);
}


void	UpdateSizeHistData()
{
  BglSizHistSt* data;
  while( BglMM_HowManySizeHist() ){
    data = BglMM_PickSizeHist();
    tsprintf(bglStrBuf,
	     "%4.4hu %d %d", data->size, data->num, data->count);
    InfoWriteStr(data->index, bglStrBuf);
    InfoBarDraw(data->index, data->count);
  }
}

void	UpdateGeneHistData()
{
  BglGenHistSt* data;
  while( BglMM_HowManyGeneHist() ){
    data = BglMM_PickGeneHist();
    tsprintf(bglStrBuf,
	     "%4.4hu%3.3s %d", data->size, data->lbl, data->count);
    InfoWriteStr(data->index, bglStrBuf);
    InfoBarDraw(data->index, data->count);
  }
}

void	UpdateMemHistData()
{
  BglMemHistSt* data;
  while( BglMM_HowManyMemHist() ){
    data = BglMM_PickMemHist();
    tsprintf(bglStrBuf,
	     "%4.4hu %d %d", data->size, data->num, data->count);
    InfoWriteStr(data->index, bglStrBuf);
    InfoBarDraw( data->index, data->count/100);
  }
}

void	UpdateESHistData()
{
  BglEfficSHistSt* data;
  while( BglMM_HowManyEfficSHist() ){
    data = BglMM_PickEfficSHist();
    tsprintf(bglStrBuf,
	     "%4.4hu %d %6.3f", data->size, data->num, data->dblcount);
    InfoWriteStr(data->index, bglStrBuf);
    InfoBarDraw(data->index, (I32s)(data->dblcount*10));
  }
}

void	UpdateEGHistData()
{
  BglEfficGHistSt* data;
  while( BglMM_HowManyEfficGHist() ){
    data = BglMM_PickEfficGHist();
    tsprintf(bglStrBuf,
	     "%4.4hu%3.3s %6.3f", data->size, data->lbl, data->dblcount);
    InfoWriteStr(data->index, bglStrBuf);
    InfoBarDraw(data->index, (I32s)(data->dblcount*10));
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
  PutModeButton(menuBar);
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
			      smeBSBObjectClass,	menu,
			      NULL);
  XtAddCallback(button,
		XtNcallback, (XtCallbackProc)CloseButtonPress,
		NULL);
}


static void	PutModeButton(w)
     Widget w;
{
  Widget openButton;
  Widget menu, form;
  Widget sizeButton, geneButton, memButton, efficButton;
  Widget sizeArrow,  geneArrow,  memArrow,  efficArrow;
  String cascMenuTrans = "#override\n\
<BtnUp>: allCascMenuPopdown() MenuPopdown(modePullDownMenu)\n\
<EnterWindow>: cascMenuEnter(modePullDownMenu)";
  String buttTrans = "#override\n\
<EnterWindow>: set()\n\
<LeaveWindow>: unset()";
  String fS_ArrowTrans ="#replace\n\
<EnterWindow>: positionCascPullDownMenu(size_FS_Menu) cascMenuPopup(size_FS_Menu)";
  String fG_ArrowTrans ="#replace\n\
<EnterWindow>: positionCascPullDownMenu(gene_FG_Menu) cascMenuPopup(gene_FG_Menu)";
  String fM_ArrowTrans ="#replace\n\
<EnterWindow>: positionCascPullDownMenu(mem_FM_Menu) cascMenuPopup(mem_FM_Menu)";
  String sG_ArrowTrans ="#replace\n\
<EnterWindow>: positionCascPullDownMenu(effic_SG_Menu) cascMenuPopup(effic_SG_Menu)";

  openButton
    = XtVaCreateManagedWidget("modeButton",
			      menuButtonWidgetClass, w,
			      XtNlabel,		"Mode",
			      XtNmenuName,	"modePullDownMenu",
			      NULL);
  menu
    =  XtVaCreatePopupShell("modePullDownMenu",
			    overrideShellWidgetClass, openButton,
			    XtNtranslations,
			    XtParseTranslationTable(cascMenuTrans),
			    NULL);
  form
    = XtVaCreateManagedWidget("modePullDownMenuForm",
			      formWidgetClass, menu,
			      XtNdefaultDistance,	0,
			      NULL);

  CreateSize_FS_Menu( form );
  CreateGene_FG_Menu( form );
  CreateMem_FM_Menu( form );
  CreateEffic_SG_Menu( form );

  sizeButton
    = XtVaCreateManagedWidget("histoModeSizeButton",
			      commandWidgetClass, form,
			      XtNlabel,		"Size  ",
			      XtNborderWidth,	0,
			      XtNtranslations,
			      XtParseTranslationTable(buttTrans),
			      NULL);
  sizeArrow
    = XtVaCreateManagedWidget("arrow",
			      commandWidgetClass, form,
			      XtNfromHoriz,	sizeButton,
			      XtNborderWidth,	0,
			      XtNtranslations,
			      XtParseTranslationTable(fS_ArrowTrans),
			      NULL);
  geneButton
    = XtVaCreateManagedWidget("histoModeGeneButton",
			      commandWidgetClass, form,
			      XtNlabel,		"Gene  ",
			      XtNfromVert,	sizeButton,
			      XtNborderWidth,	0,
			      XtNtranslations,
			      XtParseTranslationTable(buttTrans),
			      NULL);
  geneArrow
    = XtVaCreateManagedWidget("arrow",
			      commandWidgetClass, form,
			      XtNfromVert,	sizeArrow,
			      XtNfromHoriz,	geneButton,
			      XtNborderWidth,	0,
			      XtNtranslations,
			      XtParseTranslationTable(fG_ArrowTrans),
			      NULL);
  memButton
    = XtVaCreateManagedWidget("histoModeMemButton",
			      commandWidgetClass, form,
			      XtNlabel,		"Mem   ",
			      XtNfromVert,	geneButton,
			      XtNborderWidth,	0,
			      XtNtranslations,
			      XtParseTranslationTable(buttTrans),
			      NULL);
  memArrow
    = XtVaCreateManagedWidget("arrow",
			      commandWidgetClass, form,
			      XtNfromVert,	geneArrow,
			      XtNfromHoriz,	geneButton,
			      XtNborderWidth,	0,
			      XtNtranslations,
			      XtParseTranslationTable(fM_ArrowTrans),
			      NULL);
  efficButton
    = XtVaCreateManagedWidget("histoModeEfficButton",
			      commandWidgetClass, form,
			      XtNlabel,		"Effic ",
			      XtNfromVert,	memButton,
			      XtNborderWidth,	0,
			      XtNtranslations,
			      XtParseTranslationTable(buttTrans),
			      NULL);
  efficArrow
    = XtVaCreateManagedWidget("arrow",
			      commandWidgetClass, form,
			      XtNfromVert,	memArrow,
			      XtNfromHoriz,	memButton,
			      XtNborderWidth,	0,
			      XtNtranslations,
			      XtParseTranslationTable(sG_ArrowTrans),
			      NULL);
}


static void	CreateSize_FS_Menu(w)
     Widget w;
{
  Widget menu, button;
  String menuTrans ="#override\n\
<LeaveWindow>: unhighlight()";

  menu
    = XtVaCreatePopupShell("size_FS_Menu",
			   simpleMenuWidgetClass, w,
			   XtNtranslations,
			   XtParseTranslationTable(menuTrans),
			   NULL);
  button
    = XtVaCreateManagedWidget("size_F_Button",
			      smeBSBObjectClass,	menu,
			      XtNlabel,	"freq/size",
			      NULL);
  XtAddCallback(button,
		XtNcallback, (XtCallbackProc)HistModeSize_f_Req,
		NULL);
  button
    = XtVaCreateManagedWidget("size_S_Button",
			      smeBSBObjectClass,	menu,
			      XtNlabel,	"size/freq",
			      NULL);
  XtAddCallback(button,
		XtNcallback, (XtCallbackProc)HistModeSize_s_Req,
		NULL);
}

static void	CreateGene_FG_Menu(w)
     Widget w;
{
  Widget menu, button;
  String menuTrans ="#override\n\
<LeaveWindow>: unhighlight()";

  menu
    = XtVaCreatePopupShell("gene_FG_Menu",
			   simpleMenuWidgetClass, w,
			   XtNtranslations,
			   XtParseTranslationTable(menuTrans),
			   NULL);
  button
    = XtVaCreateManagedWidget("gene_F_Button",
			      smeBSBObjectClass,	menu,
			      XtNlabel,	"freq/gene",
			      NULL);
  XtAddCallback(button,
		XtNcallback, (XtCallbackProc)HistModeGene_f_Req,
		NULL);
  button
    = XtVaCreateManagedWidget("geen_G_Button",
			      smeBSBObjectClass,	menu,
			      XtNlabel,	"gene/freq",
			      NULL);
  XtAddCallback(button,
		XtNcallback, (XtCallbackProc)HistModeGene_g_Req,
		NULL);
}

static void	CreateMem_FM_Menu(w)
     Widget w;
{
  Widget menu, button;
  String menuTrans ="#override\n\
<LeaveWindow>: unhighlight()";

  menu
    = XtVaCreatePopupShell("mem_FM_Menu",
			   simpleMenuWidgetClass, w,
			   XtNtranslations,
			   XtParseTranslationTable(menuTrans),
			   NULL);
  button
    = XtVaCreateManagedWidget("mem_F_Button",
			      smeBSBObjectClass,	menu,
			      XtNlabel,	"freq/mem",
			      NULL);
  XtAddCallback(button,
		XtNcallback, (XtCallbackProc)HistModeMem_f_Req,
		NULL);
  button
    = XtVaCreateManagedWidget("mem_M_Button",
			      smeBSBObjectClass,	menu,
			      XtNlabel,	"mem/freq",
			      NULL);
  XtAddCallback(button,
		XtNcallback, (XtCallbackProc)HistModeMem_m_Req,
		NULL);
}

static void	CreateEffic_SG_Menu(w)
     Widget w;
{
  Widget menu, form;
  Widget eSButton, eGButton;
  Widget eSArrow,  eGArrow;
  String menuTrans ="#override\n\
<EnterWindow>: cascMenuEnter(effic_SG_Menu)";
  String buttTrans = "#override\n\
<EnterWindow>: set()\n\
<LeaveWindow>: unset()";
  String eS_ArrowTrans ="#replace\n\
<EnterWindow>: positionCascPullDownMenu(eS_12_Menu) cascMenuPopup(eS_12_Menu)";
  String eG_ArrowTrans ="#replace\n\
<EnterWindow>: positionCascPullDownMenu(eG_12_Menu) cascMenuPopup(eG_12_Menu)";

  menu
    = XtVaCreatePopupShell("effic_SG_Menu",
			   overrideShellWidgetClass, w,
			   XtNtranslations,
			   XtParseTranslationTable(menuTrans),
			   NULL);
  form
    = XtVaCreateManagedWidget("effic_SG_MenuForm",
			      formWidgetClass, menu,
			      XtNdefaultDistance,	0,
			      NULL);

  CreateES_12_Menu( form );
  CreateEG_12_Menu( form );

  eSButton
    = XtVaCreateManagedWidget("efficSizeButton",
			      commandWidgetClass, form,
			      XtNlabel,		"Size  ",
			      XtNborderWidth,	0,
			      XtNtranslations,
			      XtParseTranslationTable(buttTrans),
			      NULL);
  eSArrow
    = XtVaCreateManagedWidget("arrow",
			      commandWidgetClass, form,
			      XtNfromHoriz,	eSButton,
			      XtNborderWidth,	0,
			      XtNtranslations,
			      XtParseTranslationTable(eS_ArrowTrans),
			      NULL);
  eGButton
    = XtVaCreateManagedWidget("histoModeGeneButton",
			      commandWidgetClass, form,
			      XtNlabel,		"Gene  ",
			      XtNfromVert,	eSButton,
			      XtNborderWidth,	0,
			      XtNtranslations,
			      XtParseTranslationTable(buttTrans),
			      NULL);
  eGArrow
    = XtVaCreateManagedWidget("arrow",
			      commandWidgetClass, form,
			      XtNfromVert,	eSArrow,
			      XtNfromHoriz,	eGButton,
			      XtNborderWidth,	0,
			      XtNtranslations,
			      XtParseTranslationTable(eG_ArrowTrans),
			      NULL);
}

static void	CreateES_12_Menu(w)
     Widget w;
{
  Widget menu, form;
  Widget eS1Button, eS2Button;
  Widget eS1Arrow,  eS2Arrow;
  String menuTrans ="#override\n\
<EnterWindow>: cascMenuEnter(eS_12_Menu)";
  String buttTrans = "#override\n\
<EnterWindow>: set()\n\
<LeaveWindow>: unset()";
  String eS1_ArrowTrans ="#replace\n\
<EnterWindow>: positionCascPullDownMenu(eS1_FS_Menu) cascMenuPopup(eS1_FS_Menu)";
  String eS2_ArrowTrans ="#replace\n\
<EnterWindow>: positionCascPullDownMenu(eS2_FS_Menu) cascMenuPopup(eS2_FS_Menu)";

  menu
    = XtVaCreatePopupShell("eS_12_Menu",
			   overrideShellWidgetClass, w,
			   XtNtranslations,
			   XtParseTranslationTable(menuTrans),
			   NULL);
  form
    = XtVaCreateManagedWidget("eS_12_MenuForm",
			      formWidgetClass, menu,
			      XtNdefaultDistance,	0,
			      NULL);

  CreateES1_FS_Menu( form );
  CreateES2_FS_Menu( form );

  eS1Button
    = XtVaCreateManagedWidget("efficSize1Button",
			      commandWidgetClass, form,
			      XtNlabel,		"1st.  ",
			      XtNborderWidth,	0,
			      XtNtranslations,
			      XtParseTranslationTable(buttTrans),
			      NULL);
  eS1Arrow
    = XtVaCreateManagedWidget("arrow",
			      commandWidgetClass, form,
			      XtNfromHoriz,	eS1Button,
			      XtNborderWidth,	0,
			      XtNtranslations,
			      XtParseTranslationTable(eS1_ArrowTrans),
			      NULL);
  eS2Button
    = XtVaCreateManagedWidget("efficSize2Button",
			      commandWidgetClass, form,
			      XtNlabel,		"2nd.  ",
			      XtNfromVert,	eS1Button,
			      XtNborderWidth,	0,
			      XtNtranslations,
			      XtParseTranslationTable(buttTrans),
			      NULL);
  eS2Arrow
    = XtVaCreateManagedWidget("arrow",
			      commandWidgetClass, form,
			      XtNfromVert,	eS1Arrow,
			      XtNfromHoriz,	eS2Button,
			      XtNborderWidth,	0,
			      XtNtranslations,
			      XtParseTranslationTable(eS2_ArrowTrans),
			      NULL);
}

static void	CreateEG_12_Menu(w)
     Widget w;
{
  Widget menu, form;
  Widget eG1Button, eG2Button;
  Widget eG1Arrow,  eG2Arrow;
  String menuTrans ="#override\n\
<EnterWindow>: cascMenuEnter(eG_12_Menu)";
  String buttTrans = "#override\n\
<EnterWindow>: set()\n\
<LeaveWindow>: unset()";
  String eG1_ArrowTrans ="#replace\n\
<EnterWindow>: positionCascPullDownMenu(eG1_FS_Menu) cascMenuPopup(eG1_FS_Menu)";
  String eG2_ArrowTrans ="#replace\n\
<EnterWindow>: positionCascPullDownMenu(eG2_FS_Menu) cascMenuPopup(eG2_FS_Menu)";

  menu
    = XtVaCreatePopupShell("eG_12_Menu",
			   overrideShellWidgetClass, w,
			   XtNtranslations,
			   XtParseTranslationTable(menuTrans),
			   NULL);
  form
    = XtVaCreateManagedWidget("eG_12_MenuForm",
			      formWidgetClass, menu,
			      XtNdefaultDistance,	0,
			      NULL);

  CreateEG1_FS_Menu( form );
  CreateEG2_FS_Menu( form );

  eG1Button
    = XtVaCreateManagedWidget("efficGene1Button",
			      commandWidgetClass, form,
			      XtNlabel,		"1st.  ",
			      XtNborderWidth,	0,
			      XtNtranslations,
			      XtParseTranslationTable(buttTrans),
			      NULL);
  eG1Arrow
    = XtVaCreateManagedWidget("arrow",
			      commandWidgetClass, form,
			      XtNfromHoriz,	eG1Button,
			      XtNborderWidth,	0,
			      XtNtranslations,
			      XtParseTranslationTable(eG1_ArrowTrans),
			      NULL);
  eG2Button
    = XtVaCreateManagedWidget("efficGene2Button",
			      commandWidgetClass, form,
			      XtNlabel,		"2nd.  ",
			      XtNfromVert,	eG1Button,
			      XtNborderWidth,	0,
			      XtNtranslations,
			      XtParseTranslationTable(buttTrans),
			      NULL);
  eG2Arrow
    = XtVaCreateManagedWidget("arrow",
			      commandWidgetClass, form,
			      XtNfromVert,	eG1Arrow,
			      XtNfromHoriz,	eG2Button,
			      XtNborderWidth,	0,
			      XtNtranslations,
			      XtParseTranslationTable(eG2_ArrowTrans),
			      NULL);
}

static void	CreateES1_FS_Menu(w)
     Widget w;
{
  Widget menu, button;
  String menuTrans ="#override\n\
<LeaveWindow>: unhighlight()";

  menu
    = XtVaCreatePopupShell("eS1_FS_Menu",
			   simpleMenuWidgetClass, w,
			   XtNtranslations,
			   XtParseTranslationTable(menuTrans),
			   NULL);
  button
    = XtVaCreateManagedWidget("eS1_F_Button",
			      smeBSBObjectClass,	menu,
			      XtNlabel,			"effic/size",
			      NULL);
  XtAddCallback(button,
		XtNcallback, (XtCallbackProc)HistModeES_1f_Req,
		NULL);
  button
    = XtVaCreateManagedWidget("eS1_S_Button",
			      smeBSBObjectClass,	menu,
			      XtNlabel,			"size/effic",
			      NULL);
  XtAddCallback(button,
		XtNcallback, (XtCallbackProc)HistModeES_1s_Req,
		NULL);
}

static void	CreateES2_FS_Menu(w)
     Widget w;
{
  Widget menu, button;
  String menuTrans ="#override\n\
<LeaveWindow>: unhighlight()";

  menu
    = XtVaCreatePopupShell("eS2_FS_Menu",
			   simpleMenuWidgetClass, w,
			   XtNtranslations,
			   XtParseTranslationTable(menuTrans),
			   NULL);
  button
    = XtVaCreateManagedWidget("eS2_F_Button",
			      smeBSBObjectClass,	menu,
			      XtNlabel,			"effic/size",
			      NULL);
  XtAddCallback(button,
		XtNcallback, (XtCallbackProc)HistModeES_2f_Req,
		NULL);
  button
    = XtVaCreateManagedWidget("eS2_S_Button",
			      smeBSBObjectClass,	menu,
			      XtNlabel,			"size/effic",
			      NULL);
  XtAddCallback(button,
		XtNcallback, (XtCallbackProc)HistModeES_2s_Req,
		NULL);
}

static void	CreateEG1_FS_Menu(w)
     Widget w;
{
  Widget menu, button;
  String menuTrans ="#override\n\
<LeaveWindow>: unhighlight()";

  menu
    = XtVaCreatePopupShell("eG1_FS_Menu",
			   simpleMenuWidgetClass, w,
			   XtNtranslations,
			   XtParseTranslationTable(menuTrans),
			   NULL);
  button
    = XtVaCreateManagedWidget("eG1_F_Button",
			      smeBSBObjectClass,	menu,
			      XtNlabel,			"effic/gene",
			      NULL);
  XtAddCallback(button,
		XtNcallback, (XtCallbackProc)HistModeEG_1f_Req,
		NULL);
  button
    = XtVaCreateManagedWidget("eG1_G_Button",
			      smeBSBObjectClass,	menu,
			      XtNlabel,			"gene/effic",
			      NULL);
  XtAddCallback(button,
		XtNcallback, (XtCallbackProc)HistModeEG_1g_Req,
		NULL);
}

static void	CreateEG2_FS_Menu(w)
     Widget w;
{
  Widget menu, button;
  String menuTrans ="#override\n\
<LeaveWindow>: unhighlight()";

  menu
    = XtVaCreatePopupShell("eG2_FS_Menu",
			   simpleMenuWidgetClass, w,
			   XtNtranslations,
			   XtParseTranslationTable(menuTrans),
			   NULL);
  button
    = XtVaCreateManagedWidget("eG2_F_Button",
			      smeBSBObjectClass,	menu,
			      XtNlabel,			"effic/gene",
			      NULL);
  XtAddCallback(button,
		XtNcallback, (XtCallbackProc)HistModeEG_2f_Req,
		NULL);
  button
    = XtVaCreateManagedWidget("eG2_G_Button",
			      smeBSBObjectClass,	menu,
			      XtNlabel,			"gene/effic",
			      NULL);
  XtAddCallback(button,
		XtNcallback, (XtCallbackProc)HistModeEG_2g_Req,
		NULL);
}

static void	CreateHistoInfoArea(w)
     Widget w;
{
  Widget viewport;
  String trans =  "<Expose>:    redrawHistoInfoArea()";

  viewport
    = XtVaCreateManagedWidget("viewport",
			      viewportWidgetClass,	w,
			      XtNshowGrip,	False,
			      XtNallowHoriz,	True,
			      XtNallowVert,	True,
			      XtNwidth,		300,
			      XtNheight,	500,
			      NULL);
  histoInfoArea
    = XtVaCreateManagedWidget("histoInfo",
			      coreWidgetClass, viewport,
			      XtNtranslations,
			      XtParseTranslationTable(trans),
			      XtNwidth,		PIXMAP_WIDTH,
			      XtNheight,	PIXMAP_HEIGHT,
			      XtNbackground,
			      WhitePixelOfScreen
			      ( DefaultScreenOfDisplay( display ) ),
			      NULL);
  histoInfoAreaBackUpPixmap
    = XCreatePixmap(display, DefaultRootWindow(display),
		    PIXMAP_WIDTH, PIXMAP_HEIGHT,
		    DefaultDepth(display,DefaultScreen(display)));

  XFillRectangle(display, histoInfoAreaBackUpPixmap, white_gc,
		 0, 0, PIXMAP_WIDTH, PIXMAP_HEIGHT );
}


/*
 *- Action definition.
 */
static void	CloseButtonPress(w)
     Widget w;
{
  Arg args[1];
  BglMM_Info_stop();
  XtPopdown(histoWindow);
  XtSetArg(args[0], XtNsensitive, True);
  XtSetValues(histoOpenButton, args, 1);
}

static void	HistModeSize_f_Req(w)
     Widget w;
{
  (void)BglMM_Info_SortByFreq();
  HistModeSizeReq();
  return;
}

static void	HistModeSize_s_Req(w)
     Widget w;
{
  (void)BglMM_Info_SortByNotFreq();
  HistModeSizeReq();
  return;
}

static void	HistModeSizeReq()
{
  if ( BglMM_Info_s_need() ){
    HistoAreaClean();
  }
  else{  MessageMessage("Request Info-Size histgram was refused.");  }
  return;
}

static void	HistModeGene_f_Req(w)
     Widget w;
{
  (void)BglMM_Info_SortByFreq();
  HistModeGeneReq();
  return;
}

static void	HistModeGene_g_Req(w)
     Widget w;
{
  (void)BglMM_Info_SortByNotFreq();
  HistModeGeneReq();
  return;
}

static void	HistModeGeneReq()
{
  if (BglMM_Info_g_need()){
    HistoAreaClean();
  }
  else{ MessageMessage("Request Info-Gene histgram was refused."); }
  return;
}

static void	HistModeMem_f_Req(w)
     Widget w;
{
  (void)BglMM_Info_SortByFreq();
  HistModeMemReq();
  return;
}

static void	HistModeMem_m_Req(w)
     Widget w;
{
  (void)BglMM_Info_SortByNotFreq();
  HistModeMemReq();
  return;
}

static void	HistModeMemReq()
{
  if (BglMM_Info_m_need()){
    HistoAreaClean();
  }
  else{ MessageMessage("Request Info-Mem histgram was refused.");  }
  return;
}

static void	HistModeES_1f_Req(w)
     Widget w;
{
  (void)BglMM_Info_Daught1();
  (void)BglMM_Info_SortByFreq();
  HistModeESReq();
  return;
}

static void	HistModeES_1s_Req(w)
     Widget w;
{
  (void)BglMM_Info_Daught1();
  (void)BglMM_Info_SortByNotFreq();
  HistModeESReq();
  return;
}

static void	HistModeES_2f_Req(w)
     Widget w;
{
  (void)BglMM_Info_Daught2();
  (void)BglMM_Info_SortByFreq();
  HistModeESReq();
  return;
}

static void	HistModeES_2s_Req(w)
     Widget w;
{
  (void)BglMM_Info_Daught2();
  (void)BglMM_Info_SortByNotFreq();
  HistModeESReq();
  return;
}

static void	HistModeESReq()
{
  if (BglMM_Info_es_need()){
    HistoAreaClean();
  }
  else{ MessageMessage("Request Reprod Effic histgram was refused.");  }
  return;
}

static void	HistModeEG_1f_Req(w)
     Widget w;
{
  (void)BglMM_Info_Daught1();
  (void)BglMM_Info_SortByFreq();
  HistModeEGReq();
  return;
}

static void	HistModeEG_1g_Req(w)
     Widget w;
{
  (void)BglMM_Info_Daught1();
  (void)BglMM_Info_SortByNotFreq();
  HistModeEGReq();
  return;
}

static void	HistModeEG_2f_Req(w)
     Widget w;
{
  (void)BglMM_Info_Daught2();
  (void)BglMM_Info_SortByFreq();
  HistModeEGReq();
  return;
}

static void	HistModeEG_2g_Req(w)
     Widget w;
{
  (void)BglMM_Info_Daught2();
  (void)BglMM_Info_SortByNotFreq();
  HistModeEGReq();
  return;
}

static void	HistModeEGReq()
{
  if (BglMM_Info_eg_need()){
    HistoAreaClean();
  }
  else{ MessageMessage("Request Reprod Effic histgram was refused.");  }
  return;
}

/*
 *- This windows process routines.
 */
static void	InfoWriteStr(l, msg)
     int l;
     char* msg;
{
  XClearArea(display,
	     XtWindow(histoInfoArea),
	     20, l*20+25, 100, 20, False);
  XDrawString(display,
	      XtWindow(histoInfoArea),
	      text_gc, 20, l*20+40,
	      msg, strlen(msg) );
  XFillRectangle(display,
		 histoInfoAreaBackUpPixmap,
		 white_gc,
		 20, l*20+25, 100, 20);
  XDrawString(display,
	      histoInfoAreaBackUpPixmap,
	      text_gc, 20, l*20+40,
	      msg, strlen(msg) );
}

static void	InfoBarDraw(l, n)
     int l, n;
{
  XClearArea(display,
	     XtWindow(histoInfoArea),
	     120, l*20+25, 5000, 19, False);
  XFillRectangle(display,
		 XtWindow(histoInfoArea),
		 text_gc, 120, l*20+25, n , 19);
  XFillRectangle(display,
		 histoInfoAreaBackUpPixmap,
		 white_gc,
		 120, l*20+25, 5000, 19);
  XFillRectangle(display,
		 histoInfoAreaBackUpPixmap,
		 text_gc, 120, l*20+25, n , 19);
}
