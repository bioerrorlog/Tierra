/*
 * beagleMessage_File.c -- 
 * 
 * This work has been done at ATR HIP
 * 
 * Author          : Marc Chaland
 * Created On      : Tue Nov 18 10:02:41 1997
 * Last Modified By: YOSHIKAWA Tooru
 * Last Modified On: Tue Apr 20 17:15:31 1999
 * Update Count    : 25
 * Status          : Still under work
 */

#ifndef lint
static char* sccsProgramId="@(#)beagleMessage_File.c	1.7	04/20/99 17:16:08";
#endif /* lint */

#include "clnt.h"
#include "beagleProcCtrl.h"
#include "beagleMessageMgrP.h"
#include "beagleMessage_Connect.h"
#include "beagleUTIL_SockMgr.h"
#include "beagleMessage_File.h"

#ifdef NB_MECHANISM
static MM_funcstate_t	BglMM_NB_File_s		P_((void));
static MM_funcstate_t	BglMM_NB_File_s_Wait	P_((void));
static MM_funcstate_t	BglMM_NB_File_q		P_((void));
static MM_funcstate_t	BglMM_NB_File_q_Wait	P_((void));
static MM_funcstate_t	BglMM_NB_File_Q		P_((void));
static MM_funcstate_t	BglMM_NB_File_Q_Wait	P_((void));
static MM_funcstate_t	BglMM_NB_Exit		P_((void));
#endif /* NB_MECHANISM */



void	BglMM_File_Save()
{
#ifdef NB_MECHANISM
  MM_SequenceList_t* sP;

  BglMM_MakeProcessCell( sP = BglMM_MakeFuncCell( BglMM_NB_MenuBreak ),
			 NULL, NULL, BglMM_NB_Cont,
			 NULL, 0);
  sP->next = BglMM_MakeFuncCell( BglMM_NB_MenuBreak_Wait );	sP = sP->next;
  sP->next = BglMM_MakeFuncCell( BglMM_NB_File_s );		sP = sP->next;
  sP->next = BglMM_MakeFuncCell( BglMM_NB_File_s_Wait );	sP = sP->next;
  sP->next = BglMM_MakeFuncCell( BglMM_NB_Cont );
#endif /* NB_MECHANISM */

#ifdef B_MECHANISM
  BglMM_B_File_Save();
#endif /* B_MECHANISM */
}

#ifdef B_MECHANISM
I32s	BglMM_B_File_Save()
{
  if ( BglMM_B_MenuBreak() ){
    if( BglMM_B_File_s() ){
      BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0, "Soup was saved.");
    } 
    else{
      BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
		"ERROR : Request save soup was refused.");
    }
    if ( BglMM_B_Cont() ) return 1; /* Communication Error */
  }
  return 0;
}

I32s	BglMM_B_File_s()
{
  /*DebugPrint("/==== BglMM_B_File_s ====/\n");*/
  if ( BglMM_ClntSend( &BglReqSave ) ) return 0; /* Communication Error */
  if ( BglMM_B_ReplyWait( BGL_SAVE_OK, BGL_SAVE_REF != BGL_REPLY_RET_OK ) ){
    return 0; /* Eerror Return */
  }
  else{
    return 1;
  }
}
#endif /* B_MECHANISM */

#ifdef NB_MECHANISM
static MM_funcstate_t	BglMM_NB_File_s()
{
  /*DebugPrint("/==== BglMM_NB_File_s ====/\n");*/
  if ( !BglMM_ClntSend( &BglReqSave ) ){
    /* Communication No Error */
    return MMFS_CONTINUE;
  }
  return MMFS_FAIL;
}

static MM_funcstate_t	BglMM_NB_File_s_Wait()
{
  I32s ret;
  /*DebugPrint("/==== BglMM_NB_File_s_Wait ====/\n");*/
  ret = BglMM_NB_ReplyWait( BGL_SAVE_OK, BGL_SAVE_REF );
  switch( ret ){
  case BGL_REPLY_RET_OK:
    BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0, "Soup was saved.");
    return MMFS_CONTINUE;
    break;

  case BGL_REPLY_RET_TIMEOUT:
  case BGL_REPLY_RET_REF:
      BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
		"ERROR : Request save soup was refused.");
  case BGL_REPLY_RET_ILL_FD:	/* unexpected FD */
  case BGL_REPLY_COM_ERR:	/* Communication Error */
  case BGL_REPLY_SELECT_ERR:	/* bgl_select return with -1 */
    return MMFS_FAIL;
    break;

  default:
    /* NOP */
    break;
  }
  return MMFS_AGAIN;
}
#endif /* NB_MECHANISM */




void	BglMM_File_Quit()
{
#ifdef NB_MECHANISM
  MM_SequenceList_t* sP;

  BglMM_MakeProcessCell( sP = BglMM_MakeFuncCell( BglMM_NB_MenuBreak ),
			 NULL, NULL, BglMM_NB_Cont,
			 NULL, 0 );
  sP->next = BglMM_MakeFuncCell( BglMM_NB_MenuBreak_Wait );	sP = sP->next;
  sP->next = BglMM_MakeFuncCell( BglMM_NB_File_Q );		sP = sP->next;
  sP->next = BglMM_MakeFuncCell( BglMM_NB_File_Q_Wait );	sP = sP->next;
  sP->next = BglMM_MakeFuncCell( BglMM_NB_Cont );
#endif /* NB_MECHANISM */
  
#ifdef B_MECHANISM
  BglMM_B_File_Quit();
#endif /* B_MECHANISM */
}

#ifdef B_MECHANISM
I32s	BglMM_B_File_Quit()
{
  I32s ret;

  if ( BglMM_B_MenuBreak() ){
    if(BglMM_B_File_Q()){
      BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
		"Tierra will quit.");
      BglMM_Connect_Close();
    }
    else{
      BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
		"ERROR : Request quit tierra was refused.");
      if ( BglMM_B_Cont() ) return 1; /* Communication Error */
    }
  }
  else
    BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
	      "Request BglMM_B_File_Quit was refused.");

  return 0; /* Normal return */
}

I32s	BglMM_B_File_Q()
{
  /*DebugPrint("/==== BglMM_B_File_Q ====/\n");*/
  if ( BglMM_ClntSend( &BglReqQuit ) ) return 0;/*Communication Error*/
  if ( BglMM_B_ReplyWait( BGL_QUIT_OK, BGL_QUIT_REF ) != BGL_REPLY_RET_OK){
    return 0; /* Eerror Return */
  }
  return 1;
}
#endif /* B_MECHANISM */

#ifdef NB_MECHANISM
static MM_funcstate_t	BglMM_NB_File_Q()
{
  /*DebugPrint("/==== BglMM_NB_File_Q ====/\n");*/
  if ( !BglMM_ClntSend( &BglReqQuit ) ){
    /* Communication No Error */
    return MMFS_CONTINUE;
  }
  return MMFS_FAIL;
}


static MM_funcstate_t	BglMM_NB_File_Q_Wait()
{
  I32s ret;
  /*DebugPrint("/==== BglMM_NB_File_Q_Wait ====/\n");*/
  ret = BglMM_NB_ReplyWait( BGL_QUIT_OK, BGL_QUIT_REF );
  switch( ret ){
  case BGL_REPLY_RET_OK:
    BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
	      "Tierra will quit.");
    BglMM_Connect_Close();
    return MMFS_CONTINUE;
    break;

  case BGL_REPLY_RET_TIMEOUT:
  case BGL_REPLY_RET_REF:
      BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
		"ERROR : Request quit tierra was refused.");
  case BGL_REPLY_RET_ILL_FD:	/* unexpected FD */
  case BGL_REPLY_COM_ERR:	/* Communication Error */
  case BGL_REPLY_SELECT_ERR:	/* bgl_select return with -1 */
    return MMFS_FAIL;
    break;

  default:
    /* NOP */
    break;
  }
  return MMFS_AGAIN;
}
#endif /* NB_MECHANISM */




void	BglMM_File_SaveAndQuit()
{
#ifdef NB_MECHANISM
  MM_SequenceList_t* sP;

  BglMM_MakeProcessCell( sP = BglMM_MakeFuncCell( BglMM_NB_MenuBreak ),
			 NULL, NULL, BglMM_NB_Cont,
			 NULL, 0 );
  sP->next = BglMM_MakeFuncCell( BglMM_NB_MenuBreak_Wait );	sP = sP->next;
  sP->next = BglMM_MakeFuncCell( BglMM_NB_File_q );		sP = sP->next;
  sP->next = BglMM_MakeFuncCell( BglMM_NB_File_q_Wait );	sP = sP->next;
  sP->next = BglMM_MakeFuncCell( BglMM_NB_Cont );
#endif /* NB_MECHANISM */
  
#ifdef B_MECHANISM
  BglMM_B_File_SaveAndQuit();
#endif /* B_MECHANISM */
}

#ifdef B_MECHANISM
I32s	BglMM_B_File_SaveAndQuit()
{
  I32s ret;

  if ( BglMM_B_MenuBreak() ){
    if( BglMM_B_File_q() ) {
      BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
		"Soup saved and Tierra is going down.");
      BglMM_Connect_Close();
    }
    else{
      BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
		"ERROR : Request save soup and quit was refused.");
      if ( BglMM_B_Cont() ) return 1; /* Communication Error */
    }
  }
  else
    BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
	      "Request BglMM_B_File_SaveAndQuit was refused.");

  return 0; /* Normal return */
}

I32s	BglMM_B_File_q()
{
  /*DebugPrint("/==== BglMM_B_File_q ====/\n");*/
  if (BglMM_ClntSend( &BglReqSaveQuit ) ) return 0;/*Communication Error*/
  if (BglMM_B_ReplyWait( BGL_SAVEQUIT_OK, BGL_SAVEQUIT_REF)!=BGL_REPLY_RET_OK){
    return 0; /* Eerror Return */
  }
  return 1;
}
#endif /* B_MECHANISM */

#ifdef NB_MECHANISM
static MM_funcstate_t	BglMM_NB_File_q()
{
  /*DebugPrint("/==== BglMM_NB_File_q ====/\n");*/
  if ( !BglMM_ClntSend( &BglReqSaveQuit ) ){
    /* Communication No Error */
    return MMFS_CONTINUE;
  }
  return MMFS_FAIL;
}

static MM_funcstate_t	BglMM_NB_File_q_Wait()
{
  I32s ret;
  /*DebugPrint("/==== BglMM_NB_File_q_Wait ====/\n");*/
  ret = BglMM_NB_ReplyWait( BGL_SAVEQUIT_OK, BGL_SAVEQUIT_REF );
  switch( ret ){
  case BGL_REPLY_RET_OK:
    BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
	      "Soup saved and Tierra is going down.");
    BglMM_Connect_Close();
    return MMFS_CONTINUE;
    break;

  case BGL_REPLY_RET_TIMEOUT:
    /* Don't put BREAK here */
  case BGL_REPLY_RET_REF:
    BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
	      "ERROR : Request save soup and quit was refused.");
  case BGL_REPLY_RET_ILL_FD:	/* unexpected FD */
  case BGL_REPLY_COM_ERR:	/* Communication Error */
  case BGL_REPLY_SELECT_ERR:	/* bgl_select return with -1 */
    return MMFS_FAIL;
    break;

  default:
    /* NOP */
    break;
  }
  return MMFS_AGAIN;
}
#endif /* NB_MECHANISM */





I32s	BglMM_B_File_Close()
{
  BglMM_Connect_Close();	/* for compatibility with stdio */
  return 0; /* Normal return */
}




void	BglMM_File_Exit()
{
#ifdef NB_MECHANISM  
  BglMM_MakeProcessCell( BglMM_MakeFuncCell(  BglMM_NB_Exit ),
			 NULL, NULL, BglMM_NB_Cont,
			 NULL, 0 );
#endif /* NB_MECHANISM */

#ifdef B_MECHANISM
  BglMM_B_File_Exit();
#endif /* B_MECHANISM */
}

  
#ifdef B_MECHANISM
I32s	BglMM_B_File_Exit()
{
  BglMM_B_File_E();

  return 0; /* Normal return */
}

void	BglMM_B_File_E()
{
  /*DebugPrint("/==== BglMM_B_File_E ====/\n");*/
  BglMM_CloseBglSockFD(UTIL_Sock_GetCurrent_Connection()->fd);
  BglExit(0);	/* Nornal exit */
}
#endif /* B_MECHANISM */

#ifdef NB_MECHANISM
static MM_funcstate_t	BglMM_NB_Exit()
{
  BglMM_CloseBglSockFD(UTIL_Sock_GetCurrent_Connection()->fd);
  BglExit(0);	/* Nornal exit */

  return MMFS_CONTINUE;
}
#endif /* NB_MECHANISM */
