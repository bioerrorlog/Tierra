/* @(#)thrdana.c */
/* thrdana.c 30-3-98 thread analysis routines for the Tierra Simulator */
/* Tierra Simulator V5.0: Copyright (c) 1991 - 1998 */
/* MemProf labels: #22XX, in use: 2201-2231,2233-2235 */
/* FEError labels: #24XX, in use: 2400 */

#include "license.h"
#include "tierra.h"
#include "globals.h"

#if defined(TIERRA)||defined(ARGTIE)

typedef struct
{   I32s dvoft_devstat;
    I32s dvoft_offstgene;
    I32s dvoft_thrdid;
} DevOfstThd;

typedef struct
{   DevOfstThd *dvfta_devofsthd;
    DynArrCtrl  dvfta_ctrl;
} DevOfstThdArr;

#endif /* defined(TIERRA)||defined(ARGTIE) */

#if TIERRA
static void ChkThrdAnaDatRdy P_((Pcells cp));
static void ThrdAnaDatRdySv P_((Pcells cp));
static I32s ChkInclEvntList P_((I32s *genetrn));
static void PropCalLvlIntPromFlg P_((CallLvlIntrv *rtcalvlint));
static void PromClsfyOfstThdThd P_((
    MemGenThrdDatEl *segbti_el, I32s threadid));
static void PromClsfyOfstThdTrk P_((
    MemGenThrdDat *segbti_el1, I32s track));
static void PromClsfyOfstThdOfst P_((Pcells cp, I32s ofst));
static void PromClsfyOfstThd P_((Pcells cp));
static void FreeInstThrdCalLvlIntAr P_((Pcells cp));
static void FreeInstThrdCalLvlIntArOfst P_((Pcells cp, I32s ofst));
static void FreeCalLvlIntDataCell P_((Pcells cp));
static I32s FecundCondMet P_((I32s fecundity));
static I32s DeathCondMet P_((Pcells tc));

#ifdef NET
    static void Rpt_IPMap_Strt P_((FILE *fp));
#endif /* NET */

#endif /* TIERRA */

#if defined(TIERRA)||defined(ARGTIE)
static void write_FilThrdAnaDat P_((FILE *fp, FilThrdAnaDat *filthrdanadat));
static void WriteThrdAnaDatBti P_((MemThrdAnaDat *memthrdanadat,
    MemGenThrdDatArr *segbti, XDR *xdrhndl));
static void WriteCalLvlIntTree P_((CallLvlIntrv
    *rtcalvlint, XDR *xdrhndl));
static void WriteMemInstThrdArr P_((MemGenThrdDat
    *segbti_el, XDR *xdrhndl));
static void Rpt_soup_update_event_list P_((FILE *fp, Pgl g));
static void Rpt_gen_spec P_((FILE *fp, Pgl g,
    double sucsiznslrat, I32s sucsiznsl, I32s siznsl));
static void BldOfstThd P_((OfstThdArr *ofstthdarr,
    MemThrdAnaDat *memthrdanadat,I32s track,I32s detailrpt,
    I32s exeptrn,I32s cpysegflg,I32s gensize,
    I16s *genelkup,I32s spltisana,I32s tisbygen));
static void BldDevOfstThd P_((DevOfstThdArr *devofstthdarr,
    MemThrdAnaDat *memthrdanadat, I32s track, I32s detailrpt,
    I32s exeptrn, I32s cpysegflg, I32s gensize,
    I16s *genelkup, I32s spltisana, I32s tisbygen));
static I32s ChkExeThdTagExe P_((I32s ofst,
    MemThrdAnaDat *memthrdanadat,I32s cpysegflg,I16s *genelkup));
static I32s ChkExeThdTagThd P_((MemThrdAnaDat *memthrdanadat,
    I32s thrdid,I32s cpysegflg,I16s *genelkup,I32s offset,I32s track));
static I32s ChkThrdSpan P_((MemThrdAnaDat *memthrdanadat,I32s thrdid));
static void AddDevOfstThdArr P_((DevOfstThdArr *devofstthdarr,
    I32s devstat, I32s offset, I32s threadid,
    I32s tisbygen,I16s *genelkup));
static void AddThdTsTyArr P_((ThdTsTyArr *thdtstyarr,
    I32s thrdid, I32s tissuetype));
static void TypeTissue P_((OfstThdArr *ofstthdbuf,
    ThdTisArr *thdtisarr, ThdTsTyArr *thdtstyarr, BodyDef *body));
static void AddTissueDef P_((BodyDef *body,
    I32s intrvstrt, I32s npromthdticnt, I32s promthdticnt));
static void AddThdTisArr P_((ThdTisArr *thdtisarr,
    I32s thrd, I32s thdtistrt, I32s npromthdticnt, I32s promthdticnt));
static void Rpt_ofst_thd P_((FILE *fp, OfstThdArr *ofstthdarr,
    I32s codsegrpt, I32s trkidx, MemThreadDatArr *thdatara,
    I32s detailrpt, I32s clstrfmt, I32s exeptrn));
static void AddLine_ofst_thd P_((FILE *fp, OfstThdArr *ofstthdarr,
    I32s ofstthdidx, I32s codsegrpt, MemThreadDatArr *thdatara,
    I32s detailrpt, I32s clstrfmt, I32s exeptrn));
static void BldThdTisArr P_((ThdTisArr *thdtisarr,
    OfstThdArr *ofstthdarr, I32s detailrpt, I32s exeptrn));
static void Rpt_thd_ofst P_((FILE *fp, OfstThdArr *ofstthdarr,
    I32s codsegrpt, I32s skipline, I32s trkidx,
    MemThreadDatArr *thdatara,
    I32s detailrpt, I32s clstrfmt, I32s exeptrn));
static void Rpt_thd_dat P_((FILE *fp, MemThrdAnaDat *memthrdanadat,
    ThdTsTyArr *thdtstyarr, I32s detailrpt, I32s exeptrn, I32s tisbygen));
static void Rpt_event P_((FILE *fp, Pgl g, I32s i,
    I16s *genelkup, GeneDefArr *gendef));
static void Rpt_event_list P_((FILE *fp, Pgl g,
    I16s *genelkup, GeneDefArr *gendef));
static void AddOfstThd P_((OfstThdArr *ofstthdarr,
    I32s thrdid, I32s intrvstrt, I32s intrvend, I32s devstat));
static void Rpt_Tissue_Types P_((FILE *fp,
    BodyDef *body, OfstThdArr *ofstthdbuf,
    I32s tisbygen, GeneDefArr *gendef));
static void Rpt_Tissue_Similarity P_((FILE *fp,
    BodyDef *body, OfstThdArr *ofstthdarr,
    I32s clstrfmt, I32s symclstranafmt,
    double *clsimtx,I32s tisbygen));
static void Rpt_Tissue_Similarity_cols P_((FILE *fp,
    BodyDef *body, OfstThdArr *ofstthdarr, I32s tisrow,
    I32s rowstrtis, I32s rowdevidx, I32s rowtissize,
    I32s clstrfmt, I32s tiscolstrt, I32s tiscolend,
    I32s colstrtisstrt, I32s coldevidxstrt,
    I32s symclstranafmt, I32s *colstrtisend,
    I32s *coldevidxend,double *clsimtx));
static I32s CalcIntrvalOverlap P_((
    I32s start1, I32s end1, I32s start2, I32s end2));
static int CmpDevOfstThdArr P_((const void *a, const void *b));
static int cmp_ofst_thd_dev P_((const void *i, const void *j));
static int cmp_thd_dev_ofst P_((const void *i, const void *j));
static int cmp_thd_ofst_dev P_((const void *i, const void *j));

#ifdef GRPNARROWTIS
static void ClstrNarrTis P_((double *clsimtx,
    I32s strtiscnt, FILE *fp, I32s tisbygen));
static void PrintNarTiss P_((ClstrArr *clstrarr,
    int nartiscnt,FILE *fp,int strtiscnt, int tisbygen));
static void CollNarrStrTiss P_((Clstr *rtclstr,
    int *narstrtisar,int *narstrtiscnt));
static void PrintNarrStrTiss P_((FILE *fp,
    int *narstrtisar,int narstrtiscnt,int nartisid));
static int narstrtiscmp P_((const void *i,const void *j));
static void InitClstrArray P_((ClstrArr *clstrarr,int clstrcnt));
static void FreeClstrArray P_((ClstrArr *clstrarr));
static void FreeClstrTree P_((Clstr *rtclstr));
static double *CalcClstrDistMtx P_((double *clsimtx,int clstrcnt,int ndims));
#ifdef CLSTRANADBG
static void DumpClstrArray P_((ClstrArr *clstrarr,int clstrcnt));
static void DumpClstrTree P_((Clstr *rtclstr,int level));
static void DumpClstrSimMtx P_((double *clsimtx,int clstrcnt,int ndims));
static void DumpClstrDistMtx P_((double *clsdstmtx,int clstrcnt));
#endif /* CLSTRANADBG */
#endif /* GRPNARROWTIS */
#endif /* defined(TIERRA)||defined(ARGTIE) */

#if defined(TIERRA)||defined(ARGTIE)||\
    defined(GENALIGN)||defined(THREADTREE)||\
    defined(PROBE)||defined(SOUPUPDTRC)

static void FreeMemGenThrdDatArr P_((
    MemGenThrdDatArr *segbtidynarr, I32s memprof1,
    I32s memprof2, I32s memprof3, I32s memprof4));
static void FreeInstThrdCalLvlIntArTrk P_((
    MemGenThrdDat *segbti_el1, I32s track, I32s dbgoffset));
static void FreeCalLvlIntTree P_((CallLvlIntrv *rtcalvlint));
static void InitThrdAnaDynArr P_((MemThrdAnaDat *memthrdanadat));
static void read_FilThrdAnaDat P_((FILE *fp, FilThrdAnaDat *filthrdanadat));
static void ReadThrdAnaDatBti P_((MemThrdAnaDat *
    memthrdanadat, I32s seg_ct, MemGenThrdDatArr *segbti,
    I32s segincr, I32s memprof1, I32s memprof2,
    I32s memprof3, I32s memprof4, CalLvlIntByIDArr *
    calvlintrvlkupid, XDR *xdrhndl));
static void ReadCalLvlIntByIDArr P_((
    FilInstrThrd *filinstrthrdbuf, MemInstThrd *mita_el,
    CalLvlIntByIDArr *calvlintrvlkupid,
    MemThrdAnaDat *memthrdanadat, XDR *xdrhndl));
static void ReadCalLvlIntTree P_((
    CallLvlIntrv **rtcalvlint, CalLvlIntByIDArr
        *calvlintrvlkupid, XDR *xdrhndl));
static I32s AllocThrdAna P_((I32s segofst, MemGenThrdDatArr *segbti,
    I32s segincr, I32s trkidx, I32s thrdid, I32s memprof1,
    I32s memprof2, I32s memprof3, I32s memprof4));
static void AddCalLvlIntByIDArr P_((MemInstThrd  *mita_el,
   MemThrdAnaDat *memthrdanadat, CallLvlIntrv *crcalvlint));
static void AddMemInstThrdArr P_((MemInstThrdArr *thrdara,
    I32s thrdid, I32s memprof1, I32s memprof2));

#endif /* defined(TIERRA)||defined(ARGTIE)||
          defined(GENALIGN)||defined(THREADTREE)||
          defined(PROBE)||defined(SOUPUPDTRC) */

#if TIERRA

/*
 * ThrdAnaCollTst - check if currently executing cell should have thread
 *                  analysis data collected for it
 *
 * thread analysis data is collected for each cell which meets the following
 * criteria :
 *    the global ThreadAnalysis parameter is non-zero
 *    cell must be currently executing (adult)
 *    thread analysis data collection is not already complete for this cell
 *    no instruction operation flaws have occurred for the cell
 *    no mutations except for self-induced mutations have occurred in the cell
 *    the birth of this cell must have occurred following the last time
 *       thread analysis data collection was put into effect
 *
 * Returns: 0 - thread analysis data should not be collected
 *          1 - thread analysis data should be collected
 */
I32s ThrdAnaCollTst()
{   I32s collflg;

    collflg=0;
    if(((GeneBnker)&&(ThreadAnalysis)&&(!(ce->d.thrdanacolldone))))
    {   if((!(ce->d.flaw))&&(!(ce->d.nonslfmut)))
        {   if((ce->d.birthtime)>ThrdAnaTimeLastOn)
                collflg=1;
        }
        else
        {   /* free cell data */
            FreeThrdAnaDatCell(ce);

            /* mark cell data collection complete */
            ce->d.thrdanacolldone=1;
        }
    }
    return collflg;
}

/*
 * AllocThrdAnaCpy - allocate thread analysis data structures
 *                   for copied instructions
 * memthrdanadat - pointer to MemThrdAnaDat structure for cell or
 *                 g_list structure in ram bank.
 * cpysegofst - cell offset of copied instruction
 */
I32s AllocThrdAnaCpy(memthrdanadat, cpysegofst)
MemThrdAnaDat *memthrdanadat;
I32s cpysegofst;
{   return AllocThrdAna(cpysegofst, &(memthrdanadat->
        mtad_cpysegbti), COPY_SEG_INCR,
#if PLOIDY == 1
        0,
#else /* PLOIDY == 1 */
        ce->c.c->so,
#endif /* PLOIDY == 1 */
        is.othreadid, 2205, 2206, 2208, 2221);
}

/*
 * ThrdAnaCollDone - check for thread analysis data collection completion
 *                   for given cell
 *
 * tc - pointer to given cell
 * deathflag - 0 = not called at death of given cell
 *             1 = called at death of given cell
 * migrflag  - 0 = not called at migration of given cell
 *             1 = called at migration of given cell
 *
 * Returns: 0 - thread analysis data collection not complete for given cell
 *          1 - thread analysis data collection complete for given cell
 */
I32s ThrdAnaCollDone(tc, deathflag, migrflag)
Pcells tc;
I32s deathflag, migrflag;
{   I32s rtnval=0;
    I32s fecundity=tc->d.fecundity;

    if((fecundity)>=1)
    {   if((deathflag)&&(ThrdAnaPartSave))
        {   if(ThrdAnaMaxFec>=0)
                rtnval=1;
        }
        else
        {   switch(ThrdAnaStop)
            {   case 0:
                    if(FecundCondMet(fecundity))
                        if(ThrdAnaMaxFec>=0)
                            rtnval=1;
                    break;
                case 1: /* death */
                    if(FecundCondMet(fecundity))
                        if(deathflag)
                            rtnval=DeathCondMet(tc);
                    break;
                case 2: /* migration */
                    if(FecundCondMet(fecundity))
                        if(deathflag&&migrflag)
                            rtnval=1;
                    break;
                case 3: /* remote divide */
                    if(FecundCondMet(fecundity))
                        if((is.divide)&&(is.onodetype))
                            rtnval=1;
                    break;
                case 4: /* local divide */
                    if(FecundCondMet(fecundity))
                        if((is.divide)&&(!(is.onodetype)))
                            rtnval=1;
                    break;
                case 5: /* stop on complete life cycle */
                    rtnval=(deathflag)?DeathCondMet(tc):
                        FecundCondMet(fecundity);
                    break;
                default:
                    break;
            }
        }
    }
    return rtnval;
}

/*
 * FecundCondMet - check if fecundity conditions are met
 * Returns: 0 - fecundity conditions are not met
 *          1 - fecundity conditions are met
 */
static I32s FecundCondMet(fecundity)
I32s fecundity;
{   return(((ThrdAnaMaxFec>0)&&(fecundity==ThrdAnaMaxFec))||
          ((!ThrdAnaMaxFec)&&(fecundity>=FecundityAvg))||
          (ThrdAnaMaxFec<0))?1:0;
}

/*
 * DeathCondMet - check if cause of death conditions are met
 * Returns: 0 - conditions are not met
 *          1 - conditions are met
 */
static I32s DeathCondMet(tc)
Pcells tc;
{   return((!ThrdAnaTrmCode)?1:(((tc->
        cell_thrdanadat.mtad_base.
        btad_extrnterm)==ThrdAnaTrmCode)?1:0));
}

/*
 * ThrdAnaDatCollMov - collect thread analysis data for copied instruction
 *
 * srccpysegofst - cell offset of source instruction
 * cpysrc - soup address of source instruction
 * cpydst - soup address of copy destination
 * 
 */
void ThrdAnaDatCollMov(srccpysegofst, cpysrc, cpydst)
I32s srccpysegofst, cpysrc, cpydst;
{   I32s mmp=ce->mm.p;
    I32s mms=ce->mm.s;
    I32s mdp=ce->md.p;
    I32s mds=ce->md.s;

    if(ThrdAnaCollTst())
    {   if((cpysrc>=mmp)&&(cpysrc<(mmp+mms))&&
            (mds)&&(cpydst>=mdp)&&(cpydst<(mdp+mds)))
        {   AllocThrdAnaCpy(&(ce->cell_thrdanadat), srccpysegofst);

            /* thread analysis data collection */
            if((ce->cell_thrdanadat.
                mtad_cpysegbti.mgda_segbti[srccpysegofst]
#if PLOIDY == 1
                ->
#else /* PLOIDY == 1 */
                [ce->c.c->so].
#endif /* PLOIDY == 1 */
                mgdt_thrd_ara.mita_array)[is.othreadid])
                (ce->cell_thrdanadat.mtad_base.
                    btad_FilInstrThrd_ct)++;
        }
    }
}

/*
 * ThrdAnaTdtBits - set flag bits for cell_thrdanadat.thdatara structure
 *                     for currently executing cell
 *                  similar to "bits" element in g_list structure
 *                  but indexed by thread id
 */
void ThrdAnaTdtBits(thrdanacolflg, bitnum)
I32s thrdanacolflg;
I32u bitnum;
{   if(thrdanacolflg)
        ce->cell_thrdanadat.mtad_thdatara.mtda_thdatara[
            is.othreadid]->mtdt_basthrdat.tdt_bits|=I32UONE<<bitnum;
}

/*
 * ThrdAnaGenExMov - set cell_thrdanadat.thdatara structure flag bits
 *                   for instruction being copied for currently executing cell
 *                   (similar to "bits" element in g_list structure
 *                      but indexed by thread id)
 *                   also increment copy counts for copied instruction
 * thrdanacolflg - 0 = thread analysis data is not being collected for the
 *                     current cell
 *                 1 = thread analysis data is being collected for the current
 *                     cell
 * srccpysegofst - cell offset of source instruction
 * cpysrc        - soup address of source instruction
 * cpydst        - soup address of copy destination
 * who (source address is in):
 *    0 - same cell
 *    1 - own daughter
 *    2 - another cell
 *    3 - free memory
 *    4 - daughter of another cell
 */
void ThrdAnaGenExMov(thrdanacolflg, srccpysegofst, cpysrc, cpydst, whobit)
I32s thrdanacolflg, srccpysegofst, cpysrc, cpydst;
I32u whobit;
{   I32s mmp=ce->mm.p;
    I32s mms=ce->mm.s;
    I32s mdp=ce->md.p;
    I32s mds=ce->md.s;
    MemThreadDat *thdatara_el;
    MemGenThrdDat *cpysegbti_el=0;

    if(thrdanacolflg)
    {   thdatara_el=ce->cell_thrdanadat.mtad_thdatara.
            mtda_thdatara[is.othreadid];
        thdatara_el->mtdt_basthrdat.tdt_bits|=(I32UONE<<whobit);
        if((cpysrc>=mmp)&&(cpysrc<(mmp+mms))&&
            mds&&(cpydst>=mdp)&&(cpydst<(mdp+mds)))
        {   cpysegbti_el=ce->cell_thrdanadat.
                mtad_cpysegbti.mgda_segbti[srccpysegofst];
#if PLOIDY == 1
            (cpysegbti_el->mgdt_gbdt.gdt_opcnt)++;
#else /* PLOIDY == 1 */
            (cpysegbti_el[ce->c.c->so].mgdt_gbdt.gdt_opcnt)++;
#endif /* PLOIDY == 1 */
        }
        if((ce->d.fecundity)<1)
        {   (thdatara_el->mtdt_basthrdat.tdt_dght1bytcpd)++;
            if((cpysrc>=mmp)&&(cpysrc<(mmp+mms))&&
                mds&&(cpydst>=mdp)&&(cpydst<(mdp+mds)))
            {
#if PLOIDY == 1
                (cpysegbti_el->mgdt_gbdt.gdt_dght1opcnt)++;
#else /* PLOIDY == 1 */
                (cpysegbti_el[ce->c.c->so].mgdt_gbdt.gdt_dght1opcnt)++;
#endif /* PLOIDY == 1 */
            }
        }
        else if((ce->d.fecundity)<2)
        {   (thdatara_el->mtdt_basthrdat.tdt_dght2bytcpd)++;
            if((cpysrc>=mmp)&&(cpysrc<(mmp+mms))&&
                mds&&(cpydst>=mdp)&&(cpydst<(mdp+mds)))
            {
#if PLOIDY == 1
                (cpysegbti_el->mgdt_gbdt.gdt_dght2opcnt)++;
#else /* PLOIDY == 1 */
                (cpysegbti_el[ce->c.c->so].mgdt_gbdt.gdt_dght2opcnt)++;
#endif /* PLOIDY == 1 */
            }
        }
    }
}

/*
 * ThrdAnaGenExUsed - set cell_thrdanadat.thdatara structure flag bits
 *                    for instruction being executed for current cell
 *                    (similar to "bits" element in g_list structure
 *                       but indexed by thread id)
 *                    also increment operation counts for executed instruction
 * thrdanacolflg - 0 = thread analysis data is not being collected for the
 *                     current cell
 *                 1 = thread analysis data is being collected for the current
 *                     cell
 */
void ThrdAnaGenExUsed(thrdanacolflg, addr)
I32s thrdanacolflg, addr;
{   I32s mgp=ce->d.mg.p;
    I32s mgs=ce->d.mg.s;
    MemGenThrdDat *codsegbti_el;
    I32s curcelofst;

    if(thrdanacolflg)
    {   curcelofst=addr-(ce->mm.p);
        if((curcelofst>=mgp)&&(curcelofst<(mgp+mgs)))
        {   codsegbti_el=&(ce->cell_thrdanadat.
                mtad_codsegbti.mgda_segbti[curcelofst][is.oextrk]);
            (codsegbti_el->mgdt_gbdt.gdt_gbits)|=1;
            (codsegbti_el->mgdt_gbdt.gdt_opcnt)++;
        }
    }
}

/*
 * ThrdAnaDatColl - collect thread analysis data for executed instruction
 *
 * deathflag - 0 = not called at death of given cell
 *             1 = called at death of given cell
 * migrflag  - 0 = not called at migration of given cell
 *             1 = called at migration of given cell
 */
void ThrdAnaDatColl(deathflag, migrflag, addr)
I32s deathflag, migrflag, addr;
{   MemThrdAnaDat *memthrdanadat;
    MemThreadDat *thdatara_el;
    I32s mgp=ce->d.mg.p, mgs=ce->d.mg.s, curcelofst;
    MemGenThrdDat *codsegbti_el=0;
    MemInstThrd  *mita_el;

    if(ThrdAnaCollTst())
    {   memthrdanadat=&(ce->cell_thrdanadat);
        thdatara_el=memthrdanadat->mtad_thdatara.
            mtda_thdatara[is.othreadid];
        curcelofst=addr-(ce->mm.p);
        if((curcelofst>=mgp)&&(curcelofst<(mgp+mgs)))
        {   codsegbti_el=memthrdanadat->
                mtad_codsegbti.mgda_segbti[curcelofst];
            if((mita_el=memthrdanadat->mtad_codsegbti.
                mgda_segbti[curcelofst][is.oextrk].
                mgdt_thrd_ara.mita_array[is.othreadid]))
                (memthrdanadat->mtad_base.btad_FilInstrThrd_ct)++;
            if((sl[ce->d.gen.size]->genelkup)&&(SplitTissueAna))
                AddCalLvlIntByIDArr(mita_el, memthrdanadat, is.crcalvlint);
        }

        if(addr==is.oip)
            (thdatara_el->mtdt_basthrdat.tdt_instex)++;

        if((curcelofst>=mgp)&&(curcelofst<(mgp+mgs)))
        {   if((is.odem.fecundity)<1)
            {   if(addr==is.oip)
                    (thdatara_el->mtdt_basthrdat.tdt_dght1instex)++;
                (codsegbti_el[is.oextrk].mgdt_gbdt.gdt_dght1opcnt)++;
            }
            else if((is.odem.fecundity)<2)
            {   if(addr==is.oip)
                    (thdatara_el->mtdt_basthrdat.tdt_dght2instex)++;
                (codsegbti_el[is.oextrk].mgdt_gbdt.gdt_dght2opcnt)++;
            }
        }

        if(addr==is.oip)
        {   if((thdatara_el->mtdt_basthrdat.tdt_fstinstex)<0)
            {   thdatara_el->mtdt_basthrdat.tdt_fstinstex=is.hipofst;
#ifdef HAVE_TDT_FST_LST_OFSTTYP
                thdatara_el->mtdt_basthrdat.tdt_fstofsttyp=is.hipofsttype;
#endif /* HAVE_TDT_FST_LST_OFSTTYP */
                thdatara_el->mtdt_basthrdat.tdt_fstInstP=ce->d.instP;
            }
            thdatara_el->mtdt_basthrdat.tdt_lstinstex=is.hipofst;
#ifdef HAVE_TDT_FST_LST_OFSTTYP
            thdatara_el->mtdt_basthrdat.tdt_lstofsttyp=is.hipofsttype;
#endif /* HAVE_TDT_FST_LST_OFSTTYP */
            thdatara_el->mtdt_basthrdat.tdt_lstInstP=ce->d.instP;
            if(CpuHalt||CellHalt)
            {   thdatara_el->mtdt_basthrdat.tdt_termcause=is.di;
                if(CellHalt)
                {   memthrdanadat->mtad_base.btad_termcause=is.di;
                    memthrdanadat->mtad_base.btad_termthrdid=is.othreadid;
                    memthrdanadat->mtad_base.btad_termInstP=ce->d.instP;
                }
            }
        }

        /* if cell thread analysis data */
        /*     collection is complete */
        if(ThrdAnaCollDone(ce, deathflag, migrflag))
            ThrdAnaDatSave(ce);
    }
}

/*
 * ThrdAnaDatSave - save thread analysis data of given cell to ram bank under
 *                  original genotype of cell on completion of data collection
 *                  for given cell.
 *                  Also check "thread analysis data ready" break trap
 *
 * tc - pointer to given cell
 */
void ThrdAnaDatSave(tc)
Pcells tc;
{   GList *anatgl;

    if((ThreadAnalysis)&&((tc->d.birthtime)>ThrdAnaTimeLastOn)&&
        (GeneBnker)&&(!(tc->d.nonslfmut))&&(!(tc->d.flaw))&&
        (!(tc->d.thrdanacolldone)))
    {
        extcelcallvlintrvl(tc);
        PropCalLvlIntPromFlg(tc->
            cell_thrdanadat.mtad_rtcalvlint);
        PromClsfyOfstThd(tc);
        FreeCalLvlIntDataCell(tc);

        anatgl=sl[tc->d.gen.size]->g[tc->d.origgi];

        /* free any old gb thrd analysis data */
        FreeThrdAnaDat(&(anatgl->glst_thrdanadat));

        /* move cell thrd analysis data to gb */
        anatgl->glst_thrdanadat=tc->cell_thrdanadat;

        InitThrdAnaDat(&(tc->cell_thrdanadat));

        tc->d.thrdanacolldone=1;

#ifdef MICRO
        ChkThrdAnaDatRdy(tc);
#endif /* MICRO */
    }
}

/*
 * ThrdAnaDatDump - delete all thread analysis data from all cells
 *                  and ram bank
 */
void ThrdAnaDatDump()
{   I32s si, gi, ar, ci;
    Pcells cp;
    SList *slel;
    GList **g, *tgl;

    for(si=0;si<siz_sl;si++)
    {   slel=sl[si];
        if(slel)
        {   g=slel->g;
            if(g)
                for(gi=0;gi<(slel->a_num);gi++)
                {   if(!TNULL(tgl=g[gi]))
                        FreeThrdAnaDat(&(tgl->glst_thrdanadat));
                }
        }
    }

    for(ar=0; ar<NumCelAr; ar++)
        for (ci=0; ci<CelArSiz; ci++)
        {   if((ar==0)&&(ci<2))
                continue;
            cp=&cells[ar][ci];
            if(cp->ld)
                FreeThrdAnaDatCell(cp);
        }

    if(BrkTrapCondBuf.curbrkdat.BrkSet)
        if((BrkTrapCondBuf.curbrkdat.
            TrapType)==BRKTYP_THRDANADATRDY)
                GoBrkClr();

    initrcnttardy();
}

/*
 * AllocThrdAnaEvent - record thread analysis "instruction execution" 
 *                     event in thread analysis data
 *
 * memthrdanadat - pointer to MemThrdAnaDat structure for cell or
 *                 g_list structure in ram bank.
 */
void AllocThrdAnaEvent(memthrdanadat)
MemThrdAnaDat *memthrdanadat;
{   ThrdAnaEvent *thrdanaevt;
    I8s *mn=&((id[is.di].mn)[0]);
    I32s genetrn;

    if(!ce->c.c->fl.E)
        if(ChkInclEvntList(&genetrn))
            if(ThrdAnaCollTst())
                if(!(((!strcmp((const char *)mn,(const char *)"surf"))||
                    (!strcmp((const char *)mn,(const char *)"surff")))&&
                    (!(is.oi32a))))
                {   thrdanaevt=(memthrdanadat->mtad_event.taea_event)[
                        AddDynArrEl((DynArr *)(&(memthrdanadat->mtad_event)),
                        -1, THREADID_INCR, 2210, 2211)];
                    thrdanaevt->taev_eventcode=(I8u)is.di;
                    thrdanaevt->taev_ipoffset=is.hipofst;
                    thrdanaevt->taev_ipoffsettype=is.hipofsttype;
                    thrdanaevt->taev_thrd=is.othreadid;
                    thrdanaevt->taev_dstoffset=is.odstoffset;
                    thrdanaevt->taev_dstoffsettype=is.odstoffsettype;
                    thrdanaevt->taev_nodetype=is.onodetype;
                    thrdanaevt->taev_i32a=is.oi32a;
#ifdef USE_PORT
                    thrdanaevt->taev_port=is.oport;
#endif /* USE_PORT */
                    thrdanaevt->taev_instP=ce->d.instP;
                    thrdanaevt->taev_genetrn=(I8u)genetrn;
                }
}

/*
 * ChkInclEvntList - check for inclusion in event list
 *
 * genetrn - pointer to gene transition flag
 *
 * Returns: inclevnt - 0 = omit from event list
 *                     1 = include in event list
 *          *genetrn - 1 = gene transition
 */
static I32s ChkInclEvntList(genetrn)
I32s *genetrn;
{   I32s inclevnt=0;
    I8s *mn=&((id[is.di].mn)[0]);
    I32s mgp=ce->d.mg.p;
    I32s mgs=ce->d.mg.s;

    (*genetrn)=0;
    if(!(is.hipofsttype))
        if(((is.hipofst)>=mgp)&&((is.hipofst)<(mgp+mgs)))
            if((ce->c.c->curgene)!=(ce->c.c->prvgene))
                (*genetrn)=1;

    if( !strcmp((const char *)mn,(const char *)"call") ||
        !strcmp((const char *)mn,(const char *)"csync")   ||
        !strcmp((const char *)mn,(const char *)"divide")  ||
        !strcmp((const char *)mn,(const char *)"getipp")  ||
        !strcmp((const char *)mn,(const char *)"getippf") ||
        !strcmp((const char *)mn,(const char *)"halt")    ||
        !strcmp((const char *)mn,(const char *)"ifgrtr")  ||
        !strcmp((const char *)mn,(const char *)"mal")     ||
        !strcmp((const char *)mn,(const char *)"movdi4")  ||
        !strcmp((const char *)mn,(const char *)"offAACD") ||
        !strcmp((const char *)mn,(const char *)"offBBCD") ||
        !strcmp((const char *)mn,(const char *)"ret")     ||
        !strcmp((const char *)mn,(const char *)"split")   ||
        !strcmp((const char *)mn,(const char *)"surf")    ||
        !strcmp((const char *)mn,(const char *)"surff"))
        inclevnt=1;
    else
        if(*genetrn)
            inclevnt=1;
    return inclevnt;
} 

/*
 * AllocSoupUpdEvent - record soup memory update event in
 *                     thread analysis data
 *
 * memthrdanadat - pointer to MemThrdAnaDat structure for cell or
 *                 g_list structure in ram bank.
 */
void AllocSoupUpdEvent(memthrdanadat, src, dest)
MemThrdAnaDat *memthrdanadat;
I32s src, dest;
{   SoupUpdEvent *soupupdevt;
    Pcells cp;

    if(SouUpdTrk)
        if(!ce->c.c->fl.E)
            if(ThrdAnaCollTst())
            {   soupupdevt=(memthrdanadat->mtad_soupupdevt.suea_event)[
                    AddDynArrEl((DynArr *)(&(memthrdanadat->mtad_soupupdevt)),
                    -1, THREADID_INCR, 2222, 2223)];
                soupupdevt->supd_eventcode=(I8u)is.di;
                soupupdevt->supd_ipoffset=is.hipofst;
                soupupdevt->supd_ipoffsettype=is.hipofsttype;
                soupupdevt->supd_extrk=(I8u)(is.oextrk);
                soupupdevt->supd_thrd=is.othreadid;
                if(src<0)
                {   soupupdevt->supd_srcoffsettype=-1;
                    soupupdevt->supd_srcoffset=-1;
                }
                else
                    soupupdevt->supd_srcoffsettype=(I8u)
                        WhoIs(&cp, src, &(soupupdevt->supd_srcoffset));
                soupupdevt->supd_stra=is.stra;
                soupupdevt->supd_dstoffsettype=(I8u)
                   WhoIs(&cp, dest, &(soupupdevt->supd_dstoffset));
                soupupdevt->supd_dtra=is.dtra;
                soupupdevt->supd_instP=ce->d.instP;
            }
}

/*
 * entcallvlintrvl - set call level structure elements on level interval entry
 */
void entcallvlintrvl()
{   CallLvlIntrv *calvlint=(CallLvlIntrv *)
        thcalloc(1, sizeof(CallLvlIntrv), 2217);
    CallLvlIntrv *crcalvlint=ce->c.c->cpu_crcalvlint;

    if(!(ce->cell_thrdanadat.mtad_rtcalvlint))
    {   calvlint->clv_base.clb_parentid=(-1);
        ce->cell_thrdanadat.mtad_rtcalvlint=calvlint;
    }
    else
    {   AddTailNode((ListHead *)(&(crcalvlint->clv_subintrv)),
            (ListNode *)(&(calvlint->clv_listnode)));
        (crcalvlint->clv_base.clb_subintrvcnt)++;
        calvlint->clv_parent=crcalvlint;
        calvlint->clv_base.clb_parentid=crcalvlint->clv_base.clb_id;
    }
    InitListHead((ListHead *)(&(calvlint->clv_subintrv)));
    calvlint->clv_base.clb_id=(ce->cell_thrdanadat.
        mtad_base.btad_callvlintcnt)++;
    calvlint->clv_base.clb_celcallvl=ce->c.c->curcallvlcell;
    calvlint->clv_base.clb_thdcallvl=ce->c.c->curcallvlthrd;
    calvlint->clv_base.clb_entinp=ce->d.instP;
    calvlint->clv_base.clb_enthipofsttype=is.hipofsttype;
    calvlint->clv_base.clb_enthipofst=is.hipofst;
    ce->c.c->cpu_crcalvlint=calvlint;
    ce->c.c->cpu_crcalvlid=calvlint->clv_base.clb_id;
}

/*
 * extcallvlintrvl - set call level structure elements on
 *                   call level interval exit
 *
 * cp - pointer to cell of cpu of call level interval to exit
 * cpu - pointer to cpu of call level interval to exit
 */
void extcallvlintrvl(cp, cpu)
Pcells cp;
Cpu *cpu;
{   CallLvlIntrv *crcalvlint=cpu->cpu_crcalvlint;
    if(crcalvlint)
    {   crcalvlint->clv_base.clb_extinp=cp->d.instP;
        crcalvlint->clv_base.clb_exthipofsttype=is.hipofsttype;
        crcalvlint->clv_base.clb_exthipofst=is.hipofst;
        cpu->cpu_crcalvlint=(crcalvlint->
            clv_base.clb_thdcallvl)?crcalvlint->clv_parent:NULL;
        if(crcalvlint->clv_base.clb_thdcallvl)
        {   cpu->cpu_crcalvlint=crcalvlint->clv_parent;
            cpu->cpu_crcalvlid=crcalvlint->
                clv_parent->clv_base.clb_id;
        }
    }
}

/*
 * extthdcallvlintrvl - exit all call level intervals of thread
 *
 * cp - pointer to cell of cpu of call level interval to exit
 * cpu - pointer to cpu of call level interval to exit
 */
void extthdcallvlintrvl(cp, cpu)
Pcells cp;
Cpu *cpu;
{   while(cpu->cpu_crcalvlint)
        extcallvlintrvl(cp, cpu);
}

/*
 * extcelcallvlintrvl - exit all current call level intervals for
 *                      all threads of given cell
 *
 * cp - pointer to given cell
 */
void extcelcallvlintrvl(cp)
Pcells cp;
{   int i;
    if(!(cp->cell_thrdanadat.mtad_rtcalvlint))
        for(i=0;i<(cp->c.n);i++)
            extthdcallvlintrvl(cp, &(cp->c.ar[i]));
}

/*
 * CallLvlIntrvlDevChk - check call level interval for dev gene call
 *                       and flag interval if necessary
 */
void CallLvlIntrvlDevChk()
{   SList *psl=sl[ce->d.gen.size];
    I16s *genelkup, gene;
    Cpu *cpu=ce->c.c;

    if(SplitTissueAna)

        /* if previous instruction was indirect call */
        if(((cpu->curcallvlcell)>1)&&(cpu->prvcallins))
    
            /* if in dev gene */
            if((genelkup=psl->genelkup))
                if(!(is.hipofsttype))
                    if((gene=genelkup[is.hipofst])>=0)
                        if(psl->slst_gendef.gdf_array[gene].gdf_typ)
                            /* flag call level interval
                               interval as promoted */
                            ce->c.c->cpu_crcalvlint->
                               clv_parent->clv_base.clb_prom=1;
}

/*
 * PropCalLvlIntPromFlg - propagate call level interval promotion flags
 *                        for given call level interval tree
 *
 * rtcalvlint - pointer to root of given call level interval tree
 *
 */
static void PropCalLvlIntPromFlg(rtcalvlint)
CallLvlIntrv *rtcalvlint;
{   CallLvlIntrv *listend, *crcalvlint, *parent;

    if((parent=rtcalvlint->clv_parent))
        if(parent->clv_base.clb_prom)
           rtcalvlint->clv_base.clb_prom=1;
    listend=(struct CallLvlIntrv *)
        (&(rtcalvlint->clv_subintrv.lsh_firstlast));
    for(crcalvlint=(CallLvlIntrv *)(rtcalvlint->
        clv_subintrv.lsh_first);crcalvlint!=listend;
        crcalvlint=(CallLvlIntrv *)(crcalvlint->
        clv_listnode.lnd_next))
        PropCalLvlIntPromFlg(crcalvlint);
}

/*
 * PromClsfyOfstThd - classify  all offset/thread combinations 
 *                     as promoted, nonpromoted, or both for given cell
 *
 * cp - pointer to given cell
 *
 */
static void PromClsfyOfstThd(cp)
Pcells cp;
{   I32s i;
    if(cp->cell_thrdanadat.mtad_codsegbti.mgda_segbti)
        /* for each offset */
        for(i=0;i<(cp->cell_thrdanadat.
            mtad_codsegbti.mgda_ctrl.dync_elmaloc);i++)
            PromClsfyOfstThdOfst(cp,i);
}

/*
 * PromClsfyOfstThdOfst - classify  all offset/thread combinations 
 *                         as promoted, nonpromoted, or both for given offset
 *                         in given cell
 *
 * cp   - pointer to given cell
 * ofst - given offset
 *
 */
static void PromClsfyOfstThdOfst(cp, ofst)
Pcells cp;
I32s ofst;
{   I32s i;
    MemGenThrdDat *segbti_el;
    if((segbti_el=cp->cell_thrdanadat.mtad_codsegbti.mgda_segbti[ofst]))
        /* for each track */
        for(i=0;i<PLOIDY;i++)
            PromClsfyOfstThdTrk(segbti_el, i);
}

/*
 * PromClsfyOfstThdTrk - classify  all offset/thread combinations 
 *                        as promoted, nonpromoted, or both for given track
 *
 * segbti_el1 - pointer to MemGenThrdDat structure
 *                         (MemGenThrdDatEl array)
 * track - given track
 *
 */
static void PromClsfyOfstThdTrk(segbti_el1, track)
MemGenThrdDat *segbti_el1;
I32s track;
{   I32s i;
    MemGenThrdDatEl *segbti_el2=&(segbti_el1[track]);
    if(segbti_el2->mgdt_thrd_ara.mita_array)
        /* for each thread */
        for(i=0;i<(segbti_el2->mgdt_thrd_ara.
            mita_ctrl.dync_elmaloc);i++)
            PromClsfyOfstThdThd(segbti_el2,i);
}

/*
 * PromClsfyOfstThdThd - classify  all offset/thread combinations 
 *                        as promoted, nonpromoted, or both for given thread id
 *
 * segbti_el - pointer to MemGenThrdDatEl structure
 * threadid - given thread id
 *
 */
static void PromClsfyOfstThdThd(segbti_el, threadid)
MemGenThrdDatEl *segbti_el;
I32s threadid;
{   MemInstThrd  *mita_el;
    I32s i;
    CallLvlIntrv   *clid_array_el;

    if((mita_el=(segbti_el->mgdt_thrd_ara.
        mita_array)[threadid]))
        if(mita_el->mith_calvlint.clid_array)
            /* for each call level interval */
            if((!(mita_el->mith_base.ith_prom))||
                (!(mita_el->mith_base.ith_nonprom)))
                for(i=0;i<(mita_el->mith_calvlint.
                    clid_ctrl.dync_elmaloc);i++)
                    if((clid_array_el=mita_el->
                        mith_calvlint.clid_array[i]))
                    {   if(clid_array_el->clv_base.clb_prom)
                            mita_el->mith_base.ith_prom=1;
                        else
                            mita_el->mith_base.ith_nonprom=1;
                    }
}

/*
 * FreeInstThrdCalLvlIntAr - free "instr/thread/call level interval" array
 *                           for given cell
 *
 * cp - pointer to given cell
 *
 */
static void FreeInstThrdCalLvlIntAr(cp)
Pcells cp;
{   I32s i;
    if(cp->cell_thrdanadat.mtad_codsegbti.mgda_segbti)
        /* for each offset */
        for(i=0;i<(cp->cell_thrdanadat.
            mtad_codsegbti.mgda_ctrl.dync_elmaloc);i++)
            FreeInstThrdCalLvlIntArOfst(cp,i);
}

/*
 * FreeInstThrdCalLvlIntArOfst - free "instr/thread/call level interval" array
 *                               for given offset in given cell
 *
 * cp - pointer to given cell
 * ofst - given offset
 *
 */
static void FreeInstThrdCalLvlIntArOfst(cp, ofst)
Pcells cp;
I32s ofst;
{   I32s i;
    MemGenThrdDat *segbti_el;
    if((segbti_el=cp->cell_thrdanadat.mtad_codsegbti.mgda_segbti[ofst]))
        /* for each track */
        for(i=0;i<PLOIDY;i++)
            FreeInstThrdCalLvlIntArTrk(segbti_el, i, ofst);
}

/*
 * FreeCalLvlIntDataCell - free call level interval thread analysis data
 *                     for given cell
 *
 * cp - pointer to given cell
 *
 */
static void FreeCalLvlIntDataCell(cp)
Pcells cp;
{   I32s i;
    FreeCalLvlIntTree(cp->cell_thrdanadat.mtad_rtcalvlint);
    cp->cell_thrdanadat.mtad_rtcalvlint=NULL;
    for(i=0;i<(cp->c.n);i++)
    {   cp->c.ar[i].cpu_crcalvlint=NULL;
        cp->c.ar[i].cpu_crcalvlid=(-1);
    }
    FreeInstThrdCalLvlIntAr(cp);
}

/*
 * FreeThrdAnaDatCell - free thread analysis data for given cell
 *
 * cp - pointer to given cell
 */
void FreeThrdAnaDatCell(cp)
Pcells cp;
{   FreeCalLvlIntDataCell(cp);
    FreeThrdAnaDat(&(cp->cell_thrdanadat));
}

/*
 * AllocThrdAnaCod - allocate thread analysis data structures
 *                   for executed code
 * memthrdanadat - pointer to MemThrdAnaDat structure for cell or
 *                 g_list structure in ram bank.
 */
void AllocThrdAnaCod(memthrdanadat, addr)
MemThrdAnaDat *memthrdanadat;
I32s addr;
{   MemThreadDat *thdatara_el;
    I32s curcelofst;
    I32s mgp=ce->d.mg.p;
    I32s mgs=ce->d.mg.s;

    if(addr==(is.oip))
    {   memthrdanadat->mtad_base.btad_extrnterm=(-1);
        memthrdanadat->mtad_base.btad_termcause=(-1);
        memthrdanadat->mtad_base.btad_termthrdid=(-1);
        memthrdanadat->mtad_base.btad_termInstP=(-1);
        if((AddDynArrEl((DynArr *)(&(memthrdanadat->mtad_thdatara)),
            is.othreadid, THREADID_INCR, 2201, 2202))>=0)
        {   thdatara_el=(memthrdanadat->mtad_thdatara.
                mtda_thdatara)[is.othreadid];
            thdatara_el->mtdt_basthrdat.tdt_fstinstex=(-1);
            thdatara_el->mtdt_basthrdat.tdt_termcause=(-1);
            thdatara_el->mtdt_basthrdat.tdt_parent=ce->c.c->parthrdid;
            thdatara_el->mtdt_basthrdat.tdt_cpu=ce->c.ac;
        }
    }
    curcelofst=addr-(ce->mm.p);
    if((curcelofst>=mgp)&&((curcelofst)<(mgp+mgs)))
        AllocThrdAna(curcelofst, &(memthrdanadat->
            mtad_codsegbti), CODE_SEG_INCR, is.oextrk,
            is.othreadid, 2203, 2204, 2207, 2220);
}

#ifdef MICRO
/*
 * ChkThrdAnaDatRdy - check for time to trigger thread analysis break trap
 *                    using thread analysis data for given cell
 *
 * cp - pointer to given cell
 */
static void ChkThrdAnaDatRdy(cp)
Pcells cp;
{   GList *tgl;
    I32s cpsize=cp->d.gen.size;
    I32s cpgi=cp->d.gi;

#ifdef NET
    I32s IPstopflg=1;
#endif /* NET */

    tgl=0;
    if(GeneBnker)
        tgl=sl[cpsize]->g[cpgi];
    if(BrkTrapCondBuf.curbrkdat.BrkSet)
    {
#ifdef NET
        if((sl[cpsize]->svsiznsl>=NodeSelSucRatPrec)||
            (NodeSelSucRatPrec<0))
        {
#endif /* NET */
            switch(BrkTrapCondBuf.curbrkdat.TrapType)
            {   case BRKTYP_THRDANADATRDY:

#ifdef NET
                    if(BrkIPPStrtIdxC>=0)
                    {   if(BrkIPPStrtIdxC!=(is.oipic1))
                        {   if(BrkIPPStrtIdxS>=0)
                            {   if(BrkIPPStrtIdxS!=(is.oipis1))
                                    IPstopflg=0;
                            }
                            else
                                IPstopflg=0;
                        }
                    }
                    else
                        if(BrkIPPStrtIdxS>=0)
                            if(BrkIPPStrtIdxS!=(is.oipis1))
                                IPstopflg=0;

                    if(IPstopflg)
                    {   if(BrkIPPStopIdxC>=0)
                        {   if(BrkIPPStopIdxC!=(is.oipic1))
                            {   if(BrkIPPStopIdxS>=0)
                                    IPstopflg=0;
                                else
                                    IPstopflg=0;
                            }
                        }
                        else
                            if(BrkIPPStopIdxS>=0)
                                IPstopflg=0;

                        if(IPstopflg)
                        {
#endif /* NET */
                            switch(BrkTrapCondBuf.curbrkdat.szclsgn)
                            {   case 's':
                                    if(cpsize==(BrkTrapCondBuf.
                                        curbrkdat.sizecls))
                                        ThrdAnaDatRdySv(cp);
                                    break;
                                case 'g':
                                    if((cpgi==(BrkTrapCondBuf.
                                        curbrkdat.prim_genome.gi))&&
                                        (cpsize==(BrkTrapCondBuf.
                                        curbrkdat.prim_genome.si)))
                                        ThrdAnaDatRdySv(cp);
                                    break;
                                default :
                                    break;
                            }
#ifdef NET
                        }
                    }
#endif /* NET */
                    break;
                default:
                    break;
            }
#ifdef NET
        }
#endif /* NET */
    }
}

/*
 * ThrdAnaDatRdySv - extract genotype of cell which triggered "thread analysis
 *                   data ready" break trap
 *
 * cp - pointer to cell which triggered break trap.
 */
static void ThrdAnaDatRdySv(cp)
Pcells cp;
{   GList *tgl;
    I32s cporgi=cp->d.origgi;
    I32s cpsize=cp->d.gen.size;

    tgl=sl[cpsize]->g[cporgi];
    BrkTrapCondBuf.curbrkdat.BrkFnd=1;
    SetBit(&tgl->bits, 0, 1);
    SetBit(&tgl->bits, 1, 0);
    extract(cpsize, 0, cporgi, 1);
    rcnttardy.si=cpsize;
    rcnttardy.gi=cporgi;
}

/*
 * initrcnttardy - initialize global structure for holding the original
 *                 genotype of the cell which last triggered the
 *                 "thread analysis data ready" break trap.
 */
void initrcnttardy()
{   memset(&rcnttardy, 0, sizeof(rcnttardy));
    rcnttardy.si=-1;
    rcnttardy.gi=-1;
}
#endif /* MICRO */

#ifdef NET
/*
 * Rpt_IPMap_Strt - report ip map starting nodes for 
 *                  successful node selections
 *
 * fp - pointer to FILE structure for disassembly listing
 */
static void Rpt_IPMap_Strt(fp)
FILE *fp;
{   I32s i;

    if(NumNodesC>0)
    {   if(NumNodesS>0)
            tfprintf(fp,"; for Cluster Map\n");
        else
            tfprintf(fp,"; for IP Map\n");
        tfprintf(fp,";            successful\n");
        tfprintf(fp,"; IPMap      selection\n");
        tfprintf(fp,"; start        count\n");
        tfprintf(fp,"; --------   ----------\n");
        for(i=0; i<(I32s)NumNodesC; i++)
            if(IPMapC[i].ipi1)
                tfprintf(fp,"; %8d   %10d\n", i, IPMapC[i].ipi1);
    }

    if(NumNodesS>0)
    {   tfprintf(fp,"; for Server Map\n");
        tfprintf(fp,";            successful\n");
        tfprintf(fp,"; IPMap      selection\n");
        tfprintf(fp,"; start        count\n");
        tfprintf(fp,"; --------   ----------\n");
        for(i=0; i<(I32s)NumNodesS; i++)
            if(IPMapS[i].ipi1)
                tfprintf(fp,"; %8d   %10d\n", i, IPMapS[i].ipi1);
    }
}
#endif /* NET */

#endif /* TIERRA */

#if defined(TIERRA)||defined(ARGTIE)

/*
 * WriteThrdAnaDat - save all thread analysis data for a given MemThrdAnaDat
 *                  structure of a rambank g_list structure to
 *                  a disk bank archive.
 *
 * fp - pointer to FILE structure for output file
 * memthrdanadat - pointer to MemThrdAnaDat structure for ram bank
 *                 g_list structure.
 */
void WriteThrdAnaDat(memthrdanadat, fp)
MemThrdAnaDat *memthrdanadat;
FILE *fp;
{   I32s i, xdrbuflen;
    MemThreadDat *thdatara_el;
    IdxThreadDat idxthrinf;
    ThrdAnaEvent *thdevntar_el;
    SoupUpdEvent *supdevntar_el;
    FilThrdAnaDat filthrdanadat;
    XDR xdrhndl;
    I8u *xdrbuf;

    filthrdanadat.ftad_thdatara_ctrl=
        memthrdanadat->mtad_thdatara.mtda_ctrl;
    filthrdanadat.ftad_cpysegbti_ctrl=
        memthrdanadat->mtad_cpysegbti.mgda_ctrl;
    filthrdanadat.ftad_codsegbti_ctrl=
        memthrdanadat->mtad_codsegbti.mgda_ctrl;
    filthrdanadat.ftad_event_ctrl=
        memthrdanadat->mtad_event.taea_ctrl;
    filthrdanadat.ftad_soupupdevt_ctrl=
        memthrdanadat->mtad_soupupdevt.suea_ctrl;
    filthrdanadat.ftad_rtcalvlint=
        (memthrdanadat->mtad_rtcalvlint)?
        (memthrdanadat->mtad_rtcalvlint->clv_base.clb_id):(-1);
    filthrdanadat.ftad_base=memthrdanadat->mtad_base;

    write_FilThrdAnaDat(fp, &filthrdanadat);

    if((xdrbuflen=GTaDatSize(memthrdanadat)))
    {   xdrbuf=(I8u *)thcalloc(xdrbuflen, 1, 2226);
        txdrmem_create(&xdrhndl, (I8s *)xdrbuf, xdrbuflen, XDR_ENCODE);
    
        WriteCalLvlIntTree(memthrdanadat->mtad_rtcalvlint, &xdrhndl);
    
        if(memthrdanadat->mtad_thdatara.mtda_ctrl.dync_elmused)
            for(i=0; i<memthrdanadat->mtad_thdatara.
                mtda_ctrl.dync_elmaloc; i++)
            {   if((thdatara_el=(memthrdanadat->
                    mtad_thdatara.mtda_thdatara)[i]))
                {   idxthrinf.xtdt_threadid=i;
                    idxthrinf.xtdt_thrdat=thdatara_el->mtdt_basthrdat;
                    txdr_IdxThreadDat(&xdrhndl, &idxthrinf);
                }
            }
    
        if(memthrdanadat->mtad_event.taea_ctrl.dync_elmused)
            for(i=0; i<(memthrdanadat->mtad_event.
                taea_ctrl.dync_elmaloc); i++)
                if((thdevntar_el=(memthrdanadat->
                    mtad_event.taea_event)[i]))
                    txdr_ThrdAnaEvent(&xdrhndl, thdevntar_el);
    
        if(memthrdanadat->mtad_soupupdevt.
            suea_ctrl.dync_elmused)
            for(i=0; i<(memthrdanadat->mtad_soupupdevt.
                suea_ctrl.dync_elmaloc); i++)
                if((supdevntar_el=(memthrdanadat->
                    mtad_soupupdevt.suea_event)[i]))
                    txdr_SoupUpdEvent(&xdrhndl, supdevntar_el);
    
        WriteThrdAnaDatBti(memthrdanadat, &(
            memthrdanadat->mtad_cpysegbti), &xdrhndl);
        WriteThrdAnaDatBti(memthrdanadat, &(
            memthrdanadat->mtad_codsegbti), &xdrhndl);
    
        tfwrite((I8s *)xdrbuf, xdrbuflen, 1, fp);
        txdr_destroy(&xdrhndl);
        tfree(xdrbuf, xdrbuflen, 2226);
    }
}

/*
 * WriteThrdAnaDatBti - write MemGenThrdDatArr structure contents
 *                      to gene bank archive
 * fp - pointer to FILE structure for output file
 * memthrdanadat - pointer to MemThrdAnaDat structure for cell or
 *                 g_list structure in ram bank.
 * segbti - pointer to MemGenThrdDatArr structure
 * xdrhndl - pointer to XDR handle
 * xdrbuf - pointer to XDR encode/decode buffer
 */
static void WriteThrdAnaDatBti(memthrdanadat, segbti, xdrhndl)
MemThrdAnaDat *memthrdanadat;
MemGenThrdDatArr *segbti;
XDR *xdrhndl;
{   I32s i, trkidx;
    MemGenThrdDat *segbti_el1, *segbti_el2;
    FilGenThrdDat filgenbytthrinf;

    if(segbti->mgda_ctrl.dync_elmused)
        for(i=0; i<(segbti->mgda_ctrl.dync_elmaloc); i++)
        {   if((segbti_el1=(segbti->mgda_segbti)[i]))
            {   for(trkidx=0; trkidx<PLOIDY; trkidx++)
                {   segbti_el2=&(segbti_el1[trkidx]);
                    filgenbytthrinf.xgdt_bytoffst=i;
                    filgenbytthrinf.xgdt_thrd_ara_ctrl=
                        segbti_el2->mgdt_thrd_ara.mita_ctrl;
                    filgenbytthrinf.xgdt_gbdt=segbti_el2->mgdt_gbdt;
                    txdr_FilGenThrdDat(xdrhndl, &filgenbytthrinf);
                    WriteMemInstThrdArr(segbti_el2, xdrhndl);
                }
            }
        }
}

/*
 * WriteCalLvlIntTree - save call level interval subtree
 *
 * fp - pointer to FILE structure for output file
 * rtcalvlint - pointer to subtree root
 * xdrhndl - pointer to XDR handle
 */
static void WriteCalLvlIntTree(rtcalvlint, xdrhndl)
CallLvlIntrv *rtcalvlint;
XDR *xdrhndl;
{   BaseCallLvlIntrv clvbase;
    CallLvlIntrv *listend, *crcalvlint;

    if(rtcalvlint)
    {   clvbase=rtcalvlint->clv_base;
        txdr_BaseCallLvlIntrv(xdrhndl, &clvbase);
        listend=(struct CallLvlIntrv *)
            (&(rtcalvlint->clv_subintrv.lsh_firstlast));
    
        for(crcalvlint=(CallLvlIntrv *)(rtcalvlint->
            clv_subintrv.lsh_first);crcalvlint!=listend;
            crcalvlint=(CallLvlIntrv *)(crcalvlint->
            clv_listnode.lnd_next))
            WriteCalLvlIntTree(crcalvlint, xdrhndl);
    }
}

/*
 * WriteMemInstThrdArr - save MemInstThrdArr structure
 *
 * fp - pointer to FILE structure for output file
 * segbti_el - pointer to MemGenThrdDat structure
 * xdrhndl - pointer to XDR handle
 * xdrbuf - pointer to XDR encode/decode buffer
 *
 */
static void WriteMemInstThrdArr(segbti_el, xdrhndl)
MemGenThrdDatEl *segbti_el;
XDR *xdrhndl;
{   I32s i,j;
    MemInstThrd  *mita_el;
    FilInstrThrd filinstrthrdbuf;

    for(i=0; i<(segbti_el->mgdt_thrd_ara.
        mita_ctrl.dync_elmaloc); i++)
    {   if((mita_el=segbti_el->mgdt_thrd_ara.mita_array[i]))
        {   filinstrthrdbuf.fith_threadid=i;
            filinstrthrdbuf.fith_base=
                mita_el->mith_base;
            filinstrthrdbuf.fith_calvlint_ctrl=
                mita_el->mith_calvlint.clid_ctrl;
            txdr_FilInstrThrd(xdrhndl, &filinstrthrdbuf);
            for(j=0;j<(mita_el->mith_calvlint.
                clid_ctrl.dync_elmaloc); j++)
                if(mita_el->mith_calvlint.clid_array[j])
                    txdr_I32s(xdrhndl, &j);
        }
    }
}

static void write_FilThrdAnaDat(fp, filthrdanadat)
FILE *fp;
FilThrdAnaDat *filthrdanadat;
{   I8u *xdrbuf;
    XDR xdrhndl;

    xdrbuf=(I8u *)thcalloc(prcencsize.pec_FilThrdAnaDat, 1, 2227);
    txdrmem_create(&xdrhndl, (I8s *)xdrbuf,
        prcencsize.pec_FilThrdAnaDat, XDR_ENCODE);
    txdr_FilThrdAnaDat(&xdrhndl, filthrdanadat);
    tfwrite((I8s *)xdrbuf, prcencsize.pec_FilThrdAnaDat, 1, fp);
    txdr_destroy(&xdrhndl);
    tfree(xdrbuf, prcencsize.pec_FilThrdAnaDat, 2227);
}

static I32s ChkExeThdTagExe(ofst,memthrdanadat,cpysegflg,genelkup)
I32s ofst,cpysegflg;
I16s *genelkup;
MemThrdAnaDat *memthrdanadat;
{   I32s flg=0,i,thrdcnt;
    MemGenThrdDat *mgda_segbti_el1;
    MemGenThrdDatArr *segbti;

    segbti=(cpysegflg)?(&(memthrdanadat->
        mtad_cpysegbti)):(&(memthrdanadat->
        mtad_codsegbti));
    if(ofst<(segbti->mgda_ctrl.dync_elmaloc))
        if((mgda_segbti_el1=segbti->mgda_segbti[ofst]))
        {   if(cpysegflg)
                flg=1;
            else if(LifeCycFrct>=0)
            {   thrdcnt=mgda_segbti_el1->mgdt_thrd_ara.
                    mita_ctrl.dync_elmaloc;
                for(i=0;i<thrdcnt;i++)
                    if(ChkThrdSpan(memthrdanadat,i))
                        break;
                if(i<thrdcnt)
                    flg=1;
            }
            else
                flg=1;
        }
    return flg;
}

static I32s ChkExeThdTagThd(memthrdanadat,thrdid,
    cpysegflg,genelkup,offset,track)
MemThrdAnaDat *memthrdanadat;
I32s thrdid,cpysegflg,offset,track;
I16s *genelkup;
{   I32s flg=0;
    MemGenThrdDatArr *segbti;

    segbti=(cpysegflg)?(&(memthrdanadat->
        mtad_cpysegbti)):(&(memthrdanadat->
        mtad_codsegbti));
    if(segbti->mgda_segbti[offset][track].
        mgdt_thrd_ara.mita_array[thrdid])
    {   if(cpysegflg)
            flg=1;
        else if(ChkThrdSpan(memthrdanadat,thrdid))
            flg=1;
    }
    return flg;
}

static I32s ChkThrdSpan(memthrdanadat,thrdid)
MemThrdAnaDat *memthrdanadat;
I32s thrdid;
{   I32s flg=0,lst_evt_instp;
    MemThreadDat *mtda_thdatara_el;

    if(LifeCycFrct>=0)
    {   mtda_thdatara_el=memthrdanadat->
            mtad_thdatara.mtda_thdatara[thrdid];
        lst_evt_instp=memthrdanadat->mtad_event.
            taea_event[(memthrdanadat->mtad_event.
            taea_ctrl.dync_elmused)-1]->taev_instP;
        if((((double)(mtda_thdatara_el->mtdt_basthrdat.
            tdt_instex))/((double)lst_evt_instp))>=LifeCycFrct)
            flg=1;
    }
    else
        flg=1;
    return flg;
}

#endif /* defined(TIERRA)||defined(ARGTIE) */

#if defined(TIERRA)||defined(ARGTIE)||\
    defined(GENALIGN)||defined(THREADTREE)||\
    defined(PROBE)||defined(SOUPUPDTRC)

/*
 * InitThrdAnaDat - initialize empty MemThrdAnaDat structure
 *
 * memthrdanadat - pointer to MemThrdAnaDat structure
 */
void InitThrdAnaDat(memthrdanadat)
MemThrdAnaDat *memthrdanadat;
{   InitThrdAnaDynArr(memthrdanadat);
    memthrdanadat->mtad_rtcalvlint=NULL;
    memthrdanadat->mtad_base.btad_extrnterm=(-1);
    memthrdanadat->mtad_base.btad_termcause=(-1);
    memthrdanadat->mtad_base.btad_termthrdid=(-1);
    memthrdanadat->mtad_base.btad_termInstP=(-1);
    memthrdanadat->mtad_base.btad_FilInstrThrd_ct=0;
    memthrdanadat->mtad_base.btad_CalLvlIntByID_ct=0;
    memthrdanadat->mtad_base.btad_callvlintcnt=0;
}

static void InitThrdAnaDynArr(memthrdanadat)
MemThrdAnaDat *memthrdanadat;
{   InitDynArr((DynArr *)(&(memthrdanadat->mtad_thdatara)),
        sizeof(MemThreadDat), 1);
    InitDynArr((DynArr *)(&(memthrdanadat->mtad_cpysegbti)),
        sizeof(MemGenThrdDat), 1);
    InitDynArr((DynArr *)(&(memthrdanadat->mtad_codsegbti)),
        sizeof(MemGenThrdDat), 1);
    InitDynArr((DynArr *)(&(memthrdanadat->mtad_event)),
        sizeof(ThrdAnaEvent), 1);
    InitDynArr((DynArr *)(&(memthrdanadat->mtad_soupupdevt)),
        sizeof(SoupUpdEvent), 1);
}

/*
 * ReadThrdAnaDat - read thread analysis data from gene bank archive
 *
 * memthrdanadat - pointer to ThrdAnaDat structure for cell or
 *                 g_list structure in ram bank.
 * calvlintrvlkupid - pointer to call level interval lookup table
 * fp - pointer to FILE structure for disassembly listing
 */
void ReadThrdAnaDat(memthrdanadat, calvlintrvlkupid, fp)
MemThrdAnaDat *memthrdanadat;
CalLvlIntByIDArr *calvlintrvlkupid;
FILE *fp;
{   I32s i, xdrbuflen;
    IdxThreadDat idxthrinf;
    FilThrdAnaDat filthrdanadat;
    XDR xdrhndl;
    I8u *xdrbuf;

    InitThrdAnaDat(memthrdanadat);
    read_FilThrdAnaDat(fp, &filthrdanadat);
    memthrdanadat->mtad_base=filthrdanadat.ftad_base;
    InitDynArr((DynArr *)calvlintrvlkupid, sizeof(CallLvlIntrv *), 0);

    if((xdrbuflen=FilGTaDatSize(&filthrdanadat)))
    {   xdrbuf=(I8u *)thcalloc(xdrbuflen, 1, 2225);
        txdrmem_create(&xdrhndl, (I8s *)xdrbuf, xdrbuflen, XDR_DECODE);
        tfread((I8s *)xdrbuf, xdrbuflen, 1, fp);
    
        if((filthrdanadat.ftad_rtcalvlint)>=0)
            ReadCalLvlIntTree(&(memthrdanadat->mtad_rtcalvlint),
                calvlintrvlkupid, &xdrhndl);
    
        for(i=0; i<(filthrdanadat.ftad_thdatara_ctrl.dync_elmused); i++)
        {   txdr_IdxThreadDat(&xdrhndl, &idxthrinf);
            ((memthrdanadat->mtad_thdatara.mtda_thdatara)[
                AddDynArrEl((DynArr *)(&(memthrdanadat->
                mtad_thdatara)), idxthrinf.xtdt_threadid,
                THREADID_INCR, 2201, 2202)]->mtdt_basthrdat)=
                idxthrinf.xtdt_thrdat;
        }
    
        for(i=0; i<(filthrdanadat.ftad_event_ctrl.dync_elmused); i++)
            txdr_ThrdAnaEvent(&xdrhndl, ((memthrdanadat->mtad_event.
                taea_event)[AddDynArrEl((DynArr *)(
                &(memthrdanadat->mtad_event)), -1,
                THREADID_INCR, 2210, 2211)]));
    
        for(i=0; i<(filthrdanadat.ftad_soupupdevt_ctrl.dync_elmused); i++)
            txdr_SoupUpdEvent(&xdrhndl, ((memthrdanadat->mtad_soupupdevt.
                suea_event)[AddDynArrEl((DynArr *)(
                &(memthrdanadat->mtad_soupupdevt)), -1,
                THREADID_INCR, 2222, 2223)]));
    
        ReadThrdAnaDatBti(memthrdanadat, filthrdanadat.
            ftad_cpysegbti_ctrl.dync_elmused,
            &(memthrdanadat->mtad_cpysegbti),
            COPY_SEG_INCR, 2205, 2206, 2208, 2221,
            calvlintrvlkupid, &xdrhndl);
    
        ReadThrdAnaDatBti(memthrdanadat,
            filthrdanadat.ftad_codsegbti_ctrl.dync_elmused,
            &(memthrdanadat->mtad_codsegbti),
            CODE_SEG_INCR, 2203, 2204, 2207, 2220,
            calvlintrvlkupid, &xdrhndl);
    
        txdr_destroy(&xdrhndl);
        tfree(xdrbuf, xdrbuflen, 2225);
    }
}

/*
 * GTaDatSize - return total encoded size of thread analysis data for a 
 *              genotype
 *
 * thrdanadat - pointer to MemThrdAnaDat structure
 */
I32s GTaDatSize(thrdanadat)
MemThrdAnaDat *thrdanadat;
{   return(((thrdanadat->mtad_base.btad_callvlintcnt)*
        (prcencsize.pec_BaseCallLvlIntrv))+

        ((prcencsize.pec_IdxThreadDat)*(thrdanadat->
        mtad_thdatara.mtda_ctrl.dync_elmused))+

        ((prcencsize.pec_ThrdAnaEvent)*(thrdanadat->
        mtad_event.taea_ctrl.dync_elmused))+

        ((prcencsize.pec_SoupUpdEvent)*(thrdanadat->
        mtad_soupupdevt.suea_ctrl.dync_elmused))+

        ((((thrdanadat->mtad_cpysegbti.mgda_ctrl.dync_elmused)+
        (thrdanadat->mtad_codsegbti.mgda_ctrl.dync_elmused))*
        (prcencsize.pec_FilGenThrdDat))*(PLOIDY))+

        ((thrdanadat->mtad_base.btad_FilInstrThrd_ct)*
        (prcencsize.pec_FilInstrThrd))+

        ((thrdanadat->mtad_base.btad_CalLvlIntByID_ct)*sizeof(I32s)));
}

/*
 * read_FilThrdAnaDat - read FilThrdAnaDat structure from gene bank
 *
 * fp - handle for gene bank archive to be read
 */
static void read_FilThrdAnaDat(fp, filthrdanadat)
FILE *fp;
FilThrdAnaDat *filthrdanadat;
{   I8u *xdrbuf;
    XDR xdrhndl;

    xdrbuf=(I8u *)thcalloc(prcencsize.pec_FilThrdAnaDat, 1, 2224);
    txdrmem_create(&xdrhndl, (I8s *)xdrbuf,
        prcencsize.pec_FilThrdAnaDat, XDR_DECODE);
    tfread((I8s *)xdrbuf, prcencsize.pec_FilThrdAnaDat, 1, fp);
    txdr_FilThrdAnaDat(&xdrhndl, filthrdanadat);
    txdr_destroy(&xdrhndl);
    tfree(xdrbuf, prcencsize.pec_FilThrdAnaDat, 2224);
}

/*
 * ReadThrdAnaDatBti - read MemGenThrdDatArr structure contents
 *                     from gene bank archive
 * fp - pointer to FILE structure for disassembly listing
 * memthrdanadat - pointer to MemThrdAnaDat structure for cell or
 *                 g_list structure in ram bank.
 * seg_ct - count of elements to read
 * segbti - pointer to MemGenThrdDatArr structure
 * calvlintrvlkupid - pointer to call level interval lookup table
 * xdrhndl - pointer to XDR handle
 */
static void ReadThrdAnaDatBti(memthrdanadat,
    seg_ct, segbti, segincr, memprof1, memprof2,
    memprof3, memprof4, calvlintrvlkupid, xdrhndl)
MemThrdAnaDat *memthrdanadat;
I32s seg_ct, segincr, memprof1, memprof2, memprof3, memprof4;
MemGenThrdDatArr *segbti;
CalLvlIntByIDArr *calvlintrvlkupid;
XDR *xdrhndl;
{   I32s i,j, trkidx;
    FilGenThrdDat filgenbytthrinf;
    FilInstrThrd filinstrthrdbuf;
    MemGenThrdDat *segbti_el;
    MemInstThrd  *mita_el;

    if(seg_ct)
    {   for(i=0; i<seg_ct; i++)
        {   for(trkidx=0; trkidx<PLOIDY; trkidx++)
            {   txdr_FilGenThrdDat(xdrhndl, &filgenbytthrinf);
                segbti_el=0;
                for(j=0;j<(filgenbytthrinf.
                    xgdt_thrd_ara_ctrl.dync_elmused); j++)
                {   txdr_FilInstrThrd(xdrhndl, &filinstrthrdbuf);
                    AllocThrdAna(filgenbytthrinf.xgdt_bytoffst,
                        segbti, segincr, trkidx,
                        filinstrthrdbuf.fith_threadid,
                        memprof1, memprof2, memprof3, memprof4);
                    segbti_el=&((segbti->mgda_segbti)[
                        filgenbytthrinf.xgdt_bytoffst][trkidx]);
                    mita_el=segbti_el->mgdt_thrd_ara.
                        mita_array[filinstrthrdbuf.fith_threadid];
                    mita_el->mith_base=
                        filinstrthrdbuf.fith_base;
                    ReadCalLvlIntByIDArr(&filinstrthrdbuf,
                        mita_el, calvlintrvlkupid,
                        memthrdanadat, xdrhndl);
                }
                if(segbti_el)
                    segbti_el->mgdt_gbdt=filgenbytthrinf.xgdt_gbdt;
            }
        }
    }
}

/*
 * ReadCalLvlIntByIDArr - read CalLvlIntByIDArr structure
 *
 * fp - pointer to FILE structure for output file
 * filinstrthrdbuf - pointer to FilInstrThrd structure
 * mita_el - pointer to MemInstThrdArr dynamic array element
 * memthrdanadat - pointer to MemThrdAnaDat structure holding
 *                 id count to be updated
 * xdrhndl - pointer to XDR handle
 */
static void ReadCalLvlIntByIDArr(filinstrthrdbuf,
    mita_el, calvlintrvlkupid, memthrdanadat, xdrhndl)
FilInstrThrd *filinstrthrdbuf;
MemInstThrd  *mita_el;
CalLvlIntByIDArr *calvlintrvlkupid;
MemThrdAnaDat *memthrdanadat;
XDR *xdrhndl;
{   I32s clvid,i;
    for(i=0;i<filinstrthrdbuf->
        fith_calvlint_ctrl.dync_elmused; i++)
    {   txdr_I32s(xdrhndl, &clvid);
        AddCalLvlIntByIDArr(mita_el, memthrdanadat,
            calvlintrvlkupid->clid_array[clvid]);
    }
}

/*
 * ReadCalLvlIntTree - read call level interval subtree
 *
 * fp - pointer to FILE structure for input file
 * rtcalvlint - pointer to pointer to tree root
 * xdrhndl - pointer to XDR handle
 */
static void ReadCalLvlIntTree(rtcalvlint, calvlintrvlkupid, xdrhndl)
CallLvlIntrv **rtcalvlint;
CalLvlIntByIDArr *calvlintrvlkupid;
XDR *xdrhndl;
{   I32s i;
    CallLvlIntrv *calvlint=(CallLvlIntrv *)
        thcalloc(1, sizeof(CallLvlIntrv), 2218);

    txdr_BaseCallLvlIntrv(xdrhndl, &calvlint->clv_base);
    if(!(*rtcalvlint))
        (*rtcalvlint)=calvlint;
    else
    {   AddTailNode((ListHead *)(&((*rtcalvlint)->clv_subintrv)),
            (ListNode *)(&(calvlint->clv_listnode)));
        calvlint->clv_parent=*rtcalvlint;
        AddDynArrEl((DynArr *)calvlintrvlkupid,
            calvlint->clv_base.clb_id, CALVLINT_INCR, 2219, 0);
    }
    InitListHead((ListHead *)(&(calvlint->clv_subintrv)));
    for(i=0;i<(calvlint->clv_base.clb_subintrvcnt);i++)
        ReadCalLvlIntTree(&calvlint, calvlintrvlkupid, xdrhndl);
}

/*
 * FreeMemGenThrdDatArr - free FreeMemGenThrdDatArr structure 
 *
 * segbtidynarr - pointer to MemGenThrdDatArr structure
 * memprof1 - memory profile label of MemGenThrdDatArr pointer array
 * memprof2 - memory profile label of allocated structures referenced by
 *            pointers in MemGenThrdDatArr pointer array
 * memprof3 - memory profile for mgdt_thrd_ara main array
 * memprof4 - memory profile for mgdt_thrd_ara element data structures
 */
static void FreeMemGenThrdDatArr(segbtidynarr, memprof1,
    memprof2, memprof3, memprof4)
MemGenThrdDatArr *segbtidynarr;
I32s memprof1, memprof2, memprof3, memprof4;
{   I32s i, trkidx;
    MemGenThrdDat **segbti=segbtidynarr->mgda_segbti;
    MemGenThrdDat *segbti_el;
    I32s seg_sz=segbtidynarr->mgda_ctrl.dync_elmaloc;

    for(i=0; i<seg_sz; i++)
        if((segbti_el=segbti[i]))
            for(trkidx=0; trkidx<PLOIDY; trkidx++)
            {   FreeInstThrdCalLvlIntArTrk(segbti_el, trkidx, i);
                FreeDynArr((DynArr *)(&(segbti_el[
                    trkidx].mgdt_thrd_ara)), memprof3, memprof4);
            }
    FreeDynArr((DynArr *)segbtidynarr, memprof1, memprof2);
}

/*
 * FreeInstThrdCalLvlIntArTrk - free "instr/thread/call level interval" array
 *                              for all threads for given track
 *
 * segbti_el1 - pointer to MemGenThrdDat structure
 *                         (MemGenThrdDatEl array)
 * track - given track
 *
 */
static void FreeInstThrdCalLvlIntArTrk(segbti_el1, track, dbgoffset)
MemGenThrdDat *segbti_el1;
I32s track, dbgoffset;
{   I32s i;
    MemGenThrdDatEl *segbti_el2=&(segbti_el1[track]);
    MemInstThrd  **mita_array;
    MemInstThrd *mita_el;

    if((mita_array=segbti_el2->mgdt_thrd_ara.mita_array))
        /* for each thread */
        for(i=0;i<(segbti_el2->mgdt_thrd_ara.
            mita_ctrl.dync_elmaloc);i++)
            if((mita_el=mita_array[i]))
                FreeDynArr((DynArr *)(&(mita_el->
                    mith_calvlint)), 2215, 0);
}

/*
 * FreeThrdAnaDat - delete all thread analysis data for a given MemThrdAnaDat
 *                  structure in a cell or rambank g_list structure.
 *
 * memthrdanadat - pointer to MemThrdAnaDat structure for cell or
 *                 g_list structure in ram bank.
 */
void FreeThrdAnaDat(memthrdanadat)
MemThrdAnaDat *memthrdanadat;
{   FreeDynArr((DynArr *)(&(memthrdanadat->mtad_thdatara)), 2201, 2202);
    FreeMemGenThrdDatArr(&(memthrdanadat->mtad_cpysegbti),
        2205, 2206, 2208, 2221);
    FreeMemGenThrdDatArr(&(memthrdanadat->mtad_codsegbti),
        2203, 2204, 2207, 2220);
    FreeDynArr((DynArr *)(&(memthrdanadat->mtad_event)), 2210, 2211);
    FreeDynArr((DynArr *)(&(memthrdanadat->mtad_soupupdevt)), 2222, 2223);
    FreeCalLvlIntTree(memthrdanadat->mtad_rtcalvlint);
    InitThrdAnaDat(memthrdanadat);
}

/*
 * FreeCalLvlIntTree - free call level interval structure subtree
 *
 * rtcalvlint - pointer to subtree root
 *
 */
static void FreeCalLvlIntTree(rtcalvlint)
CallLvlIntrv *rtcalvlint;
{   CallLvlIntrv *listend, *crcalvlint, *parent;
    if(rtcalvlint)
    {   listend=(struct CallLvlIntrv *)
            (&(rtcalvlint->clv_subintrv.lsh_firstlast));
        for(crcalvlint=(CallLvlIntrv *)
            (rtcalvlint->clv_subintrv.lsh_first);
            crcalvlint!=listend;crcalvlint=
            (CallLvlIntrv *)rtcalvlint->
            clv_subintrv.lsh_first)
            FreeCalLvlIntTree(crcalvlint);
        if((parent=rtcalvlint->clv_parent))
        {   RemHeadNode((ListHead *)(&(parent->clv_subintrv)));
            if((parent->clv_base.clb_subintrvcnt)>0)
                (parent->clv_base.clb_subintrvcnt)--;
            else
                (parent->clv_base.clb_subintrvcnt)=0;
        }
        thfree(rtcalvlint, sizeof(CallLvlIntrv), 2217);
    }
}

/*
 * FilGTaDatSize - return total encoded size of thread analysis data for a 
 *              genotype
 *
 * thrdanadat - pointer to FilThrdAnaDat structure
 */
I32s FilGTaDatSize(thrdanadat)
FilThrdAnaDat *thrdanadat;
{   return(((thrdanadat->ftad_base.btad_callvlintcnt)*
        (prcencsize.pec_BaseCallLvlIntrv))+

        ((prcencsize.pec_IdxThreadDat)*(thrdanadat->
        ftad_thdatara_ctrl.dync_elmused))+

        ((prcencsize.pec_ThrdAnaEvent)*(thrdanadat->
        ftad_event_ctrl.dync_elmused))+

        ((prcencsize.pec_SoupUpdEvent)*(thrdanadat->
        ftad_soupupdevt_ctrl.dync_elmused))+

        ((((thrdanadat->ftad_cpysegbti_ctrl.dync_elmused)+
        (thrdanadat->ftad_codsegbti_ctrl.dync_elmused))*
        (prcencsize.pec_FilGenThrdDat))*(PLOIDY))+

        ((thrdanadat->ftad_base.btad_FilInstrThrd_ct)*
        (prcencsize.pec_FilInstrThrd))+

        ((thrdanadat->ftad_base.btad_CalLvlIntByID_ct)*sizeof(I32s)));
}

/*
 * AllocThrdAna - allocate MemGenThrdDat structure
 *
 * segofst - MemGenThrdDat element offset
 * segincr - segment allocation increment
 * trkidx - track index
 * thrid - thread id
 * memprof1 - memory profile for segbti main array
 * memprof2 - memory profile for segbti element data structures
 * memprof3 - memory profile for mgdt_thrd_ara main array
 * memprof4 - memory profile for mgdt_thrd_ara element data structures
 */
static I32s AllocThrdAna(segofst, segbti,
    segincr, trkidx, thrdid, memprof1, memprof2, memprof3, memprof4)
I32s segofst, segincr, trkidx, thrdid, memprof1;
I32s memprof2, memprof3, memprof4;
MemGenThrdDatArr *segbti;
{   I32s rtnval,i;

    if((rtnval=AddDynArrEl((DynArr *)segbti,
        segofst, segincr, memprof1, memprof2))>=0)
        for(i=0;i<PLOIDY;i++)
            InitDynArr((DynArr *)(&(segbti->mgda_segbti[
                segofst][i].mgdt_thrd_ara)), sizeof(MemInstThrd), 1);
    AddMemInstThrdArr(&(segbti->mgda_segbti[
        segofst][trkidx].mgdt_thrd_ara), thrdid, memprof3, memprof4);
    return rtnval;
}

/*
 * AddCalLvlIntByIDArr - add entry to CalLvlIntByIDArr array
 *
 * mita_el - pointer to MemInstThrdArr dynamic array element
 *           for CalLvlIntByIDArr array
 * memthrdanadat - pointer to MemThrdAnaDat structure holding
 *                 id count to be updated
 * crcalvlint - CallLvlIntrv pointer for new entry
 */
static void AddCalLvlIntByIDArr(mita_el, memthrdanadat, crcalvlint)
MemInstThrd  *mita_el;
MemThrdAnaDat *memthrdanadat;
CallLvlIntrv *crcalvlint;
{   CallLvlIntrv **clid_el;
    if(!(*(clid_el=&(mita_el->mith_calvlint.
        clid_array[AddDynArrEl((DynArr *)(&(mita_el->
            mith_calvlint)), crcalvlint->clv_base.clb_id,
        CALVLINT_INCR, 2215, 0)]))))
    {   *(clid_el)=crcalvlint;
        (mita_el->mith_calvlint.clid_ctrl.dync_elmused)++;
        (memthrdanadat->mtad_base.btad_CalLvlIntByID_ct)++;
    }
}

/*
 * AddMemInstThrdArr - add entry to "MemInstThrdArr" lookup table
 *                     (indexed by thread id)
 *
 * thrdara - pointer to lookup table
 * thrdid - thread id table index
 * memprof1 - memory profile label for main thrdara
 * memprof2 - memory profile label for thrdara data structure
 *
 */
static void AddMemInstThrdArr(thrdara, thrdid, memprof1, memprof2)
MemInstThrdArr *thrdara;
I32s thrdid, memprof1, memprof2;
{   I32s oldsize;

    oldsize=thrdara->mita_ctrl.dync_elmaloc;
    if(AddDynArrEl((DynArr *)thrdara, thrdid,
        THREADID_INCR, memprof1, memprof2)>=0)
        InitDynArr((DynArr *)(&((thrdara->mita_array)[
            thrdid]->mith_calvlint)), sizeof(CallLvlIntrv *), 0);
}

#endif /* defined(TIERRA)||defined(ARGTIE)||
          defined(GENALIGN)||defined(THREADTREE)||
          defined(PROBE)||defined(SOUPUPDTRC) */

#if defined(TIERRA)||defined(ARGTIE)
/*
 * WrtThrdAnalysis - add thread analysis report to genome disassembly listing
 *
 * fp - pointer to FILE structure for disassembly listing
 * g - pointer to g_list structure
 * sucsiznslrat - successful node selection ratio for size class
 * svsucsiznsl - successful node selections for size class
 * svsiznsl - total node selection attempts for size class
 * detailrpt - 0 - summary report, >0 detail report
 * clstrfmt -  0 - non cluster analysis format
 *          - >0 - cluster analysis format
 *          available via arg disassembler
 * symclstranafmt -  0 - asymmetrical similarity matrix 
 *                       cluster analysis format not called for
 *                - >0 - symmetrical similarity matrix
 *                       cluster analysis format called for
 * codthdtstyarr - pointer to "tissue type" table (indexed by thread id)
 * exeptrn - 0 - not execution pattern report
 *           1 - execution pattern report
 */
void WrtThrdAnalysis(fp, g, sucsiznslrat, sucsiznsl, siznsl,
    detailrpt, clstrfmt, symclstranafmt, codthdtstyarr,
    exeptrn, genelkup, gendef, spltisana)
FILE *fp;
Pgl g;
I32s detailrpt, exeptrn;
I32s clstrfmt, symclstranafmt, spltisana;
I16s *genelkup;
double sucsiznslrat;
I32s sucsiznsl, siznsl;
ThdTsTyArr (*codthdtstyarr)[PLOIDY][NSTRTISTYP];
GeneDefArr *gendef;
{   I32s trkidx,tisbygen;
    MemThrdAnaDat *memthrdanadat=&(g->glst_thrdanadat);
    double *clsimtx=NULL;

    OfstThdArr cpyofstthdarr,codofstthdarr;
    ThdTisArr codthdtisarr;
    MemThreadDatArr *thdatara=&(g->glst_thrdanadat.mtad_thdatara);
    BodyDef codbody;

#ifdef ARGTIE
    /* if cluster analysis format similarity */
    /*     matrix not requested */
    if((!clstrfmt)&&(!exeptrn))
    {
#endif /* ARGTIE */
        /* report genotype specifications */
        /* ex. bytes executed/copied, genome length, */
        /*     reproductive efficiencies */
        Rpt_gen_spec(fp, g, sucsiznslrat, sucsiznsl, siznsl);

#ifdef TIERRA
#ifdef NET
        /* detail report selected and getipp start */
        /* is selected at random */
        if((detailrpt)&&(GetIPPStrtIdxC==(-2)))
        {   tfprintf(fp, "\n");

            /* report ip map starting nodes for */
            /* successful node selections */
                Rpt_IPMap_Strt(fp);
        }
#endif /* NET */
#endif /* TIERRA */

        tisbygen=STRTISINST;
        for(trkidx=0; trkidx<PLOIDY; trkidx++)
        {
            BldOfstThd(&cpyofstthdarr,memthrdanadat,
                trkidx,detailrpt,exeptrn,1,g->gen.size,
                genelkup,spltisana,tisbygen);

            /* report code copied (by offset, thread) */
            Rpt_ofst_thd(fp, &cpyofstthdarr, 0,
                trkidx,thdatara,detailrpt,clstrfmt,exeptrn);

            /* report code copied (by thread,offset) */
            Rpt_thd_ofst(fp,&cpyofstthdarr,0,0,trkidx,
                thdatara,detailrpt,clstrfmt,exeptrn);
    
            /* free copy cell offset thread buffer */
            FreeDynArr((DynArr*)(&cpyofstthdarr), 2209, 0);
        }
#ifdef ARGTIE
    }
#endif /* ARGTIE */

    for(tisbygen=0;tisbygen<((genelkup)?NSTRTISTYP:1);tisbygen++)
    {   for(trkidx=0; trkidx<PLOIDY; trkidx++)
        {
            BldOfstThd(&codofstthdarr,memthrdanadat,
                trkidx,detailrpt,exeptrn,0,g->gen.size,
                genelkup,spltisana,tisbygen);
    
            if(tisbygen==STRTISINST)
            {
                /* report code executed (by offset, thread) */
                Rpt_ofst_thd(fp, &codofstthdarr,1 , trkidx,
                    thdatara, detailrpt, clstrfmt, exeptrn);

                /* report code executed (by thread, offset) */
                Rpt_thd_ofst(fp, &codofstthdarr, 1, 1, trkidx,
                    thdatara, detailrpt, clstrfmt, exeptrn);
            }
    
            BldThdTisArr(&codthdtisarr,&codofstthdarr,
                detailrpt,exeptrn);
    
            /* if detail report */
            if(detailrpt||exeptrn)
            {
                /* classify tissues */
                TypeTissue(&codofstthdarr, &codthdtisarr,
                    &((*codthdtstyarr)[trkidx][tisbygen]), &codbody);
#ifdef ARGTIE
                /* if cluster analysis format similarity */
                /*     matrix not requested */
                if((!clstrfmt)&&(!exeptrn))
#endif /* ARGTIE */
                    /* report tissue type definitions */
                    if(tisbygen==STRTISGENE)
                        Rpt_Tissue_Types(fp, &codbody,
                            &codofstthdarr, tisbygen, gendef);
    
                if(!exeptrn)
                {   if(((codbody.bdy_splttiscnt)>1)&&
                        (tisbygen==STRTISGENE))
                    {
                        if(!clstrfmt)
                           clsimtx=(double *)thcalloc(
                               (codbody.bdy_tisdefar.
                               tsda_ctrl.dync_elmused)*
                               (codbody.bdy_tisdefar.
                               tsda_ctrl.dync_elmused),
                               sizeof(*clsimtx),2233);
    
                        /* print tissue similarity matrix */
                        Rpt_Tissue_Similarity(fp, &codbody,
                            &codofstthdarr, clstrfmt,
                            symclstranafmt, clsimtx, tisbygen);
        
#ifdef ARGTIE
                        if(!clstrfmt)
                        {
#endif /* ARGTIE */
#ifdef GRPNARROWTIS
                            /* group strict tissues into narrow tissues */
                            ClstrNarrTis(clsimtx,codbody.bdy_tisdefar.
                                tsda_ctrl.dync_elmused,fp,tisbygen);
#endif /* GRPNARROWTIS */
            
                            thfree(clsimtx,(codbody.
                                bdy_tisdefar.tsda_ctrl.
                                dync_elmused)*(codbody.
                                bdy_tisdefar.tsda_ctrl.
                                dync_elmused)*sizeof(*clsimtx),2233);
#ifdef ARGTIE
                        }
#endif /* ARGTIE */
                    }
                    /* if cluster analysis format similarity */
                    /*     matrix not requested */
                    /*     print thread data */
#ifdef ARGTIE
                    if(!clstrfmt)
#endif /* ARGTIE */
                        if(tisbygen==STRTISGENE)
                            Rpt_thd_dat(fp,memthrdanadat,
                                &((*codthdtstyarr)[trkidx][tisbygen]),
                                detailrpt,exeptrn,tisbygen);
                }
                /* free code tissue definition buffer */
                FreeDynArr((DynArr *)(&(codbody.bdy_tisdefar)), 2214, 0);
                codbody.bdy_splttiscnt=0;
            }
    
            /* free code thread tissue array */
            if((detailrpt||exeptrn))
                FreeDynArr((DynArr *)&codthdtisarr, 2213, 0);
    
            /* free code cell offset thread buffer */
            FreeDynArr((DynArr *)(&codofstthdarr), 2209, 0);
        }
    }
#ifdef ARGTIE
    /* if cluster analysis format similarity */
    /*     matrix not requested */
    if((!clstrfmt)&&(!exeptrn))
    {
#endif /* ARGTIE */
        tfprintf(fp, "\n");

        /* print event list */
        Rpt_event_list(fp, g, genelkup, gendef);

        tfprintf(fp, "\n");

        /* print soup update event list */
        if(detailrpt)
            Rpt_soup_update_event_list(fp, g);

#ifdef ARGTIE
    }
#endif /* ARGTIE */
}

/*
 * Rpt_gen_spec - report genotype specifications
 *                (ex bytes executed/copied, genome length,
 *                    reproductive efficiencies)
 *
 * fp - pointer to FILE structure for disassembly listing
 * g - pointer to g_list structure
 * sucsiznslrat - successful node selection ratio for size class
 * svsucsiznsl - successful node selections for size class
 * svsiznsl - total node selection attempts for size class
 */
static void Rpt_gen_spec(fp, g, sucsiznslrat, sucsiznsl, siznsl)
FILE *fp;
Pgl g;
double sucsiznslrat;
I32s sucsiznsl, siznsl;
{   double codpct, cpypct;
    double srpefcpd, srpeflen, prpefcpd, prpeflen;
    MemThrdAnaDat *memthrdanadat=&(g->glst_thrdanadat);
    I32s codseg_ct=memthrdanadat->mtad_codsegbti.mgda_ctrl.dync_elmused;
    I32s cpyseg_ct=memthrdanadat->mtad_cpysegbti.mgda_ctrl.dync_elmused;

    tfprintf(fp, "; %.4d%s - specifications\n",
        g->gen.size, &(g->gen.label[0]));

    tfprintf(fp, ";\n");
    codpct=100.0*(((double)codseg_ct)/((double)(g->gen.size)));
    tfprintf(fp, "; %-24s - %8d bytes - %6.2f %%\n",
       "bytes executed", codseg_ct, codpct);
    cpypct=100.0*(((double)cpyseg_ct)/((double)(g->gen.size)));
    tfprintf(fp, "; %-24s - %8d bytes - %6.2f %%\n",
       "bytes copied", cpyseg_ct, cpypct);
    tfprintf(fp, "; %-24s - %8d bytes\n", "genome length", g->gen.size);

#ifdef NET
    tfprintf(fp, ";\n");
    tfprintf(fp, "; node selection\n");
    tfprintf(fp, "; genotype\n");
    tfprintf(fp, ";    success rate          -  %8.2f\n", g->sucgennslrat);
    tfprintf(fp, ";    # successes           -  %5d\n", g->svsucgennsl);
    tfprintf(fp, ";    # attempts            -  %5d\n", g->svgennsl);
    tfprintf(fp, "; size class\n");
    tfprintf(fp, ";    success rate          -  %8.2f\n", sucsiznslrat);
    tfprintf(fp, ";    # successes           -  %5d\n", sucsiznsl);
    tfprintf(fp, ";    # attempts            -  %5d\n", siznsl);
#endif /* NET */

    tfprintf(fp, ";\n");
    tfprintf(fp, "; reproduction efficiency\n");
    srpefcpd=(g->d1.mov_daught)?(((double)(g->d1.inst))/
        ((double)(g->d1.mov_daught))):0.0;
    tfprintf(fp, "; %-24s -  %10.4f instr/byte copied\n",
        "serial", srpefcpd);
    srpeflen=((double)(g->d1.inst))/((double)(g->gen.size));
    tfprintf(fp, "; %-24s - (%10.4f instr/byte of genome)\n",
        " ", srpeflen);
    prpefcpd=(g->d1.mov_daught)?(((double)(g->d1.instP))/
        ((double)(g->d1.mov_daught))):0.0;
    tfprintf(fp, "; %-24s -  %10.4f instr/byte copied\n",
        "parallel", prpefcpd);
    prpeflen=((double)(g->d1.instP))/((double)(g->gen.size));
    tfprintf(fp, "; %-24s - (%10.4f instr/byte of genome)\n",
        " ", prpeflen);
}

/*
 * BldThdTisArr - build ThdTisArr dynamic array from OfstThdArr
 *                dynamic array
 *
 * thdtisarr - pointer to ThdTisArr dynamic array
 * ofstthdarr - pointer to OfstThdArr dynamic array
 * detailrpt -  0 = summary report
 *             >0 = detail report
 * exeptrn - 0 - not execution pattern report
 *           1 - execution pattern report
 */
static void BldThdTisArr(thdtisarr, ofstthdarr, detailrpt, exeptrn)
ThdTisArr *thdtisarr;
I32s detailrpt, exeptrn;
OfstThdArr *ofstthdarr;
{   I32s i, intrvidx, intrvcnt[2], svthrdid=0;
    ofst_thd *ofstthd;

    if((detailrpt||exeptrn))
    {   InitDynArr((DynArr *)thdtisarr, sizeof(ThdTis), 0);
        if(ofstthdarr->ofta_ofstthd)
        {
            /* sort offset/thread interval array by thd/dev/ofst */
            qsort(ofstthdarr->ofta_ofstthd,
               ofstthdarr->ofta_ctrl.dync_elmused,
               sizeof(ofst_thd), cmp_thd_dev_ofst);
    
            for(i=intrvidx=intrvcnt[0]=intrvcnt[1]=0; i<ofstthdarr->
                ofta_ctrl.dync_elmused; i++)
            {   ofstthd=&((ofstthdarr->ofta_ofstthd)[i]);
                if(!i)
                    svthrdid=ofstthd->oftd_thrd;
                else
                    if((ofstthd->oftd_thrd)!=svthrdid)
                    {   if(svthrdid>=0)
                            AddThdTisArr(thdtisarr, svthrdid,
                                intrvidx, intrvcnt[0], intrvcnt[1]);
                        svthrdid=ofstthd->oftd_thrd;
                        intrvidx=i;
                        intrvcnt[0]=intrvcnt[1]=0;
                    }
                /* increment offset/thread interval count */
                (intrvcnt[ofstthd->oftd_devstat])++;
            }
            if(ofstthdarr->ofta_ctrl.dync_elmused)
                AddThdTisArr(thdtisarr, svthrdid, intrvidx,
                    intrvcnt[0], intrvcnt[1]);
        }
    }
}

/*
 * BldOfstThd - build OfstThdArr dynamic array from MemGenThrdDatArr
 *              dynamic array for given track
 *
 * ofstthdarr - pointer to OfstThdArr dynamic array
 * segbti - pointer to MemGenThrdDatArr structure
 * track - index to track of segbti to be listed
 * detailrpt - 0 - summary report (contiguous offsets with associated threads
 *                                 are grouped together)
 *             1 - detail report  (contiguous offsets with different
 *                                 associated threads are in different
 *                                 intervals)
 * exeptrn - 0 - not execution pattern report
 *           1 - execution pattern report
 * cpysegflg - 1 = copied segment
 * gensize - genome size
 *
 */
static void BldOfstThd(ofstthdarr,memthrdanadat,track,detailrpt,
    exeptrn,cpysegflg,gensize,genelkup,spltisana,tisbygen)
OfstThdArr *ofstthdarr;
I32s track, cpysegflg, gensize, detailrpt, exeptrn, spltisana, tisbygen;
I16s *genelkup;
MemThrdAnaDat *memthrdanadat;
{   I32s i,intrvstrt=0,intrvend=0, newintrv, svdevstat=0, svthrdid=0;
    DevOfstThdArr devofstthdarr;
    DevOfstThd *devofstthd_el;

    InitDynArr((DynArr *)ofstthdarr, sizeof(ofst_thd), 0);
    BldDevOfstThd(&devofstthdarr,memthrdanadat,track,
        detailrpt,exeptrn,cpysegflg,gensize,
        genelkup,spltisana,tisbygen);

    for(i=0;i<(devofstthdarr.dvfta_ctrl.dync_elmused);i++)
    {   devofstthd_el=&(devofstthdarr.dvfta_devofsthd[i]);
        newintrv=1;
        if(i)
            if(devofstthd_el->dvoft_devstat==svdevstat)
                if(devofstthd_el->dvoft_thrdid==svthrdid)
                {   if(intrvend>=0)
                    {   if((devofstthd_el->
                            dvoft_offstgene)<=(intrvend+1))
                            newintrv=0;
                    }
                    else if((devofstthd_el->
                        dvoft_offstgene)==intrvend)
                        newintrv=0;
                }
        if(newintrv)
        {   if(i)
                AddOfstThd(ofstthdarr, svthrdid,
                    intrvstrt, intrvend, svdevstat);
            svdevstat=devofstthd_el->dvoft_devstat;
            svthrdid=devofstthd_el->dvoft_thrdid;
            intrvstrt=devofstthd_el->dvoft_offstgene;
        }
        intrvend=devofstthd_el->dvoft_offstgene;
    }
    if(devofstthdarr.dvfta_ctrl.dync_elmused)
        AddOfstThd(ofstthdarr, svthrdid,
            intrvstrt, intrvend, svdevstat);
    FreeDynArr((DynArr *)(&devofstthdarr), 2216, 0);
}

/*
 * BldDevOfstThd - build DevOfstThdArr dynamic array from MemGenThrdDatArr
 *                 dynamic array for given track
 *
 * devofstthdarr - pointer to DevOfstThdArr dynamic array
 * segbti - pointer to MemGenThrdDatArr structure
 * track - index to track of segbti to be listed
 * detailrpt - 0 - summary report (contiguous offsets with associated threads
 *                                 are grouped together)
 *             1 - detail report  (contiguous offsets with different
 *                                 associated threads are in different
 *                                 intervals)
 * exeptrn - 0 - not execution pattern report
 *           1 - execution pattern report
 * cpysegflg - 1 = copied segment
 * gensize   - genome size
 */
static void BldDevOfstThd(devofstthdarr,memthrdanadat,track,detailrpt,
    exeptrn,cpysegflg,gensize,genelkup,spltisana,tisbygen)
DevOfstThdArr *devofstthdarr;
I32s track, detailrpt, exeptrn, cpysegflg, gensize, spltisana, tisbygen;
I16s *genelkup;
MemThrdAnaDat *memthrdanadat;
{   MemGenThrdDat **mgda_segbti,*mgda_segbti_el1;
    I32s i,j,k,thrdcnt;
    MemInstThrd  *mita_el;
    MemGenThrdDatArr *segbti;

    segbti=(cpysegflg)?(&(memthrdanadat->
        mtad_cpysegbti)):(&(memthrdanadat->mtad_codsegbti));
    InitDynArr((DynArr *)devofstthdarr, sizeof(DevOfstThd), 0);
    if((mgda_segbti=segbti->mgda_segbti))
    {   /* for each "promoted/nonpromoted" group */
        for(i=0;i<((cpysegflg||(!(detailrpt||exeptrn)))?1:2);i++)
        {   /* for each offset of segment */
            for(j=0;j<gensize;j++)
            {   if(ChkExeThdTagExe(j,memthrdanadat,cpysegflg,genelkup))
                {   mgda_segbti_el1=&(mgda_segbti[j][track]);
                    thrdcnt=mgda_segbti_el1->mgdt_thrd_ara.
                        mita_ctrl.dync_elmaloc;
                    /* for each thread of offset */
                    for(k=0;k<thrdcnt;k++)
                        if(ChkExeThdTagThd(memthrdanadat,
                            k,cpysegflg,genelkup,j,track))
                        {   mita_el=mgda_segbti_el1->
                                mgdt_thrd_ara.mita_array[k];
                            if(!i)
                            {   if((mita_el->mith_base.ith_nonprom)||
                                    cpysegflg||(!genelkup)||
                                    (!spltisana)||
                                    (!(detailrpt||exeptrn)))
                                {   AddDevOfstThdArr(
                                        devofstthdarr,0,j,
                                        (detailrpt||exeptrn)?
                                        k:-1,tisbygen,genelkup);
                                    if(!(detailrpt||exeptrn))
                                        break;
                                }
                            }
                            else
                                if(mita_el->mith_base.ith_prom)
                                    AddDevOfstThdArr(
                                        devofstthdarr,1,
                                        j,k,tisbygen,genelkup);
                        }
                }
                else if(!i)
                    if(detailrpt||exeptrn)
                        AddDevOfstThdArr(devofstthdarr,0,
                            j,-1,tisbygen,genelkup);
            }
        }
    }
    qsort((void *)(devofstthdarr->dvfta_devofsthd),
        devofstthdarr->dvfta_ctrl.dync_elmused,
        sizeof(DevOfstThd), CmpDevOfstThdArr);
}

/*
 * AddDevOfstThdArr - add entry to DevOfstThdArr dynamic array
 *
 * devofstthdarr - pointer to DevOfstThdArr dynamic array
 * devstat - 0 = nonpromoted
 *           1 = promoted
 * offset - instruction offset
 * threadid - thread id
 * tisbygen - 0 - tissues determined by instruction offset
 *            1 - tissues determined by instruction gene
 */
static void AddDevOfstThdArr(devofstthdarr,
    devstat,offset,threadid,tisbygen,genelkup)
DevOfstThdArr *devofstthdarr;
I32s devstat,offset,threadid,tisbygen;
I16s *genelkup;
{   DevOfstThd *devofsthd_el;
    devofsthd_el=&(devofstthdarr->dvfta_devofsthd[AddDynArrEl(
        (DynArr *)devofstthdarr, -1, CODE_SEG_INCR, 2216, 0)]);
    devofsthd_el->dvoft_devstat=devstat;
    devofsthd_el->dvoft_offstgene=
        (tisbygen==STRTISGENE)?(genelkup[offset]):offset;
    devofsthd_el->dvoft_thrdid=threadid;
    (devofstthdarr->dvfta_ctrl.dync_elmused)++;
}

/*
 * CmpDevOfstThdArr - qsort comparison - sort DevOfstThdArr dynamic array
 *                    by dev/thread/offset
 */
static int CmpDevOfstThdArr(a,b)
const void *a, *b;
{   return(((((DevOfstThd *)a)->dvoft_devstat)<
        (((DevOfstThd *)b)->dvoft_devstat))?(-1):
        (((((DevOfstThd *)a)->dvoft_devstat)==
        (((DevOfstThd *)b)->dvoft_devstat))?
        (((((DevOfstThd *)a)->dvoft_thrdid)<
        (((DevOfstThd *)b)->dvoft_thrdid))?(-1):
        (((((DevOfstThd *)a)->dvoft_thrdid)==
        (((DevOfstThd *)b)->dvoft_thrdid))?
        (((((DevOfstThd *)a)->dvoft_offstgene)<
        (((DevOfstThd *)b)->dvoft_offstgene))?(-1):1):1)):1));
}

/*
 * cmp_ofst_thd_dev - qsort comparison - sort OfstThdArr dynamic
 *                    array by offset/thread/dev
 */
static int cmp_ofst_thd_dev(i,j)
const void *i,*j;
{   return(((((ofst_thd *)i)->oftd_start)<
        (((ofst_thd *)j)->oftd_start))?(-1):
        (((((ofst_thd *)i)->oftd_start)==
        (((ofst_thd *)j)->oftd_start))?
        (((((ofst_thd *)i)->oftd_end)<
        (((ofst_thd *)j)->oftd_end))?(-1):
        (((((ofst_thd *)i)->oftd_end)==
        (((ofst_thd *)j)->oftd_end))?
        (((((ofst_thd *)i)->oftd_thrd)<
        (((ofst_thd *)j)->oftd_thrd))?(-1):
        (((((ofst_thd *)i)->oftd_thrd)==
        (((ofst_thd *)j)->oftd_thrd))?
        (((((ofst_thd *)i)->oftd_devstat)<
        (((ofst_thd *)j)->oftd_devstat))?(-1):
        1):1)):1)):1));
}

/*
 * cmp_thd_ofst_dev - qsort comparison - sort OfstThdArr dynamic
 *                    array by thread/offset/dev
 */
static int cmp_thd_ofst_dev(i,j)
const void *i,*j;
{   return(((((ofst_thd *)i)->oftd_thrd)<
        (((ofst_thd *)j)->oftd_thrd))?(-1):
        (((((ofst_thd *)i)->oftd_thrd)==
        (((ofst_thd *)j)->oftd_thrd))?
        (((((ofst_thd *)i)->oftd_start)<
        (((ofst_thd *)j)->oftd_start))?(-1):
        (((((ofst_thd *)i)->oftd_start)==
        (((ofst_thd *)j)->oftd_start))?
        (((((ofst_thd *)i)->oftd_end)<
        (((ofst_thd *)j)->oftd_end))?(-1):
        (((((ofst_thd *)i)->oftd_end)==
        (((ofst_thd *)j)->oftd_end))?
        (((((ofst_thd *)i)->oftd_devstat)<
        (((ofst_thd *)j)->oftd_devstat))?(-1):
        1):1)):1)):1));
}

/*
 * TypeTissue - classify tissues
 *
 * ofstthdarr - pointer to lookup table
 * thdtisarr  - pointer to "thread-tissue" lookup table
 * thdtiscnt  - count of elements used
 * thdtstyarr - pointer to "tissue type" table indexed by thread id
 * tisdefbuf  - to pointer to lookup table
 */
static void TypeTissue(ofstthdarr, thdtisarr, thdtstyarr, body)
OfstThdArr *ofstthdarr;
ThdTisArr *thdtisarr;
ThdTsTyArr *thdtstyarr;
BodyDef *body;
{   I32s unkthdidx, unkintidx;
    I32s stdthdidx, stdintidx, stdintlim;
    ofst_thd *ofstthd=ofstthdarr->ofta_ofstthd;
    ThdTis *thdtis=thdtisarr->thda_thdtis;
    TissueDefArr *tisdefar=&(body->bdy_tisdefar);

    /* initialize empty "tissue definition" lookup table */
    InitDynArr((DynArr *)tisdefar, sizeof(TissueDef), 0);
    body->bdy_splttiscnt=0;

    /* initialize empty "tissue type" lookup table */
    InitDynArr((DynArr *)thdtstyarr, sizeof(I32s), 0);

    /* for each entry in "thread-tissue" lookup table */
    for(unkthdidx=0; unkthdidx<(thdtisarr->
        thda_ctrl.dync_elmused); unkthdidx++)
    {
        /* for each classified tissue */
        for(stdthdidx=0; stdthdidx<unkthdidx; stdthdidx++)
        {
            /* if number of intervals match */
            if(((thdtis[stdthdidx].thdi_thdticnt[0])==
                (thdtis[unkthdidx].thdi_thdticnt[0]))&&
              ((thdtis[stdthdidx].thdi_thdticnt[1])==
                (thdtis[unkthdidx].thdi_thdticnt[1])))
            {
                /* set "standard interval limit" */
                stdintlim=(thdtis[stdthdidx].thdi_thdtistrt)+
                    (thdtis[stdthdidx].thdi_thdticnt[0])+
                    (thdtis[stdthdidx].thdi_thdticnt[1]);

                /* for each interval of classified tissue */
                for(unkintidx=thdtis[unkthdidx].thdi_thdtistrt,
                    stdintidx=thdtis[stdthdidx].thdi_thdtistrt;
                    stdintidx<stdintlim; unkintidx++, stdintidx++)
                {
                    /* if interval of classified tissue does not
                         match interval of unclassified tissue */
                    if(!(((ofstthd[stdintidx].oftd_start)==
                        (ofstthd[unkintidx].oftd_start))&&
                        ((ofstthd[stdintidx].oftd_end)==
                        (ofstthd[unkintidx].oftd_end))&&
                        ((ofstthd[stdintidx].oftd_devstat)==
                        (ofstthd[unkintidx].oftd_devstat))))
                        break;
                }
                /* if unknown tissue matches standard tissue */
                if(stdintidx==stdintlim)
                {
                    /* if unknown tissue has associated thread */
                    if((thdtis[unkthdidx].thdi_thrd)>=0)
                    {
                        /* add entry to "tissue type" lookup table */
                        AddThdTsTyArr(thdtstyarr,
                            thdtis[unkthdidx].thdi_thrd,
                            (thdtstyarr->ttta_thdtsty)[
                            thdtis[stdthdidx].thdi_thrd]);
                    }
                    break;
                }
            }
        }

        /* if unknown tissue not yet classified */
        if(stdthdidx==unkthdidx)
        {
            /* if unknown tissue has associated thread */
            if((thdtis[unkthdidx].thdi_thrd)>=0)
            {
                /* add entry to "tissue definition" lookup table */
                AddTissueDef(body,
                    thdtis[unkthdidx].thdi_thdtistrt,
                    thdtis[unkthdidx].thdi_thdticnt[0],
                    thdtis[unkthdidx].thdi_thdticnt[1]);

                /* add entry to "tissue type" lookup table */
                AddThdTsTyArr(thdtstyarr, thdtis[unkthdidx].thdi_thrd,
                    (tisdefar->tsda_ctrl.dync_elmused)-1);
            }
        }
    }
}

/*
 * AddOfstThd - add entry to "offset/thread interval" table
 *              indexed sequentially
 * ofstthdarr - pointer to lookup table
 */
static void AddOfstThd(ofstthdarr, thrdid, intrvstrt, intrvend, devstat)
OfstThdArr *ofstthdarr;
I32s thrdid, intrvstrt, intrvend, devstat;
{   ofst_thd *ofstthdarr_el=&((ofstthdarr->ofta_ofstthd)[
        AddDynArrEl((DynArr *)ofstthdarr, -1, THREADID_INCR, 2209, 0)]);
    ofstthdarr_el->oftd_start=intrvstrt;
    ofstthdarr_el->oftd_end=intrvend;
    ofstthdarr_el->oftd_thrd=thrdid;
    ofstthdarr_el->oftd_devstat=devstat;
    ofstthdarr->ofta_ctrl.dync_elmused++;
}

/*
 * AddTissueDef - add entry to "tissue definition" lookup table
 *                indexed sequentially
 *
 * tisdefarr - pointer to lookup table
 */
static void AddTissueDef(body, intrvstrt,
    npromthdticnt, promthdticnt)
BodyDef *body;
I32s intrvstrt, npromthdticnt, promthdticnt;
{   TissueDef *tisdefarr_el=&((body->bdy_tisdefar.
        tsda_tisdef)[AddDynArrEl((DynArr *)
        (&(body->bdy_tisdefar.tsda_tisdef)), -1,
        TISSUEDEF_INCR, 2214, 0)]);
    tisdefarr_el->tsdf_tsdfstrt=intrvstrt;
    tisdefarr_el->tsdf_tsdfcnt[0]=npromthdticnt;
    tisdefarr_el->tsdf_tsdfcnt[1]=promthdticnt;
    if(npromthdticnt)
        (body->bdy_splttiscnt)++;
    if(promthdticnt)
        (body->bdy_splttiscnt)++;
    (body->bdy_tisdefar.tsda_ctrl.dync_elmused)++;
}

/*
 * AddThdTisArr - add entry to "thread-tissue" lookup table
 *                indexed sequentially
 *
 * thdtisarr - pointer to "thread-tissue" table
 * thrd      - given thread
 * thdtistrt - index to first interval for thread in
 *                "offset/thread interval array"
 *                (after sorting by thread)
 * npromthdticnt - count of nonpromoted intervals for given thread
 * promthdticnt - count of promoted intervals for given thread
 */
static void AddThdTisArr(thdtisarr, thrd, thdtistrt,
   npromthdticnt, promthdticnt)
ThdTisArr *thdtisarr;
I32s thrd, thdtistrt, npromthdticnt, promthdticnt;
{   ThdTis *thdtisarr_el=&((thdtisarr->thda_thdtis)[
        AddDynArrEl((DynArr *)thdtisarr, -1, THREADID_INCR, 2213, 0)]);
    thdtisarr_el->thdi_thrd=thrd;
    thdtisarr_el->thdi_thdtistrt=thdtistrt;
    thdtisarr_el->thdi_thdticnt[0]=npromthdticnt;
    thdtisarr_el->thdi_thdticnt[1]=promthdticnt;
    thdtisarr->thda_ctrl.dync_elmused++;
}

/*
 * Rpt_ofst_thd - report on segment executed or copied (by offset, thread)
 *
 * fp         - pointer to FILE structure for disk bank archive.
 * ofstthdarr - pointer to lookup table
 * codsegrpt - 1 = code segment report
 * trkidx     - track index
 * thdatara   - pointer to MemThreadDat array of MemThrdAnaDat structure
 *              for cell or g_list structure in ram bank
 * detailrpt  - 0 - summary report, >0 detail report
 * clstrfmt   -  0 - non cluster analysis format
 *            - >0 - cluster analysis format
 * exeptrn - 0 - not execution pattern report
 *           1 - execution pattern report
 */
static void Rpt_ofst_thd(fp, ofstthdarr, codsegrpt, trkidx,
    thdatara, detailrpt, clstrfmt, exeptrn)
FILE *fp;
I32s trkidx,detailrpt,clstrfmt,exeptrn,codsegrpt;
OfstThdArr *ofstthdarr;
MemThreadDatArr *thdatara;
{   I32s i;
    I8s *segtitle=(codsegrpt)?(I8s *)"executed":(I8s *)"copied";
    I8s *nothrdid=(codsegrpt)?(I8s *)"n/e":(I8s *)"n/c";

#ifdef ARGTIE
    /* if cluster analysis format similarity */
    /*     matrix not requested */
    if((!clstrfmt)&&(!exeptrn)&&
        (ofstthdarr->ofta_ctrl.dync_elmaloc))
    {
#endif /* ARGTIE */

        /* sort ofst_thd by start, end, thread, dev */
        qsort((void *)(ofstthdarr->ofta_ofstthd),
            (size_t)(ofstthdarr->ofta_ctrl.
            dync_elmused), sizeof(ofst_thd), cmp_ofst_thd_dev);

        /* print heading */
        tfprintf(fp, "\n; code %s", segtitle);
        if(detailrpt)
            tfprintf(fp, " (by offset, thread)");
        tfprintf(fp, "\n");
    
#if PLOIDY > 1
        tfprintf(fp, "; track %d\n", trkidx);
#endif /* PLOIDY */

        if(detailrpt)
        {   tfprintf(fp, "; %s = not %s\n", nothrdid, segtitle);
            if(codsegrpt)
               tfprintf(fp, "; p - promoted\n");
        }

        tfprintf(fp, "; start    end");
        if(detailrpt)
        {   tfprintf(fp, "    thrd   cpu");
            if(codsegrpt)
                tfprintf(fp, "    dev");
        }
        tfprintf(fp, "\n");

        tfprintf(fp, "; -----   -----");
        if(detailrpt)
        {   tfprintf(fp, "   ----   ----");
            if(codsegrpt)
                tfprintf(fp, "   ---");
        }

        tfprintf(fp, "\n");

        /* for each instruction of genome */
        for(i=0; i<(ofstthdarr->ofta_ctrl.dync_elmused); i++)
            AddLine_ofst_thd(fp, ofstthdarr, i, codsegrpt,
                thdatara, detailrpt, clstrfmt, exeptrn);
#ifdef ARGTIE
    }
#endif /* ARGTIE */
}


static void AddLine_ofst_thd(fp, ofstthdarr, ofstthdidx,
    codsegrpt, thdatara, detailrpt, clstrfmt, exeptrn)
I32s clstrfmt, exeptrn, ofstthdidx, codsegrpt, detailrpt;
MemThreadDatArr *thdatara;
OfstThdArr *ofstthdarr;
FILE *fp;
{   ofst_thd *ofstthd_el=&(ofstthdarr->ofta_ofstthd[ofstthdidx]);
    I8s *nothrdid=(codsegrpt)?(I8s *)"n/e":(I8s *)"n/c";

    tfprintf(fp, "; %5d   %5d",
        ofstthd_el->oftd_start, ofstthd_el->oftd_end);
    if((ofstthd_el->oftd_thrd)>=0)
    {   if(detailrpt)
        {   tfprintf(fp, "   %4d   %4d",
                ofstthd_el->oftd_thrd, thdatara->
                mtda_thdatara[ofstthd_el->
                oftd_thrd]->mtdt_basthrdat.tdt_cpu);
            if(codsegrpt)
                if(ofstthd_el->oftd_devstat)
                    tfprintf(fp,"   p");
        }
    }
    else if(detailrpt)
        tfprintf(fp, "   %4s   %4s",nothrdid, nothrdid);
    tfprintf(fp, "\n");
}

/*
 * cmp_thd_dev_ofst - qsort comparison function - sort OfstThdArr
 *                    dynamic array by thread/dev/offset
 */
static int cmp_thd_dev_ofst(i, j)
const void *i, *j;
{   return(((((ofst_thd *)i)->oftd_thrd)<
        (((ofst_thd *)j)->oftd_thrd))?(-1):
        (((((ofst_thd *)i)->oftd_thrd)==
        (((ofst_thd *)j)->oftd_thrd))?
        (((((ofst_thd *)i)->oftd_devstat)<
        (((ofst_thd *)j)->oftd_devstat))?(-1):
        (((((ofst_thd *)i)->oftd_devstat)==
        (((ofst_thd *)j)->oftd_devstat))?
        (((((ofst_thd *)i)->oftd_start)<
        (((ofst_thd *)j)->oftd_start))?(-1):
        (((((ofst_thd *)i)->oftd_start)==
        (((ofst_thd *)j)->oftd_start))?
        (((((ofst_thd *)i)->oftd_end)<
        (((ofst_thd *)j)->oftd_end))?(-1):1):1)):1)):1));
}

/*
 * Rpt_thd_ofst - generate copied or executed segment report 
 *                sorted by thread id and offset
 *
 * fp - pointer to FILE structure for disassembly listing
 * ofstthdarr - pointer to "offset/thread interval" structure array
 * codsegrpt -  0 = "copied segment" report
 *           - >0 = "executed segment" report
 * trkidx - track index
 * detailrpt -  0 = summary report
 *             >0 = detail report
 * clstrfmt -  0 - cluster analysis format similarity matrix not called for
 *            >0 - cluster analysis format similarity matrix called for
 * exeptrn - 0 - not execution pattern report
 *           1 - execution pattern report
 */
static void Rpt_thd_ofst(fp, ofstthdarr, codsegrpt,
    skipline, trkidx, thdatara, detailrpt, clstrfmt, exeptrn)
I32s clstrfmt, exeptrn, codsegrpt, detailrpt, trkidx, skipline;
FILE *fp;
OfstThdArr *ofstthdarr;
MemThreadDatArr *thdatara;
{   I8s *segtitle, *nothrdid;
    I32s i, svthrdid=0;
    ofst_thd *ofstthd;
#ifdef ARGTIE
    /* if cluster analysis format similarity */
    /*     matrix not requested */
    if((!clstrfmt)&&(!exeptrn))
    {
#endif /* ARGTIE */
        if((!codsegrpt)&&(detailrpt))
        {
            /* sort ofst_thd by thread, start, end, dev */
            qsort((void *)(ofstthdarr->ofta_ofstthd),
                (size_t)(ofstthdarr->ofta_ctrl.
                dync_elmused), sizeof(ofst_thd), cmp_thd_ofst_dev);

            /* adjust report headings for
               executed or copied segment */
            if(codsegrpt)
            {   segtitle=(I8s *)"executed";
                nothrdid=(I8s *)"n/e";
            }
            else
            {   segtitle=(I8s *)"copied";
                nothrdid=(I8s *)"n/c";
            }
            if(ofstthdarr->ofta_ofstthd)
            {   /* print heading */
                tfprintf(fp, 
                    "\n; code %s (by thread, offset)\n", segtitle);
#if PLOIDY > 1
                tfprintf(fp, "; track %d\n", trkidx);
#endif /* PLOIDY > 1 */

                tfprintf(fp, "; %s = not %s\n", nothrdid, segtitle);
                tfprintf(fp, "; thrd   cpu    start    end\n");
                tfprintf(fp, "; ----   ----   -----   -----\n");
        
                /* for each entry in offset/thread interval array */
                for(i=0; i<ofstthdarr->ofta_ctrl.dync_elmused; i++)
                {   ofstthd=&((ofstthdarr->ofta_ofstthd)[i]);

                    /* if first thread */
                    if(!i)
                        svthrdid=ofstthd->oftd_thrd;
                    else
                    {   /* on thread id break */
                        if((ofstthd->oftd_thrd)!=svthrdid)
                        {   if(skipline)
                                tfprintf(fp, ";\n");
                            svthrdid=ofstthd->oftd_thrd;
                        }
                    }
                    /* print detail line (sorted by thread/offset) */
                    if((ofstthd->oftd_thrd)>=0)
                        tfprintf(fp, "; %4d   %4d",
                            ofstthd->oftd_thrd,
                            thdatara->mtda_thdatara[ofstthd->
                            oftd_thrd]->mtdt_basthrdat.tdt_cpu);
                    else
                        tfprintf(fp, "; %4s   %4s",
                            nothrdid, nothrdid);
                    tfprintf(fp,"   %5d   %5d\n",
                        ofstthd->oftd_start,
                        ofstthd->oftd_end);
                }
            }
        }
#ifdef ARGTIE
    }
#endif /* ARGTIE */
}

/*
 * Rpt_thd_dat - print thread data (ex thread, cpu, parent, tissue type)
 *
 * fp - pointer to FILE structure for disassembly listing
 * memthrdanadat - pointer to MemThrdAnaDat structure for cell or
 *                 g_list structure in ram bank.
 * thdtstyarr - pointer to "tissue type" table (indexed by thread id)
 * detailrpt - 0 - summary report, >0 detail report
 * exeptrn - 0 - not execution pattern report
 *           1 - execution pattern report
 */
static void Rpt_thd_dat(fp,memthrdanadat,
    thdtstyarr,detailrpt,exeptrn,tisbygen)
FILE *fp;
MemThrdAnaDat *memthrdanadat;
ThdTsTyArr *thdtstyarr;
I32s detailrpt, exeptrn, tisbygen;
{   I32s i;
    MemThreadDat **thdatara=memthrdanadat->mtad_thdatara.mtda_thdatara;
    MemThreadDat *thdatara_el;
    I8u *termcause;

    /* print heading */
    tfprintf(fp, "\n");
    tfprintf(fp, ";                              thread data\n");
    tfprintf(fp, ";                             (using %s)\n",
        (tisbygen==STRTISGENE)?"genes":"instr");
    if(LifeCycFrct>=0)
        tfprintf(fp,"; %27sLifeCycFrct=%4.2f\n"," ",LifeCycFrct);
    tfprintf(fp, ";                          n/a - not applicable\n");
    tfprintf(fp, ";\n");
    if(detailrpt)
    {   tfprintf(fp, ";                                           ");
        tfprintf(fp, "strict\n");
    }
    tfprintf(fp, ";                       first      last");
    if(detailrpt)
        tfprintf(fp, "     tissue");
    tfprintf(fp, "  origin   term    term\n");
    tfprintf(fp, "; thrd  cpu   parent    instr      instr");
    if(detailrpt)
        tfprintf(fp, "     type");
    tfprintf(fp, "   InstP   InstP   cause\n");
    tfprintf(fp, "; ----  ----  ------  ---------  ---------");
    if(detailrpt)
        tfprintf(fp, "  ------");
    tfprintf(fp, "  ------  ------  ------");
    tfprintf(fp, "\n");

    /* for each element of genotype thread array */
    for(i=0; i<(memthrdanadat->mtad_thdatara.mtda_ctrl.dync_elmaloc); i++)
        /* if thread array element exists */
        if((thdatara_el=thdatara[i]))
        {   if(ChkThrdSpan(memthrdanadat,i))
            {   tfprintf(fp, "; %4d  %4d", i,
                    thdatara_el->mtdt_basthrdat.tdt_cpu);
                if((thdatara_el->mtdt_basthrdat.tdt_parent)>=0)
                    tfprintf(fp, "  %6d",
                        thdatara_el->mtdt_basthrdat.tdt_parent);
                else
                    tfprintf(fp, "  %6s", "n/a");
                tfprintf(fp, " ");
    
                Rpt_Event_List_Offset(fp,
#ifdef HAVE_TDT_FST_LST_OFSTTYP
                    thdatara_el->mtdt_basthrdat.tdt_fstofsttyp,
#else /* HAVE_TDT_FST_LST_OFSTTYP */
                    0,
#endif /* HAVE_TDT_FST_LST_OFSTTYP */
                    thdatara_el->mtdt_basthrdat.tdt_fstinstex);
    
                tfprintf(fp, " ");
    
                Rpt_Event_List_Offset(fp,
#ifdef HAVE_TDT_FST_LST_OFSTTYP
                    thdatara_el->mtdt_basthrdat.tdt_lstofsttyp,
#else /* HAVE_TDT_FST_LST_OFSTTYP */
                    0,
#endif /* HAVE_TDT_FST_LST_OFSTTYP */
                    thdatara_el->mtdt_basthrdat.tdt_lstinstex);
    
                if(detailrpt)
                    tfprintf(fp, "  %6d", (thdtstyarr->ttta_thdtsty)[i]);
                tfprintf(fp, "  %6d", thdatara_el->
                    mtdt_basthrdat.tdt_fstInstP);
                if((thdatara_el->mtdt_basthrdat.tdt_termcause)<0)
                    tfprintf(fp, "  %6d", memthrdanadat->
                        mtad_base.btad_termInstP);
                else
                    tfprintf(fp, "  %6d", thdatara_el->
                        mtdt_basthrdat.tdt_lstInstP);
                if((thdatara_el->mtdt_basthrdat.tdt_termcause)<0)
                {   if((memthrdanadat->mtad_base.btad_termcause)<0)
                    {   switch(memthrdanadat->mtad_base.btad_extrnterm)
                        {   case REAP_LAZY:
                                termcause=(I8u *)"steril";
                                break;
                            case REAP_DISTURB:
                                termcause=(I8u *)"distrb";
                                break;
                            case REAP_SOUP_FULL:
                                termcause=(I8u *)"no mem";
                                break;
#ifdef NET
                            case REAP_APOCALYPSE:
                                termcause=(I8u *)"apoc";
                                break;
#endif /* NET */
                            default:
                                termcause=(I8u *)" ";
                                break;
                        }
                    }
                    else
                        termcause=(I8u *)(&(id[memthrdanadat->
                            mtad_base.btad_termcause].mn[0]));
                }
                else
                    termcause=(I8u *)(&(id[thdatara_el->
                        mtdt_basthrdat.tdt_termcause].mn[0]));
                tfprintf(fp, "  %6s\n", termcause);
            }
        }
}

/*
 * Rpt_event_list - report event list
 *
 * fp - pointer to FILE structure for disassembly listing
 * g - pointer to g_list structure
 * genelkup - gene lookup table
 */
static void Rpt_event_list(fp, g, genelkup, gendef)
FILE *fp;
Pgl g;
I16s *genelkup;
GeneDefArr *gendef;
{   I32s i;
    ThrdAnaEvent **thdevntar=g->glst_thrdanadat.mtad_event.taea_event;

    if(thdevntar)
    {   tfprintf(fp, ";                      event list\n");
        tfprintf(fp, ";\n");
        tfprintf(fp, ";            ip           src/dst        ip\n");

        tfprintf(fp, ";  event   offset   thrd  offset");
        tfprintf(fp, "       address      instP");
        if(genelkup)
            fprintf(fp, "      gene");
        tfprintf(fp, "\n");

        tfprintf(fp, "; ------- --------- ---- ---------");
        tfprintf(fp, " --------------- --------");
        if(genelkup)
            tfprintf(fp, " -----------");
        tfprintf(fp, "\n");

        for(i=0; i<(g->glst_thrdanadat.mtad_event.
            taea_ctrl.dync_elmaloc); i++)
            Rpt_event(fp, g, i, genelkup, gendef);
    }
}

/*
 * Rpt_event - report event
 *
 * fp - pointer to FILE structure for disassembly listing
 * g - pointer to g_list structure
 * i - event list index
 * genelkup - gene lookup table
 */
static void Rpt_event(fp, g, i, genelkup, gendef)
FILE *fp;
Pgl g;
I32s i;
I16s *genelkup;
GeneDefArr *gendef;
{   ThrdAnaEvent *thdevntbuf;
    ThrdAnaEvent **thdevntar=g->glst_thrdanadat.mtad_event.taea_event;
    I8u *nodebyte;
    I32s eventcode, geneidx;
    I8s *mn, *gdf_name;

    thdevntbuf=thdevntar[i];
    if(thdevntbuf)
    {   nodebyte=(I8u *)(&(thdevntbuf->taev_i32a));
        eventcode=thdevntbuf->taev_eventcode;
        mn=&((id[eventcode].mn)[0]);

        tfprintf(fp, "; %-7s", mn);

        Rpt_Event_List_Offset(fp,
            (I32s)(thdevntbuf->taev_ipoffsettype),
            thdevntbuf->taev_ipoffset);

        tfprintf(fp, " %4d", thdevntbuf->taev_thrd);

        if( (!(strncmp((const char *)mn, (const char *)"getipp",6)))||
            (!strcmp((const char *)mn, (const char *)"movdi4"))||
            (!(strncmp((const char *)mn, (const char *)"off",3))))
            Rpt_Event_List_Offset(fp,
                (I32s)(thdevntbuf->taev_dstoffsettype),
                thdevntbuf->taev_dstoffset);
        else
            tfprintf(fp, " %9s", " ");

        if( (!strncmp((const char *)mn, (const char *)"getipp",6))||
            (!strcmp((const char *)mn, (const char *)"movdi4"))||
            (!strncmp((const char *)mn, (const char *)"surf",4)))
        {   tsprintf((char *)(&(Fbuf[0])),"%d.%d.%d.%d",
                nodebyte[0], nodebyte[1],
                nodebyte[2], nodebyte[3]);
            tfprintf(fp, " %15s", &(Fbuf[0]));
        }
        else
            if(!strcmp((const char *)mn, (const char *)"divide"))
                switch(thdevntbuf->taev_nodetype)
                {   case 0:
                        tfprintf(fp, " %15s", "local");
                        break;
#ifdef NET
                    case 1:
                        tsprintf((char *)(&(Fbuf[0])),"%d.%d.%d.%d",
                            nodebyte[0], nodebyte[1],
                            nodebyte[2], nodebyte[3]);
                        tfprintf(fp, " %15s", &(Fbuf[0]));
                        break;
#endif /* NET */
                    default:
                        tfprintf(fp, " %15s", " ");
                        break;
                }
            else
                tfprintf(fp, " %15s", " ");

        tfprintf(fp, " %8d", thdevntbuf->taev_instP);

        if(genelkup)
        {   gdf_name=(I8s *)" ";
            if(!(thdevntbuf->taev_ipoffsettype))
            {   geneidx=genelkup[thdevntbuf->taev_ipoffset];
                if(geneidx>=0)
                    gdf_name=gendef->gdf_array[geneidx].gdf_name;
            }
            tfprintf(fp, " ");
            if(thdevntbuf->taev_genetrn)
                tfprintf(fp, "*");
            else
                tfprintf(fp, " ");
            tfprintf(fp, "%-10s", gdf_name);
        }

        tfprintf(fp, "\n");
    }
}

/*
 * Rpt_Tissue_Types - list tissue type definitions
 *
 * fp - pointer to FILE structure for disassembly listing
 * body - pointer to BodyDef structure
 * ofstthdarr - pointer to "offset/thread interval" structure array
 */
static void Rpt_Tissue_Types(fp, body, ofstthdarr, tisbygen, gendef)
FILE *fp;
BodyDef *body;
OfstThdArr *ofstthdarr;
I32s tisbygen;
GeneDefArr *gendef;
{   I32s i,j,k,svtistyp,flag1=0;
    ofst_thd *ofstthd=ofstthdarr->ofta_ofstthd;
    TissueDef *tisdef=body->bdy_tisdefar.tsda_tisdef;

    tfprintf(fp,"\n; strict tissue types\n");
    if(LifeCycFrct>=0)
        tfprintf(fp,"; LifeCycFrct=%4.2f\n",LifeCycFrct);
    if(tisbygen==STRTISGENE)
    {   tfprintf(fp,"; type   genes\n");
        tfprintf(fp,"; ----   --------------------\n");
    }
    else
    {   tfprintf(fp,"; type   start    end\n");
        tfprintf(fp,"; ----   -----   -----\n");
    }

    /* for each tissue type */
    for(i=svtistyp=0;i<(body->bdy_tisdefar.tsda_ctrl.dync_elmused);i++)
    {
        /* for each interval of tissue type */
        for(j=tisdef[i].tsdf_tsdfstrt;
            j<((tisdef[i].tsdf_tsdfstrt)+
            (tisdef[i].tsdf_tsdfcnt[0])+
            (tisdef[i].tsdf_tsdfcnt[1])); j++)
        {   for(k=((tisbygen==STRTISGENE)?(ofstthd[j].oftd_start):0);
                k<=((tisbygen==STRTISGENE)?(ofstthd[j].oftd_end):0);k++)
            {   if(((k>=0)&&(tisbygen==STRTISGENE))||
                    (tisbygen!=STRTISGENE))
                {   tfprintf(fp,"; ");
                    if((i!=svtistyp)||(!flag1))
                    {
                        /* print tissue type code */
                        tfprintf(fp,"%4d",i);
        
                        /* save current tissue type */
                        svtistyp=i;
                    }
                    else
                        tfprintf(fp,"    ");
    
                    if(tisbygen==STRTISGENE)
                        tfprintf(fp,"   %s", (k>=0)?(gendef->
                            gdf_array[k].gdf_name):(I8s *)" ");
                    else
                    {   tfprintf(fp,"   %5d   %5d",
                            ofstthd[j].oftd_start, ofstthd[j].oftd_end);
                        if(ofstthd[j].oftd_devstat)
                            tfprintf(fp,"   p");
                    }
                    tfprintf(fp,"\n");
                    if(!flag1)
                        flag1=1;
                }
            }
        }
    }
}

/*
 * CalcIntrvalOverlap - calculate overlap size between two intervals
 *
 * start1 - start of interval 1
 * end1   - end   of interval 1
 * start2 - start of interval 2
 * end2   - end   of interval 2
 *
 * Returns: overlap size between two intervals
 */
static I32s CalcIntrvalOverlap(start1, end1, start2, end2)
I32s start1, end1, start2, end2;
{   return(((end1<start2)||(end2<start1))?0:
        ((((end1<=end2)?end1:end2)-
        ((start1>=start2)?start1:start2))+1));
}

/*
 * AddThdTsTyArr - add entry to "tissue type" lookup table
 *                 (indexed by thread id)
 *
 * thdtstyarr - pointer to lookup table
 * thrdid - thread id table index
 * tissuetype - "tissue type"
 */
static void AddThdTsTyArr(thdtstyarr, thrdid, tissuetype)
ThdTsTyArr *thdtstyarr;
I32s thrdid, tissuetype;
{   I32s i, oldsize;

    oldsize=thdtstyarr->ttta_ctrl.dync_elmaloc;
    AddDynArrEl((DynArr *)thdtstyarr, thrdid, THREADID_INCR, 2212, 0);
    for(i=oldsize; i<(thdtstyarr->ttta_ctrl.dync_elmaloc); i++)
        (thdtstyarr->ttta_thdtsty)[i]=(-1);
    (thdtstyarr->ttta_thdtsty)[thrdid]=tissuetype;
    thdtstyarr->ttta_ctrl.dync_elmused++;
}

/*
 * Rpt_Tissue_Similarity - generate tissue similarity matrix
 *
 * fp - pointer to FILE structure for disassembly listing
 * body - pointer to BodyDef structure containing tissue definitions
 * ofstthdarr - pointer to OfstThdArr dynamic array
 * tisdefbuf - pointer to tissue type array
 * ofstthdbuf - pointer to "offset/thread interval" structure array
 * clstrfmt -  0 - non cluster analysis format
 *          - >0 - cluster analysis format
 *          available via arg disassembler
 * symclstranafmt -  0 - asymmetrical similarity matrix
 *                       cluster analysis format not called for
 *                - >0 - symmetrical similarity matrix
 *                       cluster analysis format called for
 */
static void Rpt_Tissue_Similarity(fp, body, ofstthdarr,
    clstrfmt, symclstranafmt,clsimtx,tisbygen)
FILE *fp;
BodyDef *body;
I32s clstrfmt, symclstranafmt,tisbygen;
OfstThdArr *ofstthdarr;
double *clsimtx;
{   I32s colperlin, ncolgrp, tiscolstrt, colstrtisstrt, coldevidxstrt;
    I32s colgrp, remtiscol, tiscolend, tiscol, colstrtis, coldevidx;
    I32s tisrow, rowstrtis, rowdevidx, rowofstthdidx, rowtissize;
    I32s colstrtisend, coldevidxend,i;
    ofst_thd *ofstthd=ofstthdarr->ofta_ofstthd;
    TissueDef *tisdef=body->bdy_tisdefar.tsda_tisdef;

    /* if more than one tissue */
    if((body->bdy_splttiscnt)>1)
    {   colperlin=(clstrfmt)?(body->bdy_splttiscnt):4;
        ncolgrp=(((body->bdy_splttiscnt)-1)/colperlin)+1;

        /* print headings */
        tfprintf(fp, "\n");
        if(clstrfmt)
        {   if(!symclstranafmt)
                tfprintf(fp,"a");
            tfprintf(fp,"symmetrical cluster analysis format\n");
            tfprintf(fp, "\n");
            tfprintf(fp, "%5d tiss\n", body->bdy_splttiscnt);
            tfprintf(fp, "%5d comp\n", body->bdy_splttiscnt);
        }
        else
        {   tfprintf(fp,"; tissue similarity\n");
            if(tisbygen==STRTISGENE)
                tfprintf(fp,"; (using genes)\n");
            else
                tfprintf(fp,"; (using instr)\n");
            if(LifeCycFrct>=0)
                tfprintf(fp,"; LifeCycFrct=%4.2f\n",LifeCycFrct);
            tfprintf(fp,"; [instr exec : common/(row_len, col_len)]\n");
        }

        /* for each column group */
        for(tiscolstrt=colstrtisstrt=coldevidxstrt=
            colgrp=0, remtiscol=body->bdy_splttiscnt;
            colgrp<ncolgrp; colgrp++)
        {
            /* set column group end and */
            /*     decrement remaining tissue count */
            if(remtiscol>=colperlin)
            {   tiscolend=tiscolstrt+colperlin;
                remtiscol-=colperlin;
            }
            else
            {   tiscolend=tiscolstrt+remtiscol;
                remtiscol=0;
            }

            /* print column group headings */
            if(clstrfmt)
            {   tfprintf(fp, "      ");
                for(tiscol=tiscolstrt; tiscol<tiscolend; tiscol++)
                    tfprintf(fp," %-5s", "Q");
                tfprintf(fp,"\n      ");
            }
            else
                tfprintf(fp,";\n; type:");

            for(tiscol=tiscolstrt, colstrtis=colstrtisstrt,
                coldevidx=coldevidxstrt; tiscol<tiscolend;)
            {   if(body->bdy_tisdefar.tsda_tisdef[
                    colstrtis].tsdf_tsdfcnt[coldevidx])
                {   if(clstrfmt)
                    {   tsprintf((char *)(&(Fbuf[0])), "T%d%s", colstrtis,
                            (coldevidx)?"''":" ");
                        tfprintf(fp," %-5s", &(Fbuf[0]));
                    }
                    else
                        tfprintf(fp,"  %13d%s", colstrtis,
                            (coldevidx)?"''":" ");
                    tiscol++;
                }
                coldevidx++;
                if(coldevidx>1)
                {   colstrtis++;
                    coldevidx=0;
                }
            }

            tfprintf(fp,"\n");
            if(!clstrfmt)
            {   tfprintf(fp, "; -----"); 
                for(tiscol=tiscolstrt; tiscol<tiscolend; tiscol++)
                    tfprintf(fp,"  ----/----/----");
                tfprintf(fp,"\n");
            }

            /* for each defined tissue */
            for(tisrow=rowstrtis=rowdevidx=0;
                tisrow<(body->bdy_splttiscnt); rowdevidx++)
            {   if(rowdevidx>1)
                {   rowstrtis++;
                    rowdevidx=0;
                }
                if(body->bdy_tisdefar.tsda_tisdef[
                    rowstrtis].tsdf_tsdfcnt[rowdevidx])
                {
                    /* calculate "row tissue size" */
                    for(rowofstthdidx=(tisdef[rowstrtis].
                        tsdf_tsdfstrt)+((rowdevidx)?(
                        tisdef[rowstrtis].tsdf_tsdfcnt[0]
                        ):0), rowtissize=0; rowofstthdidx
                        <((tisdef[rowstrtis].
                        tsdf_tsdfstrt)+(tisdef[rowstrtis
                        ].tsdf_tsdfcnt[0])+((rowdevidx)?(
                        tisdef[rowstrtis].tsdf_tsdfcnt[1]
                        ):0)); rowofstthdidx++)
                        rowtissize+=((ofstthd[rowofstthdidx].oftd_end)-
                           (ofstthd[rowofstthdidx].oftd_start))+1;
    
                    if(tiscolend>(tisrow+((clstrfmt)?0:1)))
                    {
                        /* print row tissue type */
                        if(clstrfmt)
                        {   tsprintf((char *)(&(Fbuf[0])),"T%d%s",
                                rowstrtis, (rowdevidx)?"'":" ");
                            tfprintf(fp,"%-6s", &(Fbuf[0]));
                        }
                        else
                            tfprintf(fp,"; %4d%s", rowstrtis,
                                (rowdevidx)?"'":" ");
    
                        Rpt_Tissue_Similarity_cols(fp, body,
                            ofstthdarr, tisrow, rowstrtis,
                            rowdevidx, rowtissize, clstrfmt,
                            tiscolstrt, tiscolend, colstrtisstrt,
                            coldevidxstrt, symclstranafmt,
                            &colstrtisend, &coldevidxend,clsimtx);
                    }
                    tisrow++;
                }
            }
            tiscolstrt=tiscolend;
            coldevidxstrt=coldevidxend;
            colstrtisstrt=colstrtisend;
        }
        if(!clstrfmt)
            for(i=0;i<(body->bdy_splttiscnt);i++)
                clsimtx[i*((body->bdy_splttiscnt)+1)]=1.00;
    }
}
/*
 * Rpt_Tissue_Similarity_cols - print columns of tissue similarity
 *                              matrix for given row
 *
 * fp - pointer to FILE structure for disassembly listing
 * body - pointer to BodyDef structure containing tissue definitions
 * ofstthdarr - pointer to OfstThdArr dynamic array
 * tisrow - given row
 * rowstrtis - strict tissue for given row
 * rowdevidx - development index for given row
 * rowtissize = total tissue size for strict tissue for given row
 * clstrfmt -  0 - non cluster analysis format
 *          - >0 - cluster analysis format
 *          available via arg disassembler
 * tiscolstrt - starting column
 * tiscolend - starting column
 * colstrtisstrt - starting strict tissue
 * coldevidxstrt - starting development index
 * symclstranafmt -  0 - asymmetrical similarity matrix
 *                       cluster analysis format not called for
 *                - >0 - symmetrical similarity matrix
 *                       cluster analysis format called for
 */
static void Rpt_Tissue_Similarity_cols(fp, body, ofstthdarr,
    tisrow, rowstrtis, rowdevidx, rowtissize, clstrfmt,
    tiscolstrt, tiscolend, colstrtisstrt, coldevidxstrt,
    symclstranafmt, colstrtisend, coldevidxend,clsimtx)
I32s tiscolstrt, tiscolend, colstrtisstrt, coldevidxstrt, clstrfmt;
I32s tisrow, rowdevidx, symclstranafmt, rowstrtis, rowtissize;
BodyDef *body;
FILE *fp;
OfstThdArr *ofstthdarr;
I32s *colstrtisend, *coldevidxend;
double *clsimtx;
{   I32s tiscol, colstrtis, coldevidx, comnsiz, colofstthdidx;
    I32s coltissize, rowofstthdidx, maxtissiz;
    TissueDef *tisdef=body->bdy_tisdefar.tsda_tisdef;
    ofst_thd *ofstthd=ofstthdarr->ofta_ofstthd;
    double comnrowsizrat, comncolsizrat, mincomnsizrat;
    I32s tiscnt=body->bdy_tisdefar.tsda_ctrl.dync_elmused;

    for(tiscol=tiscolstrt, colstrtis=colstrtisstrt,
        coldevidx=coldevidxstrt; tiscol<tiscolend;)
    {   if(tisdef[colstrtis].tsdf_tsdfcnt[coldevidx])
        {   if((tiscol<=tisrow)&&(!clstrfmt))
                tfprintf(fp,"                ");
            else
            {
                /*
                 * calculate
                 *     total common tissue size for
                 *         row and column tissues
                 *     total column tissue size
                 *     max of row and column tissue sizes
                 */
                comnsiz=0;
                for(colofstthdidx=(tisdef[colstrtis].
                    tsdf_tsdfstrt)+((coldevidx)?(tisdef[
                    colstrtis].tsdf_tsdfcnt[0]):0), coltissize=0;
                    colofstthdidx<((tisdef[colstrtis].
                    tsdf_tsdfstrt)+(tisdef[colstrtis].
                    tsdf_tsdfcnt[0])+((coldevidx)?(tisdef[
                    colstrtis].tsdf_tsdfcnt[1]):0)); colofstthdidx++)
                {   for(rowofstthdidx=(tisdef[rowstrtis].
                        tsdf_tsdfstrt)+((rowdevidx)?(
                        tisdef[rowstrtis].tsdf_tsdfcnt[0]):0);
                        rowofstthdidx<((tisdef[rowstrtis].
                        tsdf_tsdfstrt)+(tisdef[rowstrtis].
                        tsdf_tsdfcnt[0])+((rowdevidx)?(
                        tisdef[rowstrtis].tsdf_tsdfcnt[1]):0));
                        rowofstthdidx++)
            
                        comnsiz+=CalcIntrvalOverlap(ofstthd[
                            colofstthdidx].oftd_start,
                            ofstthd[colofstthdidx].oftd_end,
                            ofstthd[rowofstthdidx].oftd_start,
                            ofstthd[rowofstthdidx].oftd_end);
            
                    coltissize+=((ofstthd[colofstthdidx].oftd_end)-
                       (ofstthd[colofstthdidx].oftd_start))+1;
                }
                maxtissiz=(rowtissize>coltissize)?
                    rowtissize:coltissize;
        
                 /* "common/row size ratio"=
                     "common tissue size"/"row tissue size" */
                comnrowsizrat=((double)comnsiz)/((double)rowtissize);
        
                /* "common/column size ratio"=
                    "common tissue size"/"column tissue size"; */
                comncolsizrat=((double)comnsiz)/((double)coltissize);
        
                /* minimum common tissue size ratio */
                mincomnsizrat=(comnrowsizrat<
                    comncolsizrat)?(comnrowsizrat):
                    (comncolsizrat);
        
                /* if cluster analysis format
                    similarity matrix requested */
                if(clstrfmt)
                {
                    /* if symmetrical format */
                    if(symclstranafmt)
                    {
                        /* minimum common tissue size ratio */
                        tfprintf(fp," %-5.2lf", mincomnsizrat);
                    }
                    else
                    {
                        /* common/row size ratio */
                        tfprintf(fp," %-5.2lf", comnrowsizrat);
                    }
                }
                else
                {
                    /*
                     * "max of row and column tissue sizes"
                     * "common/row size ratio"
                     * "common/column size ratio"
                     */
                    tfprintf(fp,"  %4d %4.2lf-%4.2lf",
                        maxtissiz, comnrowsizrat,
                        comncolsizrat);

                    clsimtx[(rowstrtis*tiscnt)+colstrtis]=comnrowsizrat;
                    clsimtx[(colstrtis*tiscnt)+rowstrtis]=comncolsizrat;
                }
            }
            tiscol++;
        }
        coldevidx++;
        if(coldevidx>1)
        {   colstrtis++;
            coldevidx=0;
        }
    }
    tfprintf(fp,"\n");
    (*coldevidxend)=coldevidx;
    (*colstrtisend)=colstrtis;
}

#ifdef GRPNARROWTIS

static void ClstrNarrTis(clsimtx,strtiscnt,fp,tisbygen)
double *clsimtx;
I32s strtiscnt,tisbygen;
FILE *fp;
{   int i,j,k,l,mrgvalset,mrgvalidx[CLSIMTXDIMS],cmprsix,rplcix;
    double mrgval=0.0,*clsdstmtx;
    ClstrArr clstrarr;
    Clstr *pclstr;

    clsdstmtx=CalcClstrDistMtx(clsimtx,strtiscnt,strtiscnt);
    InitClstrArray(&clstrarr,strtiscnt);

    for(k=strtiscnt;k>1;k--)
    {
        for(i=0,mrgvalset=0;i<(k-1);i++)
            for(j=i+1;j<k;j++)
                if((mrgvalset&&((clsdstmtx[(i*k)+j])>
                    mrgval))||(!mrgvalset))
                {   mrgval=clsdstmtx[((mrgvalidx[
                        CLSIMTXROW]=i)*k)+(
                        mrgvalidx[CLSIMTXCOL]=j)];
                    mrgvalset=1;
                }
        if(!mrgvalset)
            porterrmsg(2400,"tissue clustering error",1);
    
        if(mrgvalidx[CLSIMTXROW]>mrgvalidx[CLSIMTXCOL])    
        {   cmprsix=mrgvalidx[CLSIMTXROW];
            rplcix=mrgvalidx[CLSIMTXCOL];
        }
        else
        {   cmprsix=mrgvalidx[CLSIMTXCOL];
            rplcix=mrgvalidx[CLSIMTXROW];
        }

        if(mrgval>=MinComSizRat)
        {
            /* calculate new cluster column */
            for(i=0;i<rplcix;i++)
                if(i!=cmprsix)
                    clsdstmtx[(i*k)+rplcix]=
#ifdef CLSTRAVGLNK
                        (((clsdstmtx[(i*k)+rplcix])*
                        ((clstrarr.clsta_array[
                        rplcix]->clstr_wgt)+(clstrarr.
                        clsta_array[i]->clstr_wgt)))+((
                        clsdstmtx[(nmin(cmprsix,i)*k)+
                        nmax(cmprsix,i)])*((
                        clstrarr.clsta_array[cmprsix]->
                        clstr_wgt)+(clstrarr.clsta_array[
                        i]->clstr_wgt))))/((
                        clstrarr.clsta_array[rplcix]->
                        clstr_wgt)+(clstrarr.clsta_array[
                        cmprsix]->clstr_wgt)+(2.0*((clstrarr.
                        clsta_array[i]->clstr_wgt))));
#else /* CLSTRAVGLNK */
                        nmax(clsdstmtx[(i*k)+rplcix],
                        clsdstmtx[(nmin(cmprsix,i)*
                        k)+nmax(cmprsix,i)]);
#endif /* CLSTRAVGLNK */

            /* calculate new cluster row */
            for(j=(rplcix+1);j<k;j++)
                if(j!=cmprsix)
                    clsdstmtx[(rplcix*k)+j]=
#ifdef CLSTRAVGLNK
                        (((clsdstmtx[(rplcix*k)+j])*
                        ((clstrarr.clsta_array[
                        rplcix]->clstr_wgt)+(clstrarr.
                        clsta_array[j]->clstr_wgt)))+((
                        clsdstmtx[(nmin(cmprsix,j)*k)+
                        nmax(cmprsix,j)])*((
                        clstrarr.clsta_array[cmprsix]->
                        clstr_wgt)+(clstrarr.clsta_array[
                        j]->clstr_wgt))))/((
                        clstrarr.clsta_array[rplcix]->
                        clstr_wgt)+(clstrarr.clsta_array[
                        cmprsix]->clstr_wgt)+(2.0*((clstrarr.
                        clsta_array[j]->clstr_wgt))));
#else /* CLSTRAVGLNK */
                        nmax(clsdstmtx[(rplcix*k)+j],
                        clsdstmtx[(nmin(cmprsix,j)*
                        k)+nmax(cmprsix,j)]);
#endif /* CLSTRAVGLNK */
        
            /* merge clusters */
            pclstr=(Clstr *)thcalloc(1,sizeof(Clstr),2235);
            pclstr->clstr_leafid=-1;
            pclstr->clstr_mrgval=mrgval;
            InitListHead(&(pclstr->clstr_list));

            clstrarr.clsta_array[rplcix]->clstr_parent=pclstr;
#ifdef CLSTRAVGLNK
            pclstr->clstr_wgt+=(clstrarr.clsta_array[rplcix]->clstr_wgt);
#endif /* CLSTRAVGLNK */
            AddTailNode(&(pclstr->clstr_list),
                &(clstrarr.clsta_array[rplcix]->clstr_node));

            clstrarr.clsta_array[cmprsix]->clstr_parent=pclstr;
#ifdef CLSTRAVGLNK
            pclstr->clstr_wgt+=(clstrarr.clsta_array[cmprsix]->clstr_wgt);
#endif /* CLSTRAVGLNK */
            AddTailNode(&(pclstr->clstr_list),
                &(clstrarr.clsta_array[cmprsix]->clstr_node));

            clstrarr.clsta_array[rplcix]=pclstr;
            for(l=cmprsix;l<(k-1);l++)
                clstrarr.clsta_array[l]=clstrarr.clsta_array[l+1];
            clstrarr.clsta_array[k]=NULL;
            (clstrarr.clsta_ctrl.dync_elmused)--;
            (clstrarr.clsta_ctrl.dync_elmaloc)--;
    
            /* compress array */
            for(i=0;i<(k-2);i++)
                for(j=i+1;j<(k-1);j++)
                    clsdstmtx[(i*(k-1))+j]=clsdstmtx[
                        ((i+((i>=cmprsix)?1:0))*k)+
                        (j+((j>=cmprsix)?1:0))];
        }
        else
            break;
    }
    thfree(clsdstmtx,strtiscnt*strtiscnt*sizeof(*clsdstmtx),2229);
    PrintNarTiss(&clstrarr,k,fp,strtiscnt,(int)tisbygen);
}

static void PrintNarTiss(clstrarr,nartiscnt,fp,strtiscnt,tisbygen)
ClstrArr *clstrarr;
int nartiscnt,strtiscnt,tisbygen;
FILE *fp;
{   int i,narstrtiscnt;
    int *narstrtisar=(int *)
        thcalloc(strtiscnt, sizeof(int), 2228);

    tfprintf(fp,"\n; narrow tissue count");
    if(tisbygen==STRTISGENE)
        tfprintf(fp," (using genes");
    else
        tfprintf(fp," (using instr");
    if(LifeCycFrct>=0)
        tfprintf(fp,", LifeCycFrct=%4.2f",LifeCycFrct);
    tfprintf(fp,") : %d\n",nartiscnt);
    tfprintf(fp, "\n");
    tfprintf(fp, "; Narrow   Strict\n");
    tfprintf(fp, "; Tissue   Tissue\n");
    tfprintf(fp, "; ------   ------\n");
    for(i=0;i<nartiscnt;i++)
    {   narstrtiscnt=0;
        CollNarrStrTiss(clstrarr->clsta_array[i],
            narstrtisar,&narstrtiscnt);
        qsort(narstrtisar,narstrtiscnt,sizeof(int),&narstrtiscmp);
        PrintNarrStrTiss(fp,narstrtisar,narstrtiscnt,i);
    }
    thfree(narstrtisar, sizeof(*narstrtisar), 2228);
}

static void CollNarrStrTiss(rtclstr,narstrtisar,narstrtiscnt)
Clstr *rtclstr;
int *narstrtisar,*narstrtiscnt;
{   Clstr *pclstr,*listend;

    if((rtclstr->clstr_leafid)>=0)
        narstrtisar[(*narstrtiscnt)++]=rtclstr->clstr_leafid;
    listend=(Clstr *)(&(rtclstr->clstr_list.lsh_firstlast));
    for(pclstr=(Clstr *)(rtclstr->
        clstr_list.lsh_first);pclstr!=listend;
        pclstr=(Clstr *)(pclstr->clstr_node.lnd_next))
        CollNarrStrTiss(pclstr,narstrtisar,narstrtiscnt);
}

static void PrintNarrStrTiss(fp,narstrtisar,narstrtiscnt,nartisid)
FILE *fp;
int *narstrtisar,narstrtiscnt,nartisid;
{   int i;

    for(i=0;i<narstrtiscnt;i++)
    {   tfprintf(fp, "; ");
        if(!i)
            tfprintf(fp, "%6d",nartisid);
        else
            tfprintf(fp,"%6s"," ");
        tfprintf(fp,"   %6d\n",narstrtisar[i]);
    }
}

static int narstrtiscmp(i,j)
const void *i, *j;
{   return ((*((int *)i))<(*((int *)j)))?
        (-1):(((*((int *)i))==(*((int *)j)))?0:1);
}

static void InitClstrArray(clstrarr,clstrcnt)
ClstrArr *clstrarr;
{   int k;
    InitDynArr((DynArr *)clstrarr,sizeof(Clstr),1);
    for(k=0;k<clstrcnt;k++)
    {   AddDynArrEl((DynArr *)clstrarr,-1,CLSTR_INC,2234,2235);
        clstrarr->clsta_array[k]->clstr_leafid=k;
        clstrarr->clsta_array[k]->clstr_mrgval=-1.0;
#ifdef CLSTRAVGLNK
        clstrarr->clsta_array[k]->clstr_wgt=1.0;
#endif /* CLSTRAVGLNK */
        InitListHead(&(clstrarr->clsta_array[k]->clstr_list));
    }
}

static void FreeClstrArray(clstrarr)
ClstrArr *clstrarr;
{   int i;
    for(i=0;i<(clstrarr->clsta_ctrl.dync_elmaloc);i++)
        if(clstrarr->clsta_array[i])
            FreeClstrTree(clstrarr->clsta_array[i]);
    FreeDynArr((DynArr *)clstrarr,2234,2235);
}

static void FreeClstrTree(rtclstr)
Clstr *rtclstr;
{   Clstr *listend,*pclstr,*parent;

    listend=(Clstr *)(&(rtclstr->
        clstr_list.lsh_firstlast));
    for(pclstr=(Clstr *)(rtclstr->
        clstr_list.lsh_first);
        pclstr!=listend;pclstr=(Clstr *)
        (rtclstr->clstr_list.lsh_first))
        FreeClstrTree(pclstr);
    if((parent=rtclstr->clstr_parent))
    {   RemHeadNode((ListHead *)(&(parent->clstr_list)));
        thfree(rtclstr,sizeof(Clstr),2235);
    }
}

static double *CalcClstrDistMtx(clsimtx,clstrcnt,ndims)
int clstrcnt,ndims;
double *clsimtx;
{   double *clsdstmtx;
    int i,j,k;

    clsdstmtx=(double *)thcalloc(clstrcnt*
        clstrcnt,sizeof(*clsdstmtx),2229);
    for(i=0;i<(clstrcnt-1);i++)
        for(j=i+1;j<clstrcnt;j++)
        {
#if CLSTRPYTHDIST

            for(k=0,clsdstmtx[(i*clstrcnt)+j]=0.0;k<ndims;k++)
                clsdstmtx[(i*clstrcnt)+j]+=
                    (fabs((clsimtx[(i*ndims)+k])-
                    (clsimtx[(j*ndims)+k]))*fabs(
                    (clsimtx[(i*ndims)+k])-(
                    clsimtx[(j*ndims)+k])));
            clsdstmtx[(i*clstrcnt)+j]=
                sqrt(clsdstmtx[(i*clstrcnt)+j]);

#else /* CLSTRPYTHDIST */

            clsdstmtx[(i*clstrcnt)+j]=nmax(clsimtx[
                (i*ndims)+j],clsimtx[(j*ndims)+i]);

#endif /* CLSTRPYTHDIST */
        }
    for(i=0;i<clstrcnt;i++)
        for(j=0;j<(i+1);j++)
            clsdstmtx[(i*clstrcnt)+j]=0.0;
    return clsdstmtx;
}

#ifdef CLSTRANADBG

static void DumpClstrSimMtx(clsimtx,clstrcnt,ndims)
int clstrcnt,ndims;
double *clsimtx;
{   int i,j;
    tfprintf(stdout,"%6s"," ");
    for(j=0;j<ndims;j++)
        tfprintf(stdout," %4d",j);
    tfprintf(stdout,"\n");
    tfprintf(stdout,"%6s"," ");
    for(j=0;j<ndims;j++)
        tfprintf(stdout," %4s","----");
    tfprintf(stdout,"\n");
    for(i=0;i<clstrcnt;i++)
    {   tfprintf(stdout,"%4d |",i);
        for(j=0;j<ndims;j++)
            tfprintf(stdout," %4.2f",
                clsimtx[(i*ndims)+j]);
        tfprintf(stdout,"\n");
    }
}

static void DumpClstrDistMtx(clsdstmtx,clstrcnt)
int clstrcnt;
double *clsdstmtx;
{   int i,j;
    tfprintf(stdout,"%6s"," ");
    for(j=0;j<clstrcnt;j++)
        tfprintf(stdout," %4d",j);
    tfprintf(stdout,"\n");
    tfprintf(stdout,"%6s"," ");
    for(j=0;j<clstrcnt;j++)
        tfprintf(stdout," %4s","----");
    tfprintf(stdout,"\n");
    for(i=0;i<(clstrcnt-1);i++)
    {   tfprintf(stdout,"%4d |",i);
        for(j=0;j<clstrcnt;j++)
            if(j>i)
                tfprintf(stdout," %4.2f",
                    clsdstmtx[(i*clstrcnt)+j]);
            else
                tfprintf(stdout," %4s"," ");
        tfprintf(stdout,"\n");
    }
}

static void DumpClstrArray(clstrarr,clstrcnt)
ClstrArr *clstrarr;
int clstrcnt;
{   int i;
    tfprintf(stdout,"cluster count=%d\n",clstrcnt);
    for(i=0;i<clstrcnt;i++)
        if(clstrarr->clsta_array[i])
        {   tfprintf(stdout,"for cluster tree %d\n",i);
            DumpClstrTree(clstrarr->clsta_array[i],0);
        }
}

static void DumpClstrTree(rtclstr,level)
Clstr *rtclstr;
{   Clstr *pclstr,*listend;
    int i;

    for(i=0;i<level;i++)
        tfprintf(stdout," ");
    tfprintf(stdout,"%d\n",rtclstr->clstr_leafid);

    listend=(Clstr *)(&(rtclstr->clstr_list.lsh_firstlast));
    for(pclstr=(Clstr *)(rtclstr->
        clstr_list.lsh_first);pclstr!=listend;
        pclstr=(Clstr *)(pclstr->clstr_node.lnd_next))
        DumpClstrTree(pclstr,level+1);
}
#endif /* CLSTRANADBG */
#endif /* GRPNARROWTIS */

/*
 * Rpt_soup_update_event_list - report soup update event list
 *
 * fp - pointer to FILE structure for disassembly listing
 * g - pointer to g_list structure
 */
void Rpt_soup_update_event_list(fp, g)
FILE *fp;
Pgl g;
{   I32s i;
    SoupUpdEvent **soupupdevt=g->glst_thrdanadat.
        mtad_soupupdevt.suea_event;

    if(soupupdevt)
    {   Rpt_soup_update_event_head(fp);
        for(i=0; i<(g->glst_thrdanadat.mtad_soupupdevt.
            suea_ctrl.dync_elmaloc); i++)
            Rpt_soup_update_event(fp, g, i);
    }
}
#endif /* defined(TIERRA)||defined(ARGTIE) */

#if defined(TIERRA)||defined(ARGTIE)||defined(SOUPUPDTRC)

/*
 * Rpt_soup_update_event_head - soup update event list heading
 * fp - pointer to output FILE structure
 */
void Rpt_soup_update_event_head(fp)
FILE *fp;
{   tfprintf(fp, ";                  soup memory update event list\n");
    tfprintf(fp, ";\n");
    tfprintf(fp, ";     dest         source          ip\n");
    tfprintf(fp, "; ------------- ------------- -------------\n");
    tfprintf(fp, ";  offset   trk  offset   trk");
    tfprintf(fp, "  offset   trk  instr  thrd  instP\n");
    tfprintf(fp, "; --------- --- --------- --- --------- ---");
    tfprintf(fp, " ------- ---- --------\n");
}

/*
 * Rpt_soup_update_event - report soup update event
 *
 * fp - pointer to FILE structure for disassembly listing
 * g - pointer to g_list structure
 * i - event list index
 */
void Rpt_soup_update_event(fp, g, i)
FILE *fp;
Pgl g;
I32s i;
{   SoupUpdEvent *soupupdevtbuf;
    SoupUpdEvent **soupupdevtar=g->glst_thrdanadat.mtad_soupupdevt.suea_event;
    I32s eventcode;
    I8s *mn;

    soupupdevtbuf=soupupdevtar[i];
    if(soupupdevtbuf)
    {   eventcode=soupupdevtbuf->supd_eventcode;
        mn=&((id[eventcode].mn)[0]);
        tfprintf(fp, ";");
        Rpt_Event_List_Offset(fp,
            (I32s)(soupupdevtbuf->supd_dstoffsettype),
            soupupdevtbuf->supd_dstoffset);
        tfprintf(fp, " %3d", soupupdevtbuf->supd_dtra);
        Rpt_Event_List_Offset(fp,
            (I32s)(soupupdevtbuf->supd_srcoffsettype),
            soupupdevtbuf->supd_srcoffset);
        tfprintf(fp, " %3d", soupupdevtbuf->supd_stra);
        Rpt_Event_List_Offset(fp,
            (I32s)(soupupdevtbuf->supd_ipoffsettype),
            soupupdevtbuf->supd_ipoffset);
        tfprintf(fp, " %3d", soupupdevtbuf->supd_extrk);
        tfprintf(fp, " %-7s", mn);
        tfprintf(fp, " %4d", soupupdevtbuf->supd_thrd);
        tfprintf(fp, " %8d", soupupdevtbuf->supd_instP);
        tfprintf(fp, "\n");
    }
}

/*
 * Rpt_Event_List_Offset - report event list entry cell offset and type
 *
 * fp - pointer to FILE structure for disassembly listing
 * offsettype - offset relative to start of
 *    0 = self
 *    1 = own daughter
 *    2 = host
 *    3 = host daughter
 *    4 = self (free memory)
 * offset - given offset
 */
void Rpt_Event_List_Offset(fp, offsettype, offset)
FILE *fp;
I32s offsettype;
I32s offset;
{
    switch(offsettype)
    {   case 0:
            tfprintf(fp, " %1s", " ");
            break;
        case 1:
            tfprintf(fp, " %1s", "d");
            break;
        case 2:
            tfprintf(fp, " %1s", "h");
            break;
        case 3:
            tfprintf(fp, " %1s", "e");
            break;
        case 4:
            tfprintf(fp, " %1s", "f");
            break;
        default:
            tfprintf(fp, " %1s", "x");
            break;
    }
    tfprintf(fp, "%8d", offset);
}
#endif /* defined(TIERRA)||defined(ARGTIE)||defined(SOUPUPDTRC) */
