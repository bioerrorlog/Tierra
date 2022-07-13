/* beagleProcCtrl.h -- -*- C -*-
 * 
 * @(#)beagleProcCtrl.h	1.7	05/22/98 19:51:17
 *
 * This work has been done at ATR HIP
 * 
 * Author          : Marc Chaland
 * Created On      : Fri Aug 15 18:22:10 1997
 * Last Modified By: YOSHIKAWA Tooru
 * Last Modified On: Fri May 22 19:50:52 1998
 * Update Count    : 29
 * Status          : Process Manager module
                     perfectly work
 */

#ifndef beagleProcCtrl_h
#define beagleProcCtrl_h

#include "clnt.h"
#include "beagleMessageMgrPt.h"

#define MAX_PROCESS_PARAM 10
#define MAX_PARAM_SIZE 81

enum MM_beagle_function_state
{
  MMFS_CONTINUE,        /* the fonction is normally finished */
  MMFS_AGAIN,           /* the function must be reexecuted */
  MMFS_FAIL             /* the function failed */
};
typedef enum MM_beagle_function_state           MM_funcstate_t;


/* Sequence List */
struct MM_SequenceList_T {
  struct MM_SequenceList_T *next;
  MM_funcstate_t          (*FuncP)();
};
typedef struct MM_SequenceList_T	MM_SequenceList_t;

/* Process List */
struct MM_ProcessList_T {
  struct MM_ProcessList_T* next;
  MM_SequenceList_t	*seqP;
  MM_funcstate_t	(*startP)();
  MM_funcstate_t	(*normtermP)();
  MM_funcstate_t	(*abnormtermP)();
  I8s			parameters[MAX_PROCESS_PARAM][MAX_PARAM_SIZE];
  int			numparams;
};
typedef struct MM_ProcessList_T MM_ProcessList_t;

/**************************
 * BglMM_ProcessListReset
 *	reset process manager.
 */
extern void	BglMM_ProcessListReset	P_((void));
/*
 * BglMM_ProcessOneAction
 *	Execute one action of the process manager.
 */
extern void	BglMM_ProcessOneAction	P_((void));

/************************
 * BglMM_MakeProcessCell(firstaction, start, normterm, abnormterm, parameters, num)
 *	Create a new process with start, normterm and abnormterm functions. Before
 * process start, start action is called. After last action, normterm is called if
 * no problem occured. If one error tag is returned from function, abnormterm
 * function is executed before killing process.
 *	You can define local variables with num strings array (parameters).
 */
extern I32s	BglMM_MakeProcessCell	P_((MM_SequenceList_t*,
					    MM_funcstate_t   (*startP)(),
					    MM_funcstate_t   (*normtermP)(),
					    MM_funcstate_t   (*abnormtermP)(),
					    I8s **parameters,
					    int  numparams));
/*
 * BglMM_MakeFuncCell
 *	This function creates one element of the sequence list.
 */
extern MM_SequenceList_t* BglMM_MakeFuncCell	P_((MM_funcstate_t (*funcP)()));

/************************
 * BglMM_GetProcessParam(n)
 *	This functions return the nth parameters of current process.
 */
extern I8s	*BglMM_GetProcessParam	P_((int));

/************************
 * BglMM_Catch(statevar, state, func)
 *	This function adds one catch to the catch table. That means if statevar is
 * in state state, then function func is called.
 */
void	BglMM_Catch		P_((MM_state_t *var, MM_state_t value,
				    void (*func)()));

/*
 * BglMM_CheckCatch
 *	if one state variable of the table is ok, then, it calls the function
 * associated.
 */
void	BglMM_CheckCatch	P_((void));

#endif	/* beagleProcCtrl_h */
