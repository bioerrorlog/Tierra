/* bgl_define.h -- -*-C-*-
 * 
 * This work has been done at ATR HIP
 * 
 * Author          : Tsukasa Kimezawa
 * Created On      : 
 * Last Modified By: YOSHIKAWA Tooru
 * Last Modified On: Mon Nov 22 10:31:01 1999
 * Update Count    : 65
 * Status          : 
 */

/*  Define set macro for common beagle code                                */
/* Copyright (c) : ATR Human Information Processing Research Laboratories  */

#ifndef bgl_define_h
#define bgl_define_h

/* Network Monitor Define value */
#define BGL_VERSION		"3_00"
#define OPEN_MAX_GUESS		256  /* if OPEN_MAX is indeterminate */
		                    /* we're not guaranteed this is adequate */

/* 'EXIT','NOEXIT','WRITE',and 'NOWRITE' are define in tierra.h */
#define BGL_EXIT	EXIT
#define BGL_NOEXIT	NOEXIT
#define BGL_WRITE	WRITE
#define BGL_NOWRITE	NOWRITE

#define BGL_PRIV_S	's'
#define BGL_PRIV_N	'n'
#define BGL_PRIV_F	'f'
#define BGL_FLG_TRUE	't'
#define BGL_FLG_FALSE	'f'

/* Monitor States */
#define MSSELECT	0	/* select processing */
#define	MSDO		2	/* have seen (DO/DONT) */ 
#define MSCONNECT	3	/* normal data processing */
#define MSFEMENU	4	/* Frontmenu process */
#define MSDOFEM		6	/* have seen (DOFEM) */
#define MSINFO_Z	7	/* FEMenu info size-query */
#define MSVAL_ALT	8	/* FEMenu value alter */
#define MSVAL_EXAM	9 	/* FEMenu value exam */
#define MSVAL_EXAM_ALL	11 	/* FEMenu value exam all */
#define MSMISC_INJECT	12	/* FEMenu misc inject gene */
#define MSMICRO_K 	13	/* FEMnue Micro keypress mode process */
#define MSOV_GA		14	/* overview menu gene process */
#define MSOV_GA_I	15	/* overview menu gene process with Inst. */
#define MSOV_GS		16	/* overview menu gene process */
#define MSOV_GS_I	17	/* overview menu gene process with Inst. */
#define NMSTATES	18	/* # of MC* states */

#define FSINVALID	0xff	/* an invalid state number */
#define NCHRS		256	/* number of valid character */
#define MCANY		(NCHRS+1)	/* match any character */

/* Network Monitor Command Codes: */
#define MCIP		1	/* Interrupt Process */
#define MCBRK		2       /* Break Charactor */
#define MCESC		3       /* Escape Charactor */
#define MCRET		4	/* Return Charactor */

#define MCMESSAGE	10	/* Request Message */

#define MCSELECT	20	/* Back to MSSELECT */
#define MCDO     	21	/* Reqeust To Do Option */
#define MCDONT		22	/* Request NOT To Do Option */
#define MCWILL		23	/* Desire/Confirm Will Do Option */
#define MCWONT   	24     /* Refusal To Do Option */
#define MCDATA		25	/* Request Send Data */
#define MCPRECONN	26    	/* Request Pre-Connect to Tierra */
#define MCCONNECT	27    	/* Request Connect to Tierra */
#define MCFEMENU 	28    	/* Request Frontend Menu */
#define MCDOFEM		29 	/* Request Frontend Do process */
#define MCONECELLCONT	30	/* Request Continue from one cell start */

#define MCMIG_TRACK_ON	50	/* Migration tracking on */
#define	MCMIG_TRACK_OFF	51	/* Migration tracking off */

#define MCINFO_PLAN	60	/* Request Info Plan */
#define MCINFO_SIZE	61	/* Request Info Size Histogram */
#define MCINFO_GEN 	62	/* Request Info Gene Hist. */
#define MCINFO_MEM 	63	/* Request Info Mem Hist. */
#define	MCINFO_EFFICS	64	/* Request Info Effic(size) Histogram */
#define	MCINFO_EFFICG	65	/* Request Info Effic(gene) Histogram */
#define MCINFO_Z	66	/* Request Info Size Query */

#define MCVAL_ALT  	70	/* Request Value Alter */
#define MCVAL_EXAM	71	/* Request Value Exam. */
#define MCVAL_EXAM_ALL 	72	/* Request Value Exam. */

#define MCOV_S		80	/* Request OV start */
#define MCOV_S_I	81	/* Request OV start with Instruction */
#define MCOV_Q		82	/* Request OV quit */
#define MCOV_GA		83	/* Request OV genome (address mode ) */
#define MCOV_GA_I	84	/* Request OV genome (address mode )
				   with Instruction */
#define MCOV_GS		85	/* Request OV genome (size class mode ) */
#define MCOV_GS_I	86	/* Request OV genome (size class mode )
				   with Inst. */
#define MCOV_SAVE_CELL	87	/* Request for saving one cell
				   (and thread analysis) */

#define MCMISC_MICRO_TOGGLE	100	/* Requst Micro Toggle */
#define MCMISC_HISTO		101	/* Requst Histo Log Mode */
#define MCMISC_INJECT		102	/* Request Gene Injection */
#define MCMISC_TPING_S_ON	103	/* Reqest Tping On */
#define MCMISC_TPING_S_OFF	104	/* Reqest Tping Off */
#define MCMISC_TPING_C_ON	105	/* Reqest Tping On */
#define MCMISC_TPING_C_OFF	106	/* Reqest Tping Off */

#define MCMICRO_STEP_n		120	/* Request Micro Step  */
#define MCMICRO_STEP_SLICE 	121	/* Request Micro Step Cell */
#define MCMICRO_STEP_CPU 	122	/* Request Micro Step Cpu */
#define MCMICRO_STEP_NEWCPU 	123	/* Request Micro Step Cpu */
#define MCMICRO_STEP_q		124	/* Request Micro Step q */

#define MCMICRO_BRK_PUT		125	/* Request for puting a Breakpoint */
#define MCMICRO_BRK_CLEAR	126	/* Request for clearing the Breakpoint */
#define MCMICRO_BRK_REPEAT	127	/* Request for repeating the Breakpoint */

#define	MCSUSPENDLIFE		240	/* Request Suspend Life Loop */
#define	MCRESUMELIFE		241	/* Request Resume Life Loop */

#define MCSAVE			250	/* Request Save */
#define MCSAVEQUIT		251	/* Request Save & Quit */
#define MCQUIT			252	/* Request Quit */

#define TINVALID    0xff    /* an invalid transition index */



/* Network Monitor Data types: */
#define BGL_CONNECT	1	/* For type struct BglConnect */
#define BGL_REPLY	10	/* For type Reply data(I32s) */
#define BGL_REPLYMES	11	/* For type ReplyMes BglReplyMes(I8s) */
#define BGL_QUIT	12
#define BGL_SAVE_CELL	20	/* For saving data */
#define BGL_BRK		21	/* For breakpoint data */
#define BGL_INJECT	22	/* For migration control data */
#define BGL_MIGR_CTRL	23	/* For migration control data */

#define BGL_MESSAGE	50	/* For type struct BglMessage */
#define BGL_STATS	51	/* For type struct BglStats */
#define BGL_PLAN	52	/* For type struct BglPlan */
#define BGL_SIZHIST	53	/* For type struct BglSizHist */
#define BGL_GENHIST	54	/* For type struct BglGenHist */
#define BGL_MEMHIST	55	/* For type struct BglMemHist */
#define BGL_ESHIST	56	/* For type struct BglEfficSHist */
#define BGL_EGHIST	57	/* For type struct BglEfficGHist */
#define BGL_SIZQUERY_I	58	/* For type struct BglSizQuery input */
#define BGL_SIZQUERY_O	59	/* For type struct BglSizQuery output */
#define BGL_VAR		60	/* For type struct BglVar */
#define BGL_VAR_I	61	/* For type struct BglVarIntput */
#define BGL_INJECTGENE	62	/* For type struct BglInjectGene */
#define BGL_MC_STATE	63	/* For type struct BglMCState */
#define BGL_MC_SPY	64	/* For type struct BglMCSpy */
#define BGL_TPING_S	65	/* For type struct BglTPing */
#define BGL_TPING_C	66	/* For type struct BglTPing */
#define BGL_HISTCLEAR	67	/* For type struct BglHistClear */
#define BGL_OVFRAME	68	/* For type struct BglOvFrame */
#define BGL_OV		69	/* For type struct BglOvMes */
#define BGL_OV_G	70	/* For type struct BglOvGOutput */
#define BGL_OV_G_INPUT	71	/* For type struct BglOvGInput */
#define BGL_OV_G_INST	72	/* For type struct BglOvGInst */
#define BGL_MIGRATION	73	/* For type struct BglMigrationData */
#define BGL_INVALID	0xff    /* an invalid transition index */


/* Constant definition for bgl_dat_xdr.h */
#define BGL_MAX_MES		10
#define BGL_MAX_MESSAGE_LEN	120
#define BGL_MAX_VERSION_LEN	16
#define BGL_MAX_USERNAME_LEN	32
#define BGL_MAX_PASSWD_LEN	32
#define BGL_EXTR_G_LEN		21
#define BGL_MAX_WATCH_BITS	120
#define BGL_MAX_VAR_LEN		40
#define BGL_MICRO_SPY_ID_LEN	9
#define BGL_MICRO_SPY_ID_LEN_P_1	BGL_MICRO_SPY_ID_LEN + 1
#define BGL_MICRO_SPY_ST_N	10
#define BGL_MICRO_SPY_RE_N	12
#define BGL_MICRO_SPY_DATA_LEN	80
#define NUMSPECREG 3
#define BGL_MICRO_SPY_NUMSPECREG	NUMSPECREG
#define BGL_MICRO_SPY_NUMSPECREG_P_1	BGL_MICRO_SPY_NUMSPECREG + 1
#define BGL_OV_G_INST_LEN	9
#define BGL_OV_G_COMMENT_LEN	90
#define BGL_MAX_LETTERS		81

struct fsm_trans {
  I32s  ft_state;       	/* current state */
  I32u  ft_char;        	/* input command */
  I32s  ft_next;        	/* next state */
  I32s	(*ft_action) P_((void));	/* action to take */
};

struct data_decode {
  I32u  data_type;                      /* type of data */
  I32s	(*do_xdr_function) P_((I8s*));	/* function to xdr */
};
   
/* Buffer for Reqest message  */
typedef struct {
  I8s*	buf;
  I32s	len;
}ReqComSt;

struct BglAddrSt { /* Address to Tierra */
  I8s	host[64];
  I32u	ip;
  I8s	dotted[16];
  I16u	port;
  I8s	service[6];
  I16u	sPort;		/* Short communication port number */
  I8s	sPortService[6];/* Short communication port number by string */
};
typedef struct BglAddrSt BglAddrSt;

struct BglUserSt { /* User of Beagle */
  I8s	name[64];
  I8s	passwd[64];
  I8s	priv;   /* Priveleged == 's'. No priveledged == 'n' */
};
typedef struct BglUserSt BglUserSt;

struct BglSockSt{ /* Structure for socket of Beagle */
  I32s	fd;	/* Socket file descriptor */
  I8s	bglVersion[30];
  BglAddrSt	addr;
  BglUserSt	user;
};
typedef struct BglSockSt BglSockSt;

struct BglMesSt{ /* Structure for message of Beagle */
  I32s	len;
  I8s*	buf;
};
typedef struct BglMesSt BglMesSt;

#ifdef DEBUG
#define DebugPrint( message )           (void)tfprintf( stderr, message )
#define DebugPrint1( f, a )             (void)tfprintf( stderr, f, a )
#define DebugPrint2( f, a, b )          (void)tfprintf( stderr, f, a, b )
#define DebugPrint3( f, a, b, c )       (void)tfprintf( stderr, f, a, b, c )
#define DebugPrint4( f, a, b, c, d )    (void)tfprintf( stderr, f, a, b, c, d )
#define DebugPrint5( f, a, b, c, d, e ) (void)tfprintf( stderr, f, a, b, c, d, e )
#else
#define DebugPrint( message )
#define DebugPrint1( f, a )
#define DebugPrint2( f, a, b )
#define DebugPrint3( f, a, b, c )
#define DebugPrint4( f, a, b, c, d )
#define DebugPrint5( f, a, b, c, d, e )
#endif /* DEBUG */


#define	BGL_STRBUF_SIZ		1024

#define BGL_TAG_COM		((I8s)'c')
#define BGL_TAG_DAT		((I8s)'d')
#define BGL_TAG_ERR		((I8s)'e')
#define BGL_MES_HEADER_LEN	5 /* sizeof(I8s) + sizeof(I32s) */
#define BGL_MAX_MES_LEN		1024 /*Maximun length of each message */
#define BGL_MAX_STMEM		256  /*Maximun number of each structure's member */
#define BGL_MAX_RECV_LEN	6000000

#endif /* bgl_define_h */
