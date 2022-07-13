/* beagleGUI_OvGeneWindow.c -- -*- C -*-
 * 
 *-  BEAGLE OVERVIEW window.
 * 
 * Author          : YOSHIKAWA Tooru
 * Created On      : Fri Aug 23 16:41:03 1996
 * Last Modified By: YOSHIKAWA Tooru
 * Last Modified On: Tue Dec 14 10:46:46 1999
 * Update Count    : 222
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
#include <X11/Xaw/AsciiText.h>

#include "beagleMessageMgr.h"
#include "beagleGUI_Global.h"
#include "beagleGUI_KeyInWindow.h"
#include "beagleGUI_OvGeneWindow.h"
#include "beagleUTIL_OVMgr.h"

/*
 *- SPECIAL VALIABLE NAME
 *- =====================
 *- This window show only one variable for another file using the file
 *- 'beagleGUI_Global.h'. It name and type a 'Widget ovGeneWindow'.
 *- So you can open this window from every GUI file if need it.
 */

/*
 *- HERE ARE EXTERN FUNCTION DEFINEs.
 *- ================================
 *- This window (ovGeneWindow) use here external function only.
 *- So this file need the file include here function fo linking.
 *- I tried to collect there functions in '../Bglclnt/beagleRoutines.c'.
 *- Another GUI functions are define in the 'beagleGUI_OvGeneWindow.h'.
 */

/*** NONE ***/

/*
 *- HERE ARE GLOBAL VARIABLE (EFFECTIVE IN THIS FILE ONLY) DEFINEs.
 *- ===============================================================
 *- So another files functions cannot search these.
 *- I want to avoid from linking trouble.
 */
static Widget	geneInfoArea;

#define	MAX_LINE	40000
static char	defaultFileName[10];
static char	strsource[MAX_LINE*80];
static BglOvGOutputSt* data;
static int	linePositions[MAX_LINE];
static I32s	lastAddress;
static I8s	lastCurorigDiff = 'c';

/*
 *- HERE ARE STATIC FUNCTIONs PROTOTYPE DEFINEs.
 *- ============================================
 *- These functions call from in this file only.
 *- So another files functions cannot search these.
 *- I want to avoid from linking trouble.
 */
static void	OvGene_ShowLastPoint	P_((void));
/*- Create windows.*/
static void	CreateMenuBar		P_((Widget));
static void	PutFileButton		P_((Widget));
static void	CreateGeneInfoArea	P_((Widget));
/*- Action definition.*/
static void	CloseButtonPress	P_((Widget));
static void	SaveButtonPress		P_((Widget));
static void	SaveSendRequest		P_((I8s*));
/*- This windows process routines -*/

/*
 *- HERE ARE THIS WINDOW's SERVICE FUNCTIONs FOR CALL FROM ANOTHER FILEs.
 *- =====================================================================
 *- I wrote prototype define in 'beagleGUI_OvGeneWindow.h'.
 */
void	CreateOvGeneWindow(topWidget)
     Widget topWidget;
{
  Widget	form;

  ovGeneWindow
    = XtVaCreatePopupShell("ovGeneWindow",
			   topLevelShellWidgetClass, topWidget,
			   NULL);
  form
    = XtVaCreateManagedWidget("geneForm",
			      panedWidgetClass,	ovGeneWindow,
			      XtNwidth,		400,
			      XtNheight,	screen->height / 2 - 30,
			      NULL);
  CreateMenuBar(form);
  CreateGeneInfoArea(form);
}

void	UpdateOvGeneData()
{
  int		i, j, k, l;
  String	str;
  static int	last_line = 0;
  int		line;
  char		list[MAX_LINE][81];
  I32s		addr;
  Arg		args[2];

  l = line = 0;
  while( BglMM_HowManyOvG() ){
    data = BglMM_PickOvG();

    tsprintf(list[line], "Genome = %4.4d%3s (%s)\n",
	     (int)data->genome_siz, data->label, data->comment);
    tsprintf(defaultFileName, "%4.4d%3s",
	     (int)data->genome_siz, data->label);
    lastCurorigDiff = data->difforig;
    
    l += strlen(list[line]);
    line++;

    tsprintf(list[line], "Start Address = %d, End Address = %d\n",	
	     data->start,  data->start + data->genome_siz - 1);
    l += strlen(list[line]);
    line++;

    tsprintf(list[line], "No. Address : Instruction\n");
    l += strlen(list[line]);
    line++;

    for (i=0; i < data->genome_siz; i++){
      if ( line >= MAX_LINE ){
	tsprintf(list[line-1],
		 "!! There is more information But Cannot write any more\n");
	break;
      }
      addr = data->start + i;
      tsprintf(list[line], "%4d %6d : 0x%02x %9s\n", 
	       (int)i, (int)addr, (int)(*((I8s*)data->genome.genome_val+i)),
	       BglMM_InstrDisassemble(*((I8s*)data->genome.genome_val+i)));
      l += strlen(list[line]);
      line++;
    }
  }

  str = strsource;
  for (i = j = k = 0 ; i < line ; i++)
    {
      linePositions[i] = k;
      for (j = 0 ; list[i][j] - '\0' ; j++)
	str[k++] = list[i][j];
    }
  str[k++] = '\0';
  linePositions[++i] = k;

  i = 0;
  XtSetArg(args[i], XtNstring, str);	i++;
  XtSetArg(args[i], XtNlength, l) ;	i++;
  XtSetValues(geneInfoArea, args, i);  
  last_line = line;
}

static void OvGene_ShowLastPoint()
{
  UpdateOvGeneData();
  OvGene_ShowPoint(lastAddress);
}

int OvGene_ShowPoint(address)
     I32s address;
{
  I32s lineAddress;
  char strAddress[11];
  Arg  args[1];
  int  i, startdisplay, line;
  OvData_t* p;

  if( (!data) ||
     (address < data->start) ||
     (address > data->start + data->genome_siz)){
    if (p = UTIL_Ov_SearchByPoint(address)){
      tsprintf(strAddress, "%d", p->start);
      BglMM_GeneAddress_Need(strAddress);
      BglMM_Gene_Wait(OvGene_ShowLastPoint);
      lastAddress = address;
      BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
"This address pointed to another creatures.\n
   Beagle sent request for new gene data.\n
   When after get new gene data, Beagle will show it.");
     return 1;
    }
    else
      BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
"This address pointed to UNKNOWN creatures area.\n
   Beagle cannot send request for UNKNOWN creatures gene data.");
      return 0;
  }

  lineAddress = address - data->start + 3;
  XawTextSetSelection(geneInfoArea, linePositions[lineAddress],
		      linePositions[lineAddress + 1]);
  i = 0;
  XtSetArg(args[i], XtNdisplayPosition, &startdisplay); i++;
  XtGetValues(geneInfoArea, args, i);

  for (line=0 ; (line<MAX_LINE)&&(linePositions[line]<startdisplay); line++);
  if ((lineAddress - line < 40) && (lineAddress - line >= 0)){
    XtPopup(ovGeneWindow, XtGrabNone);
    return 1;
  }

  line = lineAddress - 10 < 0 ? 0 : lineAddress - 5;

  i = 0;
  XtSetArg(args[i], XtNdisplayPosition, linePositions[line]); i++;
  XtSetValues(geneInfoArea, args, i);  
  XtPopup(ovGeneWindow, XtGrabNone);
  return 1;
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
    = XtVaCreateManagedWidget("fileSaveButton",
			      smeBSBObjectClass, menu,
			      XtNlabel,	"Save Cell",
			      NULL);
  XtAddCallback(button,
		XtNcallback, (XtCallbackProc)SaveButtonPress,
		NULL);
  button
    = XtVaCreateManagedWidget("fileCloseButton",
			      smeBSBObjectClass, menu,
			      NULL);
  XtAddCallback(button,
		XtNcallback, (XtCallbackProc)CloseButtonPress,
		NULL);
}

static void	CreateGeneInfoArea(w)
     Widget w;
{
  static  XawTextSelectType sarray[] =
  {XawselectLine, XawselectAll, XawselectNull};

  strcpy(strsource, "");
  geneInfoArea
    = XtVaCreateManagedWidget("geneInfoArea",
			      asciiTextWidgetClass,	w,
			      XtNshowGrip,		False,
			      XtNeditType,		XawtextRead,
			      XtNselectTypes,		sarray,
			      XtNwidth,			400,
			      XtNscrollVertical,	XawtextScrollAlways,
			      XtNdisplayCaret,		False,
			      XtNuseStringInPlace,	True,
			      XtNstring,		strsource,
			      XtNlength,		0,
			      NULL);
}

/*
 *- Action definition.
 */
static void	CloseButtonPress(w)
     Widget w;
{
  XtPopdown(ovGeneWindow);
}

static void	SaveButtonPress(w)
     Widget w;
{
  char value[91];
  
  BglMM_Var_Value(value, "ThreadAnalysis", SaveSendRequest);

  if (value[0])    SaveSendRequest(value);
}

static void	SaveSendRequest(value)
     I8s* value;
{
  String input[5];
  String output[5];
  SaveCodeSt saveCode;

  saveCode.mode		= 0;
  saveCode.curorig	= 'c';
  saveCode.tarpt	= atoi(value);
  saveCode.detailrpt	= 0; /* Default value is 0 (don't need detail) */

  switch (lastCurorigDiff){
  case 'c':
    output[0] = "Enter filename:";
    output[1] = "Thread analysis ?";
    output[2] = "Detail data ?";
    output[3] = "Most recent thread analysis data trapped";

    input[0] = defaultFileName;
    input[1] = "yes";
    input[2] = "yes";
    input[3] = "no";

    if (saveCode.tarpt){
      StrKeyIn(input, 4, output);
      saveCode.tarpt = strcmp(input[1], "no")? 1 : 0;
      saveCode.detailrpt = strcmp(input[2], "no")? 1 : 0;
      saveCode.mode = strcmp(input[3], "no")? 1 : 0;
    }
    else{
      StrKeyIn(input, 1, output);
    }

    if (input[0][0]){
      strcpy(saveCode.filename, input[0]);
      BglMM_GeneSave_Need(&saveCode);
    }
    break;

  default:
    output[0] = "Enter filename:";
    output[1] = "Current Genotype ?";
    output[2] = "Thread analysis ?";
    output[3] = "Detail data ?";
    output[4] = "Most recent thread analysis data trapped";

    input[0] = defaultFileName;
    input[1] = "yes";
    input[2] = "yes";
    input[3] = "yes";
    input[4] = "no";

    if (saveCode.tarpt){
      StrKeyIn(input, 5, output);
      saveCode.tarpt = strcmp(input[2], "no")? 1 : 0;
      saveCode.detailrpt = strcmp(input[3], "no")? 1 : 0;
      saveCode.mode = strcmp(input[4], "no")? 1 : 0;
    }
    else{
      StrKeyIn(input, 2, output);
    }

    saveCode.curorig = strcmp(input[1], "no") ? 'c' : 'o';
  
    if (input[0][0]){
      strcpy(saveCode.filename, input[0]);
      BglMM_GeneSave_Need(&saveCode);
    }
    break;
  }
}

/*
 *- This windows process routines.
 */
