/* bgl_data_code.h --  -*- C -*-
 * 
 * @(#)bgl_data_code.h	1.11	08/19/98 17:44:38
 * 
 * Author          : YOSHIKAWA Tooru
 * Created On      : Tue Sep 17 13:35:43 1996
 * Last Modified By: YOSHIKAWA Tooru
 * Last Modified On: Wed Aug 19 17:44:14 1998
 * Update Count    : 24
 * Status          : 
 */
/*  Define set macro for common beagle code                                */
/* Tsukasa Kimezawa                                                        */
/* Copyright (c) : ATR Human Information Processing Research Laboratories  */

#ifndef bgl_datacode_h
#define bgl_datacode_h

/* Define value for Blocking recv or Non blocking recv */
#define BGL_BLOCK 		1
#define BGL_NONBLOCK		0

/* Define value for return state from BglReplyWait */
#define	BGL_REPLY_RET_OK	0
#define	BGL_REPLY_RET_REF	1
#define	BGL_REPLY_RET_NONE	2
#define	BGL_REPLY_RET_UNEXPECT	3
#define	BGL_REPLY_RET_ILL_FD	4
#define	BGL_REPLY_RET_TIMEOUT	5
#define	BGL_REPLY_COM_ERR	6
#define	BGL_REPLY_SELECT_ERR	7

/* Define value for Connection Message */
#define BGL_ACK_DEFAULT 	0

#define BGL_FEMENU_OK 		10
#define BGL_FEMENU_REF 		11

#define BGL_PRE_CON_OK		18
#define BGL_PRE_CON_REF		19
#define BGL_SU_ACCEPTED 	20
#define BGL_NONSU_ACCEPTED 	21
#define BGL_APP_REF		22
#define BGL_WRONG_PASSWD 	23
#define BGL_OVR 		24
#define BGL_MIS_VERSION 	25
#define BGL_REF 		26

#define BGL_DO_MESSAGE_ERR	30

#define BGL_SAVE_OK 		40
#define BGL_SAVE_REF 		41
#define BGL_SAVEQUIT_OK		42
#define BGL_SAVEQUIT_REF	43
#define BGL_QUIT_OK 		44
#define BGL_QUIT_REF 		45
#define	BGL_SUSPENDLIFE_OK	46
#define	BGL_SUSPENDLIFE_REF	47
#define	BGL_RESUMELIFE_OK	48
#define	BGL_RESUMELIFE_REF	49

#define BGL_MC_TOGGLE_OK 	50
#define BGL_MC_TOGGLE_REF 	51
#define BGL_MC_BRK_OK		52
#define BGL_MC_BRK_REF		53

#define BGL_MISC_INJECT_OK 	60
#define BGL_MISC_INJECT_REF	61
#define	BGL_MISC_HIST_OK	62
#define	BGL_MISC_HIST_REF	63

#define BGL_TPING_S_OK 		70
#define BGL_TPING_S_REF 	71
#define BGL_TPING_S_REF_NONET	72

#define BGL_TPING_C_OK 		73
#define BGL_TPING_C_REF		74
#define BGL_TPING_C_REF_NONET	75

#define BGL_VAR_EXAM_OK 	80
#define BGL_VAR_EXAM_REF 	81
#define BGL_VAR_ALTER_OK 	82
#define BGL_VAR_ALTER_REF 	83

#define BGL_INFO_PLAN_OK 	90
#define BGL_INFO_PLAN_REF 	91
#define BGL_INFO_SIZ_OK 	92
#define BGL_INFO_SIZ_REF 	93
#define BGL_INFO_GEN_OK 	94
#define BGL_INFO_GEN_REF 	95
#define BGL_INFO_MEM_OK 	96
#define BGL_INFO_MEM_REF 	97
#define	BGL_INFO_EFFICS_OK	98
#define	BGL_INFO_EFFICS_REF	99
#define	BGL_INFO_EFFICG_OK	100
#define	BGL_INFO_EFFICG_REF	101
#define BGL_INFO_Z_OK 		102
#define BGL_INFO_Z_REF 		103
#define BGL_INFO_Z_CONT 	104
#define BGL_INFO_Z_BREAK 	105

#define BGL_OV_OK		110
#define BGL_OV_REF		111
#define BGL_OV_S_OK		112
#define BGL_OV_S_REF		113
#define BGL_OV_Q_OK		114
#define BGL_OV_Q_REF		115
#define BGL_OV_REQ_I		116
#define BGL_OV_REQ_D		117
#define BGL_OV_GA_OK		118
#define BGL_OV_GA_REF		119
#define BGL_OV_GS_OK		120
#define BGL_OV_GS_REF		121
#define BGL_OV_G_END		122
#define BGL_OV_G_CONT		123
#define BGL_OV_SAVE_OK		124
#define BGL_OV_SAVE_REFUSE	125

#define	BGL_MIG_REQ_OK		130
#define	BGL_MIG_REQ_REF		131
#define BGL_MIG_REQ_REF_NONET	132
#define	BGL_MIG_DATA_OK		133
#define	BGL_MIG_DATA_REF	134

#define	INVALID			-99	

/* define values for breakpoints */
#define BGL_BRK_CELL_OFF	1
#define BGL_BRK_CURCELL_OFF	2
#define BGL_BRK_ABSINST		3
#define BGL_BRK_RELINST		4
#define BGL_BRK_THRDSIZE	5
#define BGL_BRK_THRDCELL	6

#endif /* bgl_datacode_h */
