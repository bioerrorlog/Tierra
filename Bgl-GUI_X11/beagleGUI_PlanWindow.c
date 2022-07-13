/* beagleGUI_PlanWindow.c -- -*- C -*-
 * 
 *-  BEAGLE PLAN AREA window.
 * 
 * Author          : YOSHIKAWA Tooru
 * Created On      : Mon Aug 26 17:52:58 1996
 * Last Modified By: YOSHIKAWA Tooru
 * Last Modified On: Thu Jun 17 15:38:58 1999
 * Update Count    : 296
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

#include "beagleMessageMgr.h"
#include "beagleGUI_Global.h"
#include "beagleGUI_PlanWindow.h"


/*
 *- SPECIAL VALIABLE NAME
 *- =====================
 *- This window show only one variable for another file using the file
 *- 'beagleGUI_Global.h'. It name and type a 'Widget planWindow'.
 *- So you can open this window from every GUI file if need it.
 */

/*
 *- HERE ARE EXTERN FUNCTION DEFINEs.
 *- ================================
 *- This window (planWindow) use here external function only.
 *- So this file need the file include here function fo linking.
 *- I tried to collect there functions in '../Bglclnt/beagleRoutines.c'.
 *- Another GUI functions are define in the 'beagleGUI_PlanWindow.h'.
 */

/*** NONE ***/

/*
 *- HERE ARE GLOBAL VARIABLE (EFFECTIVE IN THIS FILE ONLY) DEFINEs.
 *- ===============================================================
 *- So another files functions cannot search these.
 *- I want to avoid from linking trouble.
 */
static Widget form01Line;
static Widget	planInstExe_mDataLabel;
static Widget	planGenerationsDataLabel;

static Widget form02Line;
static Widget	planNumCellsDataLabel;
static Widget	planNumGenotypesDataLabel;
static Widget	planNumSizesDataLabel;
static Widget	planSpeedDataLabel;

static Widget form03Line;
static Widget	planRateMutDataLabel;
static Widget	planRateMovMutDataLabel;
static Widget	planRateFlawDataLabel;

static Widget form04Line;
static Widget	planAverageSizeDataLabel;
static Widget	planAvgPopDataLabel;
static Widget	planFecundityAvgDataLabel;
static Widget	planRepInstEffDataLabel;

static Widget form05Line;
static Widget	planFEBirthNumDataLabel;
static Widget	planFEBirthLocalDataLabel;
static Widget	planFEBirthInjectDataLabel;
static Widget	planFEBirthEjectDataLabel;

static Widget form06Line;
static Widget	planFEDeathNumDataLabel;
static Widget	planFEFecundYesSumDataLabel;
static Widget	planFEFecunZeroSumDataLabel;
static Widget	planFEFecunEjecSumDataLabel;

static Widget form07Line;
static Widget	planMaxPopDataLabel;
static Widget	planMaxMemryDataLabel;
static Widget	planNumGenDGDataLabel;

static Widget form08Line;
static Widget	planNumNodesDataLabel;
static Widget	planFESImmigraNumDataLabel;
static Widget	planFERImmigraNumDataLabel;
static Widget	planFEEjectToSelfDataLabel;

static Widget form09Line;
static Widget	planFESPingReqNumDataLabel;
static Widget	planFESPingRepNumDataLabel;
static Widget	planFESSendAddNumDataLabel;
static Widget	planFESByeNumDataLabel;

static Widget form10Line;
static Widget	planFERPingReqNumDataLabel;
static Widget	planFERPingRepNumDataLabel;
static Widget	planFERSendAddNumDataLabel;
static Widget	planFERByeNumDataLabel;

static Widget form11Line;
static Widget	planDynMemUseDataLabel;
static Widget	planTotNumAlocDataLabel;

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
static void	CreatePlanInfoArea	P_((Widget));
/*- Action definition.*/
static void	CloseButtonPress	P_((Widget));
/*- This windows process routines -*/

/*
 *- HERE ARE THIS WINDOW's SERVICE FUNCTIONs FOR CALL FROM ANOTHER FILEs.
 *- =====================================================================
 *- I wrote prototype define in 'beagleGUI_PlanWindow.h'.
 */
void	CreatePlanWindow(topWidget)
     Widget topWidget;
{
  Widget	form;

  planWindow
    = XtVaCreatePopupShell("planWindow",
			   topLevelShellWidgetClass, topWidget,
			   NULL);
  form
    = XtVaCreateManagedWidget("planForm",
			      panedWidgetClass,	planWindow,
			      NULL);
  CreateMenuBar(form);
  CreatePlanInfoArea(form);
}

void	UpdatePlanData()
{
  BglPlanSt* data;
  static  BglPlanSt lastData;
  Arg args[1];

  while( BglMM_HowManyPlan() ){
    data = BglMM_PickPlan();

    if ( lastData.InstExe_m != data->InstExe_m ){
      lastData.InstExe_m = data->InstExe_m;
      tsprintf(bglStrBuf, "InsExeC     %8d", data->InstExe_m );
      XtSetArg(args[0], XtNlabel, bglStrBuf);
      XtSetValues(planInstExe_mDataLabel, args, 1);
    }

    tsprintf(bglStrBuf, "Generation %8.0f   %s",
	     data->Generations,  ctime((time_t*)&(data->tp)));
    XtSetArg(args[0], XtNlabel, bglStrBuf);
    XtSetValues(planGenerationsDataLabel, args, 1);

    if(data->GeneBnker){
      if ( lastData.NumCells != data->NumCells ){
	lastData.NumCells = data->NumCells;
	tsprintf(bglStrBuf, "NumCells    %8d", data->NumCells );
	XtSetArg(args[0], XtNlabel, bglStrBuf);
	XtSetValues(planNumCellsDataLabel, args, 1);
      }

      XtManageChild(planNumGenotypesDataLabel);
      if ( lastData.NumGenotypes != data->NumGenotypes ){
	lastData.NumGenotypes = data->NumGenotypes;
	tsprintf(bglStrBuf, "NumGen      %8d", data->NumGenotypes );
	XtSetArg(args[0], XtNlabel, bglStrBuf);
	XtSetValues(planNumGenotypesDataLabel, args, 1);
      }

      XtManageChild(planNumSizesDataLabel);
      if ( lastData.NumSizes != data->NumSizes ){
	lastData.NumSizes = data->NumSizes;
	tsprintf(bglStrBuf, "NumSizes    %8d", data->NumSizes);
	XtSetArg(args[0], XtNlabel, bglStrBuf);
	XtSetValues(planNumSizesDataLabel, args, 1);
      }

      if ( lastData.Speed != data->Speed ){
	lastData.Speed = data->Speed;
	tsprintf(bglStrBuf, "Speed       %8d\n", data->Speed);
	XtSetArg(args[0], XtNlabel, bglStrBuf);
	XtSetValues(planSpeedDataLabel, args, 1);
      }
    }
    else{ /* (GeneBnker) */
      if ( lastData.NumCells != data->NumCells ){
	lastData.NumCells = data->NumCells;
	tsprintf(bglStrBuf, "NumCells    %8d", data->NumCells );
	XtSetArg(args[0], XtNlabel, bglStrBuf);
	XtSetValues(planNumCellsDataLabel, args, 1);
      }

      XtSetArg(args[0], XtNlabel, "NumGen      ********");
      XtSetValues(planNumGenotypesDataLabel, args, 1);

      XtSetArg(args[0], XtNlabel, "NumSizes    ********");
      XtSetValues(planNumSizesDataLabel, args, 1);

      if ( lastData.Speed != data->Speed ){
	lastData.Speed = data->Speed;
	tsprintf(bglStrBuf, "Speed       %8d\n", data->Speed);
	XtSetArg(args[0], XtNlabel, bglStrBuf);
	XtSetValues(planSpeedDataLabel, args, 1);
      }
    }

    if ( lastData.RateMut != data->RateMut ){
      lastData.RateMut = data->RateMut;
      tsprintf(bglStrBuf, "RateMut     %8d", data->RateMut);
      XtSetArg(args[0], XtNlabel, bglStrBuf);
      XtSetValues(planRateMutDataLabel, args, 1);
    }

    if ( lastData.RateMovMut != data->RateMovMut ){
      lastData.RateMovMut = data->RateMovMut;
      tsprintf(bglStrBuf, "RatMovMut   %8d", data->RateMovMut);
      XtSetArg(args[0], XtNlabel, bglStrBuf);
      XtSetValues(planRateMovMutDataLabel, args, 1);
    }

    if ( lastData.RateFlaw != data->RateFlaw ){
      lastData.RateFlaw = data->RateFlaw;
      tsprintf(bglStrBuf, "RateFlaw    %8d", data->RateFlaw);	
      XtSetArg(args[0], XtNlabel, bglStrBuf);
      XtSetValues(planRateFlawDataLabel, args, 1);
    }

    if ( lastData.AverageSize != data->AverageSize ){
      lastData.AverageSize = data->AverageSize;
      tsprintf(bglStrBuf, "AvgSize     %8d", data->AverageSize);
      XtSetArg(args[0], XtNlabel, bglStrBuf);
      XtSetValues(planAverageSizeDataLabel, args, 1);
    }

    if ( lastData.AvgPop != data->AvgPop ){
      lastData.AvgPop = data->AvgPop;
      tsprintf(bglStrBuf, "AvgPop      %8.0f", data->AvgPop);
      XtSetArg(args[0], XtNlabel, bglStrBuf);
      XtSetValues(planAvgPopDataLabel, args, 1);
    }

    if ( lastData.FecundityAvg != data->FecundityAvg ){
      lastData.FecundityAvg = data->FecundityAvg;
      tsprintf(bglStrBuf, "FecundAvg   %8.1f", data->FecundityAvg);
      XtSetArg(args[0], XtNlabel, bglStrBuf);
      XtSetValues(planFecundityAvgDataLabel, args, 1);
    }

    if ( lastData.RepInstEff != data->RepInstEff ){
      lastData.RepInstEff = data->RepInstEff;
      tsprintf(bglStrBuf, "RepInstEf   %8.1f", data->RepInstEff);
      XtSetArg(args[0], XtNlabel, bglStrBuf);
      XtSetValues(planRepInstEffDataLabel, args, 1);
    }

    if ( lastData.FEBirthNum != data->FEBirthNum ){
      lastData.FEBirthNum = data->FEBirthNum;
      tsprintf(bglStrBuf, "Births      %8d", data->FEBirthNum);
      XtSetArg(args[0], XtNlabel, bglStrBuf);
      XtSetValues(planFEBirthNumDataLabel, args, 1);
    }

    if ( lastData.FEBirthLocal != data->FEBirthLocal ){
      lastData.FEBirthLocal = data->FEBirthLocal;
      tsprintf(bglStrBuf, "BirthLoca   %8d", data->FEBirthLocal);
      XtSetArg(args[0], XtNlabel, bglStrBuf);
      XtSetValues(planFEBirthLocalDataLabel, args, 1);
    }

    if ( lastData.FEBirthInject != data->FEBirthInject ){
      lastData.FEBirthInject = data->FEBirthInject;
      tsprintf(bglStrBuf, "BirthInje   %8d", data->FEBirthInject);
      XtSetArg(args[0], XtNlabel, bglStrBuf);
      XtSetValues(planFEBirthInjectDataLabel, args, 1);
    }

    if ( lastData.FEBirthEject != data->FEBirthEject ){
      lastData.FEBirthEject = data->FEBirthEject;
      tsprintf(bglStrBuf, "BirthEjec   %8d", data->FEBirthEject);
      XtSetArg(args[0], XtNlabel, bglStrBuf);
      XtSetValues(planFEBirthEjectDataLabel, args, 1);
    }

    if ( lastData.FEDeathNum != data->FEDeathNum ){
      lastData.FEDeathNum = data->FEDeathNum;
      tsprintf(bglStrBuf, "Deaths      %8d", data->FEDeathNum);
      XtSetArg(args[0], XtNlabel, bglStrBuf);
      XtSetValues(planFEDeathNumDataLabel, args, 1);
    }

    if ( lastData.FEFecundYesSum != data->FEFecundYesSum ){
      lastData.FEFecundYesSum = data->FEFecundYesSum;
      tsprintf(bglStrBuf, "DeathFecY   %8d", data->FEFecundYesSum);
      XtSetArg(args[0], XtNlabel, bglStrBuf);
      XtSetValues(planFEFecundYesSumDataLabel, args, 1);
    }

    if ( lastData.FEFecunZeroSum != data->FEFecunZeroSum ){
      lastData.FEFecunZeroSum = data->FEFecunZeroSum;
      tsprintf(bglStrBuf, "DeathFecZ   %8d", data->FEFecunZeroSum);
      XtSetArg(args[0], XtNlabel, bglStrBuf);
      XtSetValues(planFEFecunZeroSumDataLabel, args, 1);
    }

    if ( lastData.FEFecunEjecSum != data->FEFecunEjecSum ){
      lastData.FEFecunEjecSum = data->FEFecunEjecSum;
      tsprintf(bglStrBuf, "DeathEjec   %8d", data->FEFecunEjecSum);	
      XtSetArg(args[0], XtNlabel, bglStrBuf);
      XtSetValues(planFEFecunEjecSumDataLabel, args, 1);
    }

    if(data->GeneBnker){

      XtManageChild(form07Line);

      tsprintf(bglStrBuf, "MaxGenPop    %8d (%.4d%3.3s)",
	       data->MaxPop, data->MaxGenPop_size, data->MaxGenPop_label); 
      XtSetArg(args[0], XtNlabel, bglStrBuf);
      XtSetValues(planMaxPopDataLabel, args, 1);

      tsprintf(bglStrBuf, "MaxGenMem    %8d (%.4d%3.3s)",
	       data->MaxMemry, data->MaxGenMem_size, data->MaxGenMem_label);
      XtSetArg(args[0], XtNlabel, bglStrBuf);
      XtSetValues(planMaxMemryDataLabel, args, 1);

      if ( lastData.NumGenDG != data->NumGenDG ){
	lastData.NumGenDG = data->NumGenDG;
	tsprintf(bglStrBuf, "NumGenDG    %8d", data->NumGenDG); 
	XtSetArg(args[0], XtNlabel, bglStrBuf);
	XtSetValues(planNumGenDGDataLabel, args, 1);
      }
    }
    else{
      XtUnmanageChild(form07Line);
    }

    if(data->NetFlg == BGL_FLG_TRUE){

      XtManageChild(form08Line);
      XtManageChild(form09Line);
      XtManageChild(form10Line);

      if ( lastData.NumNodes != data->NumNodes ){
	lastData.NumNodes = data->NumNodes;
	tsprintf(bglStrBuf, "NumNodes    %8hu", data->NumNodes);
	XtSetArg(args[0], XtNlabel, bglStrBuf);
	XtSetValues(planNumNodesDataLabel, args, 1);
      }

      if ( lastData.FESImmigraNum != data->FESImmigraNum ){
	lastData.FESImmigraNum = data->FESImmigraNum;
	tsprintf(bglStrBuf, "SendImmig   %8d", data->FESImmigraNum);
	XtSetArg(args[0], XtNlabel, bglStrBuf);
	XtSetValues(planFESImmigraNumDataLabel, args, 1);
      }

      if ( lastData.FERImmigraNum != data->FERImmigraNum ){
	lastData.FERImmigraNum = data->FERImmigraNum;
	tsprintf(bglStrBuf, "RecvImmig   %8d", data->FERImmigraNum);
	XtSetArg(args[0], XtNlabel, bglStrBuf);
	XtSetValues(planFERImmigraNumDataLabel, args, 1);
      }

      if ( lastData.FEEjectToSelf != data->FEEjectToSelf ){
	lastData.FEEjectToSelf = data->FEEjectToSelf;
	tsprintf(bglStrBuf, "EjeToSelf   %8d", data->FEEjectToSelf);
	XtSetArg(args[0], XtNlabel, bglStrBuf);
	XtSetValues(planFEEjectToSelfDataLabel, args, 1);
      }

      if ( lastData.FESPingReqNum != data->FESPingReqNum ){
	lastData.FESPingReqNum = data->FESPingReqNum;
	tsprintf(bglStrBuf, "SePingReq   %8d", data->FESPingReqNum);
	XtSetArg(args[0], XtNlabel, bglStrBuf);
	XtSetValues(planFESPingReqNumDataLabel, args, 1);
      }

      if ( lastData.FESPingRepNum != data->FESPingRepNum ){
	lastData.FESPingRepNum = data->FESPingRepNum;
	tsprintf(bglStrBuf, "SePingRep   %8d", data->FESPingRepNum);
	XtSetArg(args[0], XtNlabel, bglStrBuf);
	XtSetValues(planFESPingRepNumDataLabel, args, 1);
      }

      if ( lastData.FESSendAddNum != data->FESSendAddNum ){
	lastData.FESSendAddNum = data->FESSendAddNum;
	tsprintf(bglStrBuf, "SeSendAdd   %8d", data->FESSendAddNum);
	XtSetArg(args[0], XtNlabel, bglStrBuf);
	XtSetValues(planFESSendAddNumDataLabel, args, 1);
      }

      if ( lastData.FESByeNum != data->FESByeNum ){
	lastData.FESByeNum = data->FESByeNum;
	tsprintf(bglStrBuf, "SendBye     %8d", data->FESByeNum);
	XtSetArg(args[0], XtNlabel, bglStrBuf);
	XtSetValues(planFESByeNumDataLabel, args, 1);
      }

      if ( lastData.FERPingReqNum != data->FERPingReqNum ){
	lastData.FERPingReqNum = data->FERPingReqNum;
	tsprintf(bglStrBuf, "RePingReq   %8d", data->FERPingReqNum);
	XtSetArg(args[0], XtNlabel, bglStrBuf);
	XtSetValues(planFERPingReqNumDataLabel, args, 1);
      }

      if ( lastData.FERPingRepNum != data->FERPingRepNum ){
	lastData.FERPingRepNum = data->FERPingRepNum;
	tsprintf(bglStrBuf, "RePingRep   %8d", data->FERPingRepNum);
	XtSetArg(args[0], XtNlabel, bglStrBuf);
	XtSetValues(planFERPingRepNumDataLabel, args, 1);
      }

      if ( lastData.FERSendAddNum != data->FERSendAddNum ){
	lastData.FERSendAddNum = data->FERSendAddNum;
	tsprintf(bglStrBuf, "ReSendAdd   %8d", data->FERSendAddNum);
	XtSetArg(args[0], XtNlabel, bglStrBuf);
	XtSetValues(planFERSendAddNumDataLabel, args, 1);
      }

      if ( lastData.FERByeNum != data->FERByeNum ){
	lastData.FERByeNum = data->FERByeNum;
	tsprintf(bglStrBuf, "RecvBye     %8d", data->FERByeNum);
	XtSetArg(args[0], XtNlabel, bglStrBuf);
	XtSetValues(planFERByeNumDataLabel, args, 1);
      }
    }
    else{

      XtUnmanageChild(form08Line);
      XtUnmanageChild(form09Line);
      XtUnmanageChild(form10Line);

    }

    if(data->MemProFlg == BGL_FLG_TRUE){

      XtManageChild(form11Line);

      if ( lastData.DynMemUse != data->DynMemUse ){
	lastData.DynMemUse = data->DynMemUse;
	tsprintf(bglStrBuf, "DynMemUse   %8d", data->DynMemUse);
	XtSetArg(args[0], XtNlabel, bglStrBuf);
	XtSetValues(planDynMemUseDataLabel, args, 1);
      }

      if ( lastData.TotNumAloc != data->TotNumAloc ){
	lastData.TotNumAloc = data->TotNumAloc;
	tsprintf(bglStrBuf, "TotNumAloc  %8d", data->TotNumAloc);	
	XtSetArg(args[0], XtNlabel, bglStrBuf);
	XtSetValues(planTotNumAlocDataLabel, args, 1);
      }
    }
    else{

      XtUnmanageChild(form11Line);

    }
  }  /*  END of While Loop  */
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
			      smeBSBObjectClass, menu,
			      XtNwidth,	20,
			      NULL);
  XtAddCallback(button,
		XtNcallback, (XtCallbackProc)CloseButtonPress,
		NULL);
}

static void	CreatePlanInfoArea(w)
     Widget w;
{
  Widget viewport, infoArea;

  viewport
    = XtVaCreateManagedWidget("viewport",
			      viewportWidgetClass,	w,
			      XtNshowGrip,	False,
			      XtNallowHoriz,	True,
			      XtNallowVert,	True,
			      NULL);

  infoArea
    = XtVaCreateManagedWidget("planInfoArea",
			      formWidgetClass,	viewport,
			      NULL);

  form01Line
    = XtVaCreateManagedWidget("form01LineBox", boxWidgetClass,	infoArea,
			      XtNorientation,	"horizontal",
			      NULL);
  {
      
    planInstExe_mDataLabel
      = XtVaCreateManagedWidget("planInstExe_mDataLabel",
				labelWidgetClass, form01Line,
				XtNlabel,	"InsExeC     --------",
				NULL);
    planGenerationsDataLabel
      = XtVaCreateManagedWidget("planGenerationsDataLabel",
				labelWidgetClass, form01Line,
				XtNlabel,
				"Generation --------   --- --- -- --:--:-- ----",
				NULL);
  }
  

  form02Line
    = XtVaCreateManagedWidget("form02LineBox", boxWidgetClass,  infoArea,
			      XtNfromVert,	form01Line,
			      XtNorientation,	"horizontal",
			      NULL);
  {
    planNumCellsDataLabel
      = XtVaCreateManagedWidget("planNumCellsDataLabel",
				labelWidgetClass, form02Line,
				XtNlabel,	"NumCells    --------",
				NULL);
    planNumGenotypesDataLabel
      = XtVaCreateManagedWidget("planNumGenotypesDataLabel",
				labelWidgetClass, form02Line,
				XtNlabel,	"NumGen      --------",
				NULL);
    planNumSizesDataLabel
      = XtVaCreateManagedWidget("planNumSizesDataLabel",
				labelWidgetClass, form02Line,
				XtNlabel,	"NumSizes    --------",
				NULL);
    planSpeedDataLabel
      = XtVaCreateManagedWidget("planSpeedDataLabel",
				labelWidgetClass, form02Line,
				XtNlabel,	"Speed       --------",
				NULL);
  }

  form03Line
    = XtVaCreateManagedWidget("form03LineBox", boxWidgetClass,  infoArea,
			      XtNfromVert,	form02Line,
			      XtNorientation,	"horizontal",
			      NULL);
  {
    planRateMutDataLabel
      = XtVaCreateManagedWidget("planRateMutDataLabel",
				labelWidgetClass, form03Line,
				XtNlabel,	"RateMut     --------",
				NULL);
    planRateMovMutDataLabel
      = XtVaCreateManagedWidget("planRateMovMutDataLabel",
				labelWidgetClass, form03Line,
				XtNlabel,	"RatMovMut   --------",
				NULL);
    planRateFlawDataLabel
      = XtVaCreateManagedWidget("planRateFlawDataLabel",
				labelWidgetClass, form03Line,
				XtNlabel,	"RateFlaw    --------",
				NULL);
  }

  form04Line
    = XtVaCreateManagedWidget("form04LineBox", boxWidgetClass,  infoArea,
			      XtNfromVert,	form03Line,
			      XtNorientation,	"horizontal",
			      NULL);
  {
    planAverageSizeDataLabel
      = XtVaCreateManagedWidget("planAverageSizeDataLabel",
				labelWidgetClass, form04Line,
				XtNlabel,	"AvgSize     --------",
				NULL);
    planAvgPopDataLabel
      = XtVaCreateManagedWidget("planAvgPopDataLabel",
				labelWidgetClass, form04Line,
				XtNlabel,	"AvgPop      --------",
				NULL);
    planFecundityAvgDataLabel
      = XtVaCreateManagedWidget("planFecundityAvgDataLabel",
				labelWidgetClass, form04Line,
				XtNlabel,	"FecundAvg   --------",
				NULL);

    planRepInstEffDataLabel
      = XtVaCreateManagedWidget("planRepInstEffDataLabel",
				labelWidgetClass, form04Line,
				XtNlabel,	"RepInstEf   --------",
				NULL);
  }

  form05Line
    = XtVaCreateManagedWidget("form05LineBox", boxWidgetClass,  infoArea,
			      XtNfromVert,	form04Line,
			      XtNorientation,	"horizontal",
			      NULL);
  {
    planFEBirthNumDataLabel
      = XtVaCreateManagedWidget("planFEBirthNumDataLabel",
				labelWidgetClass, form05Line,
				XtNlabel,	"Births      --------",
				NULL);
    planFEBirthLocalDataLabel
      = XtVaCreateManagedWidget("planFEBirthLocalDataLabel",
				labelWidgetClass, form05Line,
				XtNlabel,	"BirthLoca   --------",
				NULL);
    planFEBirthInjectDataLabel
      = XtVaCreateManagedWidget("planFEBirthInjectDataLabel",
				labelWidgetClass, form05Line,
				XtNlabel,	"BirthInje   --------",
				NULL);
    planFEBirthEjectDataLabel
      = XtVaCreateManagedWidget("planFEBirthEjectDataLabel",
				labelWidgetClass, form05Line,
				XtNlabel,	"BirthEjec   --------",
				NULL);
  }

  form06Line
    = XtVaCreateManagedWidget("form06LineBox", boxWidgetClass,  infoArea,
			      XtNfromVert,	form05Line,
			      XtNorientation,	"horizontal",
			      NULL);
  {
    planFEDeathNumDataLabel
      = XtVaCreateManagedWidget("planFEDeathNumDataLabel",
				labelWidgetClass, form06Line,
				XtNlabel,	"Deaths      --------",
				NULL);
    planFEFecundYesSumDataLabel
      = XtVaCreateManagedWidget("planFEFecundYesSumDataLabel",
				labelWidgetClass, form06Line,
				XtNlabel,	"DeathFecY   --------",
				NULL);
    planFEFecunZeroSumDataLabel
      = XtVaCreateManagedWidget("planFEFecunSeroSumDataLabel",
				labelWidgetClass, form06Line,
				XtNlabel,	"DeathFecZ   --------",
				NULL);
    planFEFecunEjecSumDataLabel
      = XtVaCreateManagedWidget("planFEFecunEjecSumDataLabel",
				labelWidgetClass, form06Line,
				XtNlabel,	"DeathEjec   --------",
				NULL);
  }

  form07Line
    = XtVaCreateManagedWidget("form07LineBox", boxWidgetClass,  infoArea,
			      XtNfromVert,	form06Line,
			      XtNorientation,	"horizontal",
			      NULL);
  {
    planMaxPopDataLabel
      = XtVaCreateManagedWidget("planMacPopDataLabel",
				labelWidgetClass, form07Line,
	XtNlabel,	"MaxGenPop    -------- (-------)",
				NULL);
    planMaxMemryDataLabel
      = XtVaCreateManagedWidget("planMaxMemryDataLabel",
				labelWidgetClass, form07Line,
	XtNlabel,	"MaxGenMem    -------- (-------)",
				NULL);
    planNumGenDGDataLabel
      = XtVaCreateManagedWidget("planNumGenDGDataLabel",
				labelWidgetClass, form07Line,
				XtNlabel,	"NumGenDG    --------",
				NULL);
  }

  form08Line
    = XtVaCreateManagedWidget("form08LineBox", boxWidgetClass,  infoArea,
			      XtNfromVert,	form07Line,
			      XtNorientation,	"horizontal",
			      NULL);
  {
    planNumNodesDataLabel
      = XtVaCreateManagedWidget("planNumNodesDataLabel",
				labelWidgetClass, form08Line,
				XtNlabel,	"NumNodes    --------",
				NULL);
    planFESImmigraNumDataLabel
      = XtVaCreateManagedWidget("planFESImmigraNumDataLabel",
				labelWidgetClass, form08Line,
				XtNlabel,	"SendImmig   --------",
				NULL);
    planFERImmigraNumDataLabel
      = XtVaCreateManagedWidget("planFERImmigraNumDataLabel",
				labelWidgetClass, form08Line,
				XtNlabel,	"RecvImmig   --------",
				NULL);
    planFEEjectToSelfDataLabel
      = XtVaCreateManagedWidget("planFEEjectToSelfDataLabel",
				labelWidgetClass, form08Line,
				XtNlabel,	"EjeToSelf   --------",
				NULL);
  }

  form09Line
    = XtVaCreateManagedWidget("form09LineBox", boxWidgetClass,  infoArea,
			      XtNfromVert,	form08Line,
			      XtNorientation,	"horizontal",
			      NULL);
  {
    planFESPingReqNumDataLabel
      = XtVaCreateManagedWidget("planFESPingReqNumDataLabel",
				labelWidgetClass, form09Line,
				XtNlabel,	"SePingReq   --------",
				NULL);
    planFESPingRepNumDataLabel
      = XtVaCreateManagedWidget("planFESPingRepNumDataLabel",
				labelWidgetClass, form09Line,
				XtNlabel,	"SePingRep   --------",
				NULL);
    planFESSendAddNumDataLabel
      = XtVaCreateManagedWidget("planFESSendAddNumDataLabel",
				labelWidgetClass, form09Line,
				XtNlabel,	"SeSendAdd   --------",
				NULL);
    planFESByeNumDataLabel
      = XtVaCreateManagedWidget("planFESByeNumDataLabel",
				labelWidgetClass, form09Line,
				XtNlabel,	"SendBye     --------",
				NULL);
  }

  form10Line
    = XtVaCreateManagedWidget("form10LineBox", boxWidgetClass,  infoArea,
			      XtNfromVert,	form09Line,
			      XtNorientation,	"horizontal",
			      NULL);
  {
    planFERPingReqNumDataLabel
      = XtVaCreateManagedWidget("planFERPingReqNumDataLabel",
				labelWidgetClass, form10Line,
				XtNlabel,	"RePingReq   --------",
				NULL);
    planFERPingRepNumDataLabel
      = XtVaCreateManagedWidget("planFERPingRepNumDataLabel",
				labelWidgetClass, form10Line,
				XtNlabel,	"RePingRep   --------",
				NULL);
    planFERSendAddNumDataLabel
      = XtVaCreateManagedWidget("planFERSendAddNumDataLabel",
				labelWidgetClass, form10Line,
				XtNfromHoriz,	planFERPingRepNumDataLabel,
				XtNlabel,	"ReSendAdd   --------",
				NULL);
    planFERByeNumDataLabel
      = XtVaCreateManagedWidget("planFERByeNumDataLabel",
				labelWidgetClass, form10Line,
				XtNlabel,	"RecvBye     --------",
				NULL);
  }

  form11Line
    = XtVaCreateManagedWidget("form11LineBox", boxWidgetClass,  infoArea,
			      XtNfromVert,	form10Line,
			      XtNorientation,	"horizontal",
			      NULL);
  {
    planDynMemUseDataLabel
      = XtVaCreateManagedWidget("planDynMemUseDataLabel",
				labelWidgetClass, form11Line,
				XtNlabel,	"DynMemUse   --------",
				NULL);
    planTotNumAlocDataLabel
      = XtVaCreateManagedWidget("planTotNumAlocDataLabel",
				labelWidgetClass, form11Line,
				XtNlabel,	"TotNumAloc  --------",
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
  BglMM_Plan_stop();
  XtPopdown(planWindow);
  XtSetArg(args[0], XtNsensitive, True);
  XtSetValues(planOpenButton, args, 1);
}


/*
 *- This windows process routines.
 */
