/* @(#)operator.c 1.8 2/13/95 */
/* operator.c 30-3-98 genetic operator functions for the Tierra Simulator */
/* Tierra Simulator V5.0: Copyright (c) 1991 - 1998 */
/* Tom Ray & Virtual Life */
/* MemProf labels: #2300, in use: none 2300 */
/* FEError labels: #23XX, in use: none */

#include "license.h"
#include "tierra.h"
#include "globals.h"

Pcells NextSliceQueue P_((Pcells  oc));
Pcells MoveSliceQueue P_((Pcells  oc, I32s  n));
Pcells RandomCell P_((Pcells  cp));
Pcells RandomFecundCell P_((Pcells  cp));
void MutationOps P_((void));
Pcells FindRandCellOfSize P_((Pcells  cp, I32s  size, I32s  tol));
void CrossoverInstSamSiz P_((void));
void CopyCode P_((HpInst  dest, HpInst  source, I32s  size));
HpInst AssembleDaught P_((I32s  NumFrag, I32s  Adr1, I32s  Siz1,
    I32s  Adr2, I32s  Siz2, I32s  Adr3, I32s  Siz3));
void InsertionInst P_((void));
void DeletionInst P_((void));
void CrossoverInst P_((void));
I32s SharedGenOps P_((I32s  NumFrag, I32s  Adr1, I32s  Siz1,
    I32s  Adr2, I32s  Siz2, I32s  Adr3, I32s  Siz3));
I32s IsNop P_((HpInst  inst));
I32s CountSegments P_((I32s  Adr, I32s  Siz));
I32s FindStartSegN P_((I32s  Adr, I32s  Siz, I32s  SegN));
I32s FindEndSegN P_((I32s  Adr, I32s  Siz, I32s  SegN));
void DeletionSeg P_((void));
void InsertionSeg P_((void));
void CrossoverSeg P_((void));

#ifdef COMMENTS

/*************************

Genetic operations at the moment of birth:

    Operators that don't change the size DONE - verified

        Mutations: MutationOps() DONE - verified

            bit flip: mut_site() DONE - verified

            random replacement: mut_site() DONE - verified

        Crossover (same size): CrossoverInstSamSiz() DONE - verified

    Operators that change the size:

        Performed at Segment boundaries: DONE - verified

            Crossover (different size): CrossoverSeg() DONE - verified

            Insertion: InsertionSeg() DONE - verified

            Deletion: DeletionSeg() DONE - verified

        Performed at instruct boundaries: DONE - verified

            Crossover (different size): CrossoverInst() DONE - verified

            Insertion: InsertionInst() DONE - verified

            Deletion: DeletionInst() DONE - verified

        For size changing operations:
        Crossover and Deletion, both by segment and instruction,
        can be accomplished by first identifying two code segments
        that will be joined.  A common function can be used to
        join the segments.  Insertion differs in that three segments
        will be joined.  Steps:

        1) Identify the code segments to be included in the daughter
        2) Allocate an instruction array the size of the daughter
           to be assembled
        3) Copy the code segments into the instruction array.
        4) If the daughter to be assembled is the same size as the
           present daughter, copy the code segments into the
           daughter space and quit, otherwise:
        5) Free the daughter cell memory, and randomize that code.
        6) Allocate a new daughter cell of the required size.
        7) Copy the code from the instruction array to the daughter cell.
        8) Free the instruction array.

*************************

To select the donor for a crossover operation (ideas):

    Random parent

    Random parent with non-zero fecundity

    Parent with the highest fecundity
        within a fecundity class,
        the creature to most recently achieve that fecundity

*************************/

#endif /* COMMENTS */

/*
 * n a m e
 *
 * Called from instruct.c: divide()
 *
 * Returns:
 */
void GeneticOps()
{   MutationOps();
    CrossoverInstSamSiz();
    CrossoverInst();
    InsertionInst();
    DeletionInst();
    CrossoverSeg();
    InsertionSeg();
    DeletionSeg();
}

/*
 * Pcells NextSliceQueue(Pcells  oc)
 *
 * Pcells  oc    - is a pointer to the cell from whose point of view
 * we will select the next cell.
 *
 * Returns: pointer to the next cell structure in the slicer queue
 */
Pcells NextSliceQueue(oc)
Pcells  oc;
{   return &cells[oc->q.n_time.a][oc->q.n_time.i];
}

/*
 * Pcells MoveSliceQueue(Pcells  oc, I32s  n)
 *
 * Returns: pointer to cell structure located "n" places beyond the
 * reference cell (oc) in the slicer queue
 */
Pcells MoveSliceQueue(oc, n)
Pcells  oc;
I32s  n;
{   Pcells tc = oc;

    n %= NumCells;
    for (; n > 0; n--)
        tc = &cells[tc->q.n_time.a][tc->q.n_time.i];
    return tc;
}

/*
 * Pcells RandomCell(Pcells  cp)
 *
 * Returns: pointer to randomly selected cell structure.
 *
 */
Pcells RandomCell(cp)
Pcells  cp;
{
    return MoveSliceQueue(cp, tlrand() % NumCells);
}

/*
 * n a m e
 *
 * Returns:
 */
Pcells RandomFecundCell(cp)
Pcells  cp;
{   Pcells tc, start;

    start = tc = RandomCell(cp);
    while (!tc->d.fecundity)
    {   tc = &cells[tc->q.n_time.a][tc->q.n_time.i];
        if (tc == start)
            break;
    }
    return tc;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
void mutate()
{   I32s i,nclist=0;
    Pcells *clist=NULL;

    i = tlrand() % SoupSize;

#if PLOIDY == 1
    mut_site(soup + i, 0);
#else /* PLOIDY > 1 */
    mut_site(soup + i, tcrand() % PLOIDY);
#endif /* PLOIDY > 1 */

   MutBookeep(i,0,&clist,&nclist);
   CellMutBookeep(clist,nclist);
}

/*
 * void mut_site(s, track)
 *
 * MutBitProp proportion of the time, flips a randomly selected bit
 * in the instruction pointed to by the argument 's'.  The remainder
 * of the time, replaces the instruction with another instruction
 * selected at random from the entire instruction set.
 *
 * Returns: void
 */
void mut_site(s, track)
HpInst s;
I32s track;
{   if (tdrand() < MutBitProp) /* flip bit in instruction */
    {
#if PLOIDY == 1
    s[0] ^= (1 << (tirand() % (I32s) InstBitNum));
#else /* PLOIDY > 1 */
    s[0][track] ^= (1 << (tirand() % (I32s) InstBitNum));
#endif /* PLOIDY > 1 */
    }
    else /* replace instruction with randomly selected instruction */
    {
#if PLOIDY == 1
    s[0]=(Instruction)(tirand()%(I32s)InstNum);
#else /* PLOIDY > 1 */
    s[0][track]=(Instruction)(tirand()%(I32s)InstNum);
#endif /* PLOIDY > 1 */
    }
}

/*
 * n a m e
 *
 * does a mutation to the daughter of the currently active cell
 *
 * Returns:
 */
void MutationOps()
{   I32s  site, DaughtSize, DaughtGenStart;

    while (GenPerDivMut && !(tlrand() % GenPerDivMut))
    {   TotDivMut++;
        DaughtGenStart = ce->md.p + ce->d.MovOffMin;
        DaughtSize = ce->d.MovOffMax - ce->d.MovOffMin + 1;
        site = DaughtGenStart + (tlrand() % DaughtSize);
        mut_site(soup + ad(site), 0);
    }
}

/*
 * n a m e: Pcells FindRandCellOfSize(Pcells  cp, I32s  size, I32s  tol)
 *
 * Returns: pointer to a cell of the requested (genetic memory) size (size),
 * within the requested size tolerance (tol)
 * If a cell of the requested size can not be found, returns NULL
 */
Pcells FindRandCellOfSize(cp, size, tol)
Pcells  cp;
I32s  size, tol;
{   Pcells tc, start;

    start = tc = RandomCell(cp);
    while (tc->d.mg.s < size - tol || tc->d.mg.s > size + tol)
    {   tc = &cells[tc->q.n_time.a][tc->q.n_time.i];
        if (tc == start)
            return NULL;
    }
    return tc;
}

void CopyCode(dest, source, size)
HpInst  dest, source;
I32s  size;
{   I32s  i;

    for (i = 0; i < size; i++)
        *(dest + i) = *(source + i);
}

/*
 * n a m e
 *
 * A crossover operation will be applied to the daughter of the
 * currently active cell
 *
 * Returns:
 */
void CrossoverInstSamSiz()
{   I32s  CrossPoint, DaughtSize, DaughtGenStart, MateGenStart;
    I32s  MateSize, Siz, Adr1, Adr2;
    Pcells  mp;  /* pointer to mate cell */

    while (GenPerCroInsSamSiz && !(tlrand() % GenPerCroInsSamSiz))
    {   DaughtSize = ce->d.MovOffMax - ce->d.MovOffMin + 1;
        mp = FindRandCellOfSize(ce, DaughtSize, MateSizeEp);
        if (mp == NULL)
            return;
        TotCroInsSamSiz++;

        DaughtGenStart = ce->md.p + ce->d.MovOffMin;
        MateGenStart = mp->mm.p + mp->d.mg.p;
        MateSize = mp->d.mg.s;

        Siz = DaughtSize <= MateSize ? DaughtSize: MateSize;
        CrossPoint = 1 + (tlrand() % (Siz - 1));

        if (2 * CrossPoint > DaughtSize)
        {   Adr1 = DaughtGenStart + CrossPoint;
            Adr2 = MateGenStart + CrossPoint;
            Siz = Siz - CrossPoint;
            CopyCode(soup + Adr1, soup + Adr2, Siz);
        }
        else
        {   Adr1 = DaughtGenStart;
            Adr2 = MateGenStart;
            CopyCode(soup + Adr1, soup + Adr2, CrossPoint);
        }
    }
}

/*
 * n a m e
 *
 *
 * Returns:
 */
HpInst AssembleDaught(NumFrag, Adr1, Siz1,
    Adr2, Siz2, Adr3, Siz3)
I32s  NumFrag, Adr1, Siz1, Adr2, Siz2, Adr3, Siz3;
{   I32s  DaughtSiz;
    HpInst  daught;

    DaughtSiz = Siz1;
    if (NumFrag > 1)
        DaughtSiz += Siz2;
    if (NumFrag > 2)
        DaughtSiz += Siz3;
    daught = (HpInst) thcalloc((I32u) DaughtSiz, sizeof(Instruction), 2300);
    CopyCode(daught, soup + Adr1, Siz1);
    if (NumFrag > 1)
        CopyCode(daught + Siz1, soup + Adr2, Siz2);
    if (NumFrag > 2)
        CopyCode(daught + Siz1 + Siz2, soup + Adr3, Siz3);
    return daught;
}

/*
 * Operates on the daughter of the currently active cell
 *
 * Returns: 0 on success, 1 on failure
 */
I32s SharedGenOps(NumFrag, Adr1, Siz1, Adr2, Siz2, Adr3, Siz3)
I32s NumFrag, Adr1, Siz1, Adr2, Siz2, Adr3, Siz3;
{   I32s  NDaughtGenSize, ODaughtGenStart, ODaughtDataSize, NDaughtCellSize;
    I32s  GenSpace, ODaughtGenSize, NGenSiz, MovOffMin, MovOffMax;
    I32s  Thresh, mov_daught;
    HpInst  daught;

    NDaughtGenSize = Siz1 + Siz2 + Siz3;
    ODaughtGenStart = ce->md.p + ce->d.MovOffMin;
    ODaughtGenSize  = ce->d.MovOffMax - ce->d.MovOffMin + 1;
    ODaughtDataSize = ce->md.s - ODaughtGenSize;
    NDaughtCellSize = NDaughtGenSize + ODaughtDataSize;
    Thresh = (I32s) (NDaughtCellSize * MovPropThrDiv * PLOIDY);
    if ((NDaughtCellSize > MaxMalMult * ce->mm.s) ||
        (NDaughtCellSize < MinCellSize) ||
        (NDaughtGenSize < MinGenMemSiz) ||
        (NDaughtGenSize < Thresh))
        return 1;

    /*  2) Allocate an instruction array the size of the daughter
     *     to be assembled
     *  3) Copy the code segments into the instruction array.
     */

    daught = AssembleDaught(NumFrag, Adr1, Siz1, Adr2, Siz2, Adr3, Siz3);

    /*  4) If the daughter to be assembled is the same size as the
     *     present daughter, copy the code segments into the
     *     daughter space and go to step 8
     */

    if (NDaughtGenSize == ODaughtGenSize)
    {   CopyCode(soup + ODaughtGenStart, daught, NDaughtGenSize);
        tfree((void *) daught, NDaughtGenSize * sizeof(Inst), 2300);
        return 0;
    }

    /*  5) Randomize the code of the old daughter cell.
     */

    InitDeadMem(ODaughtGenStart, ODaughtGenSize, DeadMemInit);

    /*  6) If the daughter has changed size, free the daughter cell and
     *     allocate a new daughter cell of the required size.
     */

    if (NDaughtGenSize != ODaughtGenSize)
    {   MovOffMin = ce->d.MovOffMin;
        MovOffMax = ce->d.MovOffMax;
        mov_daught = ce->d.mov_daught;
        NDaughtCellSize = mal(&(ce->md.p), NDaughtCellSize, MalMode);
        ce->d.MovOffMin = MovOffMin;
        ce->d.MovOffMax = MovOffMax;
        ce->d.mov_daught = mov_daught;
    } /* mal() will return zero if second allocation is same size as first */

    /*  7) Copy the code from the instruction array to the daughter cell.
     */

    GenSpace = NDaughtCellSize - ce->d.MovOffMin;
    NGenSiz = NDaughtGenSize < GenSpace ? NDaughtGenSize : GenSpace;
        /* the above because mal may flaw the cell size */

    CopyCode(soup + ce->md.p + ce->d.MovOffMin, daught, NGenSiz);
    ce->d.MovOffMax = ce->d.MovOffMin + NGenSiz - 1;

    /*  8) Free the instruction array.
     */

    tfree((void *) daught, NDaughtGenSize * sizeof(Inst), 2300);
    return 0;
}

/*
 * Decides if it is time to perform InsertionInst,
 * if so, it selects the segments to be joined,
 * and passes them along to SharedGenOps()
 *
 * Returns:
 */
void InsertionInst()
{   I32s  ODaughtGenStart, ODaughtGenSize, Siz1, Siz2, Adr1, Adr2;
    I32s  Siz3, Adr3, MateGenStart, MateGenSize, InsOff;
    I32s  MateChunkStart, MateChunkSize;
    Pcells  mp;  /* pointer to mate cell */

    /*  0) Decide if it is time to do this operation
     */

    while (GenPerInsIns && !(tlrand() % GenPerInsIns))
    {
    /*  1) Identify the code segments to be included in the daughter
     */

        ODaughtGenStart = ce->md.p + ce->d.MovOffMin;
        ODaughtGenSize  = ce->d.MovOffMax - ce->d.MovOffMin + 1;
        InsOff = tlrand() % (ODaughtGenSize + 1);

        mp = RandomCell(ce);
        MateGenStart = mp->mm.p + mp->d.mg.p;
        MateGenSize  = mp->d.mg.s;
        MateChunkSize = 1 + tlrand() % MateGenSize;
        MateChunkStart = 1 + tlrand() % (MateGenSize - MateChunkSize + 1);

        if (!InsOff || InsOff == ODaughtGenSize)
        {   if (!InsOff)
            {   Adr1 = MateChunkStart;
                Siz1 = MateChunkSize;
                Adr2 = ODaughtGenStart;
                Siz2 = ODaughtGenSize;
            }
            else
            {   Adr1 = ODaughtGenStart;
                Siz1 = ODaughtGenSize;
                Adr2 = MateChunkStart;
                Siz2 = MateChunkSize;
            }
            if (SharedGenOps(2, Adr1, Siz1, Adr2, Siz2, 0, 0))
                return;
        }
        else
        {   Adr1 = ODaughtGenStart;
            Siz1 = InsOff;
            Adr2 = MateChunkStart;
            Siz2 = MateChunkSize;
            Adr3 = ODaughtGenStart + Siz1;
            Siz3 = ODaughtGenSize - Siz1;
            if (SharedGenOps(3, Adr1, Siz1, Adr2, Siz2, Adr3, Siz3))
                return;
        }
        TotInsIns++;
    }
}

/*
 * Decides if it is time to perform DeletionInst,
 * if so, it selects the segments to be joined,
 * and passes them along to SharedGenOps()
 * The deletion operator will delete up to 1/2 of the genome
 *
 * Returns:
 */
void DeletionInst()
{   I32s  ODaughtGenStart, ODaughtGenSize, Siz1, Siz2, Adr1, Adr2;
    I32s  DelSiz, DelOff;

    /*  0) Decide if it is time to do this operation
     */

    while (GenPerDelIns && !(tlrand() % GenPerDelIns))
    {
    /*  1) Identify the code segments to be included in the daughter
     */

        ODaughtGenStart = ce->md.p + ce->d.MovOffMin;
        ODaughtGenSize  = ce->d.MovOffMax - ce->d.MovOffMin + 1;
        DelSiz = 1 + (tlrand() % (ODaughtGenSize / 2));
        DelOff = tlrand() % (ODaughtGenSize - DelSiz + 1);

        Adr1 = ODaughtGenStart;
        Siz1 = DelOff;
        Adr2 = ODaughtGenStart + DelOff + DelSiz;
        Siz2 = ODaughtGenSize - (DelOff + DelSiz);;

        if (!SharedGenOps(2, Adr1, Siz1, Adr2, Siz2, 0, 0))
            TotDelIns++;
    }
}

/*
 * Decides if it is time to perform CrossoverInst,
 * if so, it selects the segments to be joined,
 * and passes them along to SharedGenOps()
 *
 * Returns:
 */
void CrossoverInst()
{   I32s  ODaughtGenStart, ODaughtGenSize, Siz1, Siz2, Adr1, Adr2;
    I32s  DCrossPoint, MCrossPoint, MateGenStart, MateSize;
    Pcells  mp;  /* pointer to mate cell */

    /*  0) Decide if it is time to do this operation
     */

    while (GenPerCroIns && !(tlrand() % GenPerCroIns))
    {
    /*  1) Identify the code segments to be included in the daughter
     */

        ODaughtGenStart = ce->md.p + ce->d.MovOffMin;
        ODaughtGenSize  = ce->d.MovOffMax - ce->d.MovOffMin + 1;
        DCrossPoint = 1 + (tlrand() % (ODaughtGenSize - 1));

        mp = RandomCell(ce);
        MateGenStart = mp->mm.p + mp->d.mg.p;
        MateSize = mp->d.mg.s;
        MCrossPoint = 1 + (tlrand() % (MateSize - 1));

        if (2 * DCrossPoint > ODaughtGenSize)
        {   Adr1 = ODaughtGenStart;
            Siz1 = DCrossPoint;
            Adr2 = MateGenStart + MCrossPoint;
            Siz2 = MateSize - MCrossPoint;
        }
        else
        {   Adr1 = MateGenStart;
            Siz1 = MCrossPoint;
            Adr2 = ODaughtGenStart + DCrossPoint;
            Siz2 = ODaughtGenSize - DCrossPoint;
        }

        if (!SharedGenOps(2, Adr1, Siz1, Adr2, Siz2, 0, 0))
            TotCroIns++;
    }
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s IsNop(inst)
HpInst  inst;
{   if (*inst == Nop0 || *inst == Nop1)
        return 1;
    return 0;
}

/*
 * I32s CountSegments(I32s  Adr, I32s  Siz)
 *
 * counts the number of segments in cell tc
 * segments are rather arbitrarily defined as being
 * bounded by Nop0 and Nop1.  Therefore, the sequence:
 * inc Nop0 inc
 * has two segments
 * I32s  Adr - is the offset into the soup where the "cell" begins
 *             this should be the start of the genetic data
 * I32s  Siz - is the size in bytes of the genetic part of the "cell"
 *
 * Returns: the number of segments
 */
I32s CountSegments(Adr, Siz)
I32s  Adr, Siz;
{   I32s    i, NumSegs = 0, IsTempl = 1, stop;
    HpInst  site;

    stop  = Adr + Siz;
    for (i = Adr; i < stop; i++)
    {   site = soup + i;
        if (IsTempl)
        {   if (!IsNop(site))
            {   IsTempl = 0;
            }
        }
        else
        {   if (IsNop(site))
            {   IsTempl = 1;
                NumSegs++;
            }
        }
    }
    site = soup + stop - 1;
    if (!IsNop(site))
        NumSegs++;
    return NumSegs;
}

/*
 * I32s FindStartSegN(I32s  Adr, I32s  Siz, I32s  SegN)
 *
 * I32s  Adr - is the offset into the soup where the "cell" begins
 *             this should be the start of the genetic data
 * I32s  Siz - is the size in bytes of the genetic part of the "cell"
 * I32s  SegN - is the count of the segment to be located.  The first
 *              segment in the "cell" is counted as #1.
 *
 * Returns: offset into soup of the start of segment N of cell tc
 * includes the starting template, if there is one
 */
I32s FindStartSegN(Adr, Siz, SegN)
I32s  Adr, Siz, SegN;
{   I32s    NumSegs = 1, IsTempl = 1, stop;
    HpInst  site;

    if (SegN == 1)
        return Adr;
    stop  = Adr + Siz;
    for (; Adr < stop; Adr++)
    {   Adr = ad(Adr);
        site = soup + Adr;
        if (IsTempl)
        {   if (!IsNop(site))
            {   IsTempl = 0;
            }
        }
        else
        {   if (IsNop(site))
            {   IsTempl = 1;
                NumSegs++;
            }
        }
        if (NumSegs == SegN)
            break;
    }
    return Adr;
}

/*
 * I32s FindEndSegN(I32s  Adr, I32s  Siz, I32s  SegN)
 *
 * I32s  Adr - is the offset into the soup where the "cell" begins
 *             this should be the start of the genetic data
 * I32s  Siz - is the size in bytes of the genetic part of the "cell"
 * I32s  SegN - is the count of the segment to be located.  The first
 *              segment in the "cell" is counted as #1.
 *
 * Returns: offset into soup of the end of segment N of the "cell"
 * not including the terminating template
 */
I32s FindEndSegN(Adr, Siz, SegN)
I32s  Adr, Siz, SegN;
{   I32s    NumSegs = 1, IsTempl = 1, stop;
    HpInst  site;

    stop  = Adr + Siz;
    for (; Adr < stop; Adr++)
    {   site = soup + Adr;
        if (IsTempl)
        {   if (!IsNop(site))
            {   IsTempl = 0;
            }
        }
        else
        {   if (IsNop(site))
            {   IsTempl = 1;
                NumSegs++;
            }
        }
        if (NumSegs == SegN + 1)
            break;
    }
    return Adr;
}

/*
 * Decides if it is time to perform DeletionSeg,
 * if so, it selects the segments to be joined,
 * and passes them along to SharedGenOps()
 * The deletion operator will delete up to 1/2 of the
 * segments in the genome
 *
 * Conventions:
 * DelOffSeg = the number of the first segment that will be deleted.
 *             the first segment in the code is counted as segment #1.
 *             The deletion will remove the template preceeding the start
 *             segment (if there is one), but not the template following
 *             the last deleted segment.
 * DelNumSeg = The number of segments to be deleted.
 *
 * Returns:
 */
void DeletionSeg()
{   I32s  ODaughtGenStart, ODaughtGenSize, Siz1, Siz2, Adr1, Adr2;
    I32s  DelSiz, DelOff, ODaughtNumSegs;
    I32s  DelNumSeg, DelOffSeg, ODaughtGenEnd;

    /*  0) Decide if it is time to do this operation
     */

    while (GenPerDelSeg && !(tlrand() % GenPerDelSeg))
    {
    /*  1) Identify the code segments to be included in the daughter
     */

        ODaughtGenStart = ce->md.p + ce->d.MovOffMin;
        ODaughtGenSize  = ce->d.MovOffMax - ce->d.MovOffMin + 1;
        ODaughtGenEnd   = ce->md.p + ce->d.MovOffMax + 1;
        ODaughtNumSegs  = CountSegments(ODaughtGenStart, ODaughtGenSize);
        if (ODaughtNumSegs < 2)
            return;
        DelNumSeg = 1 + tlrand() % (ODaughtNumSegs / 2);
        DelOffSeg = 1 + tlrand() % (ODaughtNumSegs - DelNumSeg + 1);
        DelOff = FindStartSegN(ODaughtGenStart, ODaughtGenSize, DelOffSeg);
        DelSiz = FindEndSegN(ODaughtGenStart, ODaughtGenSize,
            DelOffSeg + DelNumSeg - 1) - DelOff;

        if (DelOffSeg == 1 || DelOffSeg + DelNumSeg > ODaughtNumSegs)
        {   if (DelOffSeg == 1)
            {   Adr1 = DelOff + DelSiz;
                Siz1 = ODaughtGenEnd - Adr1;
            }
            else
            {   Adr1 = ODaughtGenStart;
                Siz1 = DelOff - ODaughtGenStart;
            }

            if (SharedGenOps(1, Adr1, Siz1, 0, 0, 0, 0))
                return;
        }
        else
        {   Adr1 = ODaughtGenStart;
            Siz1 = DelOff - ODaughtGenStart;
            Adr2 = DelOff + DelSiz;
            Siz2 = ODaughtGenEnd - Adr2;

            if (SharedGenOps(2, Adr1, Siz1, Adr2, Siz2, 0, 0))
                return;
        }
        TotDelSeg++;
    }
}

/*
 * Decides if it is time to perform InsertionSeg,
 * if so, it selects the segments to be joined,
 * and passes them along to SharedGenOps()
 *
 * Returns:
 */
void InsertionSeg()
{   I32s  ODaughtGenStart, ODaughtGenSize, Siz1, Siz2, Siz3, Adr1, Adr2, Adr3;
    I32s  MateGenStart, MChunkNumSegs;
    I32s  ODaughtNumSegs, DCrossSeg, MateGenSize, MateNumSegs, MChunkStartSeg;
    I32s  MateChunkStart, MateChunkSize, DCrossOff;
    Pcells  mp;  /* pointer to mate cell */

    /*  0) Decide if it is time to do this operation
     */

    while (GenPerInsSeg && !(tlrand() % GenPerInsSeg))
    {
    /*  1) Identify the code segments to be included in the daughter
     */

        ODaughtGenStart = ce->md.p + ce->d.MovOffMin;
        ODaughtGenSize  = ce->d.MovOffMax - ce->d.MovOffMin + 1;
        ODaughtNumSegs = CountSegments(ODaughtGenStart, ODaughtGenSize);
        if (!ODaughtNumSegs)
            return;
        DCrossSeg = 1 + (tlrand() % (ODaughtNumSegs + 1));

        mp = RandomCell(ce);
        MateGenStart = mp->mm.p + mp->d.mg.p;
        MateGenSize  = mp->d.mg.s;
        MateNumSegs = CountSegments(MateGenStart, MateGenSize);
        if (!MateNumSegs)
            return;
        MChunkNumSegs  = 1 + (tlrand() % MateNumSegs);
        MChunkStartSeg = 1 + tlrand() % (MateNumSegs - MChunkNumSegs + 1);
        MateChunkStart =
            FindStartSegN(MateGenStart, MateGenSize, MChunkStartSeg);
        MateChunkSize = FindEndSegN(MateGenStart, MateGenSize,
            MChunkStartSeg + MChunkNumSegs) - MateChunkStart;

        if (DCrossSeg == 1 || DCrossSeg == (ODaughtNumSegs + 1))
        {   if (DCrossSeg == 1)
            {   Adr1 = MateChunkStart;
                Siz1 = MateChunkSize;
                Adr2 = ODaughtGenStart;
                Siz2 = ODaughtGenSize;
            }
            else
            {   Adr1 = ODaughtGenStart;
                Siz1 = ODaughtGenSize;
                Adr2 = MateChunkStart;
                Siz2 = MateChunkSize;
            }
            if (SharedGenOps(2, Adr1, Siz1, Adr2, Siz2, 0, 0))
                return;
        }
        else
        {   DCrossOff =
                FindEndSegN(ODaughtGenStart, ODaughtGenSize, DCrossSeg);
            Adr1 = ODaughtGenStart;
            Siz1 = DCrossOff - Adr1;
            Adr2 = MateChunkStart;
            Siz2 = MateChunkSize;
            Adr3 = DCrossOff;
            Siz3 = ODaughtGenStart + ODaughtGenSize - Adr3;
            if (SharedGenOps(3, Adr1, Siz1, Adr2, Siz2, Adr3, Siz3))
                return;
        }
        TotInsSeg++;
    }
}

/*
 * Decides if it is time to perform CrossoverSeg,
 * if so, it selects the segments to be joined,
 * and passes them along to SharedGenOps()
 *
 * Returns:
 */
void CrossoverSeg()
{   I32s  ODaughtGenStart, ODaughtGenSize, ODaughtNumSegs;
    I32s  Siz1, Siz2, Adr1, Adr2;
    I32s  MateGenStart, DCrossSeg;
    I32s  MCrossSeg, MateNumSegs, MateGenSize;
    Pcells  mp;  /* pointer to mate cell */

    /*  0) Decide if it is time to do this operation
     */

    while (GenPerCroSeg && !(tlrand() % GenPerCroSeg))
    {
    /*  1) Identify the code segments to be included in the daughter
     */

        ODaughtGenStart = ce->md.p + ce->d.MovOffMin;
        ODaughtGenSize  = ce->d.MovOffMax - ce->d.MovOffMin + 1;
        ODaughtNumSegs = CountSegments(ODaughtGenStart, ODaughtGenSize);
        if (ODaughtNumSegs < 2)
            return;
        DCrossSeg = 2 + (tlrand() % (ODaughtNumSegs - 1));
        /* DCrossSeg marks the point at the beginning of segment N */
        /* acceptable cross points are segments 2 thru N */

        mp = RandomCell(ce);
        MateGenStart = mp->mm.p + mp->d.mg.p;
        MateGenSize  = mp->d.mg.s;
        MateNumSegs = CountSegments(MateGenStart, MateGenSize);
        if (MateNumSegs < 2)
            return;
        MCrossSeg = 2 + (tlrand() % (MateNumSegs - 1));

        if (2 * DCrossSeg > ODaughtNumSegs) /* use left chunk of daughter */
        {   Adr1 = ODaughtGenStart;
            Siz1 = FindEndSegN(ODaughtGenStart, ODaughtGenSize, DCrossSeg)
                - Adr1;
            Adr2 = FindStartSegN(MateGenStart, MateGenSize, MCrossSeg);
            Siz2 = MateGenStart + MateGenSize - Adr2;
        }
        else  /* use right chunk of daughter */
        {   Adr1 = MateGenStart;
            Siz1 = FindEndSegN(MateGenStart, MateGenSize, MCrossSeg)
                - Adr1;
            Adr2 = FindStartSegN(ODaughtGenStart, ODaughtGenSize, DCrossSeg);
            Siz2 = ODaughtGenStart + ODaughtGenSize - Adr2;
        }

        if (!SharedGenOps(2, Adr1, Siz1, Adr2, Siz2, 0, 0))
            TotCroSeg++;
    }
}
