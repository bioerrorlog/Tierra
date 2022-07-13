/* tbgl_fsm.h -- -*- C -*-
 * 
 * @(#)tbgl_fsm.h	1.16	08/19/98 15:15:04
 *
 * Header of FSM definition for hooking the beagle code
 * 
 * Author          : Tsukasa Kimezawa
 * Created On      :
 * Last Modified By: YOSHIKAWA Tooru
 * Last Modified On: Wed Aug 19 15:14:47 1998
 * Update Count    : 56
 * Status          : 
 */

/* Copyright (c) : ATR Human Information Processing Research Laboratories  */

#ifndef tbgl_fsm_h
#define tbgl_fsm_h

#ifdef  BGL_GLOBAL_VALUE_DEFINE
struct fsm_trans ttstab[] =
{
  /* Now State,	Input Signal,		Next State,	Action  */
  /* ---------  -------------		-----------	------  */
  {MSSELECT,	MCANY,			MSSELECT,	no_op},
  {MSSELECT,	MCDO,			MSDO,		no_op},
  {MSSELECT,	MCPRECONN,		MSSELECT,	do_PreConnect},
  {MSSELECT,	MCMIG_TRACK_ON,		MSSELECT,	do_migTrack_on},
  {MSSELECT,	MCMIG_TRACK_OFF,	MSSELECT,	do_migTrack_off},

  {MSDO,	MCANY,			MSSELECT,	no_op},
  {MSDO,	MCCONNECT,		MSCONNECT,	do_Connect},
  {MSDO,	MCFEMENU,		MSFEMENU,	do_FEMenu},
  {MSDO,	MCONECELLCONT,		MSSELECT,	do_one_cell_cont},
  {MSDO,	MCMICRO_STEP_n,		MSSELECT,	do_micro_step_n},
  {MSDO,	MCMICRO_STEP_SLICE,	MSSELECT,	do_micro_step_slice},
  {MSDO,	MCMICRO_STEP_CPU,	MSSELECT,	do_micro_step_cpu},
  {MSDO,	MCMICRO_STEP_NEWCPU,	MSSELECT,	do_micro_step_newcpu},
  {MSDO,	MCMESSAGE,		MSSELECT,	do_send_message},

  {MSCONNECT,	MCANY,			MSSELECT,	no_op},
  {MSCONNECT,	MCSELECT,		MSSELECT,	no_op},

  {MSFEMENU,	MCANY,			MSFEMENU,	no_op},
  {MSFEMENU,	MCDOFEM,		MSDOFEM,	do_FEMenu_com},
  {MSFEMENU,	MCSELECT,		MSSELECT,	no_op},

  {MSDOFEM,	MCANY,			MSDOFEM,	no_op},
  {MSDOFEM,	MCSELECT,		MSSELECT,	no_op},
  {MSDOFEM,	MCINFO_PLAN,		MSDOFEM,	do_info_plan},
  {MSDOFEM,	MCINFO_SIZE,		MSDOFEM,	do_info_size},
  {MSDOFEM,	MCINFO_GEN,		MSDOFEM,	do_info_gen},
  {MSDOFEM,	MCINFO_MEM,		MSDOFEM,	do_info_mem},
  {MSDOFEM,	MCINFO_EFFICS,		MSDOFEM,	do_info_es},
  {MSDOFEM,	MCINFO_EFFICG,		MSDOFEM,	do_info_eg},
  {MSDOFEM,	MCINFO_Z,		MSINFO_Z,	do_info_z},
  {MSDOFEM,	MCVAL_ALT,		MSVAL_ALT,	do_val_alter},
  {MSDOFEM,	MCVAL_EXAM,		MSVAL_EXAM,	do_val_exam},
  {MSDOFEM,	MCVAL_EXAM_ALL,		MSVAL_EXAM_ALL,	do_val_exam_all},
  {MSDOFEM,	MCSAVE,			MSDOFEM,	do_save},
  {MSDOFEM,	MCMISC_HISTO,		MSDOFEM,	do_misc_histolog},
  {MSDOFEM,	MCMISC_INJECT,		MSMISC_INJECT,	do_misc_inject},
  {MSDOFEM,	MCMISC_TPING_S_ON,	MSDOFEM,	do_misc_tping_s_on},
  {MSDOFEM,	MCMISC_TPING_S_OFF,	MSDOFEM,	do_misc_tping_s_off},
  {MSDOFEM,	MCMISC_TPING_C_ON,	MSDOFEM,	do_misc_tping_c_on},
  {MSDOFEM,	MCMISC_TPING_C_OFF,	MSDOFEM,	do_misc_tping_c_off},
  {MSDOFEM,	MCMISC_MICRO_TOGGLE,	MSMICRO_K,	do_misc_micro},
  {MSDOFEM,	MCMICRO_BRK_PUT,	MSDOFEM,	do_micro_brk_put},
  {MSDOFEM,	MCMICRO_BRK_CLEAR,	MSDOFEM,	do_micro_brk_clear},
  {MSDOFEM,	MCMICRO_BRK_REPEAT,	MSDOFEM,	do_micro_brk_repeat},
  {MSDOFEM,	MCOV_S,			MSSELECT,	do_OV_start},
  {MSDOFEM,	MCOV_S_I,		MSSELECT,	do_OV_start_Inst},
  {MSDOFEM,	MCOV_Q,			MSSELECT,	do_OV_quit},
  {MSDOFEM,	MCOV_GA,		MSOV_GA,	do_OV_gene_addr},
  {MSDOFEM,	MCOV_GA_I,		MSOV_GA_I,	do_OV_gene_addr_Inst},
  {MSDOFEM,	MCOV_GS,		MSOV_GS,	do_OV_gene_size},
  {MSDOFEM,	MCOV_GS_I,		MSOV_GS_I,	do_OV_gene_size_Inst},
  {MSDOFEM,	MCOV_SAVE_CELL,		MSDOFEM,	do_OV_save_cell},
  {MSDOFEM,	MCESC,			MSSELECT,	no_op},
  {MSDOFEM,	MCQUIT,			MSSELECT,	do_quit},
  {MSDOFEM,	MCSUSPENDLIFE,		MSDOFEM,	do_SuspendLife},
  {MSDOFEM,	MCRESUMELIFE,		MSDOFEM,	do_ResumeLife},
  {MSDOFEM,	MCSAVEQUIT,		MSSELECT,	do_save_quit},
  {MSDOFEM,	MCPRECONN,		MSDOFEM,	do_PreConnect},
  {MSDOFEM,	MCMIG_TRACK_ON,		MSDOFEM,	do_migTrack_on},
  {MSDOFEM,	MCMIG_TRACK_OFF,	MSDOFEM,	do_migTrack_off},

  {MSMICRO_K,	MCANY,			MSMICRO_K,	no_op},
  {MSMICRO_K,	MCSELECT,		MSSELECT,	no_op},
  {MSMICRO_K,	MCDOFEM,		MSMICRO_K,	no_op},
  {MSMICRO_K,	MCMISC_MICRO_TOGGLE,	MSMICRO_K,	do_misc_micro},
  {MSMICRO_K,	MCMICRO_STEP_q,		MSFEMENU,	do_micro_step_q},
  {MSMICRO_K,	MCMICRO_STEP_SLICE,	MSSELECT,	do_micro_step_slice},
  {MSMICRO_K,	MCMICRO_STEP_CPU,	MSSELECT,	do_micro_step_cpu},
  {MSMICRO_K,	MCMICRO_STEP_NEWCPU,	MSSELECT,	do_micro_step_newcpu},
  {MSMICRO_K,	MCMICRO_STEP_n,		MSSELECT,	do_micro_step_n},
  {MSMICRO_K,	MCESC,			MSSELECT,	no_op},
  {MSMICRO_K,	MCPRECONN,		MSMICRO_K,	do_PreConnect},
  {MSMICRO_K,	MCMIG_TRACK_ON,		MSMICRO_K,	do_migTrack_on},
  {MSMICRO_K,	MCMIG_TRACK_OFF,	MSMICRO_K,	do_migTrack_off},

  {MSINFO_Z,	MCANY,			MSINFO_Z,	no_op},
  {MSINFO_Z,	MCSELECT,		MSSELECT,	no_op},

  {MSVAL_ALT,	MCANY,			MSVAL_ALT,	no_op},
  {MSVAL_ALT,	MCSELECT,		MSSELECT,	no_op},

  {MSVAL_EXAM,	MCANY,			MSVAL_EXAM,	no_op},
  {MSVAL_EXAM,	MCSELECT,		MSSELECT,	no_op},

  {MSVAL_EXAM_ALL,	MCANY,		MSVAL_EXAM_ALL,	no_op},
  {MSVAL_EXAM_ALL,	MCSELECT,	MSSELECT,	no_op},

  {MSMISC_INJECT,	MCANY,		MSMISC_INJECT,	no_op},
  {MSMISC_INJECT,	MCSELECT,	MSSELECT,	no_op},
  {MSMISC_INJECT,	MCESC,		MSSELECT,	no_op},

  {MSOV_GA,	MCANY,			MSOV_GA,	no_op},
  {MSOV_GA,	MCSELECT,		MSSELECT,	no_op},
  {MSOV_GA,	MCESC,			MSSELECT,	no_op},

  {MSOV_GS,	MCANY,			MSOV_GS,	no_op},
  {MSOV_GS,	MCSELECT,		MSSELECT,	no_op},
  {MSOV_GS,	MCESC,			MSSELECT,	no_op},

  {MSOV_GA_I,	MCANY,			MSOV_GA_I,	no_op},
  {MSOV_GA_I,	MCSELECT,		MSSELECT,	no_op},
  {MSOV_GA_I,	MCESC,			MSSELECT,	no_op},

  {MSOV_GS_I,	MCANY,			MSOV_GS_I,	no_op},
  {MSOV_GS_I,	MCSELECT,		MSSELECT,	no_op},
  {MSOV_GS_I,	MCESC,			MSSELECT,	no_op},

  {FSINVALID,	MCANY,			FSINVALID,	do_abort},
};
#define NTRANS (sizeof(ttstab)/sizeof(ttstab[0]))


struct data_decode ddstab[] =
{
  /* BGL type,		do_BGL  */
  /* -----------	------  */
  {BGL_REPLY,		do_BGL_REPLY},
  {BGL_REPLYMES,	do_BGL_REPLYMES},
  {BGL_CONNECT,		do_BGL_CONNECT},
  {BGL_SAVE_CELL,	do_BGL_SAVE_CODE},
  {BGL_BRK,		do_BGL_BRK},
#ifdef NET
  {BGL_MIGR_CTRL,	do_BGL_MIGR_CTRL},
#endif /* NET */
  {BGL_INJECT,		do_BGL_INJECT},
  {BGL_INVALID, 	do_BGL_ABORT}
};
#define NDATAS (sizeof(ddstab)/sizeof(ddstab[0]))


#endif /* BGL_GLOBAL_VALUE_DEFINE */

#endif /* tbgl_fsm_h */ 
