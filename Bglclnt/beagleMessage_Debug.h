/* beagleMessage_Debug.h -- -*- C -*-
 * 
 * @(#)beagleMessage_Debug.h	1.5	11/17/97 12:37:12
 *
 * This work has been done at ATR HIP
 * 
 * Author          : Marc Chaland
 * Created On      : Fri Sep 26 13:45:00 1997
 * Last Modified By: Marc Chaland
 * Last Modified On: Wed Nov 12 16:42:23 1997
 * Update Count    : 22
 * Status          : Unknown, Use with caution!
 */

#ifndef beagleMessage_Debug_h
#define beagleMessage_Debug_h

#include "clnt.h"
#include "beagleMessageMgrP.h"

/*******************************************************************
 *Commands for spy informations
 */
extern I32s	BglMM_MCSpy_modeReset	P_((void));
extern I32s	BglMM_HowManyMCSpy	P_((void));
extern BglMCSpySt* BglMM_PickMCSpy	P_((void));
extern void	BglMM_MCSpy_store	P_((BglMCSpySt*));


/*******************************************************************
 * Commands for debugger control
 */

/**
 * BglMM_B_Misc_m_Toggle & BglMM_Debug_Toggle
 *
 *	This function toggle debugger mode. Send sequences :
 * MCDO MCFEMENU MCDOFEM MCMISC_MICRO_TOGGLE MCSELECT
 */
#ifdef B_MECHANISM
extern I32s	BglMM_B_Misc_m_Toggle	P_((void));
#endif /* B_MECHANISM */
extern I32s	BglMM_Debug_Toggle	P_((void));

#ifdef B_MECHANISM
/**
 * BglMM_B_Misc_m_Toggle & BglMM_Debug_Slice_Toggle
 *
 *	This function toggle debugger mode between normal, delay and
 * keypressed. Send sequences :
 * MCDOFEM MCMISC_MICRO_TOGGLE
 */
extern I32s	BglMM_B_Misc_m		P_((void));
#endif /* B_MECHANISM */

/**
 * BglMM_Misc_mq
 *
 *	Return to main menu function. No data sent to server.
 */
extern void	BglMM_Misc_mq		P_((void));

/**
 * BglMM_B_Misc_mn & BglMM_Debug_Next
 *
 *	Micro step request : MCDO MCMICRO_STEP_n
 */
#ifdef B_MECHANISM
extern void	BglMM_B_Misc_mn		P_((void));
#endif /* B_MECHANISM */
extern void	BglMM_Debug_Next	P_((void));

/**
 * BglMM_B_Misc_mT & BglMM_Debut_Slice_Toggle
 *
 *	Toggle between track cell and untrack cell. Command sequence
 * is :
 * MCDO MCMICRO_STEP_SLICE
 */
#ifdef B_MECHANISM
extern void	BglMM_B_Misc_mT		P_((void));
#endif /* B_MECHANISM */
extern void	BglMM_Debug_Slice_Toggle	P_((void));

/**
 * BglMM_B_Misc_mU & BglMM_Debug_Cpu_Toggle
 *
 *	Toggle between untrack, track Cpu. Command sequence is :
 * MCDO MCMICRO_STEP_CPU
 */
#ifdef B_MECHANISM
extern void	BglMM_B_Misc_mU		P_((void));
#endif /* B_MECHANISM */
extern void	BglMM_Debug_Cpu_Toggle	P_((void));

/**
 * BglMM_B_Misc_mN & BglMM_Debug_New_Cpu
 *
 *	Toggle between untrack, track Cpu. Command sequence is :
 * MCDO MCMICRO_STEP_NEWCPU
 */
#ifdef B_MECHANISM
extern void	BglMM_B_Misc_mN		P_((void));
#endif /* B_MECHANISM */
extern void	BglMM_Debug_New_Cpu	P_((void));

/**********************************************************************************
 *				Breakpoint manager
 * Put:	   request sequence is MCDO MCFEMENU MCDOFEM MCMICRO_BRK_PUT Data MCSELECT
 * Clear:  request sequence is MCDO MCFEMENU MCDOFEM MCMICRO_BRK_CLEAR MCSELECT
 * Repeat: request sequence is MCDO MCFEMENU MCDOFEM MCMICRO_BRK_REPEAT MCSELECT
 */

/**
 * BglMM_Debug_Brk_Cell_off(namecell, offset) & BglMM_Debug_Brk_CurrCell_off(offset)
 *	Those functions send message that tierra must break on one cell at a
 * specific offset.
 */
extern void	BglMM_Debug_Brk_Cell_off P_((I8s *, I32s*));
extern void	BglMM_Debug_Brk_CurrCell_off P_((I32s*));

/**
 * BglMM_Debug_Brk_AbsTime(time) & BglMM_Debug_Brk_RelTime(time)
 *	Send request to tierra for breaking at a specific time. Absolute time
 * and relative time are possible.
 */
extern void	BglMM_Debug_Brk_AbsTime P_((BglEventSt *));
extern void	BglMM_Debug_Brk_RelTime P_((BglEventSt *));

/**
 * BglMM_Debug_Brk_ThrdSize(size) & BglMM_Debug_Brk_ThrdGene(gene)
 *	Send request to tierra for breaking when thread analysis is ready for one
 * specific size or specific gene
 */
extern void	BglMM_Debug_Brk_ThrdSize P_((I8s *));
extern void	BglMM_Debug_Brk_ThrdGene P_((I8s *));

/**
 *  BglMM_Debug_BrkClear
 *	Delete breakpoint
 */
extern void	BglMM_Debug_BrkClear	P_((void));

/**
 * BglMM_Debug_BrkRepeat
 *	Repeat last breakpoint
 */
extern void	BglMM_Debug_BrkRepeat	P_((void));

#endif
