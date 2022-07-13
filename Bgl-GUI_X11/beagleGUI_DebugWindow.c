/* beagleGUI_DebugWindow.c -- -*- C -*-
 * 
 * This work has been done at ATR HIP
 * 
 * Author          : YOSHIKAWA Tooru
 * Created On      : Wed Aug 28 10:53:34 1996
 * Last Modified By: YOSHIKAWA Tooru
 * Last Modified On: Tue Dec 21 11:51:12 1999
 * Update Count    : 283
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
#include <X11/Xaw/Label.h>
#include <X11/Xaw/AsciiText.h>

#include "beagleMessageMgr.h"
#include "beagleGUI_Global.h"
#include "beagleGUI_InfoWindows.h"
#include "beagleGUI_DebugWindow.h"
#include "beagleGUI_OvGeneWindow.h"
#include "beagleGUI_KeyInWindow.h"


/*
 *- SPECIAL VALIABLE NAME
 *- =====================
 *- This window show only one variable for another file using the file
 *- 'beagleGUI_Global.h'. It name and type a 'Widget debugWindow'.
 *- So you can open this window from every GUI file if need it.
 */

/*
 *- HERE ARE EXTERN FUNCTION DEFINEs.
 *- ================================
 *- This window (debugWindow) use here external function only.
 *- So this file need the file include here function to linking.
 *- I tried to collect there functions in '../Bglclnt/beagleRoutines.c'.
 *- Another GUI functions are define in the 'beagleGUI_DebugWindow.h'.
 */

/*** NONE ***/

/*
 *- HERE ARE GLOBAL VARIABLE (EFFECTIVE IN THIS FILE ONLY) DEFINEs.
 *- ===============================================================
 *- So another files functions cannot search these.
 *- I want to avoid from linking trouble.
 */
static Widget	mapCodeButton;
static Widget	debugInfoArea;

static int	mapcode = 0;
static char	strsource[1000];

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
static void	PutBrkButton		P_((Widget));
static void	CreateBrkPutMenu	P_((Widget));
static void	CreateBrkPutThrdMenu	P_((Widget));
static void	PutModeButton		P_((Widget));
static void	PutDisplayButton	P_((Widget));
static void	CreateDebugInfoArea	P_((Widget));
/*- Action definition.*/
static void	CloseButtonPress	P_((Widget));
static void	GenOffButtonPress	P_((Widget));
static void	CurrOffButtonPress	P_((Widget));
static void	AbsTimeButtonPress	P_((Widget));
static void	RelTimeButtonPress	P_((Widget));
static void	ThrdSizeButtonPress	P_((Widget));
static void	ThrdSizeSendRequest	P_((I8s *));
static void	ThrdGeneButtonPress	P_((Widget));
static void	ThrdGeneSendRequest	P_((I8s *));
static void	ClearButtonPress	P_((Widget));
static void	RepeatButtonPress	P_((Widget));
static void	DebuggerModeTogglePress	P_((Widget));
static void	MapCodePress		P_((Widget));
/*- This windows process routines -*/
/* NONE */

/*
 *- HERE ARE THIS WINDOW's SERVICE FUNCTIONs FOR CALL FROM ANOTHER FILEs.
 *- =====================================================================
 *- I wrote prototype define in 'beagleGUI_DebugWindow.h'.
 */
void	CreateDebugWindow(topWidget)
     Widget topWidget;
{
  Widget	form;

  debugWindow
    = XtVaCreatePopupShell("debugWindow",
			   topLevelShellWidgetClass, topWidget,
			   NULL);
  form
    = XtVaCreateManagedWidget("debugForm",
			      panedWidgetClass,	debugWindow,
			      XtNwidth,		470,
			      XtNheight,	180,
			      NULL);
  CreateMenuBar(form);
  CreateDebugInfoArea(form);
}

void	UpdateDebugInfo()
{
  BglMCSpySt*	data;
  Arg		args[2];
  int		i, l;
  XawTextPosition	start, end;

  while( BglMM_HowManyMCSpy() ){
    data = BglMM_PickMCSpy();

    XawTextGetSelectionPos(debugInfoArea, &start,  &end);
    strsource[0] = '\0';
    
    /* Display Line 0: */
    l = strlen(strsource);
    if(data->Stack_size > 0){
      tsprintf(strsource + l,
	       "  Cell %2d:%3d %.4d%3.3s @ %7d Slice=%4d Stack [%11d]  \n",
	       (int)data->cp_q_this_a, (int)data->cp_q_this_i, data->cp_mm_s,
	       data->lbl, data->cp_mm_p, (int)data->cp_c_ib,
	       data->cp_c_c_st[0]);
      if (data->cp_c_c_sp == 0) strsource[l + 65] = '<';
    }
    else{
      tsprintf(strsource + l,
	       "  Cell %2d:%3d %.4d%3s @ %7d Slice=%4d                      \n",
	       (int)data->cp_q_this_a, (int)data->cp_q_this_i, data->cp_mm_s,
	       data->lbl, data->cp_mm_p, (int)data->cp_c_ib);
    }

    /* Display Line 1: */
    l = strlen(strsource);
    if(data->Stack_size > 1){
      tsprintf(strsource + l,
	       "    IP [%11d] (%c%-7d) = 0x%02x %9.9s   [%11d]  \n",
	       data->cp_c_c_ip, data->off_char, data->off_int,
	       (unsigned int)data->soup,
	       data->id, data->cp_c_c_st[1]);
      if (data->cp_c_c_sp == 1) strsource[l + 65] = '<';
    }
    else{
      tsprintf(strsource + l,
	       "    IP [%11d] (%c%-7d) = 0x%02x %9.9s                  \n",
	       data->cp_c_c_ip, data->off_char, data->off_int,
	       (unsigned int)data->soup,
	       data->id);
    }

    /* Display Line 2: */
    l = strlen(strsource);
    if(data->Stack_size > 2){
      if(data->Numreg > 0){
	tsprintf(strsource + l,
		 "%3s AX [%11d] %-29s [%11d]  \n",
		 data->regcol0, data->cp_c_c_re[0],
		 data->hostbuf1, data->cp_c_c_st[2]);
      }
      else{
	tsprintf(strsource + l,
		 "%3s                  %-29s [%11d]  \n",
		 data->regcol0, data->hostbuf1, data->cp_c_c_st[2]);
      }
      if (data->cp_c_c_sp == 2) strsource[l + 65] = '<';
    }
    else{
      if(data->Numreg > 0){
	tsprintf(strsource + l,
		 "%3s AX [%11d] %-29s                \n",
		 data->regcol0, data->cp_c_c_re[0], data->hostbuf1);
      }
      else{
	tsprintf(strsource + l,
		 "%3s                  %-29s                \n",
		 data->regcol0, data->hostbuf1);
      }
    }

    /* Display Line 3: */
    l = strlen(strsource);
    if(data->Stack_size > 3){
      if(data->Numreg > 1){
	tsprintf(strsource + l,
		 "%3s BX [%11d] %-29s [%11d]  \n",
		 data->regcol1, data->cp_c_c_re[1],
		 data->hostbuf2, data->cp_c_c_st[3]);
      }
      else{
	tsprintf(strsource + l,
		 "%3s                  %-29s [%11d]  \n",
		 data->regcol1, data->hostbuf2, data->cp_c_c_st[3]);
      }
      if (data->cp_c_c_sp == 3) strsource[l + 65] = '<';
    }
    else{
      if(data->Numreg > 1){
	tsprintf(strsource + l,
		 "%3s BX [%11d] %-29s                \n",
		 data->regcol1, data->cp_c_c_re[1], data->hostbuf2);
      }
      else{
	tsprintf(strsource + l,
		 "%3s                  %-29s                \n",
		 data->regcol1, data->hostbuf2);
      }
    }

    /* Display Line 4: */
    l = strlen(strsource);
    if(data->Stack_size > 4){
      if(data->Numreg > 2){
	tsprintf(strsource + l,
		 "%3s CX [%11d] %-29s [%11d]  \n",
		 data->regcol2, data->cp_c_c_re[2],
		 data->hostbuf3, data->cp_c_c_st[4]);
      }
      else{
	tsprintf(strsource + l,
		 "%3s                  %-29s [%11d]  \n",
		 data->regcol2, data->hostbuf3, data->cp_c_c_st[4]);
      }
      if (data->cp_c_c_sp == 4) strsource[l + 65] = '<';
    }
    else{
      if(data->Numreg > 2){
	tsprintf(strsource + l,
		 "%3s CX [%11d] %-29s                \n",
		 data->regcol2, data->cp_c_c_re[2], data->hostbuf3);
      }
      else{
	tsprintf(strsource + l,
		 "%3s                  %-29s                \n",
		 data->regcol2, data->hostbuf3);
      }
    }

    /* Display Line 5: */
    l = strlen(strsource);
    if(data->Stack_size > 5){
      if(data->Numreg > 3){
	tsprintf(strsource + l,
		 "%3s DX [%11d] %-29s [%11d]  \n",
		 data->regcol3, data->cp_c_c_re[3],
		 data->hostbuf4, data->cp_c_c_st[5]);
      }
      else{
	tsprintf(strsource + l,
		 "%3s                  %-29s [%11d]  \n",
		 data->regcol3, data->hostbuf4, data->cp_c_c_st[5]);
      }
      if (data->cp_c_c_sp == 5) strsource[l + 65] = '<';
    }
    else{
      if(data->Numreg > 3){
	tsprintf(strsource + l,
		 "%3s DX [%11d] %-29s                \n",
		 data->regcol3, data->cp_c_c_re[3], data->hostbuf4);
      }
      else{
	tsprintf(strsource + l,
		 "%3s                  %-29s                \n",
		 data->regcol3, data->hostbuf4);
      }
    }

    /* Display Line 6: */
    l = strlen(strsource);
    if(data->Stack_size > 6){
      if(data->Numreg > 4){
	tsprintf(strsource + l,
		 "%3s EX [%11d] %-29s [%11d]  \n",
		 data->regcol4, data->cp_c_c_re[4],
		 data->hostbuf5, data->cp_c_c_st[6]);
      }
      else{
	tsprintf(strsource + l,
		 "%3s                  %-29s [%11d]  \n",
		 data->regcol4, data->hostbuf5, data->cp_c_c_st[6]);
      }
      if (data->cp_c_c_sp == 6) strsource[l + 65] = '<';
    }
    else{
      if(data->Numreg > 4){
	tsprintf(strsource + l,
		 "%3s EX [%11d] %-29s             \n",
		 data->regcol4, data->cp_c_c_re[4], data->hostbuf5);
      }
      else{
	tsprintf(strsource + l,
		 "%3s                  %-29s                \n",
		 data->regcol4, data->hostbuf5);
      }
    }

    /* Display Line 7: */
    l = strlen(strsource);
    if(data->Stack_size > 7){
      if(data->Numreg > 5){
	tsprintf(strsource + l,
		 "%3s FX [%11d] %-29s [%11d]  \n",
		 data->regcol5, data->cp_c_c_re[5],
		 data->hostbuf6, data->cp_c_c_st[7]);
      }
      else{
	tsprintf(strsource + l,
		 "%3s                  %-29s [%11d]  \n",
		 data->regcol5, data->hostbuf6, data->cp_c_c_st[7]);
      }
      if (data->cp_c_c_sp == 7) strsource[l + 65] = '<';
    }
    else{
      if(data->Numreg > 5){
	tsprintf(strsource + l,
		 "%3s FX [%11d] %-29s                \n",
		 data->regcol5, data->cp_c_c_re[5], data->hostbuf6);
      }
      else{
	tsprintf(strsource + l,
		 "%3s                  %-29s                \n",
		 data->regcol5, data->hostbuf6);
      }
    }

    /* Display Line 8: */
    l = strlen(strsource);
    if(data->Ploidy > 1){
      if(data->Stack_size > 8){
	tsprintf(strsource + l,
		 "    Flag: %2d   Stk: %2d      Tracks: Ex So De       [%11d]  \n",
		 (int)data->cp_c_c_fl_E, data->cp_c_c_sp, data->cp_c_c_st[8]);
	if (data->cp_c_c_sp == 8) strsource[l + 65] = '<';
      }
      else{
	tsprintf(strsource + l,
		 "    Flag: %2d   Stk: %2d      Tracks: Ex So De                      \n",
		 (int)data->cp_c_c_fl_E, data->cp_c_c_sp);
      }
    }
    else{
      if(data->Stack_size > 8){
	tsprintf(strsource + l,
		 "    Flag: %2d   Stk: %2d                             [%11d]  \n",
		 (int)data->cp_c_c_fl_E, data->cp_c_c_sp, data->cp_c_c_st[8]);
	if (data->cp_c_c_sp == 8) strsource[l + 65] = '<';
      }
      else{
	tsprintf(strsource + l,
		 "    Flag: %2d   Stk: %2d                                      \n",
		 (int)data->cp_c_c_fl_E, data->cp_c_c_sp);
      }
    }

    /* Display Line 9: */
    l = strlen(strsource);
    if(data->Ploidy > 1){
      if(data->Stack_size > 9){
	tsprintf(strsource + l,
		 "    %-30.30s  %2d %2d %2d       [%11d]  \n",
		 data->d_data,
		 data->cp_c_c_ex, data->cp_c_c_so, data->cp_c_c_de,
		 data->cp_c_c_st[9]);
	if (data->cp_c_c_sp == 9) strsource[l + 65] = '<';
      }
      else{
	tsprintf(strsource + l,
		 "    %-30.30s  %2d %2d %2d                      \n",
		 data->d_data,
		 data->cp_c_c_ex, data->cp_c_c_so, data->cp_c_c_de);
      }
    }
    else{
      if(data->Stack_size > 9){
	tsprintf(strsource + l,
		 "    %-30.30s                 [%11d]  \n",
		 data->d_data, data->cp_c_c_st[9]);
	if (data->cp_c_c_sp == 9) strsource[l + 65] = '<';
      }
      else{
	tsprintf(strsource + l,
		 "    %-30.30s                                \n",
		 data->d_data);
      }
    }

    /* Display Line 10: */
    l = strlen(strsource);
    tsprintf(strsource + l,
	     "           %3d of %3d Cpus  thread [%6d]                        \n",
	     data->cp_c_ac, data->cp_c_n, data->cp_c_c_threadid);
    if (data->cp_c_c_sp == 10) strsource[l + 65] = '<';

    l = strlen(strsource);
    i = 0;
    XtSetArg(args[i], XtNstring, strsource);	i++;
    XtSetArg(args[i], XtNlength, l) ;	i++;
    XtSetValues(debugInfoArea, args, i);  
    XawTextSetSelection(debugInfoArea, start, end);

    if (mapcode)
      OvGene_ShowPoint(data->cp_c_c_ip);
  }
}

void	DebugWindowResetMapCode()
{
  Arg	args[1];
  int	i;

  if (mapcode){
    i = 0;
    XtSetArg(args[i], XtNlabel, "Code of gene ON ");	i++;
    XtSetValues(mapCodeButton, args, i);
    mapcode = 0;
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
  PutBrkButton(menuBar);
  PutModeButton(menuBar);
  PutDisplayButton(menuBar);
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


static void	PutBrkButton(w)
     Widget w;
{
  Widget openButton;
  Widget menu, form, button;

  String cascMenuTrans = "#override\n\
	<BtnUp>:	allCascMenuPopdown() MenuPopdown(brkPullDownMenu)\n\
	<EnterWindow>:	cascMenuEnter(brkPullDownMenu)";
  String buttTrans = "#override\n\
	<EnterWindow>:	set()\n\
	<LeaveWindow>:	unset()\n\
	<BtnUp>:	notify() unset()";
  String arrowTrans ="#replace\n\
	<EnterWindow>:	positionCascPullDownMenu(brkPutPullDownMenu)\
			cascMenuPopup(brkPutPullDownMenu)";

  openButton
    = XtVaCreateManagedWidget("brkButton",
			      menuButtonWidgetClass, w,
			      XtNlabel,		"Breakpoint",
			      XtNmenuName,	"brkPullDownMenu",
			      NULL);
  menu
    = XtVaCreatePopupShell("brkPullDownMenu",
			   overrideShellWidgetClass, openButton,
			   XtNtranslations,
			   XtParseTranslationTable(cascMenuTrans),
			   NULL);

  form
    = XtVaCreateManagedWidget("brkForm",
			      formWidgetClass, menu,
			      XtNdefaultDistance,	0,
			      NULL);
  button
    = XtVaCreateManagedWidget("brkPutButton",
			      commandWidgetClass, form,
			      XtNlabel,		"Put       ",
			      XtNborderWidth,	0,
			      XtNtranslations,
			      XtParseTranslationTable(buttTrans),
			      NULL);
  button
    = XtVaCreateManagedWidget("arrow",
			      commandWidgetClass, form,
			      XtNfromHoriz,	button,
			      XtNborderWidth,	0,
			      XtNtranslations,
			      XtParseTranslationTable(arrowTrans),
			      NULL);
  CreateBrkPutMenu(form);
  
  button
    = XtVaCreateManagedWidget("brkClearButton",
			      commandWidgetClass, form,
			      XtNlabel,		"Clear        ",
			      XtNfromVert,	button,
			      XtNborderWidth,	0,
			      XtNtranslations,
			      XtParseTranslationTable(buttTrans),
			      NULL);
  XtAddCallback(button,
		XtNcallback, (XtCallbackProc)ClearButtonPress,
		NULL);

  button
    = XtVaCreateManagedWidget("brkRepeatButton",
			      commandWidgetClass, form,
			      XtNlabel,		"Repeat       ",
			      XtNfromVert,	button,
			      XtNborderWidth,	0,
			      XtNtranslations,
			      XtParseTranslationTable(buttTrans),
			      NULL);
  XtAddCallback(button,
		XtNcallback, (XtCallbackProc)RepeatButtonPress,
		NULL);
}

static void	CreateBrkPutMenu(w)
     Widget w;
{
  Widget menu, form, button, button1;

  String menuTrans ="#override\n\
	<EnterWindow>:	cascMenuEnter(brkPutPullDownMenu)";
  String buttTrans = "#override\n\
	<EnterWindow>:	set()\n\
	<LeaveWindow>:	unset()\n\
	<BtnUp>:	notify() unset()";
  String arrowTrans ="#replace\n\
	<EnterWindow>:	positionCascPullDownMenu(brkPutThrdPullDownMenu)\
			cascMenuPopup(brkPutThrdPullDownMenu)";

  menu
    = XtVaCreatePopupShell("brkPutPullDownMenu",
			   overrideShellWidgetClass, w,
			   XtNtranslations,
			   XtParseTranslationTable(menuTrans),
			   NULL);
  form
    = XtVaCreateManagedWidget("brkPutForm",
			      formWidgetClass,		menu,
			      XtNdefaultDistance,	0,
			      NULL);

  button
    = XtVaCreateManagedWidget("brkPutGenOffButton",
			      commandWidgetClass,	form,
			      XtNlabel,		"Gene and offset         ",
			      XtNborderWidth,		0,
			      XtNtranslations,
			      XtParseTranslationTable(buttTrans),
			      NULL);
  XtAddCallback(button,
		XtNcallback, (XtCallbackProc)GenOffButtonPress,
		NULL);

  button
    = XtVaCreateManagedWidget("brkPutCurrOffButton",
			      commandWidgetClass,	form,
			      XtNlabel,		"Current gene and offset ",
			      XtNborderWidth,		0,
			      XtNfromVert,		button,
			      XtNtranslations,
			      XtParseTranslationTable(buttTrans),
			      NULL);
  XtAddCallback(button,
		XtNcallback, (XtCallbackProc)CurrOffButtonPress,
		NULL);

  button
    = XtVaCreateManagedWidget("brkPutAbsTimeButton",
			      commandWidgetClass,	form,
			      XtNlabel,		"Absolute time           ",
			      XtNborderWidth,		0,
			      XtNfromVert,		button,
			      XtNtranslations,
			      XtParseTranslationTable(buttTrans),
			      NULL);
  XtAddCallback(button,
		XtNcallback, (XtCallbackProc)AbsTimeButtonPress,
		NULL);

  button1
    = XtVaCreateManagedWidget("brkPutRelTimeButton",
			      commandWidgetClass,	form,
			      XtNlabel,		"Relative time           ",
			      XtNfromVert,		button,
			      XtNborderWidth,		0,
			      XtNtranslations,
			      XtParseTranslationTable(buttTrans),
			      NULL);
  XtAddCallback(button1,
		XtNcallback, (XtCallbackProc)RelTimeButtonPress,
		NULL);

  button
    = XtVaCreateManagedWidget("brkPutThrdButton",
			      commandWidgetClass,	form,
			      XtNlabel,		"Thread Analysis Ready",
			      XtNborderWidth,		0,
			      XtNfromVert,		button1,
			      XtNtranslations,
			      XtParseTranslationTable(buttTrans),
			      NULL);
  button
    = XtVaCreateManagedWidget("arrow",
			      commandWidgetClass, form,
			      XtNfromHoriz,	button,
			      XtNfromVert,	button1,
			      XtNborderWidth,	0,
			      XtNtranslations,
			      XtParseTranslationTable(arrowTrans),
			      NULL);
  CreateBrkPutThrdMenu(form);
}

static void	CreateBrkPutThrdMenu(w)
     Widget w;
{
  Widget menu, button;
  String menuTrans ="#override\n\
	<LeaveWindow>:	unhighlight()";
  
  menu
    = XtVaCreatePopupShell("brkPutThrdPullDownMenu",
			   simpleMenuWidgetClass, w,
			   XtNtranslations,
			   XtParseTranslationTable(menuTrans),
			   NULL);
  
  button
    = XtVaCreateManagedWidget("putThrdSizeButton",
			      smeBSBObjectClass,	menu,
			      XtNlabel,	"Size    ",
			      NULL);
  XtAddCallback(button,
		XtNcallback, (XtCallbackProc)ThrdSizeButtonPress,
		NULL);
  button
    = XtVaCreateManagedWidget("putThrdGeneButton",
			      smeBSBObjectClass,	menu,
			      XtNlabel,	"Gene    ",
			      NULL);
  XtAddCallback(button,
		XtNcallback, (XtCallbackProc)ThrdGeneButtonPress,
		NULL);
}

static void	PutModeButton(w)
     Widget w;
{
  Widget openButton;
  Widget menu, button;

  openButton
    = XtVaCreateManagedWidget("modeButton",
			      menuButtonWidgetClass, w,
			      XtNmenuName,	"modePullDownMenu",
			      NULL);

  menu
    = XtVaCreatePopupShell("modePullDownMenu",
			    simpleMenuWidgetClass, openButton,
			    NULL);

  button
    = XtVaCreateManagedWidget("debuggerModeToggle",
			      smeBSBObjectClass, menu,
			      XtNlabel,	"MICRO Debugger Mode Toggle",
			      NULL);
  XtAddCallback(button,
		XtNcallback, (XtCallbackProc)DebuggerModeTogglePress,
		NULL);
}

static void	PutDisplayButton(w)
     Widget w;
{
  Widget openButton, menu;

  openButton
    = XtVaCreateManagedWidget("displayButton",
			      menuButtonWidgetClass, w,
			      XtNmenuName,	"displayPullDownMenu",
			      NULL);

  menu
    = XtVaCreatePopupShell("displayPullDownMenu",
			    simpleMenuWidgetClass, openButton,
			    NULL);

  mapCodeButton
    = XtVaCreateManagedWidget("mapCodeDisplay",
			      smeBSBObjectClass, menu,
			      XtNlabel,	"Code of Gene ON ",
			      NULL);
  XtAddCallback(mapCodeButton,
		XtNcallback, (XtCallbackProc)MapCodePress,
		NULL);
}

static void	CreateDebugInfoArea(w)
     Widget w;
{
  static  XawTextSelectType sarray[] =
  {XawselectLine, XawselectAll, XawselectNull};

  strcpy(strsource, "");
  debugInfoArea
    = XtVaCreateManagedWidget("debugInfoArea",
			      asciiTextWidgetClass,	w,
			      XtNshowGrip,		False,
			      XtNeditType,		XawtextRead,
			      XtNselectTypes,		sarray,
			      XtNwidth,			500,
			      XtNdisplayCaret,		False,
			      XtNuseStringInPlace,	True,
			      XtNstring,		strsource,
			      XtNlength,		strlen(strsource),
			      NULL);

}

/*
 *- Action definition.
 */
static void	CloseButtonPress(w)
     Widget w;
{
  Arg args[1];
  XtPopdown(debugWindow);
  XtSetArg(args[0], XtNsensitive, True);
  XtSetValues(debugOpenButton, args, 1);
}

static void	DebuggerModeTogglePress(w)
     Widget w;
{
  BglMM_Debug_Toggle();
}

static void	GenOffButtonPress(w)
     Widget w;
{
  I32s	offset;
  I8s	gene[BGL_EXTR_G_LEN];
  String output[] = {
    "Enter Genome for Breakpoint",
    "Enter Offset"
  };
  String input[] = {NULL, NULL};
  
  StrKeyIn(input, 2, output);
  if (input[0][0]){
    strcpy(gene, input[0]);
    offset = atoi(input[1]);
    BglMM_Debug_Brk_Cell_off(gene, &offset);
  }
}

static void	CurrOffButtonPress(w)
     Widget w;
{
  I32s	offset;
  String output[] = {
    "Enter offset of current cell"
  };
  String input[] = {NULL};
  
  StrKeyIn(input, 1, output);
  if (input[0][0]){
    offset = atoi(input[0]);
    BglMM_Debug_Brk_CurrCell_off(&offset);
  }
}

static void	AbsTimeButtonPress(w)
     Widget w;
{
  BglEventSt	instrtime = {0, 0};
  String output[] = {
    "Enter absolute instruction time for breakpoint"
  };
  String input[] = {NULL};
  int i;
  
  StrKeyIn(input, 1, output);
  if (input[0][0]){
    for( i = 0 ; (input[0][i] >= '0') && (input[0][i] <= '9') ; i++){
      instrtime.i *= 10;
      instrtime.m *= 10;
      instrtime.i += input[0][i] - '0';
      if (instrtime.i > 999999){
	instrtime.m += instrtime.i / 1000000;
	instrtime.i %= 1000000;
      }
    }
    BglMM_Debug_Brk_AbsTime(&instrtime);
  }
}

static void	RelTimeButtonPress(w)
     Widget w;
{
  BglEventSt	instrtime = {0, 0};
  String output[] = {
    "Enter relative instruction time for breakpoint"
  };
  String input[] = {NULL};
  int i;
  
  StrKeyIn(input, 1, output);
  if (input[0][0]){
    for( i = 0 ; (input[0][i] >= '0') && (input[0][i] <= '9') ; i++){
      instrtime.i *= 10;
      instrtime.m *= 10;
      instrtime.i += input[0][i] - '0';
      if (instrtime.i > 999999){
	instrtime.m += instrtime.i / 1000000;
	instrtime.i %= 1000000;
      }
    }
    BglMM_Debug_Brk_RelTime(&instrtime);
  }
}

static void	ThrdSizeButtonPress(w)
     Widget w;
{
  char value[91];
  
  BglMM_Var_Value(value, "ThreadAnalysis", ThrdSizeSendRequest);

  if (value[0])    ThrdSizeSendRequest(value);
}

static void	ThrdSizeSendRequest(thrd)
     I8s* thrd;
{
  String output[] = {
    "Enter size (ex 0960) :"
  };
  String input[] = {NULL};
  int	i, tarpt;
  Arg	args[1];
  
  tarpt = atoi(thrd);

  if (tarpt){
    StrKeyIn(input, 1, output);
    if (input[0][0]){
      if (!mapcode){
	mapcode = 1;
	i = 0;
	XtSetArg(args[i], XtNlabel, "Code of gene OFF");	i++;
	XtSetValues(mapCodeButton, args, i);
	BglMM_OvSession_need();
      }
      BglMM_Debug_Brk_ThrdSize(input[0]);
    }
  }
  else  MessageMessage("Thread Analysis is not ON on this machine");
}

static void	ThrdGeneButtonPress(w)
     Widget w;
{
  char value[91];
  
  BglMM_Var_Value(value, "ThreadAnalysis", ThrdGeneSendRequest);

  if (value[0])    ThrdGeneSendRequest(value);
}

static void	ThrdGeneSendRequest(thrd)
     I8s* thrd;
{
  String output[] = {
    "Enter genome (ex 0960aad) :"
  };
  String input[] = {NULL};
  int	i, tarpt;
  Arg	args[1];
  
  tarpt = atoi(thrd);

  if (tarpt){
    StrKeyIn(input, 1, output);
    if (input[0][0]){
      if (!mapcode){
	mapcode = 1;
	i = 0;
	XtSetArg(args[i], XtNlabel, "Code of gene OFF");	i++;
	XtSetValues(mapCodeButton, args, i);
	BglMM_OvSession_need();
      }
      BglMM_Debug_Brk_ThrdGene(input[0]);
    }
  }
  else    MessageMessage("Thread Analysis is not ON on this machine");
}

static void	ClearButtonPress(w)
     Widget w;
{
  BglMM_Debug_BrkClear();
}

static void	RepeatButtonPress(w)
     Widget w;
{
  BglMM_Debug_BrkRepeat();
}

static void	MapCodePress(w)
     Widget w;
{
  Arg	args[1];
  int	i;

  mapcode = !mapcode;
  if (mapcode){
    i = 0;
    XtSetArg(args[i], XtNlabel, "Code of gene OFF");	i++;
    XtSetValues(mapCodeButton, args, i);
    BglMM_OvSession_need();
  }
  else{
    i = 0;
    XtSetArg(args[i], XtNlabel, "Code of gene ON ");	i++;
    XtSetValues(mapCodeButton, args, i);
    BglMM_OvSession_stop();
  }
}
/*
 *- This windows process routines 
 */
