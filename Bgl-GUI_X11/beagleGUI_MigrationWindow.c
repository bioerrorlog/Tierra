/* beagleGUI_MigrationWindow.c -- -*- C -*-
 * 
 * This work has been done at ATR HIP
 * 
 * Author          : YOSHIKAWA Tooru
 * Created On      : Tue Jun 10 10:01:17 1997
 * Last Modified By: YOSHIKAWA Tooru
 * Last Modified On: Thu Jun 17 15:31:37 1999
 * Update Count    : 237
 * Status          : 
 */

#ifndef lint
static char* sccsProgramId="@(#)beagleGUI_MigrationWindow.c	1.29	06/17/99 15:31:58";
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
#include <X11/Xaw/Label.h>
#include <X11/Xaw/AsciiText.h>
#include <time.h>

#include "beagleMessageMgr.h"
#include "beagleGUI_Global.h"
#include "beagleGUI_TopWindow.h"
#include "beagleGUI_MigrationWindow.h"
#include "beagleGUI_KeyInWindow.h"
#include "beagleUTIL_Convert.h"
#include "beagleUTIL_MigrMgr.h"

/*
 *- SPECIAL VALIABLE NAME
 *- =====================
 *- This window show only three variables for another file using the file
 *- 'beagleGUI_Global.h' and following tow. another ones  name and type a
 *- 'Widget migrationWindow'.
 *- So you can open this window from every GUI file if need it.
 */


/*
 *- HERE ARE EXTERN FUNCTION DEFINEs.
 *- ================================
 *- This window (migrationWindow) use here external function only.
 *- So this file need the file include here function to linking.
 *- I tried to collect there functions in '../Bglclnt/beagleRoutines.c'.
 *- Another GUI functions are define in the 'beagleGUI_MigrationWindow.h'.
 */

/*** NONE ***/

/*
 *- HERE ARE GLOBAL VARIABLE (EFFECTIVE IN THIS FILE ONLY) DEFINEs.
 *- ===============================================================
 *- So another files functions cannot search these.
 *- I want to avoid from linking trouble.
 */
#define MIGR_SESSION_MAX 15
#define SESSION_MAX_SIZE 12000
static Widget	migrationOnOffButton;
static Widget	migrationOnOffArrow;
static Widget	migrationInfoSession[MIGR_SESSION_MAX];
static String	migrationMessageSession[MIGR_SESSION_MAX];
static int	sessionConverter[MIGR_SESSION_MAX][2];

static I32s	migrationStats = 0;/* 0: off, 1: on */

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
static void	CreateModeMigrOnMenu	P_((Widget));
static void	CreateMigrationInfoArea	P_((Widget));
/*- Action definition.*/
static void	CloseButtonPress	P_((Widget));
static void	MigrationToggleReq	P_((Widget));
static void	MigrOnNormalButtonPress	P_((Widget));
static void	MigrOnSizeButtonPress	P_((Widget));
static void	MigrOnGeneButtonPress	P_((Widget));
static void	RefreshWindowReq	P_((Widget));
/*- This windows process routines -*/
static void	MigrationInfoWrite	P_((BglMigrationDataSt*));


/*
 *- HERE ARE THIS WINDOW's SERVICE FUNCTIONs FOR CALL FROM ANOTHER FILEs.
 *- =====================================================================
 *- I wrote prototype define in 'beagleGUI_MigrationWindow.h'.
 */
void	CreateMigrationWindow(topWidget)
     Widget topWidget;
{
  Widget	form;

  migrationWindow
    = XtVaCreatePopupShell("migrationWindow",
			   topLevelShellWidgetClass, topWidget,
			   NULL);
  form
    = XtVaCreateManagedWidget("migrationForm",
			      panedWidgetClass,	migrationWindow,
			      XtNwidth,		1070,
			      XtNheight,	350,
			      NULL);
  CreateMenuBar(form);
  CreateMigrationInfoArea(form);
}


void	UpdateMigrationInfo()
{
  BglMigrationDataSt* din; 
  /* din for what receives beagle
     dctrl after a Migration control */
  int	times;

  if ( ! XtIsRealized(migrationWindow) ) return;

  times = 2;
  while( BglMM_HowManyMigration() && times-- ){
    din = BglMM_PickMigration();
    MigrationInfoWrite(din);
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
			      smeBSBObjectClass, menu,
			      NULL);
  XtAddCallback(button,
		XtNcallback, (XtCallbackProc)CloseButtonPress,
		NULL);
}

static void	PutModeButton(w)
     Widget w;
{
  Widget menu, form, button;

  String cascMenuTrans = "#override\n\
	<BtnUp>:	allCascMenuPopdown() MenuPopdown(modePullDownMenu)\n\
	<EnterWindow>:	cascMenuEnter(modePullDownMenu)";
  String buttTrans = "#override\n\
	<EnterWindow>:	set()\n\
	<LeaveWindow>:	unset()\n\
	<BtnUp>:	notify() unset()";
  String arrowTrans ="#replace\n\
	<EnterWindow>:	positionCascPullDownMenu(modeMigrOnPullDownMenu)\
			cascMenuPopup(modeMigrOnPullDownMenu)";

  button
    = XtVaCreateManagedWidget("modeButton",
			      menuButtonWidgetClass, w,
			      XtNmenuName,	"modePullDownMenu",
			      NULL);
  menu
    = XtVaCreatePopupShell("modePullDownMenu",
			   overrideShellWidgetClass, button,
			   XtNtranslations,
			   XtParseTranslationTable(cascMenuTrans),
			   NULL);
  form
    = XtVaCreateManagedWidget("modeForm",
			      formWidgetClass, menu,
			      XtNdefaultDistance,	0,
			      NULL);
  migrationOnOffButton = button
    = XtVaCreateManagedWidget("migrationToggleButton",
			      commandWidgetClass, form,
			      XtNborderWidth,	0,
			      XtNtranslations,
			      XtParseTranslationTable(buttTrans),
			      XtNlabel,		"Creature tracking On ",
			      NULL);
  XtAddCallback(button,
		XtNcallback, (XtCallbackProc)MigrationToggleReq,
		NULL);
  migrationOnOffArrow = button
    = XtVaCreateManagedWidget("arrow",
			      commandWidgetClass, form,
			      XtNfromHoriz,	button,
			      XtNborderWidth,	0,
			      XtNtranslations,
			      XtParseTranslationTable(arrowTrans),
			      NULL);

  CreateModeMigrOnMenu(form);

  button
    = XtVaCreateManagedWidget("refreshButton",
			      commandWidgetClass,	form,
			      XtNborderWidth,	0,
			      XtNfromVert,		migrationOnOffButton,
			      XtNtranslations,
			      XtParseTranslationTable(buttTrans),
			      XtNlabel,			"Refresh this window     ",
			      NULL);
  XtAddCallback(button,
		XtNcallback, (XtCallbackProc)RefreshWindowReq,
		NULL);
}

static void	CreateModeMigrOnMenu(w)
     Widget w;
{
  Widget menu, button;
  String menuTrans ="#override\n\
	<LeaveWindow>:	unhighlight()";
  
  menu
    = XtVaCreatePopupShell("modeMigrOnPullDownMenu",
			   simpleMenuWidgetClass, w,
			   XtNtranslations,
			   XtParseTranslationTable(menuTrans),
			   NULL);
  button
    = XtVaCreateManagedWidget("modeMigrOnNormalButton",
			      smeBSBObjectClass,	menu,
			      XtNlabel,	"Normal",
			      NULL);
  XtAddCallback(button,
		XtNcallback, (XtCallbackProc)MigrOnNormalButtonPress,
		NULL);
  button
    = XtVaCreateManagedWidget("modeMigrOnSizeButton",
			      smeBSBObjectClass,	menu,
			      XtNlabel,	"Size    ",
			      NULL);
  XtAddCallback(button,
		XtNcallback, (XtCallbackProc)MigrOnSizeButtonPress,
		NULL);
  button
    = XtVaCreateManagedWidget("modeMigrOnGeneButton",
			      smeBSBObjectClass,	menu,
			      XtNlabel,	"Gene    ",
			      NULL);
  XtAddCallback(button,
		XtNcallback, (XtCallbackProc)MigrOnGeneButtonPress,
		NULL);
}

static void	CreateMigrationInfoArea(w)
     Widget w;
{
  static XawTextSelectType sarray[] =
    {XawselectLine, XawselectAll, XawselectNull};
  static XtTranslations	addActionTransTable;
  static String	addActionTrans ="#override\n\
<Btn2Down>: no-op(RingBell)";
  Widget	itemLabel;
  Widget	infoArea;
  Widget	infoForm;
  char		str[250];
  int		i;
  
  tsprintf(str, " %-20s %-28s %-8s %5s %16s   %-9s %17s %28s",
	   "Information", "Node", "Session", "Name", "RandomID", "Fecundity", "Tierra Time", "Beagle Time");

  itemLabel
    = XtVaCreateManagedWidget("itemLabel",
			      labelWidgetClass,		w,
			      XtNshowGrip,		False,
			      XtNjustify,		XtJustifyLeft,
			      XtNresize,		False,
			      XtNheight,		10,
			      XtNdefaultDistance,	0,
			      XtNhorizDistance,		0,
			      XtNvertDistance,		0,
			      XtNlabel,			str,
			      NULL);
  infoArea
    = XtVaCreateManagedWidget("migrationInfoArea",
			      viewportWidgetClass,	w,
			      XtNshowGrip,		False,
			      XtNforceBars,		TRUE,
			      XtNuseRight,		TRUE,
			      XtNallowVert,		TRUE,
			      NULL);
  infoForm
    = XtVaCreateManagedWidget("migrationInfoForm",
			      formWidgetClass,		infoArea,
			      XtNdefaultDistance,	0,
			      NULL);
  addActionTransTable = XtParseTranslationTable(addActionTrans);

  for (i = 0 ; i < MIGR_SESSION_MAX ; i++){
    tsprintf(str, "migrationInfoSession%d", i);
    migrationMessageSession[i] = XtMalloc(SESSION_MAX_SIZE);
    migrationMessageSession[i][0] = '\0';
    migrationInfoSession[i]
      = XtVaCreateManagedWidget(str,
				asciiTextWidgetClass, infoForm,
				XtNtranslations,	addActionTransTable,
				XtNeditType,		XawtextRead,
				XtNselectTypes,	sarray,
				XtNwidth,		855,
				XtNborderWidth,	1,
				XtNdisplayCaret,	False,
				XtNuseStringInPlace,	True,
				XtNstring,	migrationMessageSession[i],
				XtNlength,		0,
				XtNallowResize,	TRUE,
				XtNresize,		XawtextResizeBoth,
				XtNresizable,		TRUE,
				XtNvertDistance,	0,
				XtNhorizDistance,	0,
				i ? XtNfromVert : NULL,
				migrationInfoSession[i-1],
				NULL);
  }
}

/*
 *- Action definition.
 */
static void	CloseButtonPress(w)
     Widget w;
{
  Arg args[1];
  XtPopdown(migrationWindow);
  XtSetArg(args[0], XtNsensitive, True);
  XtSetValues(migrationOpenButton, args, 1);
}

static void	MigrationToggleReq(w)
     Widget w;
{
  if ( migrationStats == 0 ){
    BglMM_MigrationOn_need();
  }
  else{
    if ( migrationStats == 1 )
      BglMM_MigrationOff_need();
  }
}

static void	MigrOnNormalButtonPress(w)
     Widget w;
{
  UTIL_MigrMgr_PutParameter("");
  MigrationToggleReq(w);
}

static void	MigrOnSizeButtonPress(w)
     Widget w;
{
  String output[] = {
    "Enter the size of creatures that you want to track"
  };
  String input[] = {NULL};

  StrKeyIn(input, 1, output);
  if (input[0][0]){
    UTIL_MigrMgr_PutParameter(input[0]);
    MigrationToggleReq(w);
  }
}

static void	MigrOnGeneButtonPress(w)
     Widget w;
{
  String output[] = {
    "Enter the genotype of creatures that you want to track"
  };
  String input[] = {NULL};

  StrKeyIn(input, 1, output);
  if (input[0][0]){
    UTIL_MigrMgr_PutParameter(input[0]);
    MigrationToggleReq(w);
  }
}


void SetMigrationDisplayToOff()
{
  Arg args[1];

  TopDisplayMigrationStatsToOff();
  XtSetArg(args[0], XtNlabel, "Creature tracking On ");
  XtSetValues(migrationOnOffButton, args, 1);
  XtManageChild(migrationOnOffArrow);
  migrationStats = 0;
}

void SetMigrationDisplayToOn()
{
  Arg args[4];
  int i;
  
  TopDisplayMigrationStatsToOn();
  i = 0;
  XtSetArg(args[i], XtNlabel, "Creature tracking Off");	i++;
  XtSetValues(migrationOnOffButton, args, i);
  XtUnmanageChild(migrationOnOffArrow);
  migrationStats = 1;
}
  

static void	RefreshWindowReq(w)
     Widget w;
{
  Arg	args[2];
  int	i, j, k;

  j = 0;
  XtSetArg(args[j], XtNlength, 0) ;	j++;

  for (i = 0 ; i < MIGR_SESSION_MAX ; i++){
    migrationMessageSession[i][0] = '\0';
    sessionConverter[i][0]	 = 0;
    sessionConverter[i][1]	 = 0;

    k = j;
    XtSetArg(args[k], XtNstring, migrationMessageSession[i]);	k++;
    XtSetValues(migrationInfoSession[i], args, k);
  }  
}


/*
 *- This windows process routines.
 */
static void	MigrationInfoWrite(d)
     BglMigrationDataSt* d;
{
  String		tmp;
  char			msg[250], *info, *curs;
  int			i, j, k, l, lmsg;
  Arg			args[4];
  XawTextPosition	start, end;
  static Dimension	fontheight = 0, topmarg, botmarg;
  char			tierraTime[26], beagleTime[26];

  /* initialise font height */
  if (!fontheight){
    j = 0;
    XtSetArg(args[j], XtNheight, &fontheight);		j++;
    XtSetArg(args[j], XtNbottomMargin, &botmarg);	j++;
    XtSetArg(args[j], XtNtopMargin, &topmarg);		j++;
    XtGetValues(migrationInfoSession[0], args, j);
    fontheight -= (botmarg + topmarg + 1);
  }

  /* look for receiving Widget */
  for (i = 0 ; (i < MIGR_SESSION_MAX) && (sessionConverter[i][0])
       && (sessionConverter[i][0] != d->session) ; i++);

  if ((i >= MIGR_SESSION_MAX)){
    /* if didn't find then offset string data */
    tmp = migrationMessageSession[0];
    for (i = 0 ; i < MIGR_SESSION_MAX - 1 ; i++){
      XawTextGetSelectionPos(migrationInfoSession[i+1], &start,  &end);
      sessionConverter[i][0]	= sessionConverter[i+1][0];
      sessionConverter[i][1]	= sessionConverter[i+1][1];
      migrationMessageSession[i]	= migrationMessageSession[i+1];

      j = 0;
      XtSetArg(args[j], XtNstring, migrationMessageSession[i]);	j++;
      XtSetArg(args[j], XtNlength, strlen(migrationMessageSession[i]));	j++;
      XtSetArg(args[j], XtNheight, botmarg + topmarg +
	       sessionConverter[i][1] * fontheight);			j++;
      XtSetValues(migrationInfoSession[i], args, j);
      if (start - end)
	XawTextSetSelection(migrationInfoSession[i], start, end);
    }
    tmp[0] = '\0';
    sessionConverter[i][0]		= d->session;
    sessionConverter[i][1]		= 0;
    migrationMessageSession[i]	= tmp;
  }
  else if ( ! sessionConverter[i][0])
    sessionConverter[i][0] = d->session;

  /* creation of message to print */
  switch(d->information){
  case 'B': info = "Birth";		break;
  case 'D': info = "Death";		break;
  case 'R': info = "Remote Birth";	break;
  case 'M': info = "Migration";		break;
  case 'I': info = "Immigration";	break;
  case 'A': info = "Apocalypse";	break;
  case 'S': info = "Sleeping Node";	break;
  case 'n': info = "normal Termination";break;
  default:  info = "unknown event";	break;
  }
  strncpy(tierraTime, ctime((time_t*)&(d->tierraTime)), 26);
  strncpy(beagleTime, ctime((time_t*)&(d->beagleTime)), 26);
  tierraTime[24] = beagleTime[24] = '\0';
  tsprintf(msg, "%-20s %-30s %5d %9s %15d %5d       %24s     %24s",
	   info, UTIL_Convert_IpToName((struct in_addr *)(&(d->node))),
	   d->session, d->localparameter, d->ranID, d->fecundity,
	   tierraTime, beagleTime);

  /* update of screen and form */
  XawTextGetSelectionPos(migrationInfoSession[i], &start,  &end);
  
  l	= strlen(migrationMessageSession[i]);
  lmsg	= strlen(msg) + 1;
  
  if (l + lmsg >= SESSION_MAX_SIZE){
    curs = migrationMessageSession[i];
    if (*curs == '\n'){
      lmsg ++;
      sessionConverter[i][1] --;
    }
    for (k = 0 ; *curs = curs[lmsg] ; curs++);
    sessionConverter[i][1] --;
  }

  l	= strlen(migrationMessageSession[i]);
  curs	= migrationMessageSession[i] + l; 
  if (l){
    *(curs++)	= '\n';
  }
  sessionConverter[i][1] ++;
  for (k = 0 ; msg[k] != '\0' ; k++)    curs[k] = msg[k];
  if (d->information == 'D'){
    curs[k++]	= '\n';
    sessionConverter[i][1] ++;
  }
  curs[k]	= '\0';

  j = 0;
  XtSetArg(args[j], XtNstring,	migrationMessageSession[i]);		j++;
  XtSetArg(args[j], XtNlength,	strlen(migrationMessageSession[i])) ;	j++;
  XtSetArg(args[j], XtNheight,	botmarg + topmarg +
	   sessionConverter[i][1] * fontheight);			j++;
  XtSetValues(migrationInfoSession[i], args, j);
  XawTextSetSelection(migrationInfoSession[i], start, end);
}
