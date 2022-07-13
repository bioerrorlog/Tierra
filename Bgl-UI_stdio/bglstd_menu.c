/* bglstd_menu.c -- -*- C -*-
 * 
 * Menu functions for stdio mode beagle
 * 
 * Author          : Tsukasa Kimezawa
 * Created On      : 
 * Last Modified By: YOSHIKAWA Tooru
 * Last Modified On: Mon Nov 22 16:11:04 1999
 * Update Count    : 271
 * Status          : 
 */

/* Copyright (c) : ATR Human Information Processing Research Laboratories  */
/* MemProf  labels: #131XX, in use:   */
/* BglError labels: #131XX, in use:   */


#include "clnt.h"
#include "bglstd_global.h"
#include "beagleUI.h"
#include "beagleMessageMgr.h"
#include "beagleUTIL_SockMgr.h"


static I8s  BglStdGetChar	P_((void));
static void BglStdGetStr	P_((I8s*));
static I32s BglStdMenuBreak	P_((void));
static void BglStdFile		P_((void));
static void BglStdFile_s	P_((void));
static void BglStdFile_q	P_((void));
static void BglStdFile_Q	P_((void));
static void BglStdFile_C	P_((void));
static void BglStdFile_E	P_((void));
static void BglStdInfo		P_((void));
static void BglStdInfo_p	P_((void));
static void BglStdInfo_s	P_((void));
static void BglStdInfo_g	P_((void));
static void BglStdInfo_m	P_((void));
static void BglStdInfo_z	P_((void));
static void BglStdInfo_e	P_((void));
static I32s BglStdInfo_es	P_((void));
static I32s BglStdInfo_eg	P_((void));
static I32s BglStdInfo_SortOrder_sf	P_((void));
static I32s BglStdInfo_SortOrder_gf	P_((void));
static I32s BglStdInfo_EffDaught	P_((void));
static I32s BglStdInfo_SortOrder_se	P_((void));
static I32s BglStdInfo_SortOrder_ge	P_((void));
static void BglStdVar		P_((void));
static void BglStdVar_a		P_((void));
static void BglStdVar_e		P_((void));
static void BglStdMisc		P_((void));
static void BglStdMisc_h	P_((void));
static void BglStdMisc_i	P_((void));
static void BglStdMisc_m	P_((void));
static void BglStdMisc_t	P_((void));
static void BglStdMisc_mig	P_((void));
static void BglStdOv		P_((void));
static void BglStdOv_s		P_((void));
static void BglStdOv_q		P_((void));
static void BglStdOv_g		P_((void));
static void BglStdCont		P_((void));
static void BglStdEsc		P_((void));
static void BglStdoPt		P_((void));
static void BglStdoPt_w		P_((void));
static void BglStdoPt_x		P_((void));
static void BglStdConnect	P_((void));


/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
void BglStdMainMenu()
{
  I8s answer;

  if (BglStdMenuBreak()) {
    tsprintf(bglStrBuf,
	     "\n=== %s : %d ========================\n",
	     UTIL_Sock_GetCurrent_Connection()->addr.host,
	     UTIL_Sock_GetCurrent_Connection()->addr.port);
    BglStdPrintf(BGL_MENU_SCR, 1,1,0,0,bglStrBuf);

    if ( UTIL_Sock_GetCurrent_Connection()->user.priv == BGL_PRIV_S )
      BglStdPrintf(BGL_MENU_SCR,1,2,0,0,
"beagle: Frontend Menu : SU mode");
    else if ( UTIL_Sock_GetCurrent_Connection()->user.priv == BGL_PRIV_N )
      BglStdPrintf(BGL_MENU_SCR,1,2,0,0,
"beagle: Frontend Menu : Non SU mode");

    tsprintf(bglStrBuf, "VERSION = %s", BGL_VERSION);
    BglStdPrintf(BGL_MENU_SCR,1, 3,0,0, bglStrBuf);

    BglStdPrintf(BGL_MENU_SCR,1, 4,0,0,
"---------------------------------------------------------");
    BglStdPrintf(BGL_MENU_SCR,1, 5,0,0,
"        Key      Function                                ");
    BglStdPrintf(BGL_MENU_SCR,1, 6,0,0,
"        f        File(save,quit)                         ");
    BglStdPrintf(BGL_MENU_SCR,1, 7,0,0,
"        i        Information on simulation               ");
    BglStdPrintf(BGL_MENU_SCR,1, 8,0,0,
"        v        Change or examine a soup_in variable    ");
    BglStdPrintf(BGL_MENU_SCR,1, 9,0,0,
"        o        OverView                                ");
    BglStdPrintf(BGL_MENU_SCR,1,10,0,0,
"        m        Misc. commands                          ");
    BglStdPrintf(BGL_MENU_SCR,1,11,0,0,
"        p        oPtion commands                         ");
    BglStdPrintf(BGL_MENU_SCR,1,12,0,0,
"        C        Connection change                       ");
    BglStdPrintf(BGL_MENU_SCR,1,13,0,0,
"        c        To Continue simulation                  ");
    BglStdPrintf(BGL_MENU_SCR,1,14,0,0,
"---------------------------------------------------------");

    while(1){
      BglStdPrintf(BGL_MENU_SCR,1,1,0,0,
"f-file i-info v-var o-ov m-misc p-option C-onnection c-continue |-> ");
      answer = BglStdGetChar();

      if (answer == 'c'){
	BglStdCont();
	BglC_Menu = 0;
	break;
      } 
      else if (answer == 'f')	BglStdFile();
      else if (answer == 'i')	BglStdInfo();
      else if (answer == 'v')	BglStdVar();
      else if (answer == 'm')	BglStdMisc();
      else if (answer == 'o')	BglStdOv();
      else if (answer == 'p')	BglStdoPt();
      else if (answer == 'C')	BglStdConnect();

      answer = ' ';
      BglC_Menu = 0;

      if(Bgl_step == 1L) break;	/* == MICRO Debugger Keypress-mode == */

      BglStdCont();
      BglStdMenuBreak();
    }
  }
  else{
    BglStdPrintf(BGL_MENU_SCR,1,1,0,0,
"\n Menu: E-Exit Beagle |->");
    answer = BglStdGetChar();

    if (answer == 'E'){
      BglStdFile_E();
      BglC_Menu = 0;
    } 
  }

  return ;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
/*
 * Get a char for stdio
 */
static I8s BglStdGetChar()
{
  static I8s ansStr[84];

  tfgets(ansStr, 84, stdin);
  DebugPrint1("ans = %c\n", ansStr[0]);
  return ansStr[0];
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
/*
 * Get a string for stdio
 */
static void BglStdGetStr(buf)
     I8s* buf;
{  
  tfgets(buf, 84, stdin);
  DebugPrint1("buf = %s\n", buf);
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
/*
 * Break the Main Menu
 */
static I32s BglStdMenuBreak()
{	
  if (BglMM_B_MenuBreak())    return 1;
  else{
    BglStdPrintf(BGL_MENU_SCR,1,1,0,0,
 "Request do_FEMenu was refused.");
    return 0;
  }
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
/*
 * File-Menu
 */
static void BglStdFile()
{
  I8s ans;

  BglStdPrintf(BGL_MENU_SCR,BGL_MENU_X,BGL_MENU_Y,0,0,
"FILE | s-save soup  q-save&quit Tierra  Q-Quit Tierra  C-lose this connection E-Exit beagle |->");

  ans = BglStdGetChar();
  if (ans== 's')	BglStdFile_s();
  else if (ans == 'q')	BglStdFile_q();
  else if (ans == 'Q')	BglStdFile_Q();
  else if (ans == 'C')	BglStdFile_C();
  else if (ans == 'E')	BglStdFile_E();
  return;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
/*
 * Save-Soup
 */
static void BglStdFile_s()
{
  I8s ans;

  BglStdPrintf(BGL_DIALOG_SCR, BGL_DIALOG_X, BGL_DIALOG_Y, 0, 0,
"Soup will be saved. Are you sure ? (y/n)->");

  ans = BglStdGetChar();
  if ((ans == 'y') || (ans == 'Y')){
    BglStdPrintf(BGL_DIALOG_SCR, BGL_DIALOG_X, BGL_DIALOG_Y, 0, 0,
"Writing soup ....");

    if(BglMM_B_File_s ()){
      BglStdPrintf(BGL_DIALOG_SCR, BGL_DIALOG_X, BGL_DIALOG_Y,0,0,
"Soup was saved.");
    } 
    else{
      BglStdPrintf(BGL_DIALOG_SCR, BGL_DIALOG_X, BGL_DIALOG_Y,0,0,
"ERROR : Request save soup was refused.");
    }
  }
  return;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
/*
 * Save-Soup & QUit Tierra
 */
static void BglStdFile_q()
{
  I8s ans;

  BglStdPrintf(BGL_DIALOG_SCR, BGL_DIALOG_X, BGL_DIALOG_Y,0,0,
"Soup will be save and Tierra will be going down. Are you sure ? (y/n)->");

  ans = BglStdGetChar();
  if ((ans == 'y') || (ans == 'Y')) {
    BglStdPrintf(BGL_DIALOG_SCR, BGL_DIALOG_X, BGL_DIALOG_Y,0,0,
"Writing soup ....");

    if(BglMM_B_File_q()) {
      BglStdPrintf(BGL_DIALOG_SCR, BGL_DIALOG_X, BGL_DIALOG_Y,0,0,
"Soup was saved and Tierra was going down. Exit beagle,too.");
      BglMM_B_File_E();
    }
    else{
      BglStdPrintf(BGL_DIALOG_SCR, BGL_DIALOG_X, BGL_DIALOG_Y,0,0,
"ERROR : Request save soup was refused.");
    }
  }
  return;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
/*
 * Quit Tierra
 */
static void BglStdFile_Q()
{
  I8s ans;

  BglStdPrintf(BGL_DIALOG_SCR, BGL_DIALOG_X, BGL_DIALOG_Y, 0, 0,
"Tierra will be going down. Are you sure ? (y/n)->");

  ans = BglStdGetChar();
  if ((ans == 'y') || (ans == 'Y')){
    if(BglMM_B_File_Q()){
      BglStdPrintf(BGL_DIALOG_SCR, BGL_DIALOG_X, BGL_DIALOG_Y,0,0,
"Tierra was going down. Exit beagle,too.");
      BglMM_B_File_E();
    }
    else{
      BglStdPrintf(BGL_DIALOG_SCR, BGL_DIALOG_X, BGL_DIALOG_Y,0,0,
"ERROR : Request quit tierra was refused.");
    }
  }
  return;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
/*
 * Close this connection
 */
static void BglStdFile_C()
{
  I8s ans;
  I32s ret;

  BglStdPrintf(BGL_DIALOG_SCR, BGL_DIALOG_X, BGL_DIALOG_Y, 0, 0,
"beagle will be close this connection. Are you sure ? (y/n)->");
  ans = BglStdGetChar();
  if ((ans == 'y') || (ans == 'Y')){
    BglMM_B_File_Close();
  }
  return;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
/*
 * Exit beagle
 */
static void BglStdFile_E()
{
  I8s ans;

  BglStdPrintf(BGL_DIALOG_SCR, BGL_DIALOG_X, BGL_DIALOG_Y, 0, 0,
"Beagle will be going down. Are you sure ? (y/n)->");

  ans = BglStdGetChar();
  if(ans == 'y' || ans == 'Y'){
    BglStdPrintf(BGL_DIALOG_SCR, BGL_DIALOG_X, BGL_DIALOG_Y, 0, 0,
"Exit beagle.");
    BglMM_B_File_E();
  }
  return;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
/*
 * Info-Menu
 */
static void BglStdInfo()
{
  I8s ans;

  BglStdPrintf(BGL_MENU_SCR,BGL_MENU_X,BGL_MENU_Y,0,0,
"INFO | p-plan s-size g-gen m-mem z-size query e-reprod eff >");
  ans = BglStdGetChar();
  if (ans == 'p')	BglStdInfo_p();
  else if (ans == 's')	BglStdInfo_s();
  else if (ans == 'g')	BglStdInfo_g();
  else if (ans == 'm')	BglStdInfo_m();
  else if (ans == 'z')	BglStdInfo_z();
  else if (ans == 'e')	BglStdInfo_e();
  return;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
/*
 * Info-plan
 */
static void BglStdInfo_p()
{ 
  if (BglMM_B_Info_plan()){
    BglStdPrintf(BGL_INFOP_SCR, BGL_INFOP_X, BGL_INFOP_Y, 0, 0,
"Now in Plan Display mode, updated every million time steps"); 
  }
  else{
    BglStdPrintf(BGL_INFOP_SCR, BGL_INFOP_X, BGL_INFOP_Y, 0, 0,
"Request Info-Plan was refused."); 
  }
  return;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
/*
 * Info-size histogram
 */
static void BglStdInfo_s()
{
  if ( BglStdInfo_SortOrder_sf() )	return; /* Cancel */
  if ( BglMM_B_Info_s() ){
    BglStdPrintf(BGL_INFOP_SCR, BGL_INFOP_X, BGL_INFOP_Y, 0, 0,
"Now in Size Histogram mode"); 
  }
  else{
    BglStdPrintf(BGL_INFOP_SCR, BGL_INFOP_X, BGL_INFOP_Y, 0, 0,
"Request Info-Size histgram was refused."); 
  }
  return;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
/*
 * Info-gen histogram
 */
static void BglStdInfo_g()
{
  if ( BglStdInfo_SortOrder_gf() )	return; /* Cancel */
  if ( BglMM_B_Info_g() ){
    BglStdPrintf(BGL_INFOP_SCR, BGL_INFOP_X, BGL_INFOP_Y, 0, 0,
"Now in Gene Histogram mode"); 
  }
  else{
    BglStdPrintf(BGL_INFOP_SCR, BGL_INFOP_X, BGL_INFOP_Y, 0, 0,
"Request Info-Gene histgram was refused."); 
  }
  return;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
/*
 * Info-mem hitogram
 */
static void BglStdInfo_m()
{
  if ( BglStdInfo_SortOrder_sf() )	return; /* Cancel */
  if ( BglMM_B_Info_m() ){
    BglStdPrintf(BGL_INFOP_SCR, BGL_INFOP_X, BGL_INFOP_Y, 0, 0,
"Now in Mem Histogram mode"); 
  }
  else{
    BglStdPrintf(BGL_INFOP_SCR, BGL_INFOP_X, BGL_INFOP_Y, 0, 0,
"Request Info-Mem histgram was refused."); 
  }
  return;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
/*
 * Info-Query size
 */
static void BglStdInfo_z()
{
  I8s inbuf[85];

  if( BglMM_B_SizeQuery_need() ){
    BglStdPrintf(BGL_INFOZ_SCR, BGL_INFOZ_X, BGL_INFOZ_Y, 0, 0,
"Enter a size class ( eg: 80 ) to examine -> ");
    BglStdGetStr(inbuf);
    BglMM_B_ReqSizeQuery_Need(inbuf);

    BglUI_SizeQuery_Changed();
  }
  return;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
/*
 * Info-reprod_eff
 */
static void BglStdInfo_e()
{
  I8s ans;

  BglStdPrintf(BGL_MENU_SCR,BGL_MENU_X,BGL_MENU_Y,0,0,
"Reprod Effic | s-size avg  g-genome ->");
  ans = BglStdGetChar();
  if (ans == 's')	BglStdInfo_es();
  else if (ans == 'g')	BglStdInfo_eg();

  return;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
/*
 * Info-reprod_eff (Size)
 */
static I32s BglStdInfo_es()
{
  if ( BglStdInfo_EffDaught() )		return 1; /* Cancel */
  if ( BglStdInfo_SortOrder_se() )	return 1; /* Cancel */
  if (BglMM_B_Info_es()){
    BglStdPrintf(BGL_INFOP_SCR, BGL_INFOP_X, BGL_INFOP_Y, 0, 0,
"Now in reprod size eff mode"); 
  }
  else{
    BglStdPrintf(BGL_INFOP_SCR, BGL_INFOP_X, BGL_INFOP_Y, 0, 0,
"Request reprod size eff histgram was refused."); 
  }

  return 0; /* Normal */
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
/*
 * Info-reprod_eff (Gene)
 */
static I32s BglStdInfo_eg()
{
  if ( BglStdInfo_EffDaught() )		return 1; /* Cancel */
  if ( BglStdInfo_SortOrder_ge() )	return 1; /* Cancel */
  if (BglMM_B_Info_eg()){
    BglStdPrintf(BGL_INFOP_SCR, BGL_INFOP_X, BGL_INFOP_Y, 0, 0,
"Now in reprod gene eff mode"); 
  }
  else{
    BglStdPrintf(BGL_INFOP_SCR, BGL_INFOP_X, BGL_INFOP_Y, 0, 0,
"Request reprod gene eff histgram was refused."); 
  }

  return 0; /* Normal */
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
/*
 * Info Histo Sort Order (Size,Freq)
 */
static I32s BglStdInfo_SortOrder_sf()
{
  I8s ans;

  BglStdPrintf(BGL_MENU_SCR,BGL_MENU_X,BGL_MENU_Y,0,0,
"sort order | f-freq/size  s-size/freq ->");
  ans = BglStdGetChar();
  if (ans == 'f')	BglMM_Info_SortByFreq();
  else if (ans == 's')	BglMM_Info_SortByNotFreq();
  else return 1; /* Cancel */

  return 0; /* Normal */
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
/*
 * Info Histo Sort Order (Gene,Freq)
 */
static I32s BglStdInfo_SortOrder_gf()
{
  I8s ans;

  BglStdPrintf(BGL_MENU_SCR,BGL_MENU_X,BGL_MENU_Y,0,0,
"sort order | f-freq/gen  g-gen/freq ->");
  ans = BglStdGetChar();
  if (ans == 'f')	BglMM_Info_SortByFreq();
  else if (ans == 'g')	BglMM_Info_SortByNotFreq();
  else return 1; /* Cancel */

  return 0; /* Normal */
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
/*
 * reprod eff daughter menu
 */
static I32s BglStdInfo_EffDaught()
{
  I8s ans;

  BglStdPrintf(BGL_MENU_SCR,BGL_MENU_X,BGL_MENU_Y,0,0,
"Reprod Effic | 1-1st daught  2-2nd daught ->");
  ans = BglStdGetChar();
  if (ans == '1')	BglMM_Info_Daught1();
  else if (ans == '2')	BglMM_Info_Daught2();
  else return 1; /* Cancel */

  return 0; /* Normal */
}


/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
/*
 * Info Histo Sort Order (Size,Effic)
 */
static I32s BglStdInfo_SortOrder_se()
{
  I8s ans;

  BglStdPrintf(BGL_MENU_SCR,BGL_MENU_X,BGL_MENU_Y,0,0,
"sort order | e-effic/size  s-size/effic ->");
  ans = BglStdGetChar();
  if (ans == 'e')	BglMM_Info_SortByFreq();
  else if (ans == 's')	BglMM_Info_SortByNotFreq();
  else return 1; /* Cancel */

  return 0; /* Normal */
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
/*
 * Info Histo Sort Order (Gene,Effic)
 */
static I32s BglStdInfo_SortOrder_ge()
{
  I8s ans;

  BglStdPrintf(BGL_MENU_SCR,BGL_MENU_X,BGL_MENU_Y,0,0,
"sort order | e-effic/gen  g-gen/effic ->");
  ans = BglStdGetChar();
  if (ans == 'e')	BglMM_Info_SortByFreq();
  else if (ans == 'g')	BglMM_Info_SortByNotFreq();
  else return 1; /* Cancel */

  return 0; /* Normal */
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
/*
 * Var-Menu
 */
static void BglStdVar()
{
  I8s ans;

  BglStdPrintf(BGL_MENU_SCR,1,1,0,0,
"VAR   |  a - alter variable       e - examine variable  |-> ");
  ans = BglStdGetChar();
  if (ans == 'a')	BglStdVar_a();
  else if (ans == 'e')	BglStdVar_e();
  return ;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
/*
 * Var-alter variables
 */
static void BglStdVar_a()
{
  I32s ret;
  I8s var[90];

  ret = BglMM_B_Var_a();
  if (ret > 0) {
    BglStdPrintf(BGL_VARA_SCR,BGL_VARA_X,BGL_VARA_Y+1,0,0,
"To alter any global variable from soup_in, type");
    BglStdPrintf(BGL_VARA_SCR,BGL_VARA_X,BGL_VARA_Y+2,0,0,
"the variable name (using proper case), a space,");
    BglStdPrintf(BGL_VARA_SCR,BGL_VARA_X,BGL_VARA_Y+3,0,0,
"an equal sign, a space, and the new value.");
    BglStdPrintf(BGL_VARA_SCR,BGL_VARA_X,BGL_VARA_Y+4,0,0,
"Use no space at start of line.  Some examples:\n");
    BglStdPrintf(BGL_VARA_SCR,BGL_VARA_X,BGL_VARA_Y+5,0,0,
"alive = 0 ");
    BglStdPrintf(BGL_VARA_SCR,BGL_VARA_X,BGL_VARA_Y+6,0,0,
"DistProp = .6 ");
    BglStdPrintf(BGL_VARA_SCR,BGL_VARA_X,BGL_VARA_Y+7,0,0,
"GenebankPath = newpath/ ");
    BglStdPrintf(BGL_VARA_SCR,BGL_VARA_X,BGL_VARA_Y+8,0,0,
"Input the global variable and the value |-> ");
    BglStdGetStr(var);
    ret = BglMM_B_Var_ae (var);
    tsprintf(bglStrBuf,"%s", BglReplyMesBuf.mes);
    BglStdPrintf(BGL_VARA_SCR,BGL_VARA_X,BGL_VARA_Y+9,0,0,bglStrBuf);
  }
  else{
    BglStdPrintf(BGL_VARA_SCR,BGL_VARA_X,BGL_VARA_Y+1,0,0,
"Request do_val_alter was refused.");
  }
  return;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
/*
 * Var-exaime variables
 */
static void BglStdVar_e()
{
  I8s ans;
  I8s getbuf[85];

  BglStdPrintf(BGL_VARE_SCR,BGL_VARE_X,BGL_VARE_Y,0,0,
"Var-exam | a - Examine All o - Examin One Variable |-> ");  
  ans = BglStdGetChar();

  if (ans == 'a'){
    if (BglMM_B_Var_e_all() < 0){
      BglStdPrintf(BGL_VARE_SCR,BGL_VARE_X,BGL_VARE_Y+1,0,0,
"Request Exmanine variable was refused.");
    }
  }
  else if (ans == 'o'){
    BglStdPrintf(BGL_VARE_SCR,BGL_VARE_X,BGL_VARE_Y,0,0,
"To examine any global variable from soup_in,");
    BglStdPrintf(BGL_VARE_SCR,BGL_VARE_X,BGL_VARE_Y+1,0,0,
"the name of the variable:");
    BglStdPrintf(BGL_VARE_SCR,BGL_VARA_X,BGL_VARE_Y+2,0,0,
"Input the global variable |-> ");
    BglStdGetStr(getbuf);
    if (BglMM_B_Var_e (getbuf) < 0){
      BglStdPrintf(BGL_VARE_SCR,BGL_VARE_X,BGL_VARE_Y+3,0,0,
"Request Exmanine variable was refused.");
    }
    else{
      tsprintf(bglStrBuf,"%s", BglReplyMesBuf.mes);
      BglStdPrintf(BGL_VARE_SCR,BGL_VARE_X,BGL_VARE_Y+3,0,0,bglStrBuf);	
    }
  }
  BglUI_Var_Changed();
  return;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
/*
 * Misc-Menu
 */
static void BglStdMisc()
{
  I8s ans;

  BglStdPrintf(BGL_MENU_SCR,BGL_MENU_X,BGL_MENU_Y,0,0,
"MISC | h-Histo Logging i-Inject Gene m-Micro Toggle t-tping M-migration |->");
  ans = BglStdGetChar();
  if (ans == 'h')	BglStdMisc_h();
  else if (ans == 'i')	BglStdMisc_i();
  else if (ans == 'm')	BglStdMisc_m();
  else if (ans == 't')	BglStdMisc_t();
  else if (ans == 'M')	BglStdMisc_mig();
  return;
}


/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
/*
 * Misc-Histolog
 */
static void BglStdMisc_h()
{
  BglMM_B_Misc_h();
  return;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
/*
 * Misc-Inject gene
 */
static void BglStdMisc_i()
{
  I32s ret;
  I8s nameStr[85];
  I8s numStr[85];
  I8s addrStr[85];

  ret = BglMM_B_Misc_i();
  if(ret){
    BglStdPrintf(BGL_INJT_SCR,BGL_INJT_X,BGL_INJT_Y+0,0,0,
"Enter genome to be injected (eg 0080aaa) (enter to abort)->");
    BglStdGetStr(nameStr);
    if( nameStr[0] == '\n' ){
      BglMM_B_Misc_ie( "\n","","");
      return;
    }

    BglStdPrintf(BGL_INJT_SCR,BGL_INJT_X,BGL_INJT_Y+1,0,0,
"Enter number of creatures to inject (enter to abort)->");
    BglStdGetStr(numStr);
    if( numStr[0] == '\n' ){
      BglMM_B_Misc_ie( "\n","","");
      return;
    }

    BglStdPrintf(BGL_INJT_SCR,BGL_INJT_X,BGL_INJT_Y+2,0,0,
"Enter preferred address, (Enter to abort, or -1 for no preference)->");
    BglStdGetStr(addrStr);
    if( addrStr[0] == '\n' ){
      BglMM_B_Misc_ie("\n","","");
      return;
    }

    BglMM_B_Misc_ie(nameStr,numStr,addrStr);
  }
  else{
    BglStdPrintf(BGL_INJT_SCR,BGL_INJT_X,BGL_INJT_Y+1,0,0,
"Request Misc-Inject gene was refused.");
  }
  return;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
/*
 * Misc-Micro debugger
 */
static void BglStdMisc_m()
{
  if( BglMM_B_Misc_m() ){
    /* NOP */
  }
  else{
    BglStdPrintf(BGL_MC_SPY_SCR, BGL_MC_SPY_X,BGL_MICRO_Y,0,0,
"Request Micro-Toggle was refused.");
  }
  return;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
/*
 * Misc-Tping
 */
static void BglStdMisc_t()
{
  I8s ans;

  DebugPrint(" /****** BglStdMisc_t *******/");
  BglStdPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
"TPING | S-Server TPing On  s-Server TPing Off\n" );
  BglStdPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y+1, 0, 0,
"TPING | C-Cluster TPing On  c-Cluster TPing Off Else return |-> ");

  ans = BglStdGetChar();
  if (ans == 'S'){
    if ( BglMM_B_Misc_tSOn() ){
      /* NOP */
    }
    else{
      BglStdPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
"Request Server Tping data was refused.");
    }
  }
  else if (ans == 's'){
    if ( BglMM_B_Misc_tSOff()){
      /* NOP */
    }
    else{
      BglStdPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
"Request Server Tping data was refused.");
    }
  }
  else if (ans == 'C'){
    if ( BglMM_B_Misc_tCOn()){
      /* NOP */
    }
    else{
      BglStdPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
"Request Cluster Tping data was refused.");
    }
  }
  else if (ans == 'c'){
    if ( BglMM_B_Misc_tCOff()){
      /* NOP */
    }
    else{
      BglStdPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
"Request Cluster Tping data was refused.");
    }
  }
  return;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
/*
 * Misc-Migration tracking On/Off
 */
static void BglStdMisc_mig()
{
  I8s ans;

  DebugPrint(" /****** BglStdMisc_mig *******/");
  BglStdPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
 "Migration Tracking | 1-On     2-Off    Else return |-> ");

  ans = BglStdGetChar();
  if (ans == '1'){
    if ( BglMM_MigrationOn_need()){
      /* NOP */
    }
    else{
      BglStdPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
"Request Migration data was refused.");
    }
  }
  else if (ans == '2'){
    if ( BglMM_MigrationOff_need()){
      /* NOP */
    }
    else{
      BglStdPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
"Request Migration data was refused.");
    }
  }
  return;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
/*
 * Misc-Micro debugger Key press mode
 */
void BglStdMisc_mk()
{
  I8s ans;

  tsprintf(bglStrBuf, "%s    %s    q-Main Menu    n-Next step  ",
	   (BglMM_WhatIsMCStateSlice()=='t')? "t-Untrack cell":"T-Track cell",
	   (BglMM_WhatIsMCStateCpu()=='u')? "u-Untrack Cpu"  : "U-Track Cpu");
  BglStdPrintf(BGL_MC_SPY_SCR,BGL_MC_SPY_X,BGL_MICRO_Y,0,0,bglStrBuf);
  while(1){
    ans = BglStdGetChar();
    if(ans =='T' || ans == 't')		{ BglMM_B_Misc_mT(); break; }
    else if(ans =='U' || ans == 'u')	{ BglMM_B_Misc_mU(); break; }
    else if(ans =='q')			{ BglMM_Misc_mq(); break; }
    else if(ans =='n')			{ BglMM_B_Misc_mn(); break; }
  }
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
/*
 * OV-Menu
 */
static void BglStdOv()
{
  I8s ans;

  BglStdPrintf(BGL_MENU_SCR,BGL_MENU_X,BGL_MENU_Y,0,0,
"OV   |  s-start overview  q-quit overview  g-genome      |->");
  ans = BglStdGetChar();
  if (ans == 's')	BglStdOv_s();
  else if (ans == 'q')	BglStdOv_q();
  else if (ans == 'g')	BglStdOv_g();
  return;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
/*
 * OV-start
 */
static void BglStdOv_s()
{
  BglStdPrintf(BGL_OV_SCR, BGL_OV_X, BGL_OV_Y, 0, 0,
"Receiving initial Overview data. Please wait....");
  if (BglMM_B_Ov_s() == 0){
    BglStdPrintf(BGL_OV_SCR, BGL_OV_X, BGL_OV_Y+1, 0, 0,
"Rquest Overview mode start was refused.");
  }
  else {
    BglStdPrintf(BGL_OV_SCR, BGL_OV_X, BGL_OV_Y+1, 0, 0,
"Start Overview mode.");
  }	
  return;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
/*
 * OV-quit
 */
static void BglStdOv_q()
{
  BglMM_B_Ov_q();
  return;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
/*
 * OV-genome
 */
static void BglStdOv_g()
{
  I8s ans, inpbuf[85];
  I32s ret;

  BglStdPrintf(BGL_GENE_SCR,BGL_GENE_X,BGL_GENE_Y,0,0,
"Gene | a - from address in Soup   s - from Size class | -> ");

  ans = BglStdGetChar();
  if (ans == 'a'){
    ret = BglMM_B_Ov_ga();
    if (ret < 0){
      BglStdPrintf(BGL_GENE_SCR,BGL_GENE_X,BGL_GENE_Y,0,0,
"Request OV genome data was refused");
      return;
    }
    while(1){
      BglStdPrintf(BGL_GENE_SCR,BGL_GENE_X,BGL_GENE_Y,0,0,
"Input start point of the cell : ");
      BglStdGetStr(inpbuf);
      BglMM_B_Ov_g_addr (inpbuf);
      BglUI_OvG_Changed();

      BglStdPrintf(BGL_GENE_SCR,BGL_GENE_X,BGL_GENE_Y,0,0,
"Genome| c-continue genome   return-main menu       |->");

      ans = BglStdGetChar();
      if (ans != 'c')	{	BglMM_B_Ov_g_end();	break;	}
      else		{	BglMM_B_Ov_g_cont();	}
    }
  }
  else if (ans == 's'){
    ret = BglMM_B_Ov_gs();
    if (ret < 0){
      BglStdPrintf(BGL_GENE_SCR,BGL_GENE_X,BGL_GENE_Y,0,0,
 "Request OV genome data was refused");
      return;
    }
    while(1){
      BglStdPrintf(BGL_GENE_SCR,BGL_GENE_X,BGL_GENE_Y,0,0,
"Input size class of the cell (ex. 0080aaa) :");
      BglStdGetStr(inpbuf);
      BglMM_B_Ov_g_size (inpbuf);
      BglUI_OvG_Changed();

      BglStdPrintf(BGL_GENE_SCR,BGL_GENE_X,BGL_GENE_Y,0,0,
"Genome| c-continue genome   return-main menu       |->");

      ans = BglStdGetChar();
     if (ans != 'c')	{	BglMM_B_Ov_g_end();	break;	}
      else		{	BglMM_B_Ov_g_cont();	}
    }
  }
  return;
}


/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
/*
 * Continue the process
 */
static void BglStdCont()
{
  BglMM_B_Cont();
  return;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
/*
 * Escape the menu
 */
static void BglStdEsc()
{
  BglMM_B_Esc();
  return;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
/*
 * Option-Menu
 */
static void BglStdoPt()
{
  I8s ans;

  BglStdPrintf(BGL_MENU_SCR,BGL_MENU_X,BGL_MENU_Y,0,0,
		"OPTION  |  w-wait time  x-XDR encode/decode buffer size |->");
  ans = BglStdGetChar();
  if (ans == 'w') {
    BglStdoPt_w();
  }
  if (ans == 'x'){
    BglStdoPt_x();
  }

  return;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
/*
 * Option-change wait time
 */
static void BglStdoPt_w()
{
  I8s ans, tmpbuf[90];
  double t;
  BglStdPrintf(BGL_OPT_SCR,BGL_OPT_X,BGL_OPT_Y,0,0,
"---------------   Change Wait time  -------------------");
  tsprintf(bglStrBuf,
"        0: Ack wait timeout (current %8.2f sec.)", Bgl_wait0);
  BglStdPrintf(BGL_OPT_SCR,BGL_OPT_X,BGL_OPT_Y+1,0,0,bglStrBuf);
  tsprintf(bglStrBuf,
"        1: Pollint time (current %5.4f sec.)", Bgl_wait1);
  BglStdPrintf(BGL_OPT_SCR,BGL_OPT_X,BGL_OPT_Y+2,0,0,bglStrBuf);
  tsprintf(bglStrBuf,
"        2: Tracking timeout (current %8.2f sec.)", Bgl_wait2);
  BglStdPrintf(BGL_OPT_SCR,BGL_OPT_X,BGL_OPT_Y+3,0,0,bglStrBuf);
  BglStdPrintf(BGL_OPT_SCR,BGL_OPT_X,BGL_OPT_Y+5,0,0,
"--------------------------------------------------------");
  BglStdPrintf(BGL_OPT_SCR,BGL_OPT_X,BGL_OPT_Y+6,0,0,
" Input 0 or 1 or 2. Escape by another key input.         |-> ");

  ans = BglStdGetChar();
  if( ans == '0' || ans == '1' || ans == '2' ){
    BglStdPrintf(BGL_OPT_SCR,BGL_OPT_X,BGL_OPT_Y+7,0,0,
"Input new wait time.                                    |-> ");
    BglStdGetStr(tmpbuf);
    t = atof(tmpbuf);
    BglMM_oPt_w (ans, t);
    tsprintf(bglStrBuf, "Wait time %c was changed to %3.3f", ans, t);
    BglStdPrintf(BGL_OPT_SCR,BGL_OPT_X,BGL_OPT_Y+8,0,0,bglStrBuf);
  }
  return;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
/*
 * Option-change max XDR encode/decode buffer size
 */
static void BglStdoPt_x()
{
  I8s ans, tmpbuf[90];
  I32s size;
  tsprintf(bglStrBuf,
"max XDR encode/decode buffer size (current %d.)", XDRBufMaxSize);
  BglStdPrintf(BGL_OPT_SCR,BGL_OPT_X,BGL_OPT_Y,0,0,bglStrBuf);
  BglStdPrintf(BGL_OPT_SCR,BGL_OPT_X,BGL_OPT_Y+1,0,0,
"Input new size. |-> ");
  BglStdGetStr(tmpbuf);
  size = atoi(tmpbuf);
  XDRBufMaxSize = size;
  tsprintf(bglStrBuf, "buffer size was changed to %d", size);
  BglStdPrintf(BGL_OPT_SCR,BGL_OPT_X,BGL_OPT_Y+2,0,0,bglStrBuf);
  return;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
static void BglStdConnect()
{
  I32s i;
  I32s numOfConnect;
  I32s ansNum;
  I8s ansStrNum[90];
  BglSockSt	*sP;

  BglStdPrintf(BGL_MENU_SCR, 1, 1, 0, 0,
"---------------------------------------------------------");

  i = 2;
  BglStdPrintf(BGL_MENU_SCR, 1, i, 0, 0,
"  0: Create New Connection");

  for(numOfConnect = 1;
      sP = UTIL_Sock_Get_Connection(numOfConnect);
      numOfConnect++ ){
    i++;
    tsprintf(bglStrBuf,"%3d: %-30s (%s mode)",
	     numOfConnect,
	     sP->addr.host,
	     (sP->user.priv == 's')? "   SU" : "NonSU");
    BglStdPrintf(BGL_MENU_SCR, 1, i, 0, 0, bglStrBuf);
  }
  BglStdPrintf(BGL_MENU_SCR, 1, 1, 0, 0,
"---------------------------------------------------------");

  BglStdPrintf(BGL_MENU_SCR,1,1,0,0,
"Please select in the most left side number (other number means escape) |-> ");
  BglStdGetStr(ansStrNum);
  if ( ansStrNum[0] == '\n' ) return;
  ansNum = atoi(ansStrNum);
  if ( (ansNum < 0)||(numOfConnect <= ansNum) ) return;
  if ( ansNum == 0 ){
    BglMM_Connect_Open
      (BglUI_StrInput("Please Key in connection Beagle URL or \
connection script file name or \"help\""), 1);
  }
  else{			 BglMM_Connect_ReOpen(ansNum);  }
  return;
}
