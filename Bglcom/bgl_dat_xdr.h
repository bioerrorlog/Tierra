#ifndef _BGL_DAT_XDR_H_RPCGEN
#define _BGL_DAT_XDR_H_RPCGEN

#ifndef _WIN32
#include <rpc/rpc.h>
#endif /* _WIN32 */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef bgl_dat_xdr_h
#define bgl_dat_xdr_h
#ifdef DECVAX
#include "multinet_root:[multinet.include.sys]types.h"
#else /* DECVAX */
#include <sys/types.h>
#endif /* DECVAX */
#ifdef _WIN32
#include <winsock.h>
#endif /* _WIN32 */
#include "portable.h"
#if defined(unix)||defined(amiga)
#include <rpc/xdr.h>
#else /* defined(unix)||defined(amiga) */
#include "xdr.h"
#endif /* defined(unix)||defined(amiga) */
#include "tierra.h"
#include "bgl_define.h"

struct BglConnectSt {
	I8s host[64];
	I8s dotted[64];
	I8s ver[BGL_MAX_VERSION_LEN];
	I8s mode;
	I8s name[BGL_MAX_USERNAME_LEN];
	I8s passwd[BGL_MAX_PASSWD_LEN];
};
typedef struct BglConnectSt BglConnectSt;

struct BglQuitSt {
	I32s state;
};
typedef struct BglQuitSt BglQuitSt;

struct BglMessageSt {
	I8s message[BGL_MAX_MESSAGE_LEN];
};
typedef struct BglMessageSt BglMessageSt;

struct BglMCStateSt {
	I8s MC_step;
	I8s Slice;
	I8s Cpu;
};
typedef struct BglMCStateSt BglMCStateSt;

struct BglStatsSt {
	I32s GeneBnker;
	I32s InstExe_m;
	I32s InstExe_i;
	I32s NumCells;
	I32s NumGenotypes;
	I32s NumSizes;
	I8s ExtrG_gen[BGL_EXTR_G_LEN];
};
typedef struct BglStatsSt BglStatsSt;

struct BglPlanSt {
	I32s GeneBnker;
	I32s InstExe_m;
	double Generations;
	I32s tp;
	I32s NumCells;
	I32s NumGenotypes;
	I32s NumSizes;
	I32s Speed;
	I32s RateMut;
	I32s RateMovMut;
	I32s RateFlaw;
	I32s AverageSize;
	double AvgPop;
	double FecundityAvg;
	double RepInstEff;
	I32s FEBirthNum;
	I32s FEBirthLocal;
	I32s FEBirthInject;
	I32s FEBirthEject;
	I32s FEDeathNum;
	I32s FEFecundYesSum;
	I32s FEFecunZeroSum;
	I32s FEFecunEjecSum;
	I32s MaxPop;
	I32s MaxGenPop_size;
	I8s MaxGenPop_label[4];
	I32s MaxMemry;
	I32s MaxGenMem_size;
	I8s MaxGenMem_label[4];
	I32s NumGenDG;
	I8s NetFlg;
	I32u NumNodes;
	I32s FESImmigraNum;
	I32s FERImmigraNum;
	I32s FEEjectToSelf;
	I32s FESPingReqNum;
	I32s FESPingRepNum;
	I32s FESSendAddNum;
	I32s FESByeNum;
	I32s FERPingReqNum;
	I32s FERPingRepNum;
	I32s FERSendAddNum;
	I32s FERByeNum;
	I8s MemProFlg;
	I32s DynMemUse;
	I32s TotNumAloc;
};
typedef struct BglPlanSt BglPlanSt;

struct BglHistClearSt {
	I8s flg;
};
typedef struct BglHistClearSt BglHistClearSt;

struct BglSizHistSt {
	I32s index;
	I32u size;
	I32s num;
	I32s count;
};
typedef struct BglSizHistSt BglSizHistSt;

struct BglGenHistSt {
	I32s index;
	I32u size;
	I8s lbl[4];
	I32s count;
};
typedef struct BglGenHistSt BglGenHistSt;

struct BglMemHistSt {
	I32s index;
	I32u size;
	I32s num;
	I32s count;
};
typedef struct BglMemHistSt BglMemHistSt;

struct BglEfficSHistSt {
	I32s index;
	I32u size;
	I32s num;
	double dblcount;
};
typedef struct BglEfficSHistSt BglEfficSHistSt;

struct BglEfficGHistSt {
	I32s index;
	I32u size;
	I8s lbl[4];
	double dblcount;
};
typedef struct BglEfficGHistSt BglEfficGHistSt;

struct BglSizQueryOutputSt {
	I32s count;
	I8s lbl[4];
	I32s moves;
	I32s flags;
	I32s bits;
	I32s csS;
	I8s WatchBits[BGL_MAX_WATCH_BITS];
};
typedef struct BglSizQueryOutputSt BglSizQueryOutputSt;

struct BglVarSt {
	I32s index;
	I8s type;
	I8s name[BGL_MAX_VAR_LEN];
	I8s value[BGL_MAX_VAR_LEN];
};
typedef struct BglVarSt BglVarSt;

struct BglInjectGeneSt {
	I8s gene[8];
	I32s num;
	I32s addr;
};
typedef struct BglInjectGeneSt BglInjectGeneSt;

struct BglEventSt {
	I32s m;
	I32s i;
};
typedef struct BglEventSt BglEventSt;

struct BrkPointSt {
	I8s type;
	I8s gene[BGL_EXTR_G_LEN];
	I8s parasite[BGL_EXTR_G_LEN];
	I32s offset;
	BglEventSt time;
};
typedef struct BrkPointSt BrkPointSt;

struct BglTPingSt {
	I8s mode;
	I32s id;
	N32u node;
	N16u portnb;
	I32s FecundityAvg;
	I32s Speed;
	I32s NumCells;
	I32s AgeAvg;
	I32s SoupSize;
	I32u TransitTime;
	I32u Time;
	I32s InstExec;
	I32s InstExecConnect;
	I32s OS;
};
typedef struct BglTPingSt BglTPingSt;

struct BglOvFrameSt {
	I32s InstNum;
	I32s SoupSize;
	I32s NumCells;
	I32s NumSizes;
	I32s NumGenotypes;
};
typedef struct BglOvFrameSt BglOvFrameSt;

struct BglOvGInstSt {
	I32u op;
	I8s mn[BGL_OV_G_INST_LEN];
};
typedef struct BglOvGInstSt BglOvGInstSt;

struct BglOvMesSt {
	I8s event;
	I32u start;
	I32u length;
};
typedef struct BglOvMesSt BglOvMesSt;

struct BglReplySt {
	I32s ack;
};
typedef struct BglReplySt BglReplySt;

struct BglReplyMesSt {
	I8s mes[BGL_MAX_MESSAGE_LEN];
};
typedef struct BglReplyMesSt BglReplyMesSt;

struct BglMigrationConnectSt {
	N32u node;
	N16u portnb;
	I32s session;
	I8s param[8];
};
typedef struct BglMigrationConnectSt BglMigrationConnectSt;

struct BglMigrationDataSt {
	N32u node;
	I8s localparameter[8];
	I32s storelparam;
	I32s session;
	I8s information;
	I32s ranID;
	I32s tierraTime;
	I32s beagleTime;
	I32s fecundity;
};
typedef struct BglMigrationDataSt BglMigrationDataSt;

struct SaveCodeSt {
	I8s filename[BGL_MAX_LETTERS];
	I8s mode;
	I8s curorig;
	I8s tarpt;
	I8s detailrpt;
};
typedef struct SaveCodeSt SaveCodeSt;

struct BglOvGOutputSt {
	I8s comment[BGL_OV_G_COMMENT_LEN];
	I8s type;
	I32u start;
	I8s label[4];
	I8s difforig;
	I32u genome_siz;
	struct {
		u_int genome_len;
		xdrwrd *genome_val;
	} genome;
};
typedef struct BglOvGOutputSt BglOvGOutputSt;

struct BglMCSpySt {
	I32s Ploidy;
	I32s MinCellSize;
	I32s Stack_size;
	I32s Numreg;
	I32s cp_md_p;
	I32s cp_md_s;
	I32s cp_q_this_a;
	I32s cp_q_this_i;
	I32s cp_mm_s;
	I8s lbl[4];
	I32s soup;
	I8s id[BGL_MICRO_SPY_ID_LEN_P_1];
	I32s cp_mm_p;
	I32s cp_c_ib;
	I32s cp_c_c_st[BGL_MICRO_SPY_ST_N];
	I32s cp_c_c_re[BGL_MICRO_SPY_RE_N];
	I32s cp_c_c_sp;
	I32s cp_c_c_ip;
	I32s cp_c_c_threadid;
	I8s off_char;
	I32s off_int;
	I32s cp_c_c_fl_E;
	I8s d_data[BGL_MICRO_SPY_DATA_LEN];
	I32s cp_c_c_ex;
	I32s cp_c_c_so;
	I32s cp_c_c_de;
	I32s cp_c_ac;
	I32s cp_c_n;
	I8s regcol0[BGL_MICRO_SPY_NUMSPECREG_P_1];
	I8s regcol1[BGL_MICRO_SPY_NUMSPECREG_P_1];
	I8s regcol2[BGL_MICRO_SPY_NUMSPECREG_P_1];
	I8s regcol3[BGL_MICRO_SPY_NUMSPECREG_P_1];
	I8s regcol4[BGL_MICRO_SPY_NUMSPECREG_P_1];
	I8s regcol5[BGL_MICRO_SPY_NUMSPECREG_P_1];
	I8s hostbuf1[30];
	I8s hostbuf2[30];
	I8s hostbuf3[30];
	I8s hostbuf4[30];
	I8s hostbuf5[30];
	I8s hostbuf6[30];
};
typedef struct BglMCSpySt BglMCSpySt;
#endif /* bgl_dat_xdr_h */

/* the xdr functions */

#if defined(__STDC__) || defined(__cplusplus)
extern  bool_t xdr_BglConnectSt (XDR *, BglConnectSt*);
extern  bool_t xdr_BglQuitSt (XDR *, BglQuitSt*);
extern  bool_t xdr_BglMessageSt (XDR *, BglMessageSt*);
extern  bool_t xdr_BglMCStateSt (XDR *, BglMCStateSt*);
extern  bool_t xdr_BglStatsSt (XDR *, BglStatsSt*);
extern  bool_t xdr_BglPlanSt (XDR *, BglPlanSt*);
extern  bool_t xdr_BglHistClearSt (XDR *, BglHistClearSt*);
extern  bool_t xdr_BglSizHistSt (XDR *, BglSizHistSt*);
extern  bool_t xdr_BglGenHistSt (XDR *, BglGenHistSt*);
extern  bool_t xdr_BglMemHistSt (XDR *, BglMemHistSt*);
extern  bool_t xdr_BglEfficSHistSt (XDR *, BglEfficSHistSt*);
extern  bool_t xdr_BglEfficGHistSt (XDR *, BglEfficGHistSt*);
extern  bool_t xdr_BglSizQueryOutputSt (XDR *, BglSizQueryOutputSt*);
extern  bool_t xdr_BglVarSt (XDR *, BglVarSt*);
extern  bool_t xdr_BglInjectGeneSt (XDR *, BglInjectGeneSt*);
extern  bool_t xdr_BglEventSt (XDR *, BglEventSt*);
extern  bool_t xdr_BrkPointSt (XDR *, BrkPointSt*);
extern  bool_t xdr_BglTPingSt (XDR *, BglTPingSt*);
extern  bool_t xdr_BglOvFrameSt (XDR *, BglOvFrameSt*);
extern  bool_t xdr_BglOvGInstSt (XDR *, BglOvGInstSt*);
extern  bool_t xdr_BglOvMesSt (XDR *, BglOvMesSt*);
extern  bool_t xdr_BglReplySt (XDR *, BglReplySt*);
extern  bool_t xdr_BglReplyMesSt (XDR *, BglReplyMesSt*);
extern  bool_t xdr_BglMigrationConnectSt (XDR *, BglMigrationConnectSt*);
extern  bool_t xdr_BglMigrationDataSt (XDR *, BglMigrationDataSt*);
extern  bool_t xdr_SaveCodeSt (XDR *, SaveCodeSt*);
extern  bool_t xdr_BglOvGOutputSt (XDR *, BglOvGOutputSt*);
extern  bool_t xdr_BglMCSpySt (XDR *, BglMCSpySt*);

#else /* K&R C */
extern bool_t xdr_BglConnectSt ();
extern bool_t xdr_BglQuitSt ();
extern bool_t xdr_BglMessageSt ();
extern bool_t xdr_BglMCStateSt ();
extern bool_t xdr_BglStatsSt ();
extern bool_t xdr_BglPlanSt ();
extern bool_t xdr_BglHistClearSt ();
extern bool_t xdr_BglSizHistSt ();
extern bool_t xdr_BglGenHistSt ();
extern bool_t xdr_BglMemHistSt ();
extern bool_t xdr_BglEfficSHistSt ();
extern bool_t xdr_BglEfficGHistSt ();
extern bool_t xdr_BglSizQueryOutputSt ();
extern bool_t xdr_BglVarSt ();
extern bool_t xdr_BglInjectGeneSt ();
extern bool_t xdr_BglEventSt ();
extern bool_t xdr_BrkPointSt ();
extern bool_t xdr_BglTPingSt ();
extern bool_t xdr_BglOvFrameSt ();
extern bool_t xdr_BglOvGInstSt ();
extern bool_t xdr_BglOvMesSt ();
extern bool_t xdr_BglReplySt ();
extern bool_t xdr_BglReplyMesSt ();
extern bool_t xdr_BglMigrationConnectSt ();
extern bool_t xdr_BglMigrationDataSt ();
extern bool_t xdr_SaveCodeSt ();
extern bool_t xdr_BglOvGOutputSt ();
extern bool_t xdr_BglMCSpySt ();

#endif /* K&R C */

#ifdef __cplusplus
}
#endif

#endif /* !_BGL_DAT_XDR_H_RPCGEN */
