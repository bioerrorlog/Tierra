/* beagleUTIL_OVMgr.c -- -*- C -*-
 * 
 * This work has been done at ATR HIP
 * 
 * Author          : YOSHIKAWA Tooru
 * Created On      : Tue Dec 24 11:14:02 1996
 * Last Modified By: YOSHIKAWA Tooru
 * Last Modified On: Thu Mar 18 18:07:27 1999
 * Update Count    : 41
 * Status          : 
 */

#include "clnt.h"
#include "beagleUTIL_OVMgr.h"


/*
 *  This file is include utility functions for Beagle
 *  about Overviews data management.
 *
 *                             [data]
 *
 *                              +--------+
 *                              | next   | ----> point to nex data
 *                              +--------+
 *  point to previous data <--- | prev   |
 *                              +--------+
 *                              | start  |
 *                              +--------+
 *                              | length |
 *                              +--------+
 */


/*
 *- SPECIAL VALIABLE NAME/TYPE
 *- ===========================
 */


/*
 *- HERE ARE EXTERN FUNCTION DEFINEs.
 *- ================================
 */


/*
 *- HERE ARE GLOBAL VARIABLE (EFFECTIVE IN THIS FILE ONLY) DEFINEs.
 *- ===============================================================
 *- So another files functions cannot search these.
 *- I want to avoid from linking trouble.
 */
static OvData_t*	ovDataRootP = (OvData_t*)0;
static OvData_t*	ovDataLastP = (OvData_t*)0;

/*
 *- HERE ARE STATIC FUNCTIONs PROTOTYPE DEFINEs.
 *- ============================================
 *- These functions call from in this file only.
 *- So another files functions cannot search these.
 *- I want to avoid from linking trouble.
 */


/*
 *- HERE ARE SERVICE FUNCTIONs FOR CALL FROM ANOTHER FILEs.
 *- =====================================================================
 *- I wrote prototype define in 'beagleUTIL_SockMgr.h'.
 */
int	UTIL_Ov_AddData(start, length)
     I32u start;
     I32u length;
{
  OvData_t* p;

  if ( (p = (OvData_t*)tcalloc(1,sizeof(OvData_t),50000)) != 0 ){
    p->start  = start;
    p->length = length;

    if ( ovDataLastP ){
      ovDataLastP->next = p;
      p->prev = ovDataLastP;
      p->next = (OvData_t*)0;
      ovDataLastP = p;
    }
    else{
      ovDataRootP = ovDataLastP = p;
      p->prev = p->next = (OvData_t*)0;
    }
    return 0;
  }
  return -1;
}


int	UTIL_Ov_DelThis(p)
     OvData_t* p;
{
  if (!p){
    return -1;
  }

  if ( p->prev ){
    if ( (p->prev->next = p->next) != 0 ){
      p->next->prev = p->prev;
    }
    else{
      ovDataLastP = p->prev;
    }
  }
  else{
    if ( (ovDataRootP = p->next) != 0 ){
      p->next->prev = (OvData_t*)0;
    }
    else{
      ovDataLastP = p->prev;
    }
  }
  tfree(p,sizeof(OvData_t),50000);

  return 0;
}

OvData_t*	UTIL_Ov_SearchByStart(start)
     I32u start;
{
  OvData_t* p = ovDataRootP;

  while(p){
    if ( p->start == start )	return p;
    p = p->next;
  }

  return 0;
}

OvData_t*	UTIL_Ov_SearchByLength(length)
     I32u length;
{
  OvData_t* p = ovDataRootP;

  while(p){
    if ( p->length == length )	return p;
    p = p->next;
  }

  return 0;
}

OvData_t*	UTIL_Ov_SearchByPoint(point)
     I32u point;
{
  OvData_t* p = ovDataRootP;

  while(p){
    if ( ( p->start <= point ) &&
	 ( point < (p->start + p->length) ) ) return p;
    p = p->next;
  }

  return 0;
}

int	UTIL_Ov_ClearAll()
{
  if ( ! ovDataRootP )	return 0;

  while( ovDataRootP != ovDataLastP ){ /* ther are 2 or more data */
    ovDataLastP = ovDataLastP->prev;
    tfree(ovDataLastP->next,sizeof(OvData_t),50000);
    /* ovDataLast->next = (OvData_t*)0; */ /* <- more politely but need not. */
  }
  tfree(ovDataLastP,sizeof(OvData_t),50000);
  ovDataRootP = ovDataLastP = (OvData_t*)0;

  return 0;
}


OvData_t*	UTIL_Ov_GetRoot(){	return ovDataRootP;	}

/*
 *- HERE ARE STATIC FUNCTIONs DEFINITION FOR CALL FROM IN THIS FILE ONLY.
 *- =====================================================================
 *- So another files functions cannot search these.
 *- I want to avoid from linking trouble.
 */
