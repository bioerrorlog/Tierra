/* beagleMessage_Info.c -- -*- C -*-
 * 
 * This work has been done at ATR HIP
 * 
 * Author          : Marc Chaland
 * Created On      : Tue Sep 30 09:27:09 1997
 * Last Modified By: YOSHIKAWA Tooru
 * Last Modified On: Thu Nov 12 10:12:34 1998
 * Update Count    : 78
 * Status          : Unknown, Use with caution!
 */

#ifndef lint
static char* sccsProgramId="@(#)beagleMessage_Info.c	1.15	11/12/98 10:12:49";
#endif /* lint */

#include "clnt.h"
#include "beagleProcCtrl.h"
#include "beagleMessageMgrP.h"
#include "beagleUTIL_SockMgr.h"
#include "beagleMessage_Info.h"


static I32s	MM_SortOrder = 1;	/* 1: Sort by Freq or Effic 2: other */
static I32s	MM_DaughtRequest = 1;	/* 1: 1st 2: 2dn */


/* size histo */
MM_state_t MM_infoSize_s = MMS_NORMAL;
struct MM_BglSizHistList_T {
  struct MM_BglSizHistList_T*	next;
  BglSizHistSt			d;
};
typedef struct MM_BglSizHistList_T	MM_BglSizHistList_t;
static MM_BglSizHistList_t	MM_sizHistDataRoot;
static MM_BglSizHistList_t*	MM_sizHistWp = &MM_sizHistDataRoot;/*write p*/
static MM_BglSizHistList_t*	MM_sizHistRp = &MM_sizHistDataRoot;/*read p*/
static I32s			MM_sizHistData_n = 0;

/* gene histo */
MM_state_t MM_infoGene_s = MMS_NORMAL;
struct MM_BglGenHistList_T {
  struct MM_BglGenHistList_T*	next;
  BglGenHistSt			d;
};
typedef struct MM_BglGenHistList_T	MM_BglGenHistList_t;
static MM_BglGenHistList_t	MM_genHistDataRoot;
static MM_BglGenHistList_t*	MM_genHistWp = &MM_genHistDataRoot;/*write p*/
static MM_BglGenHistList_t*	MM_genHistRp = &MM_genHistDataRoot;/*read p*/
static I32s			MM_genHistData_n = 0;

/* mem histo */
MM_state_t MM_infoMem_s = MMS_NORMAL;
struct MM_BglMemHistList_T {
  struct MM_BglMemHistList_T*	next;
  BglMemHistSt			d;
};
typedef struct MM_BglMemHistList_T	MM_BglMemHistList_t;
static MM_BglMemHistList_t	MM_memHistDataRoot;
static MM_BglMemHistList_t*	MM_memHistWp = &MM_memHistDataRoot;/* write p*/
static MM_BglMemHistList_t*	MM_memHistRp = &MM_memHistDataRoot;/* read p*/
static I32s			MM_memHistData_n = 0;

/* effic size histo */
MM_state_t MM_infoEfficS_s = MMS_NORMAL;
struct MM_BglEfficSHistList_T {
  struct MM_BglEfficSHistList_T*	next;
  BglEfficSHistSt			d;
};
typedef struct MM_BglEfficSHistList_T	MM_BglEfficSHistList_t;
static MM_BglEfficSHistList_t	MM_efficSHistDataRoot;
static MM_BglEfficSHistList_t*	MM_efficSHistWp = &MM_efficSHistDataRoot;
static MM_BglEfficSHistList_t*	MM_efficSHistRp = &MM_efficSHistDataRoot;
static I32s			MM_efficSHistData_n = 0;

/* effic gene histo */
MM_state_t MM_infoEfficG_s = MMS_NORMAL;
struct MM_BglEfficGHistList_T {
  struct MM_BglEfficGHistList_T*	next;
  BglEfficGHistSt			d;
};
typedef struct MM_BglEfficGHistList_T	MM_BglEfficGHistList_t;
static MM_BglEfficGHistList_t	MM_efficGHistDataRoot;
static MM_BglEfficGHistList_t*	MM_efficGHistWp = &MM_efficGHistDataRoot;
static MM_BglEfficGHistList_t*	MM_efficGHistRp = &MM_efficGHistDataRoot;
static I32s			MM_efficGHistData_n = 0;

/* Plan */
#define MM_PLAN_MAX 3
MM_state_t MM_plan_s = MMS_NORMAL;
static I32s	MM_planWi = 0;	/* Write Index */
static I32s	MM_planRi = 0;	/* Read Index */
static BglPlanSt	MM_planData[MM_PLAN_MAX];

/* Histogram Clear */
MM_state_t MM_histClear_s = MMS_REQUESTED;
static BglHistClearSt	MM_histClearData;

static MM_funcstate_t	BglMM_SendOrder		P_((void));

static MM_funcstate_t   BglMM_Info_s_openP	P_((void));
static MM_funcstate_t   BglMM_Info_s_closeP     P_((void));
static MM_funcstate_t	BglMM_NB_Info_s		P_((void));
static MM_funcstate_t	BglMM_NB_Info_s_Wait	P_((void));
static MM_funcstate_t	BglMM_Info_g_openP	P_((void));
static MM_funcstate_t	BglMM_Info_g_closeP	P_((void));
static MM_funcstate_t	BglMM_NB_Info_g		P_((void));
static MM_funcstate_t	BglMM_NB_Info_g_Wait	P_((void));
static MM_funcstate_t	BglMM_Info_m_openP	P_((void));
static MM_funcstate_t	BglMM_Info_m_closeP	P_((void));
static MM_funcstate_t	BglMM_NB_Info_m		P_((void));
static MM_funcstate_t	BglMM_NB_Info_m_Wait	P_((void));
static MM_funcstate_t	BglMM_Info_es_openP	P_((void));
static MM_funcstate_t	BglMM_Info_es_closeP	P_((void));
static MM_funcstate_t	BglMM_NB_Info_es	P_((void));
static MM_funcstate_t	BglMM_NB_Info_es_Wait	P_((void));
static MM_funcstate_t	BglMM_Info_eg_openP	P_((void));
static MM_funcstate_t	BglMM_Info_eg_closeP	P_((void));
static MM_funcstate_t	BglMM_NB_Info_eg	P_((void));
static MM_funcstate_t	BglMM_NB_Info_eg_Wait	P_((void));
static I32s		BglMM_Plan_need_function P_((void));
static MM_funcstate_t	BglMM_NB_Info_plan	P_((void));
static MM_funcstate_t	BglMM_NB_Info_plan_Wait	P_((void));



/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s	BglMM_Info_RequireSortStyle(){  return MM_SortOrder;}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s	BglMM_Info_SortByFreq()
{
  MM_SortOrder = 1;	/* Sort By Freq or Effic */
  return 0;	/* Normal */
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s	BglMM_Info_SortByNotFreq()
{
  MM_SortOrder = 2;	/* Sort By Size or Gene */
  return 0;	/* Normal */
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s	BglMM_Info_RequireDought(){  return MM_DaughtRequest;}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s	BglMM_Info_Daught1()
{
  MM_DaughtRequest = 1;	/* 1st Daughter was requested	*/
  return 0;	/* Normal */
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s	BglMM_Info_Daught2()
{
  MM_DaughtRequest = 2;	/* 2nd Daughter was requested	*/
  return 0;	/* Normal */
}



/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
static MM_funcstate_t	BglMM_SendOrder()
{
  I8s  sendMessage[90];

  /*DebugPrint("/==== BglMM_SendOrder ====/\n");*/
  tsprintf( sendMessage, "%d %d", MM_SortOrder, MM_DaughtRequest );
  (void)BglReplyMes(UTIL_Sock_GetCurrent_Connection()->fd, sendMessage);

  return MMFS_CONTINUE;
}

/***************************************/
/*********  for HISTOGRAM Data *********/
/***************************************/
/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
void	BglMM_HistClear_store(d)
     BglHistClearSt* d;
{
  /*DebugPrint("/==== BglMM_HistClear_store ====/\n");*/
  if ( MM_histClear_s == MMS_REQUESTED ||
       MM_histClear_s == MMS_CHANGED ){
    MM_histClearData.flg = d->flg;

    MM_histClear_s = MMS_CHANGED;
  }
  else{ DebugPrint("/==== BglMM_HistClear_store ignored ====/\n"); }
  return;
}

/********************************************/
/*********  for SIZE HISTOGRAM Data *********/
/********************************************/
/***  SIZE HISTGRAM INFORMATION access functions  ***/
/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
int	BglMM_IsInfoSize_stop()
{
  return (MM_infoSize_s == MMS_NORMAL);
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s	BglMM_InfoSize_modeReset()
{
  MM_infoSize_s = MMS_NORMAL;
  return 0; /* Normal return */
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s	BglMM_Info_s_need()
{
  MM_SequenceList_t* sP;

  BglMM_MakeProcessCell( sP = BglMM_MakeFuncCell( BglMM_NB_MenuBreak ),
			 BglMM_Info_s_openP, BglMM_Info_s_closeP,
			 BglMM_NB_Cont,
			 NULL, 0 );
  sP->next = BglMM_MakeFuncCell( BglMM_NB_MenuBreak_Wait );	sP = sP->next;
  sP->next = BglMM_MakeFuncCell( BglMM_NB_Info_s );		sP = sP->next;
  sP->next = BglMM_MakeFuncCell( BglMM_NB_Info_s_Wait );	sP = sP->next;
  sP->next = BglMM_MakeFuncCell( BglMM_SendOrder );		sP = sP->next;
  sP->next = BglMM_MakeFuncCell( BglMM_NB_Cont );

  return 1;
}


/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
static MM_funcstate_t	BglMM_Info_s_openP()
{
  MM_infoSize_s   = MMS_NEED;
  MM_infoGene_s   = MMS_NORMAL;
  MM_infoMem_s    = MMS_NORMAL;
  MM_infoEfficS_s = MMS_NORMAL;
  MM_infoEfficG_s = MMS_NORMAL;
  MM_plan_s	  = MMS_NORMAL;
  return MMFS_CONTINUE;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
static MM_funcstate_t	BglMM_Info_s_closeP()
{
  return MMFS_CONTINUE;
}


/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s	BglMM_HowManySizeHist(){ return MM_sizHistData_n; }

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s	BglMM_B_Info_s()
{
  /*DebugPrint("/==== BglMM_B_Info_s ====/\n");*/
  (void)BglMM_Info_s_openP();
  if ( BglMM_ClntSend( &BglReqInfoSize ) ) return 0; /*Communication Error*/
  if ( BglMM_B_ReplyWait( BGL_INFO_SIZ_OK, BGL_INFO_SIZ_REF )
       != BGL_REPLY_RET_OK)
    return 0; /* Error Return */

  (void)BglMM_SendOrder();

  MM_infoSize_s = MMS_REQUESTED;
  MM_sizHistRp = MM_sizHistWp;
  MM_sizHistData_n = 0;
  if ( !(MM_sizHistDataRoot.next) )
    MM_sizHistDataRoot.next = &MM_sizHistDataRoot;

  return 1; /* No Error */
}


/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
static MM_funcstate_t	BglMM_NB_Info_s()
{
  /*DebugPrint("/==== BglMM_NB_Info_s ====/\n");*/
  if ( !BglMM_ClntSend( &BglReqInfoSize ) ){
    /* Communication No Error */
    return MMFS_CONTINUE;
  }
  return MMFS_FAIL;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
static MM_funcstate_t	BglMM_NB_Info_s_Wait()
{
  I32s ret;
  /*DebugPrint("/==== BglMM_NB_Info_s_Wait ====/\n");*/
  ret = BglMM_NB_ReplyWait( BGL_INFO_SIZ_OK, BGL_INFO_SIZ_REF );
  switch( ret ){
  case BGL_REPLY_RET_OK:
    MM_infoSize_s = MMS_REQUESTED;
    MM_sizHistRp = MM_sizHistWp;
    MM_sizHistData_n = 0;
    if ( !(MM_sizHistDataRoot.next) )
      MM_sizHistDataRoot.next = &MM_sizHistDataRoot;
    return MMFS_CONTINUE;
    break;

  case BGL_REPLY_RET_NONE:	/* No reply data come from Tierra */
    return MMFS_AGAIN;
    break;

  default:
    (void)BglMM_SendOrder();
    MM_infoSize_s = MMS_NORMAL;
    return MMFS_FAIL;
    break;
  }
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
void	BglMM_Info_s_store(d)
     BglSizHistSt* d;
{
  /*DebugPrint("/==== BglMM_Info_s_store ====/\n");*/
  if ( MM_infoSize_s == MMS_REQUESTED ||
       MM_infoSize_s == MMS_CHANGED ){
    MM_sizHistWp->d.index = d->index;
    MM_sizHistWp->d.num   = d->num;
    MM_sizHistWp->d.size  = d->size;
    MM_sizHistWp->d.count = d->count;
    if ( MM_sizHistWp->next == MM_sizHistRp ){
      MM_sizHistWp->next
	= (MM_BglSizHistList_t*)
	  tcalloc( 1, sizeof(MM_BglSizHistList_t), 52010);
      MM_sizHistWp->next->next = MM_sizHistRp;
    }
    MM_sizHistWp = MM_sizHistWp->next;
    MM_sizHistData_n++;
    MM_infoSize_s = MMS_CHANGED;
  }
  else{
    DebugPrint("/==== BglMM_Info_s_store ignored ====/\n");
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
BglSizHistSt*	BglMM_PickSizeHist()
{
  BglSizHistSt* retP = &(MM_sizHistRp->d);
  MM_sizHistRp = MM_sizHistRp->next;
  if ( MM_sizHistRp == MM_sizHistWp ) MM_infoSize_s = MMS_REQUESTED;
  MM_sizHistData_n--;
  return retP;
}


/********************************************/
/*********  for GENE HISTOGRAM Data *********/
/********************************************/
/***  GENE HISTGRAM INFORMATION access functions  ***/
/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
int	BglMM_IsInfoGene_stop()
{
  return (MM_infoGene_s == MMS_NORMAL);
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s	BglMM_InfoGene_modeReset()
{
  MM_infoGene_s = MMS_NORMAL;
  return 0; /* Normal return */
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s	BglMM_Info_g_need()
{
  MM_SequenceList_t* sP;

  BglMM_MakeProcessCell( sP = BglMM_MakeFuncCell( BglMM_NB_MenuBreak ),
			 BglMM_Info_g_openP, BglMM_Info_g_closeP,
			 BglMM_NB_Cont,
			 NULL, 0 );
  sP->next = BglMM_MakeFuncCell( BglMM_NB_MenuBreak_Wait );	sP = sP->next;
  sP->next = BglMM_MakeFuncCell( BglMM_NB_Info_g );		sP = sP->next;
  sP->next = BglMM_MakeFuncCell( BglMM_NB_Info_g_Wait );	sP = sP->next;
  sP->next = BglMM_MakeFuncCell( BglMM_SendOrder );		sP = sP->next;
  sP->next = BglMM_MakeFuncCell( BglMM_NB_Cont );

  return 1;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
static MM_funcstate_t	BglMM_Info_g_openP()
{
  MM_infoSize_s   = MMS_NORMAL;
  MM_infoGene_s   = MMS_NEED;
  MM_infoMem_s    = MMS_NORMAL;
  MM_infoEfficS_s = MMS_NORMAL;
  MM_infoEfficG_s = MMS_NORMAL;
  MM_plan_s	  = MMS_NORMAL;
  return MMFS_CONTINUE;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
static MM_funcstate_t	BglMM_Info_g_closeP()
{
  return MMFS_CONTINUE;
}


/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s	BglMM_HowManyGeneHist(){ return MM_genHistData_n; }

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s	BglMM_B_Info_g()
{
  /*DebugPrint("/==== BglMM_B_Info_g ====/\n");*/
  (void)BglMM_Info_g_openP();
  if ( BglMM_ClntSend( &BglReqInfoGen ) ) return 0; /*Communication Error*/
  if ( BglMM_B_ReplyWait( BGL_INFO_GEN_OK, BGL_INFO_GEN_REF )
       != BGL_REPLY_RET_OK )
    return 0; /* Error Return */

  (void)BglMM_SendOrder();

  MM_infoGene_s = MMS_REQUESTED;
  MM_genHistRp = MM_genHistWp;
  MM_genHistData_n = 0;
  if ( !(MM_genHistDataRoot.next) )
    MM_genHistDataRoot.next = &MM_genHistDataRoot;

  return 1;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
static MM_funcstate_t	BglMM_NB_Info_g()
{
  /*DebugPrint("/==== BglMM_NB_Info_g ====/\n");*/
  if ( !BglMM_ClntSend( &BglReqInfoGen ) ){
    /* Communication No Error */
    return MMFS_CONTINUE;
  }
  return MMFS_FAIL;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
static MM_funcstate_t	BglMM_NB_Info_g_Wait()
{
  I32s ret;
  /*DebugPrint("/==== BglMM_NB_Info_g_Wait ====/\n");*/
  ret = BglMM_NB_ReplyWait( BGL_INFO_GEN_OK, BGL_INFO_GEN_REF );
  switch( ret ){
  case BGL_REPLY_RET_OK:
    MM_infoGene_s = MMS_REQUESTED;
    MM_genHistRp = MM_genHistWp;
    MM_genHistData_n = 0;
    if ( !(MM_genHistDataRoot.next) )
      MM_genHistDataRoot.next = &MM_genHistDataRoot;
    return MMFS_CONTINUE;
    break;

  case BGL_REPLY_RET_NONE:	/* No reply data come from Tierra */
    return MMFS_AGAIN;
    break;

  default:
    (void)BglMM_SendOrder();
    MM_infoGene_s = MMS_NORMAL;
    return MMFS_FAIL;
    break;
  }
}


/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
void	BglMM_Info_g_store(d)
     BglGenHistSt* d;
{
  /*DebugPrint("/==== BglMM_Info_g_store ====/\n");*/
  if ( MM_infoGene_s == MMS_REQUESTED ||
       MM_infoGene_s == MMS_CHANGED ){
    MM_genHistWp->d.index = d->index;
    MM_genHistWp->d.size  = d->size;
    strcpy(MM_genHistWp->d.lbl, d->lbl);
    MM_genHistWp->d.count = d->count;
    if ( MM_genHistWp->next == MM_genHistRp ){
      MM_genHistWp->next
	= (MM_BglGenHistList_t*)
	  tcalloc( 1, sizeof(MM_BglGenHistList_t), 52020);
      MM_genHistWp->next->next = MM_genHistRp;
    }
    MM_genHistWp = MM_genHistWp->next;
    MM_genHistData_n++;
    MM_infoGene_s = MMS_CHANGED;
  }
  else{ DebugPrint("/==== BglMM_Info_g_store ignored ====/\n"); }
  return;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
BglGenHistSt*	BglMM_PickGeneHist()
{
  BglGenHistSt* retP = &(MM_genHistRp->d);
  MM_genHistRp = MM_genHistRp->next;
  if ( MM_genHistRp == MM_genHistWp ) MM_infoGene_s = MMS_REQUESTED;
  MM_genHistData_n--;
  return retP;
}


/*******************************************/
/*********  for MEM HISTOGRAM Data *********/
/*******************************************/
/***  MEMORY HISTGRAM INFORMATION access functions  ***/
/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
int	BglMM_IsInfoMem_stop()
{
  return (MM_infoMem_s == MMS_NORMAL);
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s	BglMM_InfoMem_modeReset()
{
  MM_infoMem_s = MMS_NORMAL;
  return 0; /* Normal return */
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s	BglMM_Info_m_need()
{
  MM_SequenceList_t* sP;

  BglMM_MakeProcessCell( sP = BglMM_MakeFuncCell( BglMM_NB_MenuBreak ),
			 BglMM_Info_m_openP, BglMM_Info_m_closeP,
			 BglMM_NB_Cont,
			 NULL, 0 );
  sP->next = BglMM_MakeFuncCell( BglMM_NB_MenuBreak_Wait );	sP = sP->next;
  sP->next = BglMM_MakeFuncCell( BglMM_NB_Info_m );		sP = sP->next;
  sP->next = BglMM_MakeFuncCell( BglMM_NB_Info_m_Wait );	sP = sP->next;
  sP->next = BglMM_MakeFuncCell( BglMM_SendOrder );		sP = sP->next;
  sP->next = BglMM_MakeFuncCell( BglMM_NB_Cont );

  return 1;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
static MM_funcstate_t	BglMM_Info_m_openP()
{
  MM_infoSize_s   = MMS_NORMAL;
  MM_infoGene_s   = MMS_NORMAL;
  MM_infoMem_s    = MMS_NEED;
  MM_infoEfficS_s = MMS_NORMAL;
  MM_infoEfficG_s = MMS_NORMAL;
  MM_plan_s	  = MMS_NORMAL;
  return MMFS_CONTINUE;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
static MM_funcstate_t	BglMM_Info_m_closeP()
{
  return MMFS_CONTINUE;
}


/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s	BglMM_HowManyMemHist(){ return MM_memHistData_n; }

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s	BglMM_B_Info_m()
{
  /*DebugPrint("/==== BglMM_B_Info_m ====/\n");*/
  (void)BglMM_Info_m_openP();
  if ( BglMM_ClntSend( &BglReqInfoMem ) ) return 0; /*Communication Error*/
  if ( BglMM_B_ReplyWait( BGL_INFO_MEM_OK, BGL_INFO_MEM_REF )
       != BGL_REPLY_RET_OK )
    return 0; /* Error Return */

  (void)BglMM_SendOrder();

  MM_infoMem_s = MMS_REQUESTED;
  MM_memHistRp = MM_memHistWp;
  MM_memHistData_n = 0;
  if ( !(MM_memHistDataRoot.next) )
    MM_memHistDataRoot.next = &MM_memHistDataRoot;
  return 1;
}


/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
static MM_funcstate_t	BglMM_NB_Info_m()
{
  /*DebugPrint("/==== BglMM_NB_Info_m ====/\n");*/
  if ( !BglMM_ClntSend( &BglReqInfoMem ) ){
    /* Communication No Error */
    return MMFS_CONTINUE;
  }
  return MMFS_FAIL;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
static MM_funcstate_t	BglMM_NB_Info_m_Wait()
{
  I32s ret;
  /*DebugPrint("/==== BglMM_NB_Info_m_Wait ====/\n");*/
  ret = BglMM_NB_ReplyWait( BGL_INFO_MEM_OK, BGL_INFO_MEM_REF );
  switch( ret ){
  case BGL_REPLY_RET_OK:
    MM_infoMem_s = MMS_REQUESTED;
    MM_memHistRp = MM_memHistWp;
    MM_memHistData_n = 0;
    if ( !(MM_memHistDataRoot.next) )
      MM_memHistDataRoot.next = &MM_memHistDataRoot;
    return MMFS_CONTINUE;
    break;

  case BGL_REPLY_RET_NONE:	/* No reply data come from Tierra */
    return MMFS_AGAIN;
    break;

  default:
    (void)BglMM_SendOrder();
    MM_infoMem_s = MMS_NORMAL;
    return MMFS_FAIL;
    break;
  }
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
void	BglMM_Info_m_store(d)
     BglMemHistSt* d;
{
  /*DebugPrint("/==== BglMM_Info_m_store ====/\n");*/
  if ( MM_infoMem_s == MMS_REQUESTED ||
       MM_infoMem_s == MMS_CHANGED ){
    MM_memHistWp->d.index = d->index;
    MM_memHistWp->d.size  = d->size;
    MM_memHistWp->d.num   = d->num;
    MM_memHistWp->d.count = d->count;
    if ( MM_memHistWp->next == MM_memHistRp ){
      MM_memHistWp->next
	= (MM_BglMemHistList_t*)
	  tcalloc( 1, sizeof(MM_BglMemHistList_t), 52030);
      MM_memHistWp->next->next = MM_memHistRp;
    }
    MM_memHistWp = MM_memHistWp->next;
    MM_memHistData_n++;
    MM_infoMem_s = MMS_CHANGED;
  }
  else{ DebugPrint("/==== BglMM_Info_m_store ignored ====/\n"); }
  return;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
BglMemHistSt*	BglMM_PickMemHist()
{
  BglMemHistSt* retP = &(MM_memHistRp->d);
  MM_memHistRp = MM_memHistRp->next;
  if ( MM_memHistRp == MM_memHistWp ) MM_infoMem_s = MMS_REQUESTED;
  MM_memHistData_n--;
  return retP;
}

/*******************************************************/
/*********  for REPROD EFF SIZE HISTOGRAM Data *********/
/*******************************************************/
/***  REPORD_EFF (SIZE)  HISTGRAM INFORMATION access functions  ***/
/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
int	BglMM_IsInfoES_stop()
{
  return (MM_infoEfficS_s == MMS_NORMAL);
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s	BglMM_InfoES_modeReset()
{
  MM_infoEfficS_s = MMS_NORMAL;
  return 0; /* Normal return */
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s	BglMM_Info_es_need()
{
  MM_SequenceList_t* sP;

  BglMM_MakeProcessCell( sP = BglMM_MakeFuncCell( BglMM_NB_MenuBreak ),
			 BglMM_Info_es_openP, BglMM_Info_es_closeP,
			 BglMM_NB_Cont,
			 NULL, 0 );
  sP->next = BglMM_MakeFuncCell( BglMM_NB_MenuBreak_Wait );	sP = sP->next;
  sP->next = BglMM_MakeFuncCell( BglMM_NB_Info_es );		sP = sP->next;
  sP->next = BglMM_MakeFuncCell( BglMM_NB_Info_es_Wait );	sP = sP->next;
  sP->next = BglMM_MakeFuncCell( BglMM_SendOrder );		sP = sP->next;
  sP->next = BglMM_MakeFuncCell( BglMM_NB_Cont );

  return 1;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
static MM_funcstate_t	BglMM_Info_es_openP()
{
  MM_infoSize_s   = MMS_NORMAL;
  MM_infoGene_s   = MMS_NORMAL;
  MM_infoMem_s    = MMS_NORMAL;
  MM_infoEfficS_s = MMS_NEED;
  MM_infoEfficG_s = MMS_NORMAL;
  MM_plan_s	  = MMS_NORMAL;
  return MMFS_CONTINUE;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
static MM_funcstate_t	BglMM_Info_es_closeP()
{
  return MMFS_CONTINUE;
}


/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s	BglMM_HowManyEfficSHist(){ return MM_efficSHistData_n; }

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s	BglMM_B_Info_es()
{
  /*DebugPrint("/==== BglMM_B_Info_es ====/\n");*/
  (void)BglMM_Info_es_openP();
  if ( BglMM_ClntSend( &BglReqInfoEfficS ) )return 0; /*Communication Error*/
  if ( BglMM_B_ReplyWait( BGL_INFO_EFFICS_OK, BGL_INFO_EFFICS_REF )
       != BGL_REPLY_RET_OK )
    return 0; /* Error Return */

  (void)BglMM_SendOrder();

  MM_infoEfficS_s = MMS_REQUESTED;
  MM_efficSHistRp = MM_efficSHistWp;
  MM_efficSHistData_n = 0;
  if ( !(MM_efficSHistDataRoot.next) )
    MM_efficSHistDataRoot.next = &MM_efficSHistDataRoot;
  return 1;
}


/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
static MM_funcstate_t	BglMM_NB_Info_es()
{
  /*DebugPrint("/==== BglMM_NB_Info_es ====/\n");*/
  if ( !BglMM_ClntSend( &BglReqInfoEfficS ) ){
    /* Communication No Error */
    return MMFS_CONTINUE;
  }
  return MMFS_FAIL;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
static MM_funcstate_t	BglMM_NB_Info_es_Wait()
{
  I32s ret;
  /*DebugPrint("/==== BglMM_NB_Info_es_Wait ====/\n");*/
  ret = BglMM_NB_ReplyWait( BGL_INFO_EFFICS_OK, BGL_INFO_EFFICS_REF );
  switch( ret ){
  case BGL_REPLY_RET_OK:
    MM_infoEfficS_s = MMS_REQUESTED;
    MM_efficSHistRp = MM_efficSHistWp;
    MM_efficSHistData_n = 0;
    if ( !(MM_efficSHistDataRoot.next) )
      MM_efficSHistDataRoot.next = &MM_efficSHistDataRoot;
    return MMFS_CONTINUE;
    break;

  case BGL_REPLY_RET_NONE:	/* No reply data come from Tierra */
    return MMFS_AGAIN;
    break;

  default:
    (void)BglMM_SendOrder();
    MM_infoEfficS_s = MMS_NORMAL;
    return MMFS_FAIL;
    break;
  }
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
void	BglMM_Info_es_store(d)
     BglEfficSHistSt* d;
{
  /*DebugPrint("/==== BglMM_Info_es_store ====/\n");*/
  if ( MM_infoEfficS_s == MMS_REQUESTED ||
       MM_infoEfficS_s == MMS_CHANGED ){
    MM_efficSHistWp->d.index = d->index;
    MM_efficSHistWp->d.size  = d->size;
    MM_efficSHistWp->d.num   = d->num;
    MM_efficSHistWp->d.dblcount = d->dblcount;
    if ( MM_efficSHistWp->next == MM_efficSHistRp ){
      MM_efficSHistWp->next
	= (MM_BglEfficSHistList_t*)
	  tcalloc( 1, sizeof(MM_BglEfficSHistList_t), 52040);
      MM_efficSHistWp->next->next = MM_efficSHistRp;
    }
    MM_efficSHistWp = MM_efficSHistWp->next;
    MM_efficSHistData_n++;
    MM_infoEfficS_s = MMS_CHANGED;
  }
  else{ DebugPrint("/==== BglMM_Info_es_store ignored ====/\n"); }
  return;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
BglEfficSHistSt*	BglMM_PickEfficSHist()
{
  BglEfficSHistSt* retP = &(MM_efficSHistRp->d);
  MM_efficSHistRp = MM_efficSHistRp->next;
  if ( MM_efficSHistRp == MM_efficSHistWp ) MM_infoEfficS_s = MMS_REQUESTED;
  MM_efficSHistData_n--;
  return retP;
}

/*******************************************************/
/*********  for REPROD EFF GENE HISTOGRAM Data *********/
/*******************************************************/
/***  REPORD_EFF (GENE)  HISTGRAM INFORMATION access functions  ***/
/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
int	BglMM_IsInfoEG_stop()
{
  return (MM_infoEfficG_s == MMS_NORMAL);
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s	BglMM_InfoEG_modeReset()
{
  MM_infoEfficG_s = MMS_NORMAL;
  return 0; /* Normal return */
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s	BglMM_Info_eg_need()
{
  MM_SequenceList_t* sP;

  BglMM_MakeProcessCell( sP = BglMM_MakeFuncCell( BglMM_NB_MenuBreak ),
			 BglMM_Info_eg_openP, BglMM_Info_eg_closeP,
			 BglMM_NB_Cont,
			 NULL, 0 );
  sP->next = BglMM_MakeFuncCell( BglMM_NB_MenuBreak_Wait );	sP = sP->next;
  sP->next = BglMM_MakeFuncCell( BglMM_NB_Info_eg );		sP = sP->next;
  sP->next = BglMM_MakeFuncCell( BglMM_NB_Info_eg_Wait );	sP = sP->next;
  sP->next = BglMM_MakeFuncCell( BglMM_SendOrder );		sP = sP->next;
  sP->next = BglMM_MakeFuncCell( BglMM_NB_Cont );

  return 1;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
static MM_funcstate_t	BglMM_Info_eg_openP()
{
  MM_infoSize_s   = MMS_NORMAL;
  MM_infoGene_s   = MMS_NORMAL;
  MM_infoMem_s    = MMS_NORMAL;
  MM_infoEfficS_s = MMS_NORMAL;
  MM_infoEfficG_s = MMS_NEED;
  MM_plan_s	  = MMS_NORMAL;
  return MMFS_CONTINUE;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
static MM_funcstate_t	BglMM_Info_eg_closeP()
{
  return MMFS_CONTINUE;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s	BglMM_HowManyEfficGHist(){ return MM_efficGHistData_n; }

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s	BglMM_B_Info_eg()
{
  /*DebugPrint("/==== BglMM_B_Info_eg ====/\n");*/
  (void)BglMM_Info_eg_openP();
  if ( BglMM_ClntSend( &BglReqInfoEfficG ) )return 0; /*Communication Error*/
  if ( BglMM_B_ReplyWait( BGL_INFO_EFFICG_OK, BGL_INFO_EFFICG_REF )
       != BGL_REPLY_RET_OK )
    return 0; /* Error Return */

  (void)BglMM_SendOrder();

  MM_infoEfficG_s = MMS_REQUESTED;
  MM_efficGHistRp = MM_efficGHistWp;
  MM_efficGHistData_n = 0;
  if ( !(MM_efficGHistDataRoot.next) )
    MM_efficGHistDataRoot.next = &MM_efficGHistDataRoot;
  return 1;
}


/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
static MM_funcstate_t	BglMM_NB_Info_eg()
{
  /*DebugPrint("/==== BglMM_NB_Info_eg ====/\n");*/
  if ( !BglMM_ClntSend( &BglReqInfoEfficG ) ){
    /* Communication No Error */
    return MMFS_CONTINUE;
  }
  return MMFS_FAIL;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
static MM_funcstate_t	BglMM_NB_Info_eg_Wait()
{
  I32s ret;
  /*DebugPrint("/==== BglMM_NB_Info_eg_Wait ====/\n");*/
  ret = BglMM_NB_ReplyWait( BGL_INFO_EFFICG_OK, BGL_INFO_EFFICG_REF );
  switch( ret ){
  case BGL_REPLY_RET_OK:
    MM_infoEfficG_s = MMS_REQUESTED;
    MM_efficGHistRp = MM_efficGHistWp;
    MM_efficGHistData_n = 0;
    if ( !(MM_efficGHistDataRoot.next) )
      MM_efficGHistDataRoot.next = &MM_efficGHistDataRoot;
    return MMFS_CONTINUE;
    break;

  case BGL_REPLY_RET_NONE:	/* No reply data come from Tierra */
    return MMFS_AGAIN;
    break;

  default:
    (void)BglMM_SendOrder();
    MM_infoEfficG_s = MMS_NORMAL;
    return MMFS_FAIL;
    break;
  }
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
void	BglMM_Info_eg_store(d)
     BglEfficGHistSt* d;
{
  /*DebugPrint("/==== BglMM_Info_e_store ====/\n");*/
  if ( MM_infoEfficG_s == MMS_REQUESTED ||
       MM_infoEfficG_s == MMS_CHANGED ){
    MM_efficGHistWp->d.index  = d->index;
    MM_efficGHistWp->d.size  = d->size;
    strcpy(MM_efficGHistWp->d.lbl, d->lbl);
    MM_efficGHistWp->d.dblcount = d->dblcount;
    if ( MM_efficGHistWp->next == MM_efficGHistRp ){
      MM_efficGHistWp->next
	= (MM_BglEfficGHistList_t*)
	  tcalloc( 1, sizeof(MM_BglEfficGHistList_t), 52050);
      MM_efficGHistWp->next->next = MM_efficGHistRp;
    }
    MM_efficGHistWp = MM_efficGHistWp->next;
    MM_efficGHistData_n++;
    MM_infoEfficG_s = MMS_CHANGED;
  }
  else{ DebugPrint("/==== BglMM_Info_eg_store ignored ====/\n"); }
  return;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
BglEfficGHistSt*	BglMM_PickEfficGHist()
{
  BglEfficGHistSt* retP = &(MM_efficGHistRp->d);
  MM_efficGHistRp = MM_efficGHistRp->next;
  if ( MM_efficGHistRp == MM_efficGHistWp ) MM_infoEfficG_s = MMS_REQUESTED;
  MM_efficGHistData_n--;
  return retP;
}

/******************************************/
/*********  for BGL_PLAN Data *************/
/******************************************/
/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s	BglMM_Info_stop()
{
  if ( MM_infoSize_s   != MMS_NORMAL ||
       MM_infoGene_s   != MMS_NORMAL ||
       MM_infoMem_s    != MMS_NORMAL ||
       MM_infoEfficS_s != MMS_NORMAL ||
       MM_infoEfficG_s != MMS_NORMAL ){

    (void)BglMM_Plan_need_function();

    return 1;
  }
  else return 0;
}

/***  BGL_PLAN INFORMATION access functions  ***/
/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
int	BglMM_IsPlan_stop()
{
  return (MM_plan_s == MMS_NORMAL);
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s	BglMM_Plan_modeReset()
{
  MM_plan_s = MMS_NORMAL;
  return 0; /* Normal return */
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s	BglMM_Plan_need()
{
  if ( MM_plan_s == MMS_NORMAL ){

    if ( MM_infoSize_s != MMS_NORMAL ||
	 MM_infoGene_s != MMS_NORMAL ||
	 MM_infoMem_s  != MMS_NORMAL ||
	 MM_infoEfficS_s != MMS_NORMAL ||
	 MM_infoEfficG_s != MMS_NORMAL ){
      BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
		"Graph update will be stop. ");
    }

    (void)BglMM_Plan_need_function();

    return 1;
  }
  else return 0;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
static I32s	BglMM_Plan_need_function()
{
  MM_SequenceList_t* sP;

  MM_plan_s = MMS_NEED;
  MM_infoSize_s = MMS_NORMAL;
  MM_infoGene_s = MMS_NORMAL;
  MM_infoMem_s  = MMS_NORMAL;
  MM_infoEfficS_s = MMS_NORMAL;
  MM_infoEfficG_s = MMS_NORMAL;

  BglMM_MakeProcessCell( sP = BglMM_MakeFuncCell( BglMM_NB_MenuBreak ),
			 NULL, NULL,
			 BglMM_NB_Cont,
			 NULL, 0 );
  sP->next = BglMM_MakeFuncCell( BglMM_NB_MenuBreak_Wait );	sP = sP->next;
  sP->next = BglMM_MakeFuncCell( BglMM_NB_Info_plan );		sP = sP->next;
  sP->next = BglMM_MakeFuncCell( BglMM_NB_Info_plan_Wait );	sP = sP->next;
  sP->next = BglMM_MakeFuncCell( BglMM_NB_Cont );

  return 1;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s	BglMM_Plan_stop()
{
  BglMM_Plan_modeReset();
  return 0; /* Normal return */
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s	BglMM_HowManyPlan()
{
  I32s ret;
  ret = MM_planWi - MM_planRi;
  if ( ret < 0 ) ret += MM_PLAN_MAX;
  return(ret);
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s	BglMM_B_Info_plan()
{
  /*DebugPrint("/==== BglMM_B_Info_plan ====/\n");*/
  if ( BglMM_ClntSend( &BglReqInfoPlan ) ) return 0; /*Communication Error*/
  if ( BglMM_B_ReplyWait( BGL_INFO_PLAN_OK, BGL_INFO_PLAN_REF )
       != BGL_REPLY_RET_OK )
    return 0; /* Communication Error */

  MM_plan_s = MMS_REQUESTED;
  MM_planRi = MM_planWi = 0;
  return 1;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
static MM_funcstate_t	BglMM_NB_Info_plan()
{
  /*DebugPrint("/==== BglMM_NB_Info_plan ====/\n");*/
  if ( !BglMM_ClntSend( &BglReqInfoPlan ) ){
    /* Communication No Error */
    return MMFS_CONTINUE;
  }
  return MMFS_FAIL;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
static MM_funcstate_t	BglMM_NB_Info_plan_Wait()
{
  I32s ret;
  /*DebugPrint("/==== BglMM_NB_Info_plan_Wait ====/\n");*/
  ret = BglMM_NB_ReplyWait( BGL_INFO_PLAN_OK, BGL_INFO_PLAN_REF );
  switch( ret ){
  case BGL_REPLY_RET_OK:
    MM_plan_s = MMS_REQUESTED;
    MM_planRi = MM_planWi = 0;
    return  MMFS_CONTINUE;
    break;

  case BGL_REPLY_RET_NONE:	/* No reply data come from Tierra */
    return MMFS_AGAIN;
    break;

  default:
    MM_plan_s = MMS_NORMAL;
    return MMFS_FAIL;
    break;
  }
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
void	BglMM_Plan_store(d)
     BglPlanSt* d;
{
  /*DebugPrint("/==== BglMM_Plan_store ====/\n");*/
  MM_planData[MM_planWi].GeneBnker = d->GeneBnker;
  MM_planData[MM_planWi].InstExe_m = d->InstExe_m;
  MM_planData[MM_planWi].Generations = d->Generations;
  MM_planData[MM_planWi].tp = d->tp;
  MM_planData[MM_planWi].NumCells = d->NumCells;
  MM_planData[MM_planWi].NumGenotypes = d->NumGenotypes;
  MM_planData[MM_planWi].NumSizes = d->NumSizes;
  MM_planData[MM_planWi].Speed = d->Speed;
  MM_planData[MM_planWi].RateMut = d->RateMut;
  MM_planData[MM_planWi].RateMovMut = d->RateMovMut;
  MM_planData[MM_planWi].RateFlaw = d->RateFlaw;
  MM_planData[MM_planWi].AverageSize = d->AverageSize;
  MM_planData[MM_planWi].AvgPop = d->AvgPop;
  MM_planData[MM_planWi].FecundityAvg = d->FecundityAvg;
  MM_planData[MM_planWi].RepInstEff = d->RepInstEff;
  MM_planData[MM_planWi].FEBirthNum = d->FEBirthNum;
  MM_planData[MM_planWi].FEBirthLocal = d->FEBirthLocal;
  MM_planData[MM_planWi].FEBirthInject = d->FEBirthInject;
  MM_planData[MM_planWi].FEBirthEject = d->FEBirthEject;
  MM_planData[MM_planWi].FEDeathNum = d->FEDeathNum;
  MM_planData[MM_planWi].FEFecundYesSum = d->FEFecundYesSum;
  MM_planData[MM_planWi].FEFecunZeroSum = d->FEFecunZeroSum;
  MM_planData[MM_planWi].FEFecunEjecSum = d->FEFecunEjecSum;
  MM_planData[MM_planWi].MaxPop = d->MaxPop;
  MM_planData[MM_planWi].MaxGenPop_size = d->MaxGenPop_size;
  strcpy(MM_planData[MM_planWi].MaxGenPop_label , d->MaxGenPop_label);
  MM_planData[MM_planWi].MaxMemry = d->MaxMemry;
  MM_planData[MM_planWi].MaxGenMem_size = d->MaxGenMem_size;
  strcpy(MM_planData[MM_planWi].MaxGenMem_label , d->MaxGenMem_label);
  MM_planData[MM_planWi].NumGenDG = d->NumGenDG;
  MM_planData[MM_planWi].NetFlg = d->NetFlg;
  MM_planData[MM_planWi].NumNodes = d->NumNodes;
  MM_planData[MM_planWi].FESImmigraNum = d->FESImmigraNum;
  MM_planData[MM_planWi].FERImmigraNum = d->FERImmigraNum;
  MM_planData[MM_planWi].FEEjectToSelf = d->FEEjectToSelf;
  MM_planData[MM_planWi].FESPingReqNum = d->FESPingReqNum;
  MM_planData[MM_planWi].FESPingRepNum = d->FESPingRepNum;
  MM_planData[MM_planWi].FESSendAddNum = d->FESSendAddNum;
  MM_planData[MM_planWi].FESByeNum = d->FESByeNum;
  MM_planData[MM_planWi].FERPingReqNum = d->FERPingReqNum;
  MM_planData[MM_planWi].FERPingRepNum = d->FERPingRepNum;
  MM_planData[MM_planWi].FERSendAddNum = d->FERSendAddNum;
  MM_planData[MM_planWi].FERByeNum = d->FERByeNum;
  MM_planData[MM_planWi].MemProFlg = d->MemProFlg;
  MM_planData[MM_planWi].DynMemUse = d->DynMemUse;
  MM_planData[MM_planWi].TotNumAloc = d->TotNumAloc;

  MM_plan_s = MMS_CHANGED;
  MM_planWi++; if( MM_planWi==MM_PLAN_MAX ) MM_planWi=0;

  return;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
BglPlanSt*	BglMM_PickPlan()
{
  int ret_i;
  ret_i = MM_planRi;
  MM_planRi++;
  if( MM_planRi==MM_PLAN_MAX ) MM_planRi=0;
  if( MM_planRi==MM_planWi ) MM_plan_s = MMS_REQUESTED;
  return &(MM_planData[ret_i]);
}
