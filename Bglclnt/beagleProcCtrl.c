/* beagleProcCtrl.c -- -*- C -*-
 * 
 * This work has been done at ATR HIP
 * 
 * Author          : Marc Chaland
 * Created On      : Wed Aug 20 10:34:32 1997
 * Last Modified By: YOSHIKAWA Tooru
 * Last Modified On: Thu Nov 12 10:31:53 1998
 * Update Count    : 41
 * Status          : Process Manager module
 *                   perfectly work
 */

#ifndef lint
static char* sccsProgramId="@(#)beagleProcCtrl.c	1.11	11/12/98 10:32:12";
#endif /* lint */

#include "clnt.h"
#include "beagleProcCtrl.h"
#include "beagleMessageMgrP.h"

#define MAX_CATCH_ARRAY 20
struct Catch_st
{
  MM_state_t*	variable;
  MM_state_t	value;
  void		(*func)();
};
typedef struct Catch_st Catch_St;
static Catch_St	CatchArray[MAX_CATCH_ARRAY];
static int	CatchArraySize = 0;

static MM_ProcessList_t  MM_ProcessDataRoot;
static MM_ProcessList_t* MM_ProcessDataWp = &MM_ProcessDataRoot;
static MM_ProcessList_t* MM_ProcessDataRp = &MM_ProcessDataRoot;


void BglMM_ProcessListReset()
{
  MM_ProcessList_t*  pP;
  MM_SequenceList_t* sP;

  if ( !(MM_ProcessDataRoot.next) ){
    MM_ProcessDataRoot.next = &MM_ProcessDataRoot;
  }
  else{
    while(1){
      pP = MM_ProcessDataRoot.next;
      /* clear the Sequens Data and area free */
      while(sP = pP->seqP){
	pP->seqP = sP->next;
	tfree( sP, sizeof(MM_SequenceList_t), 11002 );
      }
      if ( pP != &MM_ProcessDataRoot ){
	/* If it's not Root then it area free and toward next data */
	MM_ProcessDataRoot.next = pP->next;
	tfree( pP, sizeof(MM_ProcessList_t), 11001 );
      }
      else{
	break;
      }
    }
    MM_ProcessDataWp = MM_ProcessDataRp = &MM_ProcessDataRoot;
  }
  return;
}

void BglMM_ProcessOneAction()
{
  MM_funcstate_t     functionstate = MMFS_CONTINUE;
  MM_SequenceList_t *sP            = NULL;

/* Beagle execute one sequence, and skip it */

  if ( MM_ProcessDataWp != MM_ProcessDataRp ){
    if (MM_ProcessDataRp->startP){
      /************************************************
       * Start process
       */
      functionstate = MM_ProcessDataRp->startP();

      switch (functionstate){
      case MMFS_AGAIN :       /* execution of this function once again */
	break;
      case MMFS_CONTINUE :    /* execute the first sequence */
	MM_ProcessDataRp->startP = NULL;
	break;
      case MMFS_FAIL :        /* execute the abnormterm sequence */
	tsprintf(bglStrBuf2, "Last Action Failed");
	while(sP = MM_ProcessDataRp->seqP){
	  MM_ProcessDataRp->seqP = sP->next;
	  tfree(sP, sizeof(MM_SequenceList_t), 11002 );
	}
	MM_ProcessDataRp->startP    = NULL;
	MM_ProcessDataRp->normtermP = NULL;
	break;
      }
    }
    else if (MM_ProcessDataRp->seqP){
      /************************************************
       * execute one middle instruction for the process
       */
      functionstate = MM_ProcessDataRp->seqP->FuncP();

      switch (functionstate){
      case MMFS_AGAIN :       /* execution of this function once again */
	break;
      case MMFS_CONTINUE :    /* next sequence */

	sP = MM_ProcessDataRp->seqP;
	MM_ProcessDataRp->seqP = sP->next;
	tfree(sP, sizeof(MM_SequenceList_t), 11002 );
	if (!MM_ProcessDataRp->seqP)/* if process finished, delete abnormterm */
	  MM_ProcessDataRp->abnormtermP = NULL;
	break;

      case MMFS_FAIL :        /* execute the normterm sequence */
	tsprintf(bglStrBuf2, "Last Action Failed");
	while(sP = MM_ProcessDataRp->seqP){
	  MM_ProcessDataRp->seqP = sP->next;
	  tfree(sP, sizeof(MM_SequenceList_t), 11002 );
	}
	MM_ProcessDataRp->normtermP = NULL;
	break;
      }
    }
    else if (MM_ProcessDataRp->normtermP){
      /************************************************
       * execute function for closing process
       */
      functionstate = MM_ProcessDataRp->normtermP();

      switch (functionstate){
      case MMFS_AGAIN :       /* execution of this function once again */
	break;
      case MMFS_FAIL :
	tsprintf(bglStrBuf2, "Last Action Failed");
      case MMFS_CONTINUE :    /* end of this process */
	MM_ProcessDataRp->normtermP   = NULL;
	MM_ProcessDataRp->abnormtermP = NULL;
	break;
      }
    }
    else if (MM_ProcessDataRp->abnormtermP){
      /************************************************
       * execute function for closing process after error
       */
      functionstate = MM_ProcessDataRp->abnormtermP();

      switch (functionstate){
      case MMFS_AGAIN :       /* execution of this function once again */
	break;
      case MMFS_FAIL :
	tsprintf(bglStrBuf2, "Last Action Failed");
      case MMFS_CONTINUE :    /* end of this process */
	MM_ProcessDataRp->abnormtermP = NULL;
	break;
      }
    }
    else{
      /************************************************
       * All Sequence Done
       */
      MM_ProcessDataRp->numparams	= 0;
      MM_ProcessDataRp = MM_ProcessDataRp->next;
    }
  }  /*  END of if (MM_ProcessDataWp != MM_ProcessDataRp) block  */
}

I32s	BglMM_MakeProcessCell(seqP, startP, normtermP, abnormtermP,
			      parameters, numparams)
     MM_SequenceList_t* seqP;
     MM_funcstate_t   (*startP)();
     MM_funcstate_t   (*normtermP)();
     MM_funcstate_t   (*abnormtermP)();
     I8s** parameters;
     int  numparams;
{
  int i;
  
  MM_ProcessDataWp->seqP        = seqP;
  MM_ProcessDataWp->startP      = startP;
  MM_ProcessDataWp->normtermP   = normtermP;
  MM_ProcessDataWp->abnormtermP = abnormtermP;
  if (numparams && parameters)
    for ( i = 0 ; (i < MAX_PROCESS_PARAM) && (i < numparams) ; i++)
      strcpy(MM_ProcessDataWp->parameters[i], parameters[i]);
  MM_ProcessDataWp->numparams	= numparams;
  
  if ( MM_ProcessDataWp->next == MM_ProcessDataRp ){
    MM_ProcessDataWp->next
      = (MM_ProcessList_t*)tcalloc( 1, sizeof(MM_ProcessList_t), 11001 );
    MM_ProcessDataWp = MM_ProcessDataWp->next;
    MM_ProcessDataWp->next        = MM_ProcessDataRp;
    MM_ProcessDataWp->seqP        = (MM_SequenceList_t*)0;
    MM_ProcessDataWp->startP      = NULL;
    MM_ProcessDataWp->normtermP   = NULL;
    MM_ProcessDataWp->abnormtermP = NULL;
    MM_ProcessDataWp->numparams	  = 0;
  }
  else{
    MM_ProcessDataWp = MM_ProcessDataWp->next;
  }
  return 0;
}

MM_SequenceList_t*	BglMM_MakeFuncCell(funcP)
     MM_funcstate_t (*funcP)();
{
  MM_SequenceList_t* sP;
  sP = (MM_SequenceList_t*)tcalloc( 1, sizeof(MM_SequenceList_t), 11002 );
  sP->next = (MM_SequenceList_t*)0;
  sP->FuncP = funcP;
  return sP;
}

I8s*	BglMM_GetProcessParam(index)
     int index;
{
  if (index < MM_ProcessDataRp->numparams)
    return MM_ProcessDataRp->parameters[index];
  else
    return NULL;
}

void	BglMM_Catch(var, value, func)
     MM_state_t* var;
     MM_state_t value;
     void (*func)();
{
  int i;
  
  for (i = 0 ; (i < CatchArraySize) && CatchArray[i].variable ; i++);

  if (i < MAX_CATCH_ARRAY)
    {
      CatchArray[i].variable	= var;
      CatchArray[i].value	= value;
      CatchArray[i].func	= func;
      if (i >= CatchArraySize)
	CatchArraySize = i + 1;
    }
}

void	BglMM_CheckCatch()
{
  int i;
  
  for ( i = 0 ; i < CatchArraySize ; i++)
    if ( (CatchArray[i].variable) &&
	 (*(CatchArray[i].variable) == CatchArray[i].value))
      {
	(CatchArray[i].func)();
	CatchArray[i].variable = NULL;
      }
}
