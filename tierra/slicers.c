/* @(#)slicers.c 1.5 1/13/95 */
/* slicers.c 30-3-98 time slicing routines for the Tierra Simulator */
/* Tierra Simulator V5.0: Copyright (c) 1991 - 1998 */
/* Tom Ray & Virtual Life */
/* MemProf labels: #15XX, in use: none */
/* FEError labels: #15XX, in use: none */

#include "license.h"
#include "tierra.h"
#include "globals.h"

/*
 * SlicerPhoton - photon model time slicing algorithm
 *                selects currently executing cell at random
 *                SliceStyle = 1
 *
 */
void SlicerPhoton()
{   I32s size_slice;
    I32s a;
    I8s md;

    if (!NumCells)
        return;
    do
        a = tlrand() % SoupSize;
    while (IsFree(a));
    WhichCell(a, &ce, &md);
    size_slice = PhotonSlide(a, PhotonInst, PhotonSize, PhotonWidth);
    size_slice = (I32s) (10. * pow((double) size_slice, (double) PhotonPow));
    if (SizDepSlice)
    {   if(SlicePow==1.0)
            size_slice=(I32s)(((double)size_slice)*
                (((double)ce->mm.s)/((double)AverageSize)));
        else
            size_slice=(I32s)(((double)size_slice)*
                pow(((double)ce->mm.s)/((double)AverageSize),
                (double)SlicePow));
    }
    TimeSlice(size_slice);
}

/*
 * PhotonFit - find largest fit count of all tracks of photon
 *             at given address in photon search area
 *
 * a          - address in photon search area
 * PhotonInst - pointer to photon template
 * PhotonSize - photon size
 *
 * Returns: tfit - largest fit count of all tracks of photon
 */
I32s PhotonFit(a, PhotonInst, PhotonSize)
    I32s a;
    Instruction *PhotonInst;
    I32s PhotonSize;
{   I32s i, j, fit, tfit = 0;

    for (j = 0; j < PLOIDY; j++)
    {   fit = 0;
        for (i = 0; i < PhotonSize; i++)
#if PLOIDY == 1
            if (soup[ad(a + i)] == *(PhotonInst + i))
                fit++;
#else /* PLOIDY > 1 */
            if (soup[ad(a + i)][j] == PhotonInst[i][j])
                fit++;
#endif /* PLOIDY > 1 */
        if (fit > tfit)
            tfit = fit;
    }
    return tfit;
}

/*
 * PhotonSlide - find largest fit count of all tracks of photon
 *               in search area centered around given address
 *
 * a          - address of center of photon search area
 * PhotonInst - pointer to photon template
 * PhotonSize - photon size
 *
 * Returns: fit - largest fit count of all tracks of photon
 *                in search area
 */
I32s PhotonSlide(a, PhotonInst, PhotonSize, PhotonWidth)
    I32s a;
    Instruction *PhotonInst;
    I32s PhotonSize, PhotonWidth;
{   I32s i, ws2, tfit, fit = 0;

    ws2 = (PhotonSize + PhotonWidth) / 2;
    for (i = 0; i < PhotonWidth; i++)
    {   tfit = PhotonFit(ad(a - ws2 + i), PhotonInst, PhotonSize);
        if (tfit > fit)
            fit = tfit;
    }
    return fit;
}

/*
 * PhotonTranslate - translate photon mnemonic to 
 *                   photon instruction template
 *
 * PhotonInst - pointer to photon instruction template
 * PhotonWord - pointer to photon mnemonic string
 * 
 *
 * Returns:
 */
void PhotonTranslate(PhotonInst, PhotonWord)
    Instruction *PhotonInst;
    I8s *PhotonWord;
{   I32s i, j;

    for (j = 0; j < PLOIDY; j++)
    {
#if PLOIDY == 1
        for (i = 0; i < PhotonSize; i++)
        {   if (*(PhotonWord + i) > 47 && *(PhotonWord + i) < 58)
                *(PhotonInst + i) = *(PhotonWord + i) - 48;
            else if (*(PhotonWord + i) > 96 && *(PhotonWord + i) < 119)
                *(PhotonInst + i) = *(PhotonWord + i) - 87;
            else
                *(PhotonInst + i) = 0;
        }
#else /* PLOIDY > 1 */
        for (i = 0; i < PhotonSize; i++)
        {   if (*(PhotonWord + i) > 47 && *(PhotonWord + i) < 58)
                PhotonInst[i][j] = (*(PhotonWord + i) - 48);
            else if (*(PhotonWord + i) > 96 && *(PhotonWord + i) < 119)
                PhotonInst[i][j] = (*(PhotonWord + i) - 87);
            else
                PhotonInst[i][j] = 0;
        }
#endif /* PLOIDY > 1 */
    }
}

/*
 * SlicerQueue - non-random constant/"size dependent" time slice algorithm
 *               selects next cell in slicer queue for current cell
 *               SliceStyle = 0
 *
 * Returns:
 */
void SlicerQueue()
{   I32s size_slice;

    if (!NumCells)
        return;
    if (SizDepSlice)
        size_slice = (I32s) pow((double) ce->mm.s, (double) SlicePow);
    else
        size_slice = SliceSize;
    TimeSlice(size_slice);
    IncrSliceQueue();  /* increment ThisSlice to next cell in queue */
    while (InstExe.m && NumCells > NumCellsMin && LazyTol &&
        ce->d.repinst > RepInst * LazyTol)
    {
#ifdef NET
#ifdef BGL
        if (ce->d.BglTrack.node) /* this cell currently being tracked */
            MigrDeathMesg(ce);
#endif /* BGL */
#endif /* NET */
        ReapCell(ce, REAP_LAZY);
    }
}

/*
 * RanSlicerQueue - semirandom constant/"size dependent" time slice algorithm
 *                  selects next cell in slicer queue for current cell
 *                  SliceStyle = 2
 *
 * Returns:
 */
void RanSlicerQueue()
{   I32s size_slice, base_slice;

    if (!NumCells)
        return;

    if (SizDepSlice)
    {
#ifdef EXEINSTSIZTIMSLC
        base_slice=ce->d.PExeSegSiz;
#else
        base_slice=ce->mm.s;
#endif /* EXEINSTSIZTIMSLC */

        if (SlicePow == 1.0)  /* Speed hack by dan july 92 */
            size_slice = base_slice;
        else
            size_slice=(I32s)pow((double)base_slice, (double)SlicePow);
    }
    else
        size_slice = SliceSize;
    size_slice = ((I32s)(SlicFixFrac*size_slice))+
        (tlrand()%(((I32s)(SlicRanFrac*size_slice))+1));
    TimeSlice(size_slice);
    if (!NumCells)
        return;
    IncrSliceQueue();  /* increment ThisSlice to next cell in queue */
    while (InstExe.m && NumCells > NumCellsMin && LazyTol &&
        ce->d.repinst > RepInst * LazyTol)
    {
#ifdef NET
#ifdef BGL
        if (ce->d.BglTrack.node) /* this cell currently being tracked */
            MigrDeathMesg(ce);
#endif /* BGL */
#endif /* NET */
        ReapCell(ce, REAP_LAZY);
    }
}
