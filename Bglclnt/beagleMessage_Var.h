/*
 * beagleMessage_Var.h -- 
 * 
 * This work has been done at ATR HIP
 * 
 * SCCS Status     : @(#)beagleMessage_Var.h	1.4	10/26/99 11:35:22
 * Author          : Marc Chaland
 * Created On      : Thu Oct  2 10:42:14 1997
 * Last Modified By: YOSHIKAWA Tooru
 * Last Modified On: Tue Oct 26 11:34:16 1999
 * Update Count    : 12
 * Status          : Achieved
 */

#ifndef beagleMessage_Var_h
#define beagleMessage_Var_h

#include "clnt.h"
#include "beagleMessageMgrP.h"


#ifdef CATCH_ARRAY
/*
 * BglMM_CatchArray[]
 *	array of vars which must be catch by buffer for a later use into
 * client. Must end with a null string.
 */
char *BglMM_CatchArray[] = {
  "ThreadAnalysis",
  ""
};
#endif /* CATCH_ARRAY */



/*
 * BglMM_HowManyVar() & BglMM_PickVar()
 *	first returns how many vars are into buffer. Second return value
 * of the FIFO.
 */
extern I32s	BglMM_HowManyVar	P_((void));
extern BglVarSt*	BglMM_PickVar	P_((void));
/*  _
 * (-)BglMM_Var_store()
 *  T	store one value into buffer. Don't use with graphical interface.
 */
extern void	BglMM_Var_store		P_((BglVarSt*));




/*
 * BglMM_Var_need()
 *	request to tierra to obtain all variables' value.
 */
extern I32s	BglMM_Var_need		P_((void));


/*
 * BglMM_Var_modeReset()
 *	request to Beagle Message Manager to communication mode reset.
 */
extern I32s	BglMM_Var_modeReset	P_((void));


/*
 * BglMM_B_Var_Alter(string) & BglMM_VarAlter_Need(string)
 *	Send request to tierra for altering value specified by string. The string
 * must have following format : varname = value
 */
extern void	BglMM_VarAlter_Need	P_((I8s*));

#ifdef B_MECHANISM
extern I32s	BglMM_B_Var_Alter	P_((I8s*));
extern I32s	BglMM_B_Var_a		P_((void));
extern I32s	BglMM_B_Var_ae		P_((I8s*));
#endif /* B_MECHANISM */

/*
 * BglMM_Var_value(val, var)
 *	Examine if var value is in memory. If it is, return its value
 * into val. Else make a request to tierra and return value into val.
 * This var must be in BglMM_CatchArray above.
 */
#ifdef B_MECHANISM
extern void	BglMM_Var_Value		P_((I8s*, I8s*));
#endif /* B_MECHANISM */
#ifdef NB_MECHANISM
extern void	BglMM_Var_Value		P_((I8s*, I8s*, void (*func)(I8s*)));
#endif /* NB_MECHANISM */

#ifdef B_MECHANISM
extern I32s	BglMM_B_Var_e		P_((I8s*));
extern I32s	BglMM_B_Var_e_all	P_((void));
#endif /* B_MECHANISM */

#endif /* beagleMessage_Var_h */
