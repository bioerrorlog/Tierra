/* beagleMessage_Ov.h -- -*- C -*-
 * 
 * @(#)beagleMessage_Ov.h	1.9	05/22/98 19:43:08
 *
 * This work has been done at ATR HIP
 * 
 * Author          : Marc Chaland
 * Created On      : Mon Sep 22 16:52:10 1997
 * Last Modified By: YOSHIKAWA Tooru
 * Last Modified On: Fri May 22 19:42:53 1998
 * Update Count    : 36
 * Status          : Unknown, Use with caution!
 */

#ifndef beagleMessage_Ov_h
#define beagleMessage_Ov_h
#include "clnt.h"

/** Overview **/
extern I32s	BglMM_OvSession_need	P_((void));
extern I32s	BglMM_B_Ov_s		P_((void));/* Overview Start */
extern I32s	BglMM_OvSession_stop	P_((void));
extern I32s	BglMM_B_Ov_q		P_((void));/* Overview Quit */

/** OvFrame **/
extern I32s	BglMM_HowManyOvFrame	P_((void));
extern BglOvFrameSt*	BglMM_PickOvFrame	P_((void));
extern void	BglMM_OvFrame_store	P_((BglOvFrameSt*));

/** OV **/
extern I32s	BglMM_HowManyOv		P_((void));
extern BglOvMesSt*	BglMM_PickOv	P_((void));
extern void	BglMM_Ov_store		P_((BglOvMesSt*));



/**********************************************************************
 * OV_G_INST 
 */

/*
 * BglMM_HowManyOvGInst()
 *	Return the number of unread instructions in instruction
 * Buffer.
 */
extern I32s	BglMM_HowManyOvGInst	P_((void));

/*
 * BglMM_PickOvGInst()
 *	Return one pointer on the instuction into FIFO Buffer.
 */
extern BglOvGInstSt*	BglMM_PickOvGInst	P_((void));

/*  _
 * (-) BglMM_OvGInst_store(instr)
 *  T
 *	Store Instruction into Buffer. Avoid to use this function in an
 * interface...
 */
extern void	BglMM_OvGInst_store	P_((BglOvGInstSt*));


/*
 * BglMM_InstrDisassemble(code)
 *	Return an allocated string which is assembler instruction of code
 */
extern const char*	BglMM_InstrDisassemble	P_((I8s));


/**********************************************************************
 * OV_G
 */
extern I32s	BglMM_HowManyOvG	P_((void));
extern BglOvGOutputSt*	BglMM_PickOvG	P_((void));
extern void	BglMM_OvG_store		P_((BglOvGOutputSt*));

/*
 * BglMM_B_Ov_g_Address(addrstr)
 *	Send a request with a view to obtain a cell genome wich is
 * located at numstr address. Return a non zero value if an error
 * occured.
 */
extern void	BglMM_GeneAddress_Need	P_((I8s*));
extern I32s	BglMM_B_Ov_g_Address	P_((I8s*));
extern I32s	BglMM_B_Ov_ga		P_((void));
extern I32s	BglMM_B_Ov_g_addr	P_((I8s*));

/*
 * BglMM_B_Ov_g_Size(cellname)
 *	Send a request  with a view to obtain a cell genome
 * with cellname name.  Return a non zero value if an error
 * occured.
 */
extern void	BglMM_GeneSize_Need	P_((I8s*));
extern I32s	BglMM_B_Ov_g_Size	P_((I8s*));
extern I32s	BglMM_B_Ov_gs		P_((void));
extern I32s	BglMM_B_Ov_g_size	P_((I8s*));

/*
 * BglMM_Gene_Wait
 *	asynchronous mechanism which wait for gene data.
 */
extern void	BglMM_Gene_Wait		P_((void (*func)()));

extern void	BglMM_B_Ov_g_end	P_((void));/*Overview gene session End */
extern void	BglMM_B_Ov_g_cont	P_((void));
/*Overview gene session Continue*/

/*
 * BglMM_B_Ov_save_gene(file, tarpt) & BglMM_GeneSave_Need(file, tarpt)
 *	Request which asks to tierra to save gene code (and thread analysis
 * if tarpt is true). If file already exists, user must confirm.
 */
extern I32s	BglMM_B_Ov_save_gene	P_((SaveCodeSt*));
extern void	BglMM_GeneSave_Need	P_((SaveCodeSt*));

#endif	/* beagleMessage_Ov_h */
