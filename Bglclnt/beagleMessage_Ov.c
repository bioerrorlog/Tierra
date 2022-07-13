/* beagleMessage_Ov.c -- -*- C -*-
 * 
 * This work has been done at ATR HIP
 * 
 * Author          : Marc Chaland
 * Created On      : Mon Sep 22 16:33:45 1997
 * Last Modified By: YOSHIKAWA Tooru
 * Last Modified On: Mon Nov 22 16:28:05 1999
 * Update Count    : 122
 * Status          : Achieved
 */

#include "clnt.h"
#include "beagleUI.h"
#include "beagleProcCtrl.h"
#include "beagleMessageMgrP.h"
#include "beagleUTIL_SockMgr.h"
#include "beagleUTIL_OVMgr.h"
#include "beagleMessage_Ov.h"


/********************************************************************************
 * Static definitions
 */
static MM_state_t MM_ovSession_s = MMS_NORMAL;

/** OvFrame **/
#define MM_OVFRAME_MAX 3
MM_state_t MM_ovFrame_s = MMS_NORMAL;
static I32s	MM_ovFrameWi = 0;	/* Write Index */
static I32s	MM_ovFrameRi = 0;	/* Read Index */
static BglOvFrameSt	MM_ovFrameData[MM_OVFRAME_MAX];

/** OV **/
MM_state_t MM_ov_s = MMS_NORMAL;
static I32s	MM_ovFoundStart = 0;
struct MM_BglOvMesList_T {
  struct MM_BglOvMesList_T*	next;
  BglOvMesSt			d;
};
typedef struct MM_BglOvMesList_T	MM_BglOvMesList_t;
static MM_BglOvMesList_t	MM_ovDataRoot;
static MM_BglOvMesList_t*	MM_ovWp = &MM_ovDataRoot; /* Write Pointer */
static MM_BglOvMesList_t*	MM_ovRp = &MM_ovDataRoot; /* Read Pointer */
static I32s			MM_ovData_n = 0;


/** OV_G_INST **/
#define MM_OV_G_INST_MAX 200
MM_state_t MM_ovGInst_s = MMS_NORMAL;
static I32s	MM_ovGInstWi = 0;	/* Write Index */
static I32s	MM_ovGInstRi = 0;	/* Read Index */
static I32s	MM_ovGInstSizeMax = MM_OV_G_INST_MAX;
static BglOvGInstSt	MM_ovGInstData[MM_OV_G_INST_MAX];

/** OV_G **/
#define MM_OV_G_MAX 3
MM_state_t MM_ovG_s = MMS_NORMAL;
static I32s	MM_ovGWi = 0;	/* Write Index */
static I32s	MM_ovGRi = 0;	/* Read Index */
static BglOvGOutputSt	MM_ovGData[MM_OV_G_MAX];

static int userNumber = 0;	/* number of users of overview */

/* static functions for non blocking mechanism */
#ifdef NB_MECHANISM
static MM_funcstate_t	BglMM_NB_Ov_s		P_((void));
static MM_funcstate_t	BglMM_NB_Ov_s_Wait	P_((void));
static MM_funcstate_t	BglMM_NB_Ov_q		P_((void));
static MM_funcstate_t	BglMM_NB_Ov_q_Wait	P_((void));
static MM_funcstate_t	BglMM_NB_Ov_ga		P_((void));
static MM_funcstate_t	BglMM_NB_Ov_ga_Wait	P_((void));
static MM_funcstate_t	BglMM_NB_Ov_g_addr	P_((void));
static MM_funcstate_t	BglMM_NB_Ov_gs		P_((void));
static MM_funcstate_t	BglMM_NB_Ov_gs_Wait	P_((void));
static MM_funcstate_t	BglMM_NB_Ov_g_size	P_((void));
static MM_funcstate_t	BglMM_NB_Ov_g_end	P_((void));
static MM_funcstate_t	BglMM_NB_Ov_g_cont	P_((void));
static MM_funcstate_t	BglMM_NB_Ov_save_gene	P_((void));
static MM_funcstate_t	BglMM_NB_Ov_overwrite_gene_wait	P_((void));
static MM_funcstate_t	BglMM_NB_Ov_save_gene_wait	P_((void));
#endif /* NB_MECHANISM */

/********************************************************************************
 * function implementation
 */

/******************************
 * Overview buffers
 */
int	BglMM_IsOvSession_stop()
{
  return (MM_ovSession_s == MMS_NORMAL);
}

I32s	BglMM_OvSession_modeReset()
{
  MM_ovSession_s = MMS_NORMAL;
  MM_ov_s        = MMS_NORMAL;
  MM_ovFoundStart = 0;
  BglOvGInstGCnt = 0;
  userNumber = 0;
  return 1;
}

/** OvFrame **/
I32s	BglMM_HowManyOvFrame()
{
  I32s ret;
  ret = MM_ovFrameWi - MM_ovFrameRi;
  if ( ret < 0 ) ret += MM_OVFRAME_MAX;
  return(ret);
}

void	BglMM_OvFrame_store(d)
     BglOvFrameSt* d;
{
  /*DebugPrint("/==== BglMM_OvFrame_store ====/\n");*/
  MM_ovFrameData[MM_ovFrameWi].InstNum  = d->InstNum;
  MM_ovGInstSizeMax = d->InstNum;
  MM_ovFrameData[MM_ovFrameWi].SoupSize = d->SoupSize;
  MM_ovFrameData[MM_ovFrameWi].NumCells = d->NumCells;
  MM_ovFrameData[MM_ovFrameWi].NumSizes = d->NumSizes;
  MM_ovFrameData[MM_ovFrameWi].NumGenotypes = d->NumGenotypes;

  MM_ovFrame_s = MMS_CHANGED;
  MM_ovFrameWi++; if( MM_ovFrameWi==MM_OVFRAME_MAX ) MM_ovFrameWi=0;
}

BglOvFrameSt*	BglMM_PickOvFrame()
{
  int ret_i;
  ret_i = MM_ovFrameRi;
  MM_ovFrameRi++;
  if( MM_ovFrameRi==MM_OVFRAME_MAX ) MM_ovFrameRi=0;
  if( MM_ovFrameRi==MM_ovFrameWi ) MM_ovFrame_s = MMS_REQUESTED;
  return &(MM_ovFrameData[ret_i]);
}

/** OV **/
I32s	BglMM_HowManyOv(){ return MM_ovData_n; }

void	BglMM_Ov_store(d)
     BglOvMesSt* d;
{
  OvData_t*	p;

  /*DebugPrint("/==== BglMM_Ov_store ====/\n");*/
  if ( MM_ovFoundStart ){
    if ( MM_ov_s == MMS_REQUESTED ||
	 MM_ov_s == MMS_CHANGED ){
      if ( d->event == 'b' ){
	UTIL_Ov_AddData( d->start, d->length );
      }
      else if ( d->event == 'd' ){
	if ( p = UTIL_Ov_SearchByStart(d->start) ){
	  if ( p->length != d->length ){
	    DebugPrint3
	      ("===  BglMM_Ov_store:  Wrong length data  %d (s=%d l=%d) ===\n",
	       d->length, p->start, p->length );
	  }
	  UTIL_Ov_DelThis(p);
	}
	else{
	  DebugPrint2
	    ("===  BglMM_Ov_store:  Wrong start data  (s=%d l=%d)  ===\n",
	     d->start, d->length );
	  return;  /* data will not store because it is wrong data */
	}
      }
      else{
	DebugPrint3
	  ("=== BglMM_Ov_store: Wrong event character (e=%c s=%d l=%d) ===\n",
	   d->event, d->start, d->length );
	return;  /* data will not store because it is wrong data */
      }

      MM_ovWp->d.event = d->event;
      MM_ovWp->d.start = d->start;
      MM_ovWp->d.length = d->length;
      if ( MM_ovWp->next == MM_ovRp ){
	MM_ovWp->next
	  = (MM_BglOvMesList_t*)tcalloc(1,sizeof(MM_BglOvMesList_t),52040);
	MM_ovWp->next->next = MM_ovRp;
      }
      MM_ovWp = MM_ovWp->next;
      MM_ovData_n++;
      MM_ov_s = MMS_CHANGED;

    }
    else{
      DebugPrint("/==== BglMM_Ov_store ignored ====/\n");
    }
  }
  else{
    if ( MM_ov_s == MMS_REQUESTED ||
	 MM_ov_s == MMS_CHANGED ){
      if ( d->event == 's' ){
	MM_ovFoundStart = 1;
	UTIL_Ov_ClearAll();
      }
    }
  }
  return;
}

BglOvMesSt*	BglMM_PickOv()
{
  BglOvMesSt* retP = &(MM_ovRp->d);
  MM_ovRp = MM_ovRp->next;
  if ( MM_ovRp == MM_ovWp ) MM_ov_s = MMS_REQUESTED;
  MM_ovData_n--;
  return retP;
}

/** OV_G_INST **/
I32s	BglMM_HowManyOvGInst()
{
  I32s ret;
  ret = MM_ovGInstWi - MM_ovGInstRi;
  if ( ret < 0 ) ret += MM_ovGInstSizeMax;
  return(ret);
}

void	BglMM_OvGInst_store(d)
     BglOvGInstSt* d;
{
  int i;
  
  for (i = 0 ; (i < MM_ovGInstWi) && (MM_ovGInstData[i].op - d->op) ; i++);
  MM_ovGInstData[i].op = d->op;
  strcpy(MM_ovGInstData[i].mn, d->mn);
  MM_ovGInst_s = MMS_CHANGED;
  if (++i > MM_ovGInstWi)
    MM_ovGInstWi =  i;
  else
    if (--i < MM_ovGInstRi)
      MM_ovGInstRi = i;
  BglOvGInstGCnt = MM_ovGInstWi;
  return;
}

BglOvGInstSt*	BglMM_PickOvGInst()
{
  int ret_i = MM_ovGInstRi;

  if( MM_ovGInstRi >= MM_ovGInstWi ){
    MM_ovGInst_s = MMS_REQUESTED;
    return NULL;
  }
  MM_ovGInstRi++;
  if( MM_ovGInstRi >= MM_ovGInstWi )
    MM_ovGInst_s = MMS_NORMAL;
  return &(MM_ovGInstData[ret_i]);
}

const char*	BglMM_InstrDisassemble(instr)
     I8s instr;
{
  static char undefined_instr[] = "Undefined";
  int i;

  for (i = 0; (i < MM_ovGInstSizeMax) && (MM_ovGInstData[i].op - instr); i++);
  if (i < MM_ovGInstSizeMax)
    return MM_ovGInstData[i].mn;
  else
    return undefined_instr;
}

/** OV_G **/
I32s	BglMM_HowManyOvG()
{
  I32s ret;
  ret = MM_ovGWi - MM_ovGRi;
  if ( ret < 0 ) ret += MM_OV_G_MAX;
  return(ret);
}

void	BglMM_OvG_store(d)
     BglOvGOutputSt* d;
{
  /*DebugPrint("/==== BglMM_OvG_store ====/\n");*/
  if ( MM_ovG_s == MMS_REQUESTED ||
       MM_ovG_s == MMS_CHANGED ){

    strcpy(MM_ovGData[MM_ovGWi].comment, d->comment);
    MM_ovGData[MM_ovGWi].type = d->type;
    MM_ovGData[MM_ovGWi].start = d->start;
    strcpy(MM_ovGData[MM_ovGWi].label, d->label);
    MM_ovGData[MM_ovGWi].difforig = d->difforig;
    MM_ovGData[MM_ovGWi].genome_siz = d->genome_siz;
    if (MM_ovGData[MM_ovGWi].genome.genome_val){
      tfree(MM_ovGData[MM_ovGWi].genome.genome_val,
	    MM_ovGData[MM_ovGWi].genome.genome_len*sizeof(xdrwrd), 52090);
    }
    MM_ovGData[MM_ovGWi].genome.genome_len = d->genome.genome_len;
    MM_ovGData[MM_ovGWi].genome.genome_val
      = (xdrwrd*)thcalloc(d->genome.genome_len, sizeof(xdrwrd), 52090);
    memcpy(MM_ovGData[MM_ovGWi].genome.genome_val,
	   d->genome.genome_val, d->genome_siz);

    MM_ovG_s = MMS_CHANGED;
    MM_ovGWi++; if( MM_ovGWi==MM_OV_G_MAX ) MM_ovGWi=0;
  }
  else{
    DebugPrint("/==== BglMM_OvG_store ignored ====/\n");
  }
  return;
}

BglOvGOutputSt*	BglMM_PickOvG()
{
  int ret_i;
  ret_i = MM_ovGRi;
  MM_ovGRi++;
  if( MM_ovGRi==MM_OV_G_MAX ) MM_ovGRi=0;
  if( MM_ovGRi==MM_ovGWi ) MM_ovG_s = MMS_REQUESTED;
  return &(MM_ovGData[ret_i]);
}

/******************************
 * Overview session start
 */
I32s	BglMM_OvSession_need()
{
#ifdef NB_MECHANISM
  MM_SequenceList_t* sP;
#endif /* NB_MECHANISM */

  if (userNumber++)	return 1;

  if ( MM_ovSession_s == MMS_NORMAL ){      
    MM_ovSession_s = MMS_NEED;
    MM_ov_s        = MMS_NEED;

#ifdef NB_MECHANISM
    BglMM_MakeProcessCell( sP = BglMM_MakeFuncCell( BglMM_NB_MenuBreak ),
			  NULL, NULL, BglMM_NB_Cont,
			  NULL, 0 );
    sP->next = BglMM_MakeFuncCell( BglMM_NB_MenuBreak_Wait );	sP = sP->next;
    sP->next = BglMM_MakeFuncCell( BglMM_NB_Ov_s );		sP = sP->next;
    sP->next = BglMM_MakeFuncCell( BglMM_NB_Ov_s_Wait );	sP = sP->next;
    sP->next = BglMM_MakeFuncCell( BglMM_NB_Cont );
#endif /* NB_MECHANISM */

#ifdef B_MECHANISM
    if ( !BglMM_B_MenuBreak() ){
      MM_ovSession_s = MMS_NORMAL;
      MM_ov_s        = MMS_NORMAL;
      return 0;
    }
    if ( !BglMM_B_Ov_s()){
      MM_ovSession_s = MMS_NORMAL;
      MM_ov_s        = MMS_NORMAL;
      BglMM_B_Cont();
      return 0;
    }
    if ( BglMM_B_Cont() ){
      MM_ovSession_s = MMS_NORMAL;
      MM_ov_s        = MMS_NORMAL;
      return 0;
    }
#endif /* B_MECHANISM */

    return 1;
  }
  else
    return 0;
}

#ifdef B_MECHANISM
I32s	BglMM_B_Ov_s()
{
  I32s cc;

  /*DebugPrint("/==== BglMM_B_Ov_s ====/\n");*/
  if (BglOvGInstGCnt <= 0)	cc = BglMM_ClntSend(&BglReqOV_S_I);
  else				cc = BglMM_ClntSend(&BglReqOV_S);

  if (cc) return 0; /* Communication Error */

  if ( BglMM_B_ReplyWait( BGL_OV_S_OK, BGL_OV_S_REF ) != BGL_REPLY_RET_OK )
    return 0;
  else{
    MM_ovSession_s = MMS_REQUESTED;
    MM_ov_s        = MMS_REQUESTED;

    MM_ovFrameWi = MM_ovFrameRi = 0;
    MM_ovRp = MM_ovWp;
    MM_ovData_n = 0;
    if ( !(MM_ovDataRoot.next) ) MM_ovDataRoot.next = &MM_ovDataRoot;
    MM_ovFoundStart = 0;

    return 1;
  }
}
#endif /* B_MECHANISM */

#ifdef NB_MECHANISM
static MM_funcstate_t	BglMM_NB_Ov_s()
{
  I32s cc;
  /*DebugPrint("/==== BglMM_NB_Ov_s ====/\n");*/
  if (BglOvGInstGCnt <= 0)	cc = BglMM_ClntSend(&BglReqOV_S_I);
  else				cc = BglMM_ClntSend(&BglReqOV_S);

  if ( !cc ){
    /* Communication No Error */
    MM_ovSession_s = MMS_REQUESTED;
    MM_ov_s        = MMS_REQUESTED;

    MM_ovFrameWi = MM_ovFrameRi = 0;
    MM_ovRp = MM_ovWp;
    MM_ovData_n = 0;
    if ( !(MM_ovDataRoot.next) ) MM_ovDataRoot.next = &MM_ovDataRoot;
    MM_ovFoundStart = 0;

    return MMFS_CONTINUE;
  }
  MM_ovSession_s = MMS_NORMAL;
  MM_ov_s        = MMS_NORMAL;
  return MMFS_FAIL;
}

static MM_funcstate_t	BglMM_NB_Ov_s_Wait()
{
  I32s ret;
  /*DebugPrint("/==== BglMM_NB_Ov_s_Wait ====/\n");*/
  ret = BglMM_NB_ReplyWait( BGL_OV_S_OK, BGL_OV_S_REF );
  switch( ret ){
  case BGL_REPLY_RET_OK:
    return MMFS_CONTINUE;
    break;

  case BGL_REPLY_RET_NONE:	/* No reply data come from Tierra */
    return MMFS_AGAIN;
    break;

  default:
    MM_ovSession_s = MMS_NORMAL;
    MM_ov_s        = MMS_NORMAL;
    return MMFS_FAIL;
    break;
  }
}
#endif /* NB_MECHANISM */


/******************************
 * Overview session stop
 */
I32s	BglMM_OvSession_stop()
{
#ifdef NB_MECHANISM
  MM_SequenceList_t* sP;
#endif /* NB_MECHANISM */

  if (--userNumber)	return 1;

  if ( ( MM_ovSession_s == MMS_NEED )||( MM_ovSession_s == MMS_REQUESTED ) ){  
    (void)BglMM_OvSession_modeReset();
#ifdef NB_MECHANISM
    BglMM_MakeProcessCell( sP = BglMM_MakeFuncCell( BglMM_NB_MenuBreak ),
			  NULL, NULL, BglMM_NB_Cont,
			  NULL, 0 );
    sP->next = BglMM_MakeFuncCell( BglMM_NB_MenuBreak_Wait );	sP = sP->next;
    sP->next = BglMM_MakeFuncCell( BglMM_NB_Ov_q );		sP = sP->next;
    sP->next = BglMM_MakeFuncCell( BglMM_NB_Ov_q_Wait );	sP = sP->next;
    sP->next = BglMM_MakeFuncCell( BglMM_NB_Cont );
#endif /* NB_MECHANISM */

#ifdef B_MECHANISM
    if ( !BglMM_B_MenuBreak() )	return 0;
    if ( !BglMM_B_Ov_q()){
      BglMM_B_Cont();
      return 0;
    }
    if ( BglMM_B_Cont() )	return 0;
#endif /* B_MECHANISM */

    return 1;
  }
  return 0;
}

#ifdef B_MECHANISM
I32s	BglMM_B_Ov_q()
{
  /*DebugPrint("/==== BglMM_B_Ov_q ====/\n");*/
  if ( BglMM_ClntSend( &BglReqOV_Q ) ) return 0;/*Communication Error*/
  if ( BglMM_B_ReplyWait( BGL_OV_Q_OK, BGL_OV_Q_REF ) != BGL_REPLY_RET_OK )
    return 0;
  else{
    return 1;
  }
}
#endif /* B_MECHANISM */

#ifdef NB_MECHANISM
static MM_funcstate_t	BglMM_NB_Ov_q()
{
  if ( !BglMM_ClntSend( &BglReqOV_Q ) ){
    /* Communication No Error */
    return MMFS_CONTINUE;
  }
  return MMFS_FAIL;
}

static MM_funcstate_t	BglMM_NB_Ov_q_Wait()
{
  I32s ret;
  /*DebugPrint("/==== BglMM_NB_Ov_q_Wait ====/\n");*/
  ret = BglMM_NB_ReplyWait( BGL_OV_Q_OK, BGL_OV_Q_REF );
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
#endif /* NB_MECHANISM */


/******************************
 * request gene from address
 */
void	BglMM_GeneAddress_Need(address)
     I8s* address;
{
#ifdef NB_MECHANISM
  MM_SequenceList_t* sP;

  BglMM_MakeProcessCell( sP = BglMM_MakeFuncCell( BglMM_NB_MenuBreak ),
			 NULL, NULL, BglMM_NB_Cont,
			 &address, 1);
  sP->next = BglMM_MakeFuncCell( BglMM_NB_MenuBreak_Wait );	sP = sP->next;
  sP->next = BglMM_MakeFuncCell( BglMM_NB_Ov_ga );		sP = sP->next;
  sP->next = BglMM_MakeFuncCell( BglMM_NB_Ov_ga_Wait );		sP = sP->next;
  sP->next = BglMM_MakeFuncCell( BglMM_NB_Ov_g_addr );		sP = sP->next;
  sP->next = BglMM_MakeFuncCell( BglMM_NB_Ov_g_end );		sP = sP->next;
  sP->next = BglMM_MakeFuncCell( BglMM_NB_Cont );
#endif /* NB_MECHANISM */

#ifdef B_MECHANISM
  BglMM_B_Ov_g_Address(address);
#endif /* B_MECHANISM */
}

void	BglMM_Gene_Wait(func)
     void (*func)();
{
  BglMM_Catch(&MM_ovG_s, MMS_CHANGED, func);
}


#ifdef B_MECHANISM
I32s	BglMM_B_Ov_g_Address(adrStr)
     I8s* adrStr;
{
  I32s ret;

  if ( !BglMM_B_MenuBreak() ){
    BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
	      "Request BglMM_B_Ov_g_Address was refused.");
    return 1;
  }

  if ( ! BglMM_B_Ov_ga() ){
    BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
	      "Request OV genome data was refused");
    BglMM_B_Cont();
    return 2;
  }

  if ( BglMM_B_Ov_g_addr( adrStr ) ){
    BglMM_B_Cont();
    return 3;	/*Communication Error*/
  }

  BglMM_B_Ov_g_end();

  if ( BglMM_B_Cont() ){
    return 4;	/* Communication Error */ 
  }

  return 0;
}

I32s	BglMM_B_Ov_ga()
{
  I32s cc;

  /*DebugPrint("/==== BglMM_B_Ov_ga ====/\n");*/
  MM_ovG_s = MMS_REQUESTED;
  if (BglOvGInstGCnt <= 0)	cc = BglMM_ClntSend( &BglReqOV_GA_I );
  else				cc = BglMM_ClntSend( &BglReqOV_GA );

  if ( cc ) return 0; /* Communication Error */

  if ( BglMM_B_ReplyWait( BGL_OV_GA_OK, BGL_OV_GA_REF ) != BGL_REPLY_RET_OK )
    return 0;
  else
    return 1;
}

I32s	BglMM_B_Ov_g_addr(addr)
     I8s* addr;
{
  /*DebugPrint("/==== BglMM_B_Ov_g_addr ====/\n");*/
  BglReplyMes(UTIL_Sock_GetCurrent_Connection()->fd, addr);
  if ( ! BglMM_B_ClntRecv() ) return 1; /* Communication Error */
  return 0;
}
#endif /* B_MECHANISM */

#ifdef NB_MECHANISM
/*
 * BglMM_NB_Ov_ga
 *	send request for viewing gene from address
 */
static MM_funcstate_t	BglMM_NB_Ov_ga()
{
  I32s cc;

  MM_ovG_s = MMS_REQUESTED;
  if (BglOvGInstGCnt <= 0)	cc = BglMM_ClntSend( &BglReqOV_GA_I );
  else				cc = BglMM_ClntSend( &BglReqOV_GA );

  if ( !cc )	return MMFS_CONTINUE;

  return MMFS_FAIL;
}

/*
 * BglMM_NB_Ov_ga_Wait
 *	wait for acknoledgement from beagle before sending address.
 */
static MM_funcstate_t	BglMM_NB_Ov_ga_Wait()
{
  I32s ret;
  /*DebugPrint("/==== BglMM_NB_Ov_ga_Wait ====/\n");*/
  ret = BglMM_NB_ReplyWait( BGL_OV_GA_OK, BGL_OV_GA_REF );
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

/*
 * BglMM_NB_Ov_g_addr
 *	send address to tierra
 */
static MM_funcstate_t	BglMM_NB_Ov_g_addr()
{
  BglReplyMes(UTIL_Sock_GetCurrent_Connection()->fd,
	      BglMM_GetProcessParam(0));
  return MMFS_CONTINUE;
}
#endif /* NB_MECHANISM */


/******************************
 * gene from size
 */
void	BglMM_GeneSize_Need(size)
     I8s* size;
{
#ifdef NB_MECHANISM
  MM_SequenceList_t* sP;

  BglMM_MakeProcessCell( sP = BglMM_MakeFuncCell( BglMM_NB_MenuBreak ),
			 NULL, NULL, BglMM_NB_Cont,
			 &size, 1);
  sP->next = BglMM_MakeFuncCell( BglMM_NB_MenuBreak_Wait );	sP = sP->next;
  sP->next = BglMM_MakeFuncCell( BglMM_NB_Ov_gs );		sP = sP->next;
  sP->next = BglMM_MakeFuncCell( BglMM_NB_Ov_gs_Wait );		sP = sP->next;
  sP->next = BglMM_MakeFuncCell( BglMM_NB_Ov_g_size );		sP = sP->next;
  sP->next = BglMM_MakeFuncCell( BglMM_NB_Ov_g_end );		sP = sP->next;
  sP->next = BglMM_MakeFuncCell( BglMM_NB_Cont );
#endif /* NB_MECHANISM */

#ifdef B_MECHANISM
  BglMM_B_Ov_g_Size(size);
#endif /* B_MECHANISM */
}

#ifdef B_MECHANISM
I32s	BglMM_B_Ov_g_Size(sizeStr)
     I8s* sizeStr;
{
  I32s ret;

  if ( ! BglMM_B_MenuBreak() ){
    BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
	      "Request BglMM_B_Ov_g_Size was refused."); 
    return 1;
  }

  if ( ! BglMM_B_Ov_gs() ){
    BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
	      "Request OV genome data was refused");
    BglMM_B_Cont();
    return 2;
  }

  if ( BglMM_B_Ov_g_size( sizeStr ) ){
    BglMM_B_Cont();
    return 3;	/*Communication Error*/
  }

  BglMM_B_Ov_g_end();

  if ( BglMM_B_Cont() )	return 4;	/* Communication Error */ 

  return 0;
}

I32s	BglMM_B_Ov_gs()
{
  I32s cc;

  /*DebugPrint("/==== BglMM_B_Ov_gs ====/\n");*/
  MM_ovG_s = MMS_REQUESTED;
  if (BglOvGInstGCnt <= 0)	cc = BglMM_ClntSend( &BglReqOV_GS_I );
  else				cc = BglMM_ClntSend( &BglReqOV_GS );

  if ( cc ) return 0; /* Communication Error */

  if ( BglMM_B_ReplyWait( BGL_OV_GS_OK, BGL_OV_GS_REF ) != BGL_REPLY_RET_OK )
    return 0;
  else
    return 1;
}

I32s	BglMM_B_Ov_g_size(siz)
     I8s* siz;
{
  /*DebugPrint("/==== BglMM_B_Ov_g_size ====/\n");*/
  BglReplyMes(UTIL_Sock_GetCurrent_Connection()->fd, siz);
  if ( ! BglMM_B_ClntRecv() ) return 1; /* Communication Error */
  return 0;
}
#endif /* B_MECHANISM */

#ifdef NB_MECHANISM
/*
 * BglMM_NB_Ov_gs
 *	send request for viewing gene from size
 */
static MM_funcstate_t	BglMM_NB_Ov_gs()
{
  I32s cc;

  MM_ovG_s = MMS_REQUESTED;
  if (BglOvGInstGCnt <= 0)	cc = BglMM_ClntSend( &BglReqOV_GS_I );
  else				cc = BglMM_ClntSend( &BglReqOV_GS );

  if ( !cc ){
    return MMFS_CONTINUE;
  }
  return MMFS_FAIL;
}

/*
 * BglMM_NB_Ov_gs_Wait
 *	wait for acknoledgement from beagle before sending size.
 */
static MM_funcstate_t	BglMM_NB_Ov_gs_Wait()
{
  I32s ret;
  ret = BglMM_NB_ReplyWait( BGL_OV_GS_OK, BGL_OV_GS_REF );
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

/*
 * BglMM_NB_Ov_g_size
 *	send size to tierra
 */
static MM_funcstate_t	BglMM_NB_Ov_g_size()
{
  BglReplyMes(UTIL_Sock_GetCurrent_Connection()->fd,
	      BglMM_GetProcessParam(0));
  return MMFS_CONTINUE;
}
#endif /* NB_MECHANISM */


/******************************
 * Save Gene
 */
void BglMM_GeneSave_Need(saveCodeP)
     SaveCodeSt* saveCodeP;
{
#ifdef NB_MECHANISM
  MM_SequenceList_t* sP;
  I8s*	params[5];

  params[0] = saveCodeP->filename;
  params[1] = saveCodeP->mode ? "1" : "0";
  params[2] = (saveCodeP->curorig == 'o') ? "o" : "c";
  params[3] = saveCodeP->tarpt ? "1" : "0";
  params[4] = saveCodeP->detailrpt ? "1" : "0";

  BglMM_MakeProcessCell( sP = BglMM_MakeFuncCell( BglMM_NB_MenuBreak ),
			 NULL, NULL, BglMM_NB_Cont,
			 params, 5 );
  sP->next = BglMM_MakeFuncCell( BglMM_NB_MenuBreak_Wait );	sP = sP->next;
  sP->next = BglMM_MakeFuncCell( BglMM_NB_Ov_save_gene );	sP = sP->next;
  sP->next = BglMM_MakeFuncCell( BglMM_NB_Ov_overwrite_gene_wait ); sP = sP->next;
  sP->next = BglMM_MakeFuncCell( BglMM_NB_Ov_save_gene_wait );	sP = sP->next;
  sP->next = BglMM_MakeFuncCell( BglMM_NB_Cont );
#endif /* NB_MECHANISM */

#ifdef B_MECHANISM
  BglMM_B_Ov_save_gene(saveCodeP);
#endif /* B_MECHANISM */
}

#ifdef B_MECHANISM
I32s	BglMM_B_Ov_save_gene(saveCodeP)
     SaveCodeSt* saveCodeP;
{
  I32s ret, overwrite;
  int len;
  char *str;
  BglMesSt sendmes;
  XDR msgHndl;
  I8s* xdrMsgBuf;
  I8s* xdrMsgBuf2;
#ifdef SIGBLOCK
  sigset_t SigSaveSet;
#endif /* SIGBLOCK */

  if (  !BglMM_B_MenuBreak() )		return 0;

  if ( BglMM_ClntSend(&BglReqOV_Save) ) return -1; /* Communication Error */

  xdrMsgBuf = (I8s*)thcalloc(XDRBufMaxSize, sizeof(I8u), 0);
  txdrmem_create(&msgHndl, xdrMsgBuf, XDRBufMaxSize, XDR_ENCODE);
#ifdef SIGBLOCK
  SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */
  if (!xdr_SaveCodeSt(&msgHndl, saveCodeP)){
#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
    txdr_destroy(&msgHndl);
    tfree(xdrMsgBuf, XDRBufMaxSize*sizeof(char), 0);
    BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
	      "XDR encode error in the function BglMM_B_Ov_save_gene()");
    return -2; /* Communication (Encode) Error */
  }
#ifdef SIGBLOCK
  TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

  xdrMsgBuf2 = (I8s*)thcalloc(XDRBufMaxSize+4, sizeof(I8u), 1);
  len = txdr_getpos(&msgHndl);
  len = BglMakeMesData( BGL_SAVE_CELL, xdrMsgBuf, len, xdrMsgBuf2 );

  sendmes.buf = (I8s*)tcalloc(1, BGL_MAX_MES_LEN, 11502);
  BglMakeMes(BGL_TAG_DAT, xdrMsgBuf2, len, &sendmes);
  if ( BglMM_ClntSend(&sendmes) ){
    tfree(sendmes.buf,BGL_MAX_MES_LEN, 11502);
    txdr_destroy(&msgHndl);
    tfree(xdrMsgBuf, XDRBufMaxSize*sizeof(char), 0);
    tfree(xdrMsgBuf2, (XDRBufMaxSize+4)*sizeof(char), 1);
    return -3; /* Communication Error */
  }
  tfree(sendmes.buf,BGL_MAX_MES_LEN, 11502);
  txdr_destroy(&msgHndl);
  tfree(xdrMsgBuf, XDRBufMaxSize*sizeof(char), 0);
  tfree(xdrMsgBuf2, (XDRBufMaxSize+4)*sizeof(char), 1);

  ret = BglReplyWait( UTIL_Sock_GetCurrent_Connection()->fd,
		      BglNfds, &BglAfds, BGL_OV_SAVE_OK,
		      BGL_OV_SAVE_REFUSE, Bgl_wait0 );
  switch(ret){
  case BGL_REPLY_RET_OK:
    BglMM_PickReply();
    overwrite = 1;
    break;
  case BGL_REPLY_RET_REF:
    BglMM_PickReply();
    str =
      BglUI_StrInput("File already exists. Do you want to overwrite it (yes/no) ?");
    while (str[0] && strcmp(str, "yes") && strcmp(str, "no"))
      str = BglUI_StrInput("Please answer yes or no");
    if (str[0])		/* if str is a null string, we stop process */
      overwrite = strcmp(str, "no"); /* overwrite if answer is not "no" */
    else
      overwrite = 0;
    break;
  case BGL_REPLY_RET_UNEXPECT:  /* unexpected reply data come from Tierra */
    BglMM_PickReply();
  default:
    tsprintf(bglStrBuf,"Error (%d), save process is aborted", (int)ret);
    BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0, bglStrBuf);
    BglMM_B_Cont();
    return -4;
  }

  /* send if user want overwrite or not */
  if (overwrite)
    BglReplyAck(UTIL_Sock_GetCurrent_Connection()->fd, BGL_OV_SAVE_OK);
  else{
    BglReplyAck(UTIL_Sock_GetCurrent_Connection()->fd, BGL_OV_SAVE_REFUSE);
    BglMM_B_Cont();
    return -5;
  }

  /* wait for acknoledgement of tierra for request */
  ret = BglMM_B_ReplyWait(BGL_OV_SAVE_OK, BGL_OV_SAVE_REFUSE);
  switch(ret){
  case BGL_REPLY_RET_OK:
    if (saveCodeP->mode){/* most recent thread analysis data trapped */
      tsprintf(bglStrBuf,
	       "most recent thread analysis data trapped is saved own gene named file by %s format.",
	       (saveCodeP->detailrpt)? "long" : "short");
    }
    else{
      tsprintf(bglStrBuf,
	       "%sthe %s genome is saved into %s by %s format",
	       (saveCodeP->tarpt) ? "the thread analysis of " : "",
	       (saveCodeP->curorig == 'o') ? "origin of" : "current",
	       saveCodeP->filename,
	       (saveCodeP->detailrpt)? "long" : "short");
    }
    BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0, bglStrBuf);
    break;
  case BGL_REPLY_RET_REF:
    if (saveCodeP->mode){/* most recent thread analysis data trapped */
      tsprintf(bglStrBuf,
	       "Tierra refuses to save the most recent thread analysis data trapped by %s format.",
	       (saveCodeP->detailrpt)? "long" : "short");
    }
    else{
      tsprintf(bglStrBuf,
	       "Tierra refuses to save the %s %s genome into %s by %s format",
	       (saveCodeP->tarpt) ? "thread analysis of the" : "",
	       (saveCodeP->curorig == 'o') ? "origin of" : "current",
	       saveCodeP->filename,
	       (saveCodeP->detailrpt)? "long" : "short");
    }
    BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0, bglStrBuf);
    break;
  default:
    tsprintf(bglStrBuf,"Error (%d), save process is aborted", (int)ret);
    BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0, bglStrBuf);
    BglMM_B_Cont();
    return -6;
  }
  BglMM_B_Cont();

  return 0;
}
#endif /* B_MECHANISM */

#ifdef NB_MECHANISM
static MM_funcstate_t	BglMM_NB_Ov_save_gene()
{
  int	len;
  BglMesSt sendmes;
  SaveCodeSt tmpData;
  XDR msgHndl;
  I8s* xdrMsgBuf;
  I8s* xdrMsgBuf2;
#ifdef SIGBLOCK
  sigset_t SigSaveSet;
#endif /* SIGBLOCK */

  if ( BglMM_ClntSend(&BglReqOV_Save) )
    return MMFS_FAIL; /* Communication Error */

  strcpy(tmpData.filename,BglMM_GetProcessParam(0));
  tmpData.mode = BglMM_GetProcessParam(1)[0] - '0';
  tmpData.curorig = BglMM_GetProcessParam(2)[0];
  tmpData.tarpt = BglMM_GetProcessParam(3)[0] - '0';
  tmpData.detailrpt = BglMM_GetProcessParam(4)[0] - '0';

  xdrMsgBuf = (I8s*)thcalloc(XDRBufMaxSize, sizeof(I8u), 0);
  txdrmem_create(&msgHndl, xdrMsgBuf, XDRBufMaxSize, XDR_ENCODE);
#ifdef SIGBLOCK
  SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */
  if (!xdr_SaveCodeSt(&msgHndl, &tmpData)){
#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
    txdr_destroy(&msgHndl);
    tfree(xdrMsgBuf, XDRBufMaxSize*sizeof(char), 0);
    BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
	      "XDR encode error in the function BglMM_NB_Ov_save_gene()");
    return MMFS_FAIL; /* Communication (Encode) Error */
  }
#ifdef SIGBLOCK
  TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

  xdrMsgBuf2 = (I8s*)thcalloc(XDRBufMaxSize+4, sizeof(I8u), 1);
  len = txdr_getpos(&msgHndl);
  len = BglMakeMesData( BGL_SAVE_CELL, xdrMsgBuf, len, xdrMsgBuf2 );

  sendmes.buf = (I8s*)tcalloc(1, BGL_MAX_MES_LEN, 11502);
  BglMakeMes(BGL_TAG_DAT, xdrMsgBuf2, len, &sendmes);
  if ( BglMM_ClntSend(&sendmes) ){
    tfree(sendmes.buf,BGL_MAX_MES_LEN, 11502);
    txdr_destroy(&msgHndl);
    tfree(xdrMsgBuf, XDRBufMaxSize*sizeof(char), 0);
    tfree(xdrMsgBuf2, (XDRBufMaxSize+4)*sizeof(char), 1);
    return MMFS_FAIL; /* Communication Error */
  }
  tfree(sendmes.buf,BGL_MAX_MES_LEN, 11502);
  txdr_destroy(&msgHndl);
  tfree(xdrMsgBuf, XDRBufMaxSize*sizeof(char), 0);
  tfree(xdrMsgBuf2, (XDRBufMaxSize+4)*sizeof(char), 1);

  return MMFS_CONTINUE;
}

static MM_funcstate_t	BglMM_NB_Ov_overwrite_gene_wait()
{
  I32s ret, overwrite;
  char *str;
  
  ret = BglMM_NB_ReplyWait( BGL_OV_SAVE_OK, BGL_OV_SAVE_REFUSE );
  switch(ret){
  case BGL_REPLY_RET_OK:
    overwrite = 1;
    break;

  case BGL_REPLY_RET_REF:
    str =
      BglUI_StrInput("File already exists. Do you want to overwrite it (yes/no) ?");
    while (str[0] && strcmp(str, "yes") && strcmp(str, "no"))
      str = BglUI_StrInput("Please answer yes or no");
    if (str[0])		/* if str is a null string, we stop process */
      overwrite = strcmp(str, "no"); /* overwrite if answer is not "no" */
    else
      overwrite = 0;
    break;

  case BGL_REPLY_RET_UNEXPECT:  /* unexpected reply data come from Tierra */
  case BGL_REPLY_RET_TIMEOUT:  /* time out */
  case BGL_REPLY_RET_ILL_FD:	/* unexpected FD */
  case BGL_REPLY_COM_ERR:	/* Communication Error */
  case BGL_REPLY_SELECT_ERR:	/* bgl_select return with -1 */
    tsprintf(bglStrBuf,"Error (%d), save process is aborted", (int)ret);
    BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0, bglStrBuf);
    return MMFS_FAIL;
    break;

  default:
    return MMFS_AGAIN;
    break;
  }

  if (overwrite){
    BglReplyAck(UTIL_Sock_GetCurrent_Connection()->fd, BGL_OV_SAVE_OK);
    return MMFS_CONTINUE;
  }
  else{
    BglReplyAck(UTIL_Sock_GetCurrent_Connection()->fd, BGL_OV_SAVE_REFUSE);
    return MMFS_FAIL;
  }
}

static MM_funcstate_t	BglMM_NB_Ov_save_gene_wait()
{
  I32s	ret;
  const char*	filename;
  char	mode;
  char	curorig;
  int	tarpt;
  int	detailrpt;

  filename	= BglMM_GetProcessParam(0);
  mode		= BglMM_GetProcessParam(1)[0] - '0';
  curorig	= BglMM_GetProcessParam(2)[0];
  tarpt		= BglMM_GetProcessParam(3)[0] - '0';
  detailrpt	= BglMM_GetProcessParam(4)[0] - '0';

  ret = BglMM_NB_ReplyWait(BGL_OV_SAVE_OK, BGL_OV_SAVE_REFUSE);
  switch(ret){
  case BGL_REPLY_RET_OK:
    if (mode){/* most recent thread analysis data trapped */
      tsprintf(bglStrBuf,
	       "most recent thread analysis data trapped is saved own gene named file by %s format.",
	       (detailrpt)? "long" : "short");
    }
    else{
      tsprintf(bglStrBuf,
	       "%sthe %s genome is saved into %s by %s format",
	       (tarpt) ? "the thread analysis of " : "",
	       (curorig == 'o') ? "origin of" : "current",
	       filename,
	       (detailrpt)? "long" : "short");
    }
    BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0, bglStrBuf);
    break;

  case BGL_REPLY_RET_REF:
    if (mode){/* most recent thread analysis data trapped */
      tsprintf(bglStrBuf,
	       "Tierra refuses to save the most recent thread analysis data trapped by %s format.",
	       (detailrpt)? "long" : "short");
    }
    else{
      tsprintf(bglStrBuf,
	       "Tierra refuses to save the %s %s genome into %s by %s format",
	       (tarpt) ? "thread analysis of the" : "",
	       (curorig == 'o') ? "origin of" : "current",
	       filename,
	       (detailrpt)? "long" : "short");
    }
    BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0, bglStrBuf);
    break;

  case BGL_REPLY_RET_UNEXPECT:  /* unexpected reply data come from Tierra */
  case BGL_REPLY_RET_TIMEOUT:  /* time out */
  case BGL_REPLY_RET_ILL_FD:	/* unexpected FD */
  case BGL_REPLY_COM_ERR:	/* Communication Error */
  case BGL_REPLY_SELECT_ERR:	/* bgl_select return with -1 */
    tsprintf(bglStrBuf,"Error (%d), save process is aborted", (int)ret);
    BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0, bglStrBuf);
    return MMFS_FAIL;
    break;

  default:
    return MMFS_AGAIN;
    break;
  }
  return MMFS_CONTINUE;
}
#endif /* NB_MECHANISM */


/******************************
 * Gene commands
 */
#ifdef B_MECHANISM
void	BglMM_B_Ov_g_end()
{
  /*DebugPrint("/==== BglMM_B_Ov_g_end ====/\n");*/
  BglReplyAck(UTIL_Sock_GetCurrent_Connection()->fd, BGL_OV_G_END);
  return;
}

void	BglMM_B_Ov_g_cont()
{
  /*DebugPrint("/==== BglMM_B_Ov_g_cont ====/\n");*/
  BglReplyAck(UTIL_Sock_GetCurrent_Connection()->fd, BGL_OV_G_CONT);
  return;
}
#endif /* B_MECHANISM */

#ifdef NB_MECHANISM
static MM_funcstate_t	BglMM_NB_Ov_g_end()
{
  /*DebugPrint("/==== BglMM_NB_Ov_g_end ====/\n");*/
  BglReplyAck(UTIL_Sock_GetCurrent_Connection()->fd, BGL_OV_G_END);
  return MMFS_CONTINUE;
}

static MM_funcstate_t	BglMM_NB_Ov_g_cont()
{
  /*DebugPrint("/==== BglMM_NB_Ov_g_cont ====/\n");*/
  BglReplyAck(UTIL_Sock_GetCurrent_Connection()->fd, BGL_OV_G_CONT);
  return MMFS_CONTINUE;
}
#endif /* NB_MECHANISM */
