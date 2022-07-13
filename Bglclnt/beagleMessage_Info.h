/* beagleMessage_Info.h -- -*- C -*-
 * 
 * @(#)beagleMessage_Info.h	1.4	10/30/97 16:36:47
 *
 * This work has been done at ATR HIP
 * 
 * Author          : Marc Chaland
 * Created On      : Tue Sep 30 09:40:16 1997
 * Last Modified By: Marc Chaland
 * Last Modified On: Wed Oct 29 18:11:31 1997
 * Update Count    : 10
 * Status          : Unknown, Use with caution!
 */

#ifndef beagleMessage_Info_h
#define beagleMessage_Info_h

#include "clnt.h"

/******************************/
/***  HISTGRAM Information  ***/
/******************************/

extern I32s	BglMM_Info_RequireSortStyle	P_((void));
extern I32s	BglMM_Info_SortByFreq		P_((void));
extern I32s	BglMM_Info_SortByNotFreq	P_((void));
extern I32s	BglMM_Info_RequireDought	P_((void));
extern I32s	BglMM_Info_Daught1		P_((void));
extern I32s	BglMM_Info_Daught2		P_((void));


/***  SIZE HISTGRAM INFORMATION access functions  ***/
extern I32s	BglMM_Info_s_need		P_((void));
extern I32s	BglMM_B_Info_s			P_((void));
extern I32s	BglMM_HowManySizeHist		P_((void));
extern BglSizHistSt*	BglMM_PickSizeHist	P_((void));
extern void	BglMM_Info_s_store		P_((BglSizHistSt*));

/***  GENE HISTGRAM INFORMATION access functions  ***/
extern I32s	BglMM_Info_g_need		P_((void));
extern I32s	BglMM_B_Info_g			P_((void));
extern I32s	BglMM_HowManyGeneHist		P_((void));
extern BglGenHistSt*	BglMM_PickGeneHist	P_((void));
extern void	BglMM_Info_g_store		P_((BglGenHistSt*));

/***  MEMORY HISTGRAM INFORMATION access functions  ***/
extern I32s	BglMM_Info_m_need		P_((void));
extern I32s	BglMM_B_Info_m			P_((void));
extern I32s	BglMM_HowManyMemHist		P_((void));
extern BglMemHistSt*	BglMM_PickMemHist	P_((void));
extern void	BglMM_Info_m_store		P_((BglMemHistSt*));

/***  REPROD_EFF (Size) HISTGRAM INFORMATION access functions  ***/
extern I32s	BglMM_Info_es_need		P_((void));
extern I32s	BglMM_B_Info_es			P_((void));
extern I32s	BglMM_HowManyEfficSHist		P_((void));
extern BglEfficSHistSt*	BglMM_PickEfficSHist	P_((void));
extern void	BglMM_Info_eg_store		P_((BglEfficGHistSt* d));

/***  REPROD_EFF (Gene) HISTGRAM INFORMATION access functions  ***/
extern I32s	BglMM_Info_eg_need		P_((void));
extern I32s	BglMM_B_Info_eg			P_((void));
extern I32s	BglMM_HowManyEfficGHist		P_((void));
extern BglEfficGHistSt*	BglMM_PickEfficGHist	P_((void));
extern void	BglMM_Info_es_store		P_((BglEfficSHistSt* d));

extern void	BglMM_HistClear_store	P_((BglHistClearSt*));

extern I32s	BglMM_Info_stop			P_((void));
/******************************/

/***********************************************/
/***  BGL_PLAN INFORMATION access functions  ***/
extern I32s	BglMM_Plan_need			P_((void));
extern I32s	BglMM_B_Info_plan		P_((void));
extern I32s	BglMM_Plan_stop			P_((void));
extern I32s	BglMM_HowManyPlan		P_((void));
extern BglPlanSt*	BglMM_PickPlan		P_((void));
extern void	BglMM_Plan_store		P_((BglPlanSt*));

#endif /* beagleMessage_Info_h */
