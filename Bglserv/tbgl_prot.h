/* tbgl_prot.h -- -*- C -*-
 * 
 * @(#)tbgl_prot.h	1.28	05/10/99 16:09:57
 * 
 * This work has been done at ATR HIP
 * 
 * Author          : Tsukasa Kimezawa
 * Created On      : 
 * Last Modified By: YOSHIKAWA Tooru
 * Last Modified On: Wed Nov 11 13:28:09 1998
 * Update Count    : 122
 * Status          : 
 */

/* Prototype definition of functions for hooking the beagle code           */
/* Copyright (c) : ATR Human Information Processing Research Laboratories  */

#ifndef tbgl_prot_h
#define tbgl_prot_h

/* tbgl_proc.c */
extern void TBglExit		P_((I32s));
extern void TBglTierraSet	P_((void));
extern I32s TBglChangeCommunicationPort	P_((I8s*));
extern void TBglRecvFunc	P_((I32s));
extern I32s TBglSend		P_((I32s, BglMesSt*));
extern I32s TBglRecv		P_((I32s));
extern void TBglQuit		P_((I32s));
extern void TBglPlan		P_((time_t));
#ifdef NET
extern int TBglMigration	P_((void));
#endif /* NET */
extern int TBglNewestNotCurrentCpu	P_((void));
extern void TBgl_Micro_Spy	P_((Pcells));
extern I32s BglSuAuthentCheck	P_((void));
extern I32s TBglCntStats	P_((void));
extern I32s TBglMakeBufStats	P_((BglMesSt*, I32s, I32s));
extern I32s TBglCntPlan		P_((void));
extern I32s TBglMakeBufPlan	P_((BglMesSt*, I32s, I32s));
extern void TBgl_make_femessages	P_((I8s*, I32s));
extern I32s TBglCntMessage	P_((void));
extern I32s TBglMakeBufMessage	P_((BglMesSt*, I32s, I32s));
extern I32s TBglCntMCState	P_((void));
extern I32s TBglMakeBufMCState	P_((BglMesSt*, I32s, I32s));
extern I32s TBglCntMCSpy	P_((void));
extern int  TBgl_Brk_GivenGen_Offset	P_((void));
extern int  TBgl_Brk_CurCell_Offset	P_((void));
extern int  TBgl_Brk_AbsTimeInst	P_((void));
extern int  TBgl_Brk_IntrvlTimeInst	P_((void));
extern int  TBgl_Brk_ThrdAnaDatSizeRdy	P_((void));
extern int  TBgl_Brk_ThrdAnaDatGeneRdy	P_((void));
extern void TBglDebugDisplayClr	P_((void));
extern void TBglDebugDisplay	P_((Pcells, I8s*, I32s, I32s,
				    I8s*, I8s*, I8s*, I8s*, I8s*, I8s*));
extern I32s TBglMakeBufMCSpy	P_((BglMesSt*, I32s, I32s));
extern I32s TBglCntInfoHistClr	P_((void));
extern I32s TBglMakeBufInfoHistClr	P_((BglMesSt*, I32s, I32s));
extern I32s TBglCntInfoSiz	P_((void));
extern I32s TBglMakeBufInfoSiz	P_((BglMesSt*, I32s, I32s));
extern I32s TBglCntInfoGen	P_((void));
extern I32s TBglMakeBufInfoGen	P_((BglMesSt*, I32s, I32s));
extern I32s TBglCntInfoMem	P_((void));
extern I32s TBglMakeBufInfoMem	P_((BglMesSt*, I32s, I32s));
extern I32s TBglCntInfoEfficS	P_((void));
extern I32s TBglMakeBufInfoEfficS	P_((BglMesSt*, I32s, I32s));
extern I32s TBglCntInfoEfficG	P_((void));
extern I32s TBglMakeBufInfoEfficG	P_((BglMesSt*, I32s, I32s));
#ifdef NET
extern I32s TBglAlterIPMap	P_((I32s, I32s));
extern I32s TBglAddIPMap	P_((I32s, I32s));
extern I32s TBglRemoveIPMap	P_((I32s, I32s));
extern void TBglCpyIPMap        P_((I32s, I32s, I32s, I32s));
extern I32s TBglCntTPing	P_((I32s));
extern I32s TBglMakeBufTPing	P_((BglMesSt*, I32s, I32s, I32s));
#endif /* NET */
extern I32s TBglSendDat		P_((I32s, BglMesSt*, BglMesSt*, I32s, I32s));
extern I32s TBglMakeBufInfoZ	P_((BglMesSt*, I32s, I32s));
extern I32s TBglMakeBufVar	P_((BglMesSt*, I32s, I32s));
extern I32s TBglGetAVar		P_((void));
extern I32s TBglOvFrame		P_((void));
extern I32s TBglMakeBufOvFrame	P_((BglMesSt*, I32s, I32s));
extern I32s TBglOvInst		P_((void));
extern I32s TBglMakeBufOvInst	P_((BglMesSt*, I32s, I32s));
extern I32s TBglOvInit		P_((void));
extern I32s TBglGetGeneFromAddr	P_((I32s, I32s));
extern I32s TBglGetGeneFromSiz	P_((I32s, I32s, I8s*));
extern I32s TBgl_query_size	P_((I32s));
extern void TBglBirth		P_((I32u, I32u));
extern void TBglDeath		P_((I32u, I32u));
extern void TBglOneCellStart	P_((void));
extern I32s TBglChkReqApp	P_((I32u));
#ifdef NET
/* Migration control buffer routines */
extern void MigrBufReset	P_((void));
extern void MigrStartPut	P_((BglTrackFlag new));
extern int  MigrStartGet	P_((BglTrackFlag *out, Genotype *));
extern void MigrStopPut		P_((BglTrackFlag new));
extern int  MigrStopGet		P_((BglTrackFlag out));
#endif /* NET */

/* Functions in tbgl_com.c */
extern I32s no_op		P_((void));
extern I32s do_abort		P_((void));
extern I32s do_PreConnect	P_((void));
extern I32s do_Connect		P_((void));
extern I32s do_FEMenu		P_((void));
extern I32s do_FEMenu_com	P_((void));
extern I32s do_one_cell_cont	P_((void));
extern I32s do_save		P_((void));
extern I32s do_save_quit	P_((void));
extern I32s do_quit		P_((void));
extern I32s do_SuspendLife	P_((void));
extern I32s do_ResumeLife	P_((void));
extern I32s do_send_message	P_((void));
extern I32s do_info_plan	P_((void));
extern I32s do_info_size	P_((void));
extern I32s do_info_gen		P_((void));
extern I32s do_info_mem		P_((void));
extern I32s do_info_es		P_((void));
extern I32s do_info_eg		P_((void));
extern I32s do_info_z		P_((void));
extern I32s do_val_alter	P_((void));
extern I32s do_val_exam		P_((void));
extern I32s do_val_exam_all	P_((void));
extern I32s do_misc_histolog	P_((void));
extern I32s do_misc_inject	P_((void));
extern I32s do_misc_tping_s_on	P_((void));
extern I32s do_misc_tping_s_off	P_((void));
extern I32s do_misc_tping_c_on	P_((void));
extern I32s do_misc_tping_c_off	P_((void));
extern I32s do_misc_micro	P_((void));
extern I32s do_micro_step_n	P_((void));
extern I32s do_micro_step_q	P_((void));
extern I32s do_micro_step_slice	P_((void));
extern I32s do_micro_step_cpu	P_((void));
extern I32s do_micro_brk_put		P_((void));
extern I32s do_micro_brk_clear		P_((void));
extern I32s do_micro_brk_repeat		P_((void));
extern I32s do_micro_step_newcpu	P_((void));
extern I32s do_misc_inject_ret		P_((void));
extern I32s do_OV_start_Inst		P_((void));
extern I32s do_OV_start			P_((void));
extern I32s do_OV_quit			P_((void));
extern I32s do_OV_gene_addr_Inst	P_((void));
extern I32s do_OV_gene_addr		P_((void));
extern I32s do_OV_gene_size_Inst	P_((void));
extern I32s do_OV_gene_size		P_((void));
extern I32s do_OV_save_cell		P_((void));
extern I32s do_migTrack_on		P_((void));
extern I32s do_migTrack_off		P_((void));


/* Functions in tbgl_dat.c */
extern I32s do_BGL_ABORT	P_((I8s*));
extern I32s do_BGL_REPLY	P_((I8s*));
extern I32s do_BGL_REPLYMES	P_((I8s*));
extern I32s do_BGL_CONNECT	P_((I8s*));
extern I32s do_BGL_SAVE_CODE	P_((I8s*));
extern I32s do_BGL_BRK		P_((I8s*));
#ifdef NET
extern I32s do_BGL_MIGR_CTRL	P_((I8s*));
#endif /* NET */
extern I32s do_BGL_INJECT	P_((I8s*));


#endif /* tbgl_prot_h */
