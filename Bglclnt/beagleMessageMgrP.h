/* beagleMessageMgrP.h -- -*- C -*-
 * 
 * @(#)beagleMessageMgrP.h	1.10	05/20/98 12:13:15
 *
 * This work has been done at ATR HIP
 * 
 * Author          : Marc Chaland
 * Created On      : Mon Aug 18 10:00:51 1997
 * Last Modified By: YOSHIKAWA Tooru
 * Last Modified On: Wed May 20 12:12:48 1998
 * Update Count    : 52
 * Status          : Still in construction
 */

#ifndef beagleMessageMgrP_H
#define beagleMessageMgrP_H

#include "clnt.h"
#include "beagleProcCtrl.h"
#include "beagleMessageMgrPt.h"

/*
 * definition of communication mechanism for one interface
 */
#ifndef UI_stdio
#define NB_MECHANISM
#else
#define B_MECHANISM
#endif

/*
 * State definition for one buffer
 */

extern MM_state_t MM_state;


extern MM_state_t MM_mCSpy_s;		/* state for spy datas*/

extern MM_state_t MM_migration_s;	/* state for migration datas */

extern MM_state_t MM_ovFrame_s;		/* state for overview frame datas */
extern MM_state_t MM_ov_s;		/* state for overview datas */
extern MM_state_t MM_ovGInst_s;		/* state for intructions datas */
extern MM_state_t MM_ovG_s;		/* state for gene datas */

extern MM_state_t MM_infoSize_s;	/* state for size info datas */
extern MM_state_t MM_infoGene_s;	/* state for gene info datas */
extern MM_state_t MM_infoMem_s;		/* state for mem info datas */
extern MM_state_t MM_infoEfficS_s;	/* state for effic(size) info data */
extern MM_state_t MM_infoEfficG_s;	/* state for effic(gene) info data */
extern MM_state_t MM_plan_s;		/* state for plan datas */
extern MM_state_t MM_histClear_s;	/* state for clear histogram */

extern MM_state_t MM_var_s;		/* state for var buffer */

extern MM_state_t MM_tPingS_s;		/* state for TPingS buffer */
extern MM_state_t MM_tPingC_s;		/* state for TPingC buffer */

extern MM_state_t MM_sizeQuery_s;	/* state for size query buffer */
extern MM_state_t MM_misc_h_s;		/* state for Hist. Logging buffer */

/*
 * Command requests to tierra
 */
extern BglMesSt	BglReqPreConn;		/* Pre-Connect to tierra */
extern BglMesSt	BglReqConnect;		/* Connect to tierra */
extern BglMesSt BglReqSelect;		/* Back to MSSELECT */

extern BglMesSt	BglReqSave;		/* FEMenu-save */
extern BglMesSt	BglReqQuit;		/* Quit */
extern BglMesSt	BglReqSaveQuit;		/* Save & Quit */
extern BglMesSt	BglReqSuspendLife;	/* Suspend Tierra Life Loop */
extern BglMesSt	BglReqResumeLife;	/* resume Tierra Life Loop */

extern BglMesSt	BglReqMigTrackOn;	/* Migration Tracking On */
extern BglMesSt	BglReqMigTrackOff;	/* Migration tracking Off */

extern BglMesSt	BglReqOV_S;		/* OV start */
extern BglMesSt	BglReqOV_S_I;		/* OV start */
extern BglMesSt	BglReqOV_Q;		/* OV quit */
extern BglMesSt	BglReqOV_GA_I;		/* OV gene Place */
extern BglMesSt	BglReqOV_GA;		/* OV gene Place */
extern BglMesSt	BglReqOV_GS_I;		/* OV gene size class*/
extern BglMesSt	BglReqOV_GS;		/* OV gene size class*/
extern BglMesSt	BglReqOV_Save;		/* OV gene size class*/

extern BglMesSt	BglReqMiscMicroToggle;	/* Micro debugger Toggle swtich */
extern BglMesSt	BglReqMicroStepn;	/* Micro debugger input step next*/
extern BglMesSt	BglReqMicroStepSlice;	/* Micro debugger track cell */
extern BglMesSt	BglReqMicroStepCpu;	/* Micro debugger track cpu */
extern BglMesSt	BglReqMicroStepNewCpu;	/* Micro debugger track newcpu */
extern BglMesSt	BglReqMicroStepQ;	/* Micro debugger quit */
extern BglMesSt	BglReqDoMicroStepn;	/* Micro debugger input step next*/
extern BglMesSt	BglReqDoMicroStepSlice;	/* Micro debugger track cell */
extern BglMesSt	BglReqDoMicroStepCpu;	/* Micro debugger track cpu */
extern BglMesSt	BglReqDoMicroStepNewCpu;/* Micro debugger track new cpu */
extern BglMesSt	BglReqDoMicroStepQ;	/* Micro debugger quit */
extern BglMesSt	BglReqDoMicroBrkPut;	/* Micro debugger put breakpoint */
extern BglMesSt	BglReqDoMicroBrkClear;	/* Micro debugger clear breakpoint */
extern BglMesSt	BglReqDoMicroBrkRepeat;	/* Micro debugger repeat breakpoint */

extern BglMesSt BglReqInfoPlan;		/* FEMenu-info-plan */
extern BglMesSt BglReqInfoSize;		/* FEMenu-info-sizehisto */
extern BglMesSt BglReqInfoGen;		/* FEMenu-info-genhisto */
extern BglMesSt BglReqInfoMem;		/* FEMenu-info-memhisto */
extern BglMesSt BglReqInfoEfficS;	/* FEMenu-info(size)-reprod_effhisto */
extern BglMesSt BglReqInfoEfficG;	/* FEMenu-info(gene)-reprod_effhisto */

extern BglMesSt	BglReqValExam;		/* FEMenu-value-examine */
extern BglMesSt	BglReqValAlt;		/* FEMenu-value-alter */
extern BglMesSt	BglReqValExamAll;	/* FEMenu-value-examine-all */

extern BglMesSt	BglReqMiscTpingSOn;	/* FEMenu-misc-tping-server-on */
extern BglMesSt	BglReqMiscTpingSOff;	/* FEMenu-misc-tping-server-off */
extern BglMesSt	BglReqMiscTpingCOn;	/* FEMenu-misc-tping-cluster-on */
extern BglMesSt	BglReqMiscTpingCOff;	/* FEMenu-misc-tping-cluster-off */

extern BglMesSt	BglReqInfoZ;		/* FEMenu-info-size_query */
extern BglMesSt	BglReqMiscHistLog;	/* FEMenu-misc-histolog */
extern BglMesSt	BglReqMiscInject;	/* FEMenu-misc-inject */

extern fd_set	BglAfds, BglRfds;
extern I32s    	MM_isReceived;

extern I32s	BglNfds;

/*
 * Function for polling
 */
extern void	BglMM_Polling_need	P_((void));

/*
 * Functions for Reply
 */
extern I32s	BglMM_Reply_modeReset	P_((void));
extern I32s	BglMM_HowManyReply	P_((void));
extern BglReplySt*	BglMM_PickReply	P_((void));
/*
 * BglMM_B_ReplyWait(repOk, repRef)
 *	This function is wait for reply message from Tierra.
 *    default : default state befor reply receive.
 *      repOk : If this data came from Tierra then OK.
 *     repRef : If this data came from Tierra then request refused.
 *    timeOut : If no message came from Tierra over this time then
 *            :  This function return with time out value.
 * [RETURN]
 *    BGL_REPLY_RET_OK       : Normal return (Reply OK) 
 *    BGL_REPLY_RET_REF      : Refused
 *    BGL_REPLY_RET_UNEXPECT : unexpected reply
 *    BGL_REPLY_RET_TIMEOUT  : Time Out
 *    BGL_REPLY_COM_ERR      : Communication error with Tierra
 *    BGL_REPLY_SELECT_ERR   : program bug?
 */
extern I32s	BglMM_B_ReplyWait	P_((I32s, I32s));
/*
 * BglMM_NB_ReplyWait (repOk, repRef)
 *	This function is check for reply message from Tierra.
 *    default : default state befor reply receive.
 *      repOk : If this data came from Tierra then OK.
 *     repRef : If this data came from Tierra then request refused.
 * [RETURN]
 *    BGL_REPLY_RET_OK       : Normal return (Reply OK)
 *    BGL_REPLY_RET_REF      : Refused
 *    BGL_REPLY_RET_NONE     : return with no reply data
 *    BGL_REPLY_RET_UNEXPECT : unexpected reply / Still not received
 *    BGL_REPLY_RET_ILL_FD   : unexpected port/fd
 *    BGL_REPLY_RET_TIMEOUT  : Time Out
 *    BGL_REPLY_COM_ERR      : Other Error
 *    BGL_REPLY_SELECT_ERR   : program bug?
 */
extern I32s	BglMM_NB_ReplyWait	P_((I32s, I32s));


/*
 * Functions for reply message
 */
extern I32s		BglMM_HowManyReplyMes	P_((void));
extern BglReplyMesSt*	BglMM_PickReplyMes	P_((void));

/*
 * Function for blocking reception of messages
 */
extern I32s	BglMM_B_Recv		P_((I32s));
extern I32s	BglMM_B_ClntRecv	P_((void));
extern I32s	BglMM_ClntSend		P_((BglMesSt*));


/*
 * Functions for menu break
 */
extern MM_funcstate_t	BglMM_NB_MenuBreak	P_((void));
extern MM_funcstate_t	BglMM_NB_MenuBreak_Wait	P_((void));
extern MM_funcstate_t	BglMM_NB_Cont		P_((void));

#endif /* beagleMessageMgrP_H */

