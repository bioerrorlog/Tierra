/* @(#)prototyp.h 1.3 11/18/94 */
/* prototyp.h 30-3-98 */
/* function prototypes for Tierra Simulator, included at end of tierra.h */
/* Tierra Simulator V5.0: Copyright (c) 1991 - 1998 */
/* Tom Ray & Virtual Life */

/*
 * h_sccsid: @(#)prototyp.h 1.4 4/26/92
 */

#ifndef PROTOTYP_H
#define PROTOTYP_H

#include "license.h"

#ifdef DECVAX
#include <unixio.h>
#include <stat.h>
#endif /* DECVAX */

extern int main P_((int argc, char *argv[]));

#ifndef BGL_CLNT

#ifdef TIERRA

extern void GoBrkClr P_((void));
extern void SavBrkDat P_((void));
extern void GetSoup P_((int argc, char *argv[]));
extern void life P_((void));
extern void WriteSoup P_((I32s close_disk));

extern void FEExit P_((I32s n));
extern void (*slicer) P_((void));  /* pointer to slicer function */
extern void ReapCheck P_((void));
extern void TimeSlice P_((I32s size_slice));
extern I32s FetchDecode P_((void));
void extcallvlintrvl P_((Pcells cp, Cpu *cpu));
extern I32s ThrdAnaCollTst P_((void));
extern void AllocThrdAnaCod P_((MemThrdAnaDat *thrdanadatbuf, I32s addr));
void entcallvlintrvl P_((void));
void CallLvlIntrvlDevChk P_((void));
extern void GenExTemp P_((I32s adrt, I32s tsize,
    I32s tmpcall, I32s tmpcmplflg));
extern void AllocThrdAnaEvent P_((MemThrdAnaDat *thrdanadatbuf));
extern void GenExExe P_((I32s adrt));
extern void ThrdAnaDatColl P_((I32s deathflag, I32s migrflag, I32s addr));
extern void IncrementIp P_((void));
extern void SystemWork P_((void));

extern I32u WhoIs P_((Pcells Fp cp, I32s target, I32s *hoffset));
extern void IncrNPrIp P_((void));
extern void UpRprIf P_((Pcells cp));
extern void mutate P_((void));
extern void extract P_((I32s si, I32s virtextr, I32s gi, I32s tadatrdy));
extern void FreeDynMem P_((void));
extern void ReadDynMem P_((void));
extern void ReadGeneBank P_((void));
extern void stats P_((void));
extern void plan P_((void));
extern I32s reaper P_((I32s ex, I32s sad, I32s extrmcod));
extern void InitDeadMem P_((I32s start, I32s size, I32s type));
extern void ReapCell P_((Pcells cr, I32s extrmcod));
extern void MutBookeep P_((I32s i,I32s instrflg,
    Pcells **clist,I32s *nclist));
extern void CellMutBookeep P_((Pcells *clist,I32s nclist));
void extcelcallvlintrvl P_((Pcells cp));
extern I32s ThrdAnaCollDone P_((Pcells tc, I32s deathflag, I32s migrflag));
extern void ThrdAnaDatSave P_((Pcells tc));
extern void MemDealloc P_((I32s x, I32s y));
extern void IncrSliceQueue P_((void));
extern void DecrSliceQueue P_((void));
extern void EntBotSlicer P_((Pcells nc));
extern void UpReaper P_((Pcells cp));
extern void DownReaper P_((Pcells cp));
extern void DownReperIf P_((Pcells cp));
extern void EntBotReaper P_((Pcells cp));
extern void RmvFrmReaper P_((Pcells cp));
extern void RmvFrmSlicer P_((Pcells rc));
extern void InitCell P_((I32s ar, I32s ci, Pcells cp));
extern void InitCpu P_((Cpu *cpu));
extern void ReapBookeep P_((Pcells cr));
extern void GoInject P_((void));
extern void GoHistoCtrl P_((void));
extern void PageHist P_((void));
extern void GoReprodEff P_((void));
extern void GoSizRprdEff P_((void));
extern void GoRprdEffDaught P_((int *abortflag));
extern void GoGenRprdEff P_((void));
extern void SizeSortOrder P_((int *abortflag, int mode));
extern void GenSortOrder P_((int *abortflag, int mode));
extern void TmplAdd P_((I32s si, TmpList * tl));
extern void TmplRem P_((I32s si, TmpList * tl));

extern void ChangeGeneBnker P_((I32s gb));
extern void GeneBnkerOn P_((void));
extern void RestoreGeneBnker P_((void));
extern void StartGeneBanker P_((void));
extern void StopGeneBanker P_((void));
extern void OpenGeneBanker P_((void));
extern void CloseGeneBanker P_((void));
extern void clrf P_((void));
extern void toger P_((void));
extern void togbf P_((void));
extern void togdf P_((void));
extern void togdr P_((void));
extern void togsr P_((void));
extern GList *GetAGen P_((I8s * crit));
extern GList *GetAGenExt P_((I8s * crit, I8s * ext));
extern I8s IsFree P_((I32s x));
extern I32s MemAlloc P_((I32s size, I32s pref, I32s tol));
extern void MemInit P_((void));
extern Pcells Inject P_((FpInst g, I32s size, Mem mg,
        I32s sad, I32s tol, I32s disk, float *rrpi,
        I32s immigrant, N32u immsrcnode));
extern I32s InjectFromBank
    P_((I8s * crit, I32s sad, I32s tol, I32s injectnum));
extern Pcells GetFreeCell P_((void));
extern void GBGarbageCollect P_((void));
extern void CellsGarbageCollect P_((void));
extern void VerifyGB P_((I32s zeropopchk, I32s cellgbgenchk));
extern void VerifyGQ P_((Pgl g));
extern void SavGeneBank P_((void));
extern void SavDynMem P_((void));
extern void DivideBookeep P_((Pcells mc, Pcells ne, I32s eject));
extern void DivGenBook P_((Pcells cp, Event InstExe,
        I32s reaped, I32s mom, I32s same, I32s disk, I32s mutflag));
extern void OutDisk P_((I32s bd, I32s size, I8s label[4]));
extern void ReapGenBook P_((Pcells cp, I32s mutflag));
extern void CalcAverages P_((void));
extern void CalcSoupMaxes P_((void));
extern void CalcSoupStats P_((void));
extern void CalcGBMaxes P_((SList Fp Fp lsl, I32s lsiz_sl));
extern void CalcGBStats P_((SList Fp Fp lsl, I32s lsiz_sl));
extern void CalcTimeSoup P_((void));
extern SList Fp Fp MkGBFromSoup P_((I32s * lsiz_sl, SList Fp Fp Fp lsl,
        I32s flag));
extern void FreeGB P_((SList Fp Fp Fp lsl, I32s * lsiz_sl));
extern void GetGenList P_((I32s flags));
extern void InitGList P_((Pgl g, I32s si, I32s gi, I32s size));
extern void CheckGenotype P_((Dem * cd, I32s flags, I32s * lsiz_sl,
        SList Fp Fp Fp lsl));
extern int gc P_((void));
extern I8s IsNewSize P_((I32s si, I32s lsiz_sl, SList Fp Fp lsl));
extern void NewSize P_((Dem * cd, I32s flags, I32s * lsiz_sl,
        SList Fp Fp Fp lsl));
extern I32s IsInGenQueue P_((
    Dem * cd, I32s hash, SList Fp Fp lsl, I32s flags));
extern I32s IsInGenBank P_((Dem * cd, I32s hash, I32s flags,
        SList Fp Fp lsl));
extern void ChangeTrack P_((I8s *track));
extern I32s NewGenotype P_((Dem * cd, I32s hash, SList Fp Fp lsl, I32s flags));
extern I8s IsSameGen P_((I32s size, FpInst g1, FpInst g2));
extern void AdjCtrlVars P_((void));
extern void CalcFlawRates P_((void));
extern void gq_add P_((GList * p));
extern void gq_movtop P_((GList * p));
extern I8s gq_swap P_((void));
extern void gq_rem P_((GList * p));
extern GList *gq_read P_((int si, int gi));
extern void printq P_((void));
extern void IncrLbl P_((I8s * lbln, I8s * lblo));
extern void MaxLbl P_((I8s * lbl, I8s * s));
extern void DelGenFile P_((Pgl tgl));
extern I8s WritGenFile P_((Pgl g, I8s file[]));
extern I32s GetGenFormat P_((Pgl g, I8s file[]));
extern void Ascii2Ebcdic P_((I8s * s));
extern void Ebcdic2Ascii P_((I8s * s));
extern void SetBit P_((I32u * seed, I32u bit, I32u value));
extern void DoRPNd P_((void));
extern void DoRPNu P_((void));
extern void DoFlags P_((void));
extern void DoMods P_((void));
extern void DoMods2 P_((void));
extern void DoMods3 P_((void));
extern void nop P_((void));
extern void not0 P_((void));
extern void notl P_((void));
extern void not P_((void));
extern void xor P_((void));
extern void ior P_((void));
extern void clrrf P_((void));
extern void clrfi P_((void));
extern void and P_((void));
extern void skip P_((void));
extern void add P_((void));
extern void mul P_((void));
extern void idiv P_((void));
extern void push P_((void));
extern void pop P_((void));
extern void stup P_((void));
extern void stdn P_((void));
extern void tcall P_((void));
extern void call P_((void));
extern void mov P_((void));
extern void movdd P_((void));
extern void movdi P_((void));
extern void movid P_((void));
extern void movii P_((void));
extern void adr P_((void));
extern I32s mal P_((I32s *sug_addr, I32s sug_size, I32s mode));
extern void malchm P_((void));
extern void divide P_((void));
extern void CheckCells P_((void));
extern I32s flaw P_((void));
extern I32s ctemplate P_((I32s * f, I32s * b, I32s * slim, I8s * mode,
        I32s tz, I32s dir, I32s tmpcall));
extern I8s IsPriv P_((Pcells cp, I32s a));
extern I8s IsBitPriv P_((Pcells cp, I32s a, I32s mode));
#ifdef EXECPROT
extern I8s PrivExec P_((Pcells cp, I32s a));
#endif /* EXECPROT */
#ifdef WRITEPROT
extern I8s PrivWrite P_((Pcells cp, I32s a));
#endif /* WRITEPROT */
#ifdef READPROT
extern I8s PrivRead P_((Pcells cp, I32s a));
extern I8s PrivReadRange P_((Pcells cp, I32s from, I32s to));
#endif /* READPROT */
extern I8s IsInsideCell P_((Pcells cp, I32s a));
extern void WhichCell P_((I32s a, Pcells Fp wce, I8s * md));
extern void SlicerPhoton P_((void));
extern I32s PhotonFit P_((I32s a, Instruction * PhotonInst, I32s PhotonSize));
extern I32s PhotonSlide P_((I32s a, Instruction * PhotonInst, I32s PhotonSize,
        I32s PhotonWidth));
extern void PhotonTranslate P_((Instruction * PhotonInst, I8s * PhotonWord));
extern void SlicerQueue P_((void));
extern void RanSlicerQueue P_((void));
extern void mut_site P_((HpInst s, I32s track));
extern void StupGenLists P_((void));
extern void GetNewSoup P_((void));
extern void GetOldSoup P_((FILE * inf));
extern void GenExMov P_((I32s to, I32s from,
    I32s srccpysegofst, I32s dstcpysegofst));
extern void FEPlan P_((I32s tielog));
extern I32s HistoInEffect P_((void));
extern void DispQueryPage P_((int *querypgidx, size_buf * buf,
        int displinecnt, int num_spec, I32s size_class));
extern void DispSizeQuery P_((size_buf * buf,
        int num_spec, I32s size_class));
extern size_buf *InitSizeQuery P_((I32s size_class, int num_spec));
extern void GoSizeQuery P_((void));
extern void query_species P_((I32s num));
extern void FESoupImage P_((FILE * fp, I32s x, I32s y, I32s n));
extern void FEPrintSI P_((FILE * fp, I32s addr, I32s x, I32s n, I32s sz,
                I32s md));
extern void (*execute) P_((void));  /* pointer to execute function */
extern void (*decode) P_((void));  /* pointer to parse function */
extern I8s WriteIOSToSoup P_((Pcells cp, I32s dest, IOS *ios, I32s *size));
extern void InitIOS P_((IOS * ios));
extern I8s WriteIOS P_((Pcells cp, I32s sou));
extern IOS *GetIOSOut P_((Pcells cp));
extern void FEPrVQ P_((I8s * data));
extern I32s open_max P_((void));
extern void GeneticOps P_((void));
extern void ttime P_((void));
extern void slicexit P_((void));
extern void halt P_((void));
extern I8s issignal P_((I32s sig));
extern void addsignal P_((I32s sig, Pcells cp));
extern void remsignal P_((I32s sig, Pcells cp));
extern void DisplayMenu P_((int ncols, char *menutitles[],
        int displaywidth, int intrvwidth, int optnowidth,
        int rmargin, int *promptofst));
extern void GoSave P_((void));
extern void GoShell P_((void));
extern void GoSaveQuit P_((void));
extern void GoInfo P_((void));
extern void GoPlan P_((void));
extern void GoSizeQuery P_((void));
extern void GoSizeHist P_((void));
extern void GoMem P_((void));
extern void GoGene P_((void));
extern void GoMisc P_((void));
extern void GoMiscMore P_((void));
extern void GoOnLine P_((void));
extern void GoOffLine P_((void));
extern void GoHistoLog P_((void));
extern void GoMicroToggle P_((void));
extern void GoALComm P_((void));
extern void TrkCPU P_((Pcells cp, I32s newcpu, I32s thrdid));
extern void UnTrkCPU P_((void));
extern void TrkCell P_((Pcells cp));
extern void UnTrkCell P_((void));
extern GlIndex EnterGenotype
    P_((I32s promptline, char *prompttext, I32s *abortflag, I32s genchk));
extern I32s EnterLongVal
    P_((I32s promptline, char *prompttext, I32s *abortflag));
extern double EnterFloatVal
    P_((I32s promptline, char *prompttext, I32s *abortflag));
extern void csync P_((void));
extern void dec1s P_((void));
extern void dec2s P_((void));
extern void dec3s P_((void));
extern void dec4s P_((void));
extern void dec1d P_((void));
extern void dec1d1s P_((void));
extern void dec1d2s P_((void));
extern void dec2d2s P_((void));
extern void dec3d2s P_((void));
extern void dec1d3s P_((void));
extern void decadr P_((void));
extern void decjmp P_((void));
extern void enter P_((void));
extern void exch P_((void));
extern void getregs P_((void));
extern void join P_((void));
extern void offset P_((void));
extern void pmovdi P_((void));
extern void pmovid P_((void));
extern void pmovii P_((void));
extern void pnop P_((void));
extern void ptcall P_((void));
extern void rolld P_((void));
extern void rollu P_((void));
extern void shl P_((void));
extern void shr P_((void));
extern void split P_((void));
extern I32s AllocThrdAnaCpy P_((MemThrdAnaDat *thrdanadatbuf, I32s cpysegofst));
extern void FreeThrdAnaDatCell P_((Pcells cp));
extern void ThrdAnaDatDump P_((void));
void AllocSoupUpdEvent P_((MemThrdAnaDat *memthrdanadat,
    I32s src, I32s dest));
extern void ThrdAnaDatCollMov P_((
    I32s srccpysegofst, I32s cpysrc, I32s cpydst));
extern void ThrdAnaTdtBits P_((I32s thrdanacolflg, I32u bitnum));
extern void ThrdAnaGenExMov P_((
    I32s thrdanacolflg, I32s srccpysegofst,
    I32s cpysrc, I32s cpydst, I32u whobit));
extern void ThrdAnaGenExUsed P_((I32s thrdanacolflg, I32s addr));
void extthdcallvlintrvl P_((Pcells cp, Cpu *cpu));
extern void tieinitvar P_((void));

#ifdef NET
extern void STPingUpdateReq P_((void));
extern void NetRecvFunc P_((void));
extern void Apocalypse P_((void));
extern int ProcTierraMsg P_((TMessage *mesg,NetAddr *msgsrc,I32u msgid));
extern void AddIPMap P_((MapNode new, I32s  whichmap));
I32s EmigSubNetChk P_((I32s srcsubnet, I32s dstsubnet, I32s dstmap));
extern I32s NEject P_((Pcells cr, I32s IP, I32s whichmap));
extern I32s InIPMap P_((NetAddr addr, I32s  whichmap));
extern I32s Remot_from_IPMap P_((I32s index, I32s  whichmap));
extern I32s RandIP P_((void));
extern void tpingsnd P_((void));
extern void tpingrec P_((void));
extern void getip P_((void));
extern void getipp P_((void));
extern I32s TPing_recv P_((TMessage * mesg));
extern I32s ChooseIP P_((Reg addr, Reg port, I32s whichmap, I32s strict));
extern bool_t xdr_ComAddr P_((XDR * xdrsp, ComAddr * cell_addr));
extern I32u ReadIPMap P_((Pcells cp));
extern void IncrIOSnio P_((Pcells cp));
extern I8s ReadIOS P_((Pcells cp, I32s tag, I32s dest, I32s * size));
extern IOS *GetFreeIOS P_((Pcells cp));
extern IOS *GetTagIOS P_((Pcells cp, I32s tag));
extern I32s HammingDist P_((I32u a, I32u b));
extern I32s RemoveIPMap P_((NetAddr dead, I32s  whichmap));
extern void SaveIPMap P_((I32s  whichmap));
extern I32s connectTCP P_((I8s * host, I8s * service));
extern I32s connectUDP P_((I8s * host, I8s * service));
extern I32s connectsock P_((I8s * host, I8s * service, I8s * protocol));
extern I32s passiveTCP P_((I8s * service, I32s qlen));
extern I32s passiveUDP P_((I8s * service));
extern I32s passivesock P_((I8s * service, I8s * protocol, I32s qlen));
extern I32s MonInitMesFunc P_((I32s fd));
extern I32s CalcSubNetPop P_((I32s whichmap,
    I32s subnet,I32s *sbntnodct,I32s *subnetavgsiz));
extern I32s SockBlkCtl P_((int s, I32s blksktflg));
extern void migrate P_((void));

#ifdef BGL
/* Migration message routines */
extern void MigrMigrateMesg        P_((Pcells cr, I32s  index));
extern void MigrArrivalMesg        P_((Pcells cp, TMessage *mesg));
extern void MigrSleepMesg          P_((void));
extern void MigrApocalypseMesg     P_((Pcells cr));
extern void MigrToolRemotbirthMesg P_((Pcells cr));
extern void MigrToolBirthMesg      P_((Pcells cr));
extern void MigrDeathMesg          P_((Pcells  cr));
extern void MigrToolTerminateMesg P_((Pcells cr, MigrStatus status));
#endif /* BGL */

#ifdef __MWERKS__
extern int getdomainname P_((char *domainName, int maxLength));
extern int gettimeofday P_((struct timeval * time, struct timezone * zone));
#endif /* __MWERKS__ */

#endif /* NET */

#if defined(NET)||defined(BGL)
extern void GoBglComm P_((void));
#endif /* defined(NET)||defined(BGL) */

#ifdef AMIGA
int __stat(const char *, struct stat *);
#if FRONTEND == BASIC
int kbhit P_((void));
#endif /* FRONTEND == BASIC */
#if FRONTEND == STDIO
void chkabort P_((void));
#endif /* FRONTEND == STDIO */
void sleep P_((unsigned int delay));
#ifdef NET
int gettimeofday P_((struct timeval *time, struct timezone *zone));
#endif /* NET */
#endif /* AMIGA */

#if FRONTEND != STDIO || defined(BGL)
extern void query_spec_d P_((I32s size, I32s lbl));
#endif /* FRONTEND != STDIO || defined(BGL) */

#ifdef __MWERKS__
extern int cmdline P_((char *app, char ***));
#endif /* __MWERKS__ */

#ifdef __TURBOC__
extern I8s KeyHit P_((void));
#ifdef SIGBLOCK
#if FRONTEND == STDIO
extern int T_sig_int P_((void));
#endif /* FRONTEND == STDIO */
#endif /* SIGBLOCK */
#endif /* __TURBOC__ */

#ifdef DECVAX
#ifdef SIGBLOCK
extern void T_sig_int P_((int));
#endif /* SIGBLOCK */
#endif /* DECVAX */

#if OPSYS == MACTIE
extern int FEProcessEvent(void);
#if INTERFACE == BASICM
extern void FEInitToolbox(void);
extern WindowPeek FECreateNewWindow(void);
#endif /* INTERFACE == BASICM */
#endif /* OPSYS == MACTIE */

#ifdef MICRO
extern void Micro_Spy P_((Pcells cp));
extern void GoBrkMenu P_((void));
extern void GoBrkSet P_((void));
extern void DisplayHostCell P_((
        Pcells cp, char *hostbuf1, char *hostbuf2,
        char *hostbuf3, char *hostbuf4, char *hostbuf5, char *hostbuf6));
extern void MrkSpecRegCol P_((char *regcol, int regno));
extern void DebugDisplay P_((Pcells cp));
extern int ChkBrk P_((Pcells cp));
extern void ChkBrkBd P_((void));
extern Event Str2Event P_((char *eventbuf));
extern void RclBrkDat P_((void));
extern Pcells FindHost P_((I32s soupaddr, I32s *hosttype));
extern void GoRepeatBrk P_((void));
extern void ViewSoupStartAddr P_((void));
extern void DisplaySoupPage P_((void));
extern void ViewSoupHost
    P_((I32s soupviewaddr, char *hostbuf1, char *hostbuf2, char *hostbuf3));
extern double CalcAvgPopSizRng P_((void));
extern void DbgDisplCtrl P_((Pcells cp));
extern void DispDebuggerScrn P_((Pcells cp));
extern int CmpEvents P_((Event event1, Event event2));
extern void DisasmMenu P_((Pcells cp));
extern void Disasm_Creature P_((Pcells cp, I32s lineoffset));
extern void Disasm_SpecGenome P_((I32s lineoffset));
extern void Disasm_Resident P_((I32s lineoffset));
extern void Disasm_ThrdAnaDatRdy P_((I32s lineoffset));
extern char PermTempBothSpec
    P_((I32s lineoffset, char *speclbl, I32s *abortflag));
extern char YNSpec
    P_((I32s line, I32s col, char *prompt, I32s *abortflag));
extern char CurrOrigSpec P_((I32s lineoffset, I32s *abortflag));
extern char SummDtlSpec P_((I32s lineoffset, I32s *abortflag));
extern int ChkPermTemp P_((int permtempsub, Pcells cp));
extern I8s SzClsGenSpec P_((I32s promptline, I32s *abortflag));
extern void initrcnttardy P_((void));
extern void Brk_GivenGen_Offset P_((void));
extern void Brk_CurCell_Offset P_((void));
extern void Brk_AbsTimeInst P_((void));
extern void Brk_IntrvlTimeInst P_((void));
extern void Brk_GivenParaGivenHost P_((void));
extern void Brk_PctChgPop P_((void));
extern void Brk_AnyParaAnyHostTime P_((void));
extern void Brk_AnyGenGivenTime P_((void));
extern void Brk_GivenParaAnyHost P_((void));
extern void Brk_AnyParaGivenHost P_((void));
extern void Brk_AnyGenSizeRange P_((void));
extern void Brk_AnyPara_HostSizeRange P_((void));
extern void Brk_AnyHost_ParaSizeRange P_((void));
extern void Brk_ThrdAnaDatRdy P_((void));
#endif /* MICRO */

#ifdef IO
extern void put P_((void));
extern void puticc P_((void));
extern void pputicc P_((void));
extern void get P_((void));
extern void pget P_((void));
extern void Write2Get P_((Pcells cp, I32s value));
extern void Broad2Get P_((I32s value));
extern I8s ReadFPut P_((Pcells cp, I32s * value));
extern I8s ReadPGbfVal P_((PGbf *putgetbuf, I32s *value));
extern void WritePGbfVal P_((PGbf *putgetbuf, I32s maxpgbufsize));
extern Pcells FindPutCell P_((Pcells cp, I32s adre));
extern Preg GetFreeIOBuf P_((PGbf *iobuf, I32s maxbufsize));
extern void FreePGbf P_((PGbf *pgbf));
extern void SavePGbf P_((PGbf *pgbf, FILE *ouf));
extern void ReadPGbf P_((PGbf *pgbf, FILE *inf));
#endif /* IO */

#ifdef SHADOW
extern void regorder P_((void));
extern void pushst P_((void));
#endif /* SHADOW */

#if PLOIDY > 1
extern void trex P_((void));
extern void trso P_((void));
extern void trde P_((void));
extern void ptrack P_((void));
#endif /* PLOIDY > 1 */

#ifdef _WIN32
extern int close P_((int));
extern __declspec(dllimport) int  HookInstall(void);
extern __declspec(dllimport) void HookUnInstall(void);
extern void Win32Startup P_((void));
#endif /* _WIN32 */

#ifdef MEM_CHECKER
extern int chkr_stub_getdomainname P_((char *name, size_t len))
    __asm__ (CHKR_PREFIX("getdomainname"));
#endif /* MEM_CHECKER */

#ifdef EXEINSTSIZTIMSLC
extern void AllocCellGenBits P_((Pcells pc));
extern void FreeCellGenBits P_((Pcells cp));
extern void MarkCellGenBitExe P_((Pcells cp, I32s addr));
extern void ReadCellGenBits P_((FILE *inf, Pcells cp));
extern void SaveCellGenBits P_((FILE *ouf, Pcells cp));
#endif /* EXEINSTSIZTIMSLC */

#if FRONTEND == BASIC
#if INTERFACE != BASICW
extern I8s tigetch P_((void));
extern void tiegetstr P_((I8s *str));
extern void ttouchwin P_((void));
extern void trefresh P_((void));
extern I8s tmvgetch P_((I32s y, I32s x));
#endif /* INTERFACE != BASICW */

extern void tclear P_((void));
extern void techo P_((void));
extern void tnl P_((void));
extern void tnocbreak  P_((void));
extern void tendwin P_((void));
extern void tmove P_((int y, int x));
extern void tstandout P_((void));
extern I32s tmvprintw P_((I32s y, I32s x, I8s *strbuf));
extern I32s tprintw P_((I8s *strbuf));
extern void tstandend P_((void));
extern void tnonl P_((void));
extern void tcbreak P_((void));
extern void tnoecho P_((void));
extern void tclrtobot P_((void));
extern void tnodelay P_((I32s bf));
extern void tnocrmode P_((void));
extern void tcrmode P_((void));
extern void tinitscr P_((void));
extern void tmvaddch P_((int y, int x, int ch));
extern void tclrtoeol P_((void));
#endif /* FRONTEND == BASIC */ 

#ifdef MEM_PROF
extern void MemStatReport P_((void));
extern void InitMemProf P_((void));
extern void FreeMemProf P_((void));
extern void MemStats P_((I32s label, I32s af, I32s size));
extern void MemStatCleanUp P_((void));
#endif /* MEM_PROF */

#if defined(MSGCHKSUM)||(defined(PKTCHKSUM)&&(NETTYPE == UDPASM))
extern int getchksum P_((I8u *buf,I32u buflen));
extern void setchksum P_((I8u *buf,I32u buflen));
#endif /* defined(MSGCHKSUM)||(defined(PKTCHKSUM)&&(NETTYPE == UDPASM)) */

#ifdef __TURBOC__
#if FRONTEND == STDIO
extern int TieSigIO P_((void));
#endif /* FRONTEND == STDIO */
#endif /* __TURBOC__ */
#ifdef __MWERKS__
extern void TieSigIO P_((I32s sig));
#endif /* __MWERKS__ */
#ifdef unix
extern void TieSigIO P_((int sig));
#endif /* unix */
#ifdef DECVAX
extern void TieSigIO P_((int));
#endif /* DECVAX */
#ifdef _WIN32
extern void TieSigIO P_((I32s sig));
#endif /* _WIN32 */
#ifdef AMIGA
extern void TieSigIO P_((int sig));
#endif /* AMIGA */

#endif /* TIERRA */

#if defined(TIERRA)||defined(CLSTRSRVR)
extern void FEError P_((I32s errno1, I32s exit, I32s write, char * buff));
extern void tsrand P_((I32s seed));
extern void ToggleLog P_((I32s logflag, FILE **loghandle,
   I8s *logname, I32s *logfilpsn));
extern double tdrand P_((void));
extern void FEPrintf P_((I32s scr_x, I32s scr_y, I32s scr_a,
    I32s slee, char * buff));
void CtrBufStr P_((char *buf,int buflen,char *bufstr));
extern void initvar P_((void));
extern void FEStartup P_((void));
extern void SetDispVars P_((void));
extern void FEMenu P_((void));
void TieSleep P_((time_t (*starttime)(void), I32s sleepintrvl));
extern void FEBanner P_((void));
extern void FEStats P_((void));
extern void SetPlanMSG_Y P_((void));
extern void FEClear P_((void));
extern void FEVidInit P_((I32s mode));
extern I32s FEGetch P_((I32s x, I32s y));
extern void GoQuit P_((void));
extern void GoVariable P_((void));
extern void FEClrmsg P_((I32s n, I8s * buf));
extern void GoAlter P_((void));
extern void GoExamine P_((void));
extern void FEGetStr P_((I32s x, I32s y));
extern I8s GetAVar P_((I8s *data, I32s alterflag, I32s MonReq, I32s buflen));
extern void FECeol P_((I32s n));
extern I32s SubEvent P_((Event * event1, Event * event2, Event * result));
extern I32s AddEvent P_((Event * event1, Event * event2, Event * result));
void TierraExit P_((int status));
void tusleep P_((unsigned long usec));
void tsleep P_((unsigned long sec));

#ifdef _WIN32
extern I8s KeyHit P_((void));
void usleep P_((unsigned long usec));
#endif /* _WIN32 */

#ifdef NET
extern void GetIPMap P_((FILE * fp, I32s  whichmap));
extern void LdIPMapTPingDat P_((I32s whichmap));
extern I32u pr_inet P_((I8s ** listptr, I32s length));
extern void UpdateOwnMapNode P_((void));
extern void IPMapTPing_send P_((I32s whichmap));
extern void CleanIPMap P_((I32s whichmap));
extern I32s Map_send P_((I32s srcmap, I32s dstmap));
extern void WriteTPingDat P_((I32s  whichmap));
extern I32s Addr_recv P_((TMessage * mesg, NetAddr *msgsrc));
extern void TreatTPingData P_((TMessage *mesg,
    NetAddr *address, I32s reply, I32s *index));
extern I32s TPing_reply P_((TMessage * mesg,I32s index,NetAddr *msgsrc));
extern void GetTPingData P_((TPingData *data,
    TPrivPingData *PrivPingData, I32s index));
extern I32s Bye_recv P_((TMessage * mesg, NetAddr *msgsrc));
extern void SelfID P_((void *cp, ComAddr * cell_addr));
extern I32s TPing_send P_((I32s index, ComAddr * cell_addr, I32s whichmap));
extern I32s MapNode_send_mesg P_((NetAddr destaddr,
    I32s dstmap, MapNode *mapnode));
extern I32s Addr_send_mesg P_((NetAddr destaddr, I32s whichmap, I32s index));
extern I32s Bye_send_mesg P_((NetAddr destaddr, I32s whichmap, I32s index));
extern bool_t txdr_TMessage P_((XDR *xdrs, TMessage *objp));
extern void SetRemOutBndCap P_((void));
extern I32s Bye_send P_((I32s whichmap));
extern I32s Addr_send P_((I32s whichmap));
extern int tgetsockopt P_((int s, int level,
    int optname, char *optval, int *optlen));
extern void UpdRemOutBndCap P_((void));

#if NETTYPE == UDPASM
extern bool_t txdr_datpkt P_((XDR *xdrs, datpkt *objp));
extern bool_t txdr_pkthdr P_((XDR *xdrs, pkthdr *objp));
#endif /* NETTYPE == UDPASM */

#ifdef _WIN32
#endif /* _WIN32 */

#endif /* NET */

#endif /* defined(TIERRA)||defined(CLSTRSRVR) */

#ifdef CLSTRSRVR 
extern void srvrFEPlan P_((I32s tielog));
extern I32s MapNode_send P_((I32s dstmap, MapNode *mapnode));
extern void clstrinitvar P_((void));
#endif /* CLSTRSRVR */

#if defined(TIERRA)||defined(ARGTIE)
extern I32s GetAscGen P_((Pgl g, I8s ifile[]));
extern I32s add_gen P_((FILE * fp, head_t * head, indx_t ** indx, Pgl gen));
extern I32s Hash P_((I32s size, FpInst v));
extern void write_head P_((FILE * fp, head_t * head));
extern void write_indx P_((FILE * fp, head_t * head, indx_t * indx));
extern void WriteThrdAnaDat P_((MemThrdAnaDat *thrdanadatbuf, FILE *fp));
extern void WrtThrdAnalysis P_((FILE *fp, Pgl g,
    double sucsiznslrat, I32s sucsiznsl,
    I32s siznsl, I32s detailrpt, I32s clstrfmt,
    I32s symclstranafmt,
    ThdTsTyArr (*codthdtstyarr)[PLOIDY][NSTRTISTYP],
    I32s exeptrn, I16s *genelkup, GeneDefArr *gendef, I32s spltisana));
#endif /* defined(TIERRA)||defined(ARGTIE) */

#if defined(TIERRA)||defined(ARGTIE)||\
    defined(GENALIGN)||defined(THREADTREE)||\
    defined(PROBE)||defined(SOUPUPDTRC)

extern void enc_size_precomp P_((void));
extern void GetAMap P_((I8s * file));
extern head_t read_head P_((FILE * fp));
extern indx_t *read_indx P_((FILE * fp, head_t * head));
extern Pgl get_gen P_((FILE * fp, head_t * head, indx_t * indxn, I32s n));
extern void InitThrdAnaDat P_((MemThrdAnaDat *thrdanadatbuf));
extern I32s find_gen P_((indx_t * indx, I8s * gen, I32s n));
extern void ReadGenDef P_((GeneDefArr *gendef, I16s **genelkup,
    I32s size, I8s *label, I8s *defdir));
extern void AddGenDef P_((GeneDefArr *gendef,I8s *genname,I8s *gdfcmnt,
    I32s gdfstart,I32s gdfend,I32s gdftyp));
extern void FreeGenDef P_((GeneDefArr *gendef, I16s **genelkup, I32s size));
extern void BldGenLkup P_((GeneDefArr *gendef,I16s **genelkup,I32s size));
extern bool_t txdr_head_t P_((XDR *xdrs, head_t *objp));
extern bool_t txdr_indx_t P_((XDR *xdrs, indx_t *objp));
extern bool_t txdr_GBGenome P_((XDR *xdrs, GBGenome *objp));
extern bool_t txdr_GBGenBits P_((XDR *xdrs, GBGenBits *objp));
extern bool_t txdr_GBindx_t P_((XDR *xdrs, GBindx_t *objp));
extern void txdr_BaseCallLvlIntrv P_((XDR *xdrs, BaseCallLvlIntrv *objp));
extern void txdr_IdxThreadDat P_((XDR *xdrs, IdxThreadDat *objp));
extern void txdr_ThrdAnaEvent P_((XDR *xdrs, ThrdAnaEvent *objp));
extern void txdr_SoupUpdEvent P_((XDR *xdrs, SoupUpdEvent *objp));
extern void txdr_FilGenThrdDat P_((XDR *xdrs, FilGenThrdDat *objp));
extern void txdr_FilInstrThrd P_((XDR *xdrs, FilInstrThrd *objp));
extern void txdr_FilThrdAnaDat P_((XDR *xdrs, FilThrdAnaDat *objp));
extern void FreeThrdAnaDat P_((MemThrdAnaDat *thrdanadatbuf));
extern void ReadThrdAnaDat P_((MemThrdAnaDat *thrdanadatbuf,
    CalLvlIntByIDArr *calvlintrvlkupid, FILE *fp));
extern I32s InstrXdrWrdSize P_((I32s instrlen));
extern I32s GenBitsXdrWrdSize P_((I32s gbitlen));
extern void AllocGenome P_((Pgl tgl,I32s size));
extern void FreeGenome P_((Pgl tgl));
extern void AllocGenBits P_((Pgl tgl,I32s size));
extern void FreeGenBits P_((Pgl tgl));
extern I32s FilGTaDatSize P_((FilThrdAnaDat *thrdanadat));
extern void FreeGen P_((GList * g));
extern FILE *open_ar P_((I8s * file, I32s size, I32s mode));
extern I32s GTaDatSize P_((MemThrdAnaDat *thrdanadat));
void FreeAMap P_((void));

#endif /* defined(TIERRA)||defined(ARGTIE)||
          defined(GENALIGN)||defined(THREADTREE)||
          defined(PROBE)||defined(SOUPUPDTRC) */

#if defined(TIERRA)||defined(ARGTIE)||\
    defined(GENALIGN)||defined(THREADTREE)||\
    defined(PROBE)||defined(CLSTRSRVR)||defined(SOUPUPDTRC)

extern void GetAMap P_((I8s * file));
extern time_t tietime P_((time_t *tloc));
extern int tisatty P_((int fd));
extern I8s *tgets P_((I8s *s, I32s size));
extern I8s *getdirnam P_((I8s *filnam));
extern I32s ByteXdrWrdSize P_((I32s bytelen));

#include "port_xdr.h"

#endif /* defined(TIERRA)||defined(ARGTIE)||
          defined(GENALIGN)||defined(THREADTREE)||
          defined(PROBE)||defined(CLSTRSRVR)||
          defined(SOUPUPDTRC) */

#if defined(TIERRA)||defined(PROBE)
extern I8s *Int2Lbl P_((I32s i));
extern I32s Lbl2Int P_((I8s * s));
#endif /* defined(TIERRA)||defined(PROBE) */

#if defined(TIERRA)||defined(ARGTIE)||defined(PROBE)
extern void WritAscFile P_((Pgl g, I8s *file, I32s tarpt,
    double sucsiznslrat, I32s sucsiznsl, I32s siznsl,
    I32s detailrpt, I32s clstrfmt, I32s symclstranafmt,
    I32s exeptrn, I32s *expttsarr, I32s expttsarrcnt,
    I16s *genelkup, GeneDefArr *gendef, I32s spltisana));
extern void WritEcoB P_((I32u bits, I8s * buf));
#endif /* defined(TIERRA)||defined(ARGTIE)||defined(PROBE) */

#if defined(TIERRA)||defined(ARGTIE)||defined(SOUPUPDTRC)
void Rpt_soup_update_event P_((FILE *fp, Pgl g, I32s i));
void Rpt_soup_update_event_head P_((FILE *fp));
void Rpt_Event_List_Offset
    P_((FILE *fp, I32s offsettype, I32s offset));
#endif /* defined(TIERRA)||defined(ARGTIE)||defined(SOUPUPDTRC) */

#endif /* BGL_CLNT */

#if defined(TIERRA)||defined(BGL_CLNT)||defined(CLSTRSRVR)
extern I32s tgettimeofday P_((struct timeval *tp, struct timezone *tzp));

#if defined(NET)||defined(BGL_CLNT)||defined(BGL)
extern struct hostent *tgethostbyaddr P_((I8s *addr, I32s len, I32s type));
extern I32s tsendto P_((I32s s, I8s *msg, I32s len,
    I32s flags, struct sockaddr *to, I32s tolen));
extern I32s trecv P_((I32s s, I8s *buf, I32s len, I32s flags));
extern int trecvfrom P_((I32s s, I8s *buf, I32s len, I32s flags,
    struct sockaddr *from, I32s *fromlen));
extern I32s tconnect P_((I32s s, struct sockaddr *name,
    I32s namelen, I32s remoteflag));
extern struct servent *tgetservbyname P_((I8s *name, I8s *proto));
#endif /* defined(NET)||defined(BGL_CLNT)||defined(BGL) */

#if defined(NET)||defined(BGL_CLNT)||\
    defined(BGL)||defined(TIEAUDIO)
extern struct protoent *tgetprotobyname P_((I8s *name));
extern I32s tsocket P_((I32s domain, I32s type, I32s protocol));
extern I32s tbind P_((I32s s, struct sockaddr *name, I32s namelen));
extern I32s tlisten P_((I32s s, I32s bklog));
extern I32s taccept P_((I32s s, struct sockaddr *addr, I32s *addrlen));
extern I32s tclosesocket P_((I32s fd));
extern I32s tsend P_((I32s s, I8s *msg, I32s len, I32s flags));
extern I32s tselect P_((I32s width, fd_set *readfds,
    fd_set *writefds, fd_set *exceptfds, struct timeval *tmeout));
extern I32s tsetsockopt P_((int s,int level,int optname,
    void *optval, socklen_t optlen));
extern I32s tgethostname P_((I8s *name, I32s namelen));
extern struct hostent *tgethostbyname P_((I8s *name));
#endif /* defined(NET)||defined(BGL_CLNT)||
          defined(BGL)||defined(TIEAUDIO) */

#ifdef unix
#ifdef SIGBLOCK
extern void T_sig_int P_((int sig));
extern void T_sig_read P_((int sig));
extern void T_sig_write P_((int sig));
extern void T_sig_info P_((int sig));
extern void SetSigSet P_((void));
extern sigset_t TSigBlock P_((sigset_t value));
extern sigset_t TSigQuery P_((void));
extern void TSigRelease P_((sigset_t value));
extern void SetAsyncSkt P_((int sock));
#ifndef TSysV
extern void TieSignal P_((I32s sigval, void (*sighndl)()));
#else /* TSysV */
extern void TieSignal P_((I32s sigval, void (*sighndl)(int)));
#endif /* TSysV */
#ifdef QRYSIGNALHNDLR
extern __sighandler_t QrySignalHndlr P_((I32s sigval));
#endif /* QRYSIGNALHNDLR */
#endif /* SIGBLOCK */
#endif /* unix */

#endif /* defined(TIERRA)||defined(BGL_CLNT)||defined(CLSTRSRVR) */

#if defined(TIERRA)||defined(ARGTIE)||\
    defined(GENALIGN)||defined(THREADTREE)||\
    defined(PROBE)||defined(BGL_CLNT)||\
    defined(CLSTRSRVR)||defined(SOUPUPDTRC)

extern FILE *tfopen P_((I8s *filename, I8s *type));
extern I32s tfclose P_((FILE *filhndl));
extern I32s tftell P_((FILE *stream));
extern I32s tfflush P_((FILE *filhndl));
extern I8s *tfgets P_((I8s *s, I32s n, FILE *filhndl));
extern I32s tseek P_((FILE * fp, long rpos, int mode));
extern I32s tfread P_((I8s *ptr, I32s size, I32s n, FILE *stream));
extern I32s tfwrite P_((I8s *ptr, I32s size, I32s n, FILE *stream));
extern I32s tclose P_((I32s fd));
extern I32s tfcntl P_((I32s fd, I32s cmd, I32s arg));
extern I32s tsprintf P_((char *buf, char *fmt, ...));
extern I32s tfprintf P_((FILE *buf, const char *fmt, ...));
extern I32s tunlink P_((I8s *path));
extern I32s tstat P_((char *path, struct stat *buf));
extern I8s Fp tcalloc P_((I32u num, I32u siz, I32s label));
extern void tfree P_((void *ptr, I32s siz, I32s label));
extern void thfree P_((void *ptr, I32s siz, I32s label));
extern I8s Hp thcalloc P_((I32u num, I32u siz, I32s label));
extern void *trecalloc P_((void *ptr, I32u nsiz, I32u osiz, I32s label));
extern void *threcalloc P_((void *ptr, I32u nsiz, I32u osiz, I32s label));
extern void txdrmem_create P_((XDR *xdrs, I8s *addr,
    I32u size, enum xdr_op op));
extern void txdr_destroy P_((XDR *xdrs));
extern I32u txdr_getpos P_((XDR *xdrs));
extern void txdr_free P_((xdrproc_t proc, I8s *objp));
extern I32s twrite P_((I32s fd, char *buf, I32s nbyte));
extern I32s tread P_((I32s fd, I8s *buf, I32s nbyte));
extern I32s tclose P_((I32s fd));
extern void porterrmsg P_((I32s errlbl, char *errmsg, I32s fatal));
extern void InitDynArr P_((DynArr *dynarr, I32s elsize, I32s indir));
extern I32s AddDynArrEl P_((DynArr *dynarr, I32s newelem,
   I32s allocincr, I32s memprof1, I32s memprof2));
extern void FreeDynArr P_((DynArr *dynarr, I32s memprof1, I32s memprof2));
extern void txdr_I32s P_((XDR *xdrs, I32s *objp));

/**********************************************************************
 * list structure function prototypes
 **********************************************************************/
void InitListHead P_((ListHead *listheadbuf));
void AddListNode P_(( ListNode *prevnode, ListNode *addnode));
void RemListNode P_((ListNode *remnode));
void RemHeadNode P_((ListHead *listheadbuf));
void AddTailNode P_((ListHead *listheadbuf, ListNode *listnodebuf));

#endif /* defined(TIERRA)||defined(ARGTIE)||
          defined(GENALIGN)||defined(THREADTREE)||
          defined(PROBE)||defined(BGL_CLNT)||
          defined(CLSTRSRVR)||defined(SOUPUPDTRC) */
#endif /* PROTOTYP_H */
