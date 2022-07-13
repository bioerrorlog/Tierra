/*
 * beagleMessage_Var.c -- 
 * 
 * This work has been done at ATR HIP
 * 
 * Author          : Marc Chaland
 * Created On      : Thu Oct  2 10:31:01 1997
 * Last Modified By: YOSHIKAWA Tooru
 * Last Modified On: Thu Nov 12 10:30:05 1998
 * Update Count    : 72
 * Status          : achieved
 */

#define CATCH_ARRAY

#include "clnt.h"
#include "beagleMessageMgrP.h"
#include "beagleMessage_Var.h"
#include "beagleUTIL_SockMgr.h"

#undef CATCH_ARRAY

/********************************************************************************
 * Static definitions
 */

/***  about do_BGL_VAR  ***/
MM_state_t MM_var_s = MMS_NORMAL;
struct MM_BglVarList_T {
  struct MM_BglVarList_T*	next;
  BglVarSt			d;
};
typedef struct MM_BglVarList_T	MM_BglVarList_t;
static MM_BglVarList_t	MM_varDataRoot;
static MM_BglVarList_t*	MM_varWp = &MM_varDataRoot; /* Write Pointer */
static MM_BglVarList_t*	MM_varRp = &MM_varDataRoot; /* Read Pointer */
static I32s		MM_varData_n = 0;

/***  for buffer of catched values  ***/
struct MM_BglVarCatched_T 
{
  struct MM_BglVarCatched_T*	next;
  BglVarSt			varfield;
};
typedef struct MM_BglVarCatched_T MM_BglVarCatched_t;
static MM_BglVarCatched_t *MM_BglVarCatchedRoot = NULL;

static int			BglMM_MustCatch		P_((I8s*));
static MM_BglVarCatched_t	*BglMM_FindVarCatched	P_((I8s*));
static void			BglMM_PutVarCatched	P_((BglVarSt *var));


#ifdef NB_MECHANISM
/***  Private functions for non blocking mechanism  ***/
static MM_funcstate_t	BglMM_NB_Var_a		P_((void));
static MM_funcstate_t	BglMM_NB_Var_a_Wait	P_((void));
static MM_funcstate_t	BglMM_NB_Var_ae		P_((void));
static MM_funcstate_t	BglMM_NB_Var_e		P_((void));
static MM_funcstate_t	BglMM_NB_Var_e_Wait	P_((void));
static MM_funcstate_t	BglMM_NB_Var_e_all	P_((void));
static MM_funcstate_t	BglMM_NB_Var_e_all_Wait	P_((void));
#endif /* NB_MECHANISM */

/* function for catching var arrival for request */
static void	BglMM_ResetCatchedVars		P_((MM_BglVarCatched_t *));
static void	BglMM_CheckVar			P_((void));
static void	(*MM_Check_VarValue)(I8s*);


/********************************************************************************
 * function implementation
 */

/******************************
 * Var buffer
 */
int	BglMM_IsVar_stop()
{
  return (MM_var_s == MMS_NORMAL);
}


I32s	BglMM_Var_modeReset()
{
  MM_var_s        = MMS_NORMAL;
  BglMM_ResetCatchedVars(MM_BglVarCatchedRoot);
  MM_BglVarCatchedRoot = NULL;
  
  return 1;
}

I32s	BglMM_HowManyVar()
{
  return MM_varData_n;
}

void	BglMM_Var_store(d)
     BglVarSt* d;
{
  /*DebugPrint("/==== BglMM_Var_store ====/\n");*/
  if ( MM_var_s == MMS_REQUESTED ||
       MM_var_s == MMS_CHANGED ){

    if (BglMM_MustCatch(d->name)){
      BglMM_PutVarCatched(d);
    }
    
    MM_varWp->d.index = d->index;
    MM_varWp->d.type = d->type;
    strcpy(MM_varWp->d.name, d->name);
    strcpy(MM_varWp->d.value, d->value);
    if ( MM_varWp->next == MM_varRp){
      MM_varWp->next
	= (MM_BglVarList_t*)tcalloc( 1, sizeof(MM_BglVarList_t), 52050);
      MM_varWp->next->next = MM_varRp;
    }
    MM_varWp = MM_varWp->next;
    MM_varData_n++;
    MM_var_s = MMS_CHANGED;
  }
  else{ DebugPrint("/==== BglMM_Var_store ignored ====/\n"); }
  return;
}

BglVarSt*	BglMM_PickVar()
{
  BglVarSt* retP = &(MM_varRp->d);
  MM_varRp = MM_varRp->next;
  if ( MM_varRp == MM_varWp ) MM_var_s = MMS_REQUESTED;
  MM_varData_n--;
  return retP;
}

/******************************
 * Var alter
 */
void	BglMM_VarAlter_Need(str)
     I8s* str;
{
#ifdef NB_MECHANISM
  MM_SequenceList_t* sP;
  I8s *	params[1];

  if (str[strlen(str)-1] == '\n')
    str[strlen(str)-1] = '\0';
  params[0] = str;
  
  BglMM_MakeProcessCell( sP = BglMM_MakeFuncCell( BglMM_NB_MenuBreak ),
			 NULL, NULL, BglMM_NB_Cont,
			 params, 1 );
  sP->next = BglMM_MakeFuncCell( BglMM_NB_MenuBreak_Wait );	sP = sP->next;
  sP->next = BglMM_MakeFuncCell( BglMM_NB_Var_a );		sP = sP->next;
  sP->next = BglMM_MakeFuncCell( BglMM_NB_Var_a_Wait );		sP = sP->next;
  sP->next = BglMM_MakeFuncCell( BglMM_NB_Var_ae );		sP = sP->next;
  sP->next = BglMM_MakeFuncCell( BglMM_NB_Cont );
#endif /* NB_MECHANISM */

#ifdef B_MECHANISM
  BglMM_B_Var_Alter(str);
#endif /* B_MECHANISM */
}

#ifdef B_MECHANISM
I32s	BglMM_B_Var_Alter(str)
     I8s* str;
{
  if ( !BglMM_B_MenuBreak() )
    return 1;
  
  if ( BglMM_B_Var_a() <= 0 )
    {
      BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
		"Request do_val_alter was refused.");
      BglMM_B_Cont();
      return 2;
    }
  
  BglMM_B_Var_ae(str);

  if ( BglMM_B_Cont() )
    return 3; /* Communication Error */ 

  if ( ! BglMM_B_MenuBreak() )
    return 4;
  
  if ( BglMM_B_Var_e_all() < 0 )
    {
      BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
		"Request Exmanine variable was refused.");
      BglMM_B_Cont();
      return 5;
    }

  if ( BglMM_B_Cont() )
    return 6; /* Communication Error */ 

  return 0; /* Normal return */
}

  
I32s	BglMM_B_Var_a()
{
  if ( BglMM_ClntSend( &BglReqValAlt ) ) return 0; /* Communication Error */
  if ( BglMM_B_ReplyWait( BGL_VAR_ALTER_OK, BGL_VAR_ALTER_REF )
       != BGL_REPLY_RET_OK )
    return 0;
  else
    return 1;
}

I32s	BglMM_B_Var_ae(buf)
     I8s* buf;
{
  /*DebugPrint("/==== BglMM_B_Var_ae ====/\n");*/
  if (buf[strlen(buf)-1] == '\n')    buf[strlen(buf)-1] = '\0';
  BglReplyMes(UTIL_Sock_GetCurrent_Connection()->fd, buf);
  if ( ! BglMM_B_ClntRecv() ) return 1; /* Communication Error */
  return 0;
}
#endif /* B_MECHANISM */

#ifdef NB_MECHANISM
static MM_funcstate_t	BglMM_NB_Var_a()
{
  /*DebugPrint("/==== BglMM_NB_Var_a ====/\n");*/
  if ( !BglMM_ClntSend( &BglReqValAlt ) ){
    /* Communication NO Error */
    return MMFS_CONTINUE;
  }
  return MMFS_FAIL;
}

static MM_funcstate_t	BglMM_NB_Var_a_Wait()
{
  I32s ret;
  /*DebugPrint("/==== BglMM_NB_Var_a ====/\n");*/
  ret = BglMM_NB_ReplyWait( BGL_VAR_ALTER_OK, BGL_VAR_ALTER_REF );
  switch( ret ){
  case BGL_REPLY_RET_OK:
    return MMFS_CONTINUE;
    break;

  case BGL_REPLY_RET_NONE:	/* No reply data come from Tierra */
    return MMFS_AGAIN;
    break;

  default:
    return MMFS_FAIL;
    break;
  }
}

static MM_funcstate_t	BglMM_NB_Var_ae()
{
  BglReplyMes(UTIL_Sock_GetCurrent_Connection()->fd,
	      BglMM_GetProcessParam(0));
  return MMFS_CONTINUE;
}
#endif /* NB_MECHANISM */


/******************************
 * One var examination
 */
void	BglMM_Var_Value(value, varname
#ifdef NB_MECHANISM
			,func
#endif	/* NB_MECHANISM */
			)
     I8s* value;
     I8s* varname;
#ifdef NB_MECHANISM
     void (*func)(I8s* value);
#endif	/* NB_MECHANISM */
{
  MM_BglVarCatched_t *	curs;
#ifdef B_MECHANISM
  BglVarSt *		mesg;
#endif /* B_MECHANISM */
#ifdef NB_MECHANISM
  MM_SequenceList_t* sP;
  I8s *	params[1];
#endif /* NB_MECHANISM */

  /* if the value is in memory */
  if (curs = BglMM_FindVarCatched(varname))
    {
      strcpy(value, curs->varfield.value);
      return;
    }
  value[0] = '\0';
  
  /* else, we ask value to tierra */

  if ( !(MM_varDataRoot.next) ) MM_varDataRoot.next = &MM_varDataRoot;

#ifdef NB_MECHANISM
  if ( varname[strlen(varname)-1] == '\n')
    varname[strlen(varname)-1] = '\0';

  params[0] = varname;
  
  BglMM_MakeProcessCell( sP = BglMM_MakeFuncCell( BglMM_NB_MenuBreak ),
			 NULL, NULL, BglMM_NB_Cont,
			 params, 1 );
  sP->next = BglMM_MakeFuncCell( BglMM_NB_MenuBreak_Wait );	sP = sP->next;
  sP->next = BglMM_MakeFuncCell( BglMM_NB_Var_e );		sP = sP->next;
  sP->next = BglMM_MakeFuncCell( BglMM_NB_Var_e_Wait );		sP = sP->next;
  sP->next = BglMM_MakeFuncCell( BglMM_NB_Cont );
  MM_var_s = MMS_REQUESTED;
  
  /* initialize for using datas as soon as it reachs Beagle */
  MM_Check_VarValue = func;
  BglMM_Catch(&MM_var_s, MMS_CHANGED, BglMM_CheckVar);
  
#endif /* NB_MECHANISM */

#ifdef B_MECHANISM
  if ( !BglMM_B_MenuBreak() )
    {
      BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
		"Request BglMM_Var_Value impossible.");
      return;
    }
  if (BglMM_B_Var_e(varname) < 0)
    {
      BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
		"Request BglMM_Var_Value was refused.");
      BglMM_B_Cont();
      return;
    }
  if (BglMM_B_Cont() )
    {
      BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
		"Request BglMM_Var_Value misfunction.");
      return;
    }

  if(mesg = BglMM_PickVar())
    strcpy(value, mesg->value);
  else
    strcpy(value, "unknown");

#endif /* B_MECHANISM */
}

#ifdef B_MECHANISM
I32s	BglMM_B_Var_e(buf)
     I8s* buf;
{
  I32s cc;

  if ( buf[strlen(buf)-1] == '\n')buf[strlen(buf)-1] = '\0';
  if ( BglMM_ClntSend( &BglReqValExam ) ) return -1; /* Communication Error */
  if ( BglMM_B_ReplyWait( BGL_VAR_EXAM_OK, BGL_VAR_EXAM_REF )
       != BGL_REPLY_RET_OK )
    return 0;
  else{
    cc = BglReplyMes(UTIL_Sock_GetCurrent_Connection()->fd, buf);	
    if ( ! (cc = BglMM_B_ClntRecv()) ) return -1; /* Communication Error */
    return cc;
  }
}
#endif /* B_MECHANISM */

#ifdef NB_MECHANISM
static MM_funcstate_t	BglMM_NB_Var_e()
{
  if ( !BglMM_ClntSend( &BglReqValExam ) ){
    /* Communication NO Error */
    return MMFS_CONTINUE;
  }
  return MMFS_FAIL;
}

static MM_funcstate_t	BglMM_NB_Var_e_Wait()
{
  I32s ret;

  ret = BglMM_NB_ReplyWait( BGL_VAR_EXAM_OK, BGL_VAR_EXAM_REF );
  switch( ret ){
  case BGL_REPLY_RET_OK:
    BglReplyMes( UTIL_Sock_GetCurrent_Connection()->fd,
		BglMM_GetProcessParam(0) );
    return MMFS_CONTINUE;
    break;

  case BGL_REPLY_RET_NONE:	/* No reply data come from Tierra */
    return MMFS_AGAIN;
    break;

  default:
    return MMFS_FAIL;
    break;
  }
}

static void BglMM_CheckVar()
{
  BglVarSt *	mesg;
  I8s		value[91];
  
  if(mesg = BglMM_PickVar())
    strcpy(value, mesg->value);
  else
    strcpy(value, "unknown");

  MM_Check_VarValue(value);
}

#endif /* NB_MECHANISM */

/******************************
 * All variables request
 */
I32s	BglMM_Var_need()
{
#ifdef NB_MECHANISM
  MM_SequenceList_t* sP;
#endif /* NB_MECHANISM */

  MM_varRp = MM_varWp = &MM_varDataRoot;
  if ( !(MM_varDataRoot.next) ) MM_varDataRoot.next = &MM_varDataRoot;
  MM_varData_n = 0;

#ifdef NB_MECHANISM
  if ( MM_var_s == MMS_NORMAL ){
    MM_var_s = MMS_NEED;

    BglMM_MakeProcessCell( sP = BglMM_MakeFuncCell( BglMM_NB_MenuBreak ),
			   NULL, NULL, NULL,
			   NULL, 0 );
    sP->next = BglMM_MakeFuncCell( BglMM_NB_MenuBreak_Wait );	sP = sP->next;
    sP->next = BglMM_MakeFuncCell( BglMM_NB_Var_e_all );	sP = sP->next;
    sP->next = BglMM_MakeFuncCell( BglMM_NB_Var_e_all_Wait );	sP = sP->next;
    sP->next = BglMM_MakeFuncCell( BglMM_NB_Cont );
    MM_var_s = MMS_REQUESTED;

    return 1;
  }
  else return 0;
#endif /* NB_MECHANISM */

#ifdef B_MECHANISM
  BglMM_B_Var_e_all();
#endif /* B_MECHANISM */
}

#ifdef B_MECHANISM
I32s	BglMM_B_Var_e_all()
{
  I32s cc;
  /*DebugPrint("/==== BglMM_B_Var_e_all ====/\n");*/
  if ( BglMM_ClntSend( &BglReqValExamAll ) ) return -1;/* Communication Error*/
  if ( BglMM_B_ReplyWait( BGL_VAR_EXAM_OK, BGL_VAR_EXAM_REF )
       != BGL_REPLY_RET_OK )
    return 0;
  else{
    if ( ! (cc = BglMM_B_ClntRecv()) ) return -1; /* Communication Error */
    return cc;
  }		
}
#endif /* B_MECHANISM */

#ifdef NB_MECHANISM
static MM_funcstate_t	BglMM_NB_Var_e_all()
{
  /*DebugPrint("/==== BglMM_NB_Var_e_all ====/\n");*/
  if ( !BglMM_ClntSend( &BglReqValExamAll ) ){
    /* Communication NO Error */
    return MMFS_CONTINUE;
  }
  return MMFS_FAIL;
}

static MM_funcstate_t	BglMM_NB_Var_e_all_Wait()
{
  I32s ret;
  /*DebugPrint("/==== BglMM_NB_Var_e_all_Wait ====/\n");*/
  ret = BglMM_NB_ReplyWait( BGL_VAR_EXAM_OK, BGL_VAR_EXAM_REF );
  switch( ret ){
  case BGL_REPLY_RET_OK:
    return MMFS_CONTINUE;
    break;

  case BGL_REPLY_RET_NONE:	/* No reply data come from Tierra */
    return MMFS_AGAIN;
    break;

  case BGL_REPLY_RET_TIMEOUT:
    BglMM_NB_Cont();
    return MMFS_FAIL;
    break;

  default:
    return MMFS_FAIL;
    break;
  }
}
#endif /* NB_MECHANISM */


/******************************
 * Catch var values
 */
static int	BglMM_MustCatch(var)
     I8s* var;
{
  int	i, j, flag = 1;

  for (j = 0 ; BglMM_CatchArray[j][0] && flag ; j++)
    {
      for (i = 0 ; (BglMM_CatchArray[j][i] && var[i])
	     && ((BglMM_CatchArray[j][i] != ' ') && (var[i] != ' '))
	     && ( BglMM_CatchArray[j][i] == var[i]) ; i++);
      flag = ((BglMM_CatchArray[j][i] != var[i]) || (var[i] != '\0'));
    }
  return !flag;
}

static void	BglMM_ResetCatchedVars(curs)
     MM_BglVarCatched_t* curs;
{
  if (curs)
    {
      if (curs->next)
	BglMM_ResetCatchedVars(curs->next);
      tfree(curs, sizeof(MM_BglVarCatched_t), 52051);
    }
}

static MM_BglVarCatched_t*	BglMM_FindVarCatched(var)
     I8s* var;
{
  MM_BglVarCatched_t *curs = MM_BglVarCatchedRoot;
  int	i, flag = 1;

  while (curs && flag)
    {
      for (i = 0 ; (curs->varfield.name[i] && var[i])
	     && ((curs->varfield.name[i] != ' ') && (var[i] != ' '))
	     && ( curs->varfield.name[i] == var[i]) ; i++);
      flag = ((curs->varfield.name[i] != var[i]) || (var[i] != '\0'));
      if (flag)
	curs = curs->next;
    }
  return curs;
}

static void	BglMM_PutVarCatched (var)
     BglVarSt* var;
{
  MM_BglVarCatched_t *curs = MM_BglVarCatchedRoot;
  int	i, flag = 1;

  if (! curs)
    MM_BglVarCatchedRoot = curs =
      (MM_BglVarCatched_t*)tcalloc( 1, sizeof(MM_BglVarCatched_t), 52051);
  else
    {
      while (curs->next && flag)
	{
	  for (i = 0 ; (curs->varfield.name[i] && var->name[i])
		 && ((curs->varfield.name[i] != ' ') && (var->name[i] != ' '))
		 && ( curs->varfield.name[i] == var->name[i]) ; i++);
	  flag =
	    ((curs->varfield.name[i] != var->name[i]) || (var->name[i] != '\0'));
	  if (flag)
	    curs = curs->next;
	}
      if (flag){
	curs = curs->next =
	  (MM_BglVarCatched_t*)tcalloc( 1, sizeof(MM_BglVarCatched_t), 52051);
	curs->next = NULL;
      }
    }
  curs->varfield.index = var->index;
  curs->varfield.type  = var->type;
  strcpy(curs->varfield.name, var->name);
  strcpy(curs->varfield.value, var->value);
}
