/* @(#)memalloc.c 1.8 2/13/95 */
/* memalloc.c 30-3-98 memory allocation routines for the Tierra Simulator */
/* Tierra Simulator V5.0: Copyright (c) 1991 - 1998 */
/* Tom Ray & Virtual Life */
/* MemProf labels: #08XX, in use: none */
/* FEError labels: #08XX, in use: 0800-0805 */

#include "license.h"
#include "tierra.h"
#include "globals.h"

/*
 * IsPriv - return 1 if given address is inside given cell, its
 *         daughter, or free memory
 *
 * cp - pointer to given cell
 * a  - soup address
 */
I8s IsPriv(cp, a)
    Pcells cp;
    I32s a;
{
#ifdef ERRORTIE
    if (a >= SoupSize || a < 0)
    {   tsprintf((char *)(&(Fbuf[0])),
            "Tierra IsPriv() error: address %d not in soup", a);
        FEError(-800, EXIT, WRITE, (char *)(&(Fbuf[0])));
    }
#endif
    if (IsInsideCell(cp, a))
        return 1;
    return IsFree(a);
}

/*
 * IsBitPriv - return 1 if given cell has the given privilege
 *           - at the given address
 *
 * cp - pointer to the given cell
 * a  - given address
 * mode - given privilege bits (set-privileged, cleared-not privileged)
 *        0 - execute
 *        1 - write
 *        2 - read
 */
I8s IsBitPriv(cp, a, mode)
    Pcells cp;
    I32s a;                /* address being checked */
    I32s mode;  /* modes: 1 bit = execute, 2 bit = write, 4 bit = read */
{   I32s gmode;

    if (a < 0 || a >= SoupSize)
        return 0;

    if (IsInsideCell(cp, a))
        gmode = MemModeMine;
    else if (IsFree(a))
        gmode = MemModeFree;
    else
        gmode = MemModeProt;

#if PLOIDY == 1
    if ((IsBit(mode, 0) && IsBit(gmode, 0)) ||
        (IsBit(mode, 1) && IsBit(gmode, 1)) ||
        (IsBit(mode, 2) && IsBit(gmode, 2)))
#else /* PLOIDY > 1 */
    if ((IsBit(mode, 0) && IsBit(gmode, 0)) ||
        (IsBit(mode, 1) && IsBit(gmode, 1)) ||
        (IsBit(mode, 2) && IsBit(gmode, 2)))
#endif /* PLOIDY > 1 */
        return 0;

    return 1;
}

#ifdef EXECPROT
/*
 * PrivExec - return 1 if given cell has execute privilege at 
 *            given address, otherwise return 0
 *
 * cp - pointer to given cell
 * a  - given soup address
 */
I8s PrivExec(cp, a)             /* return 1 if you have execute privelage */
    Pcells cp;
    I32s a;                     /* address being checked */
{   I32s gmode;

    if (a < 0 || a >= SoupSize)
        return 0;

    if (IsInsideCell(cp, a))
        return (I8s) (!IsBit(MemModeMine, 0));
    else if (IsFree(a))
        gmode = MemModeFree;
    else
        gmode = MemModeProt;

#if PLOIDY == 1
    if (IsBit(gmode, 0))
#else /* PLOIDY > 1 */
    if (IsBit(gmode, 0))
#endif /* PLOIDY > 1 */
        return 0;

    return 1;
}
#endif /* EXECPROT */

#ifdef WRITEPROT
/*
 * PrivWrite - return 1 if given cell has write privilege at
 *             given address, otherwise return 0
 *
 * cp - pointer to given cell
 * a  - given soup address
 */

I8s PrivWrite(cp, a)
    Pcells cp;
    I32s a;
{   I32s gmode;

    if (a < 0 || a >= SoupSize)
        return 0;

    if (IsInsideCell(cp, a))
        return (I8s) (!IsBit(MemModeMine, 1));
    else if (IsFree(a))
        gmode = MemModeFree;
    else
        gmode = MemModeProt;

#if PLOIDY == 1
    if (IsBit(gmode, 1))
#else /* PLOIDY > 1 */
    if (IsBit(gmode, 1))
#endif /* PLOIDY > 1 */
        return 0;

    return 1;
}
#endif /* WRITEPROT */

#ifdef READPROT
/*
 * PrivReadRange - return 1 if given cell has read privilege in
 *            a given address range, otherwise return 0
 *
 * cp - pointer to given cell
 * from, to  - given soup address range
 */
I8s PrivReadRange(cp, from, to) /* return 1 if you have read privelage */
    Pcells cp;
    I32s from, to;              /* address range being checked */
{   I32s i;

    for (i = from; i <= to; i++)
    {   if (!PrivRead(cp, i))
            return 0;
    }
    return 1;
}

/*
 * PrivRead - return 1 if given cell has read privilege at
 *            given address, otherwise return 0
 *
 * cp - pointer to given cell
 * a  - given soup address
 */
I8s PrivRead(cp, a)
    Pcells cp;
    I32s a;
{   I32s gmode, tmp;

    if (a < 0 || a >= SoupSize)
        return 0;

    if (IsInsideCell(cp, a))
    {   tmp = !IsBit(MemModeMine, 2);
        return (I8s) tmp;
    }
    else if (IsFree(a))
        gmode = MemModeFree;
    else
        gmode = MemModeProt;

#if PLOIDY == 1
    if (IsBit(gmode, 2))
#else /* PLOIDY > 1 */
    if (IsBit(gmode, 2))
#endif /* PLOIDY > 1 */
        return 0;

    return 1;
}
#endif /* READPROT */

/*
 * IsInsideCell - return 1 if given address is inside given cell
 *                or its daughter, otherwise return 0
 *
 * cp - given cell
 * a  - given soup address
 *
 */
/* check to see if address is inside allocated memory cell cp */
I8s IsInsideCell(cp, a)
    Pcells cp;
    I32s a;
{
#ifdef ERRORTIE
    if (a >= SoupSize || a < 0)
    {   tsprintf((char *)(&(Fbuf[0])),
            "Tierra IsInsideCell() error: address %d not in soup", a);
        FEError(-801, EXIT, WRITE, (char *)(&(Fbuf[0])));
    }
#endif
    if ((cp->mm.p <= a && a < cp->mm.p + cp->mm.s) ||
        (cp->md.s > 0 &&
            (cp->md.p <= a && a < cp->md.p + cp->md.s)))
        return 1;
    return 0;
}

/*
 * WhichCell - find cell with given address inside it
 *
 * a - given address (must be inside cell)
 * wce - pointer to pointer to cell found if any
 * md - pointer to mother/daughter flag to be returned
 * 
 * returns : *wce - pointer to cell found
 *           *md - 'm' - address is in adult
 *                 'd' - address is in embryo
 *
 */
void WhichCell(a, wce, md) /* find cell with address a */
    I32s a;        /* note: a must be in a cell!, call IsFree() before */
    Pcells Fp wce;  /* WhichCell() to find out if a is in a cell or not */
    I8s *md;
{   I32s ar, ci;
    Pcells te;

    for (ar = 0; ar < NumCelAr; ar++)
        for (ci = 0; ci < CelArSiz; ci++)
        {   if (ar == 0 && ci < 2)
                continue;
            te = &cells[ar][ci];
            if (te->ld)
            {   if (te->mm.p <= a && (te->mm.p + te->mm.s) > a)
                {   *wce = te;
                    *md = 'm';
                    return;
                }
                if (te->md.p <= a && (te->md.p + te->md.s) > a)
                {   *wce = te;
                    *md = 'd';
                    return;
                }
            }
        }
    tsprintf((char *)(&(Fbuf[0])),
        "Tierra WhichCell() error: address %d not found in a cell", a);
    FEError(-802, EXIT, WRITE, (char *)(&(Fbuf[0])));
}

/*
 * mal - allocate soup memory for a new cell
 *
 * sug_addr - pointer to actual address of block (to be returned)
 *            *sug_addr - suggested address for mal
 * sug_size - size of block to get
 * mode - address preference
 *    0 - first fit
 *    1 - better fit
 *    2 - random
 *    3 - mother's address
 *    4 - address in ax
 *    5 - stack top
 *    6 - address given in *sug_addr
 * 
 * returns: actual size, or 0 on failure */

I32s mal(sug_addr, sug_size, mode)
    I32s *sug_addr, sug_size, mode;
{   I32s padr = -1;
    I32s size, sad;

    if (sug_size <= 0 || sug_size == ce->md.s ||
        sug_size > MaxMalMult * ce->mm.s)
        return 0;
    size = (I32s) sug_size + flaw();
    if (MalSamSiz)
        size = ce->mm.s;
    if (!size)
        return 0;
    if (ce->md.s)
    {
#ifdef ERRORTIE
        if (ce->md.p < 0 || ce->md.p >= SoupSize)
            FEError(-803, EXIT, WRITE, "Tierra mal() error 1");
#endif /* DAN should check return val */
        MemDealloc(ce->md.p, ce->md.s);
        InitDeadMem(ce->md.p, ce->md.s, DeadMemInit);
        ce->d.MovOffMin = ce->d.MovOffMax = ce->d.mov_daught = ce->md.s = 0;
    }
    switch (mode)
    {   case 0:    /* first fit */
        {   while ((padr = MemAlloc(size, 0, SoupSize - 1)) < 0)
                if (reaper(1, 0, REAP_SOUP_FULL))
                    break;
            break;
        }
        case 2:    /* random preference */
        {   while ((padr = MemAlloc(size, sad = tlrand() % (SoupSize - size),
                        MalLimit)) < 0)
                if (reaper(1, sad, REAP_SOUP_FULL))
                    break;
            break;
        }
        case 3:    /* preference for mother's address */
        {   while ((padr = MemAlloc(size, ce->mm.p, MalLimit)) < 0)
                if (reaper(1, ce->mm.p, REAP_SOUP_FULL))
                    break;
            break;
        }
        case 4:    /* preference for ax address */
        {   while ((padr = MemAlloc(size, sad = mo(ce->c.c->re[0],
                            SoupSize - size), MalLimit)) < 0)
                if (reaper(1, sad, REAP_SOUP_FULL))
                    break;
            break;
        }
        case 5:    /* preference for top of stack address */
        {   while ((padr = MemAlloc(size, sad = mo(ce->c.c->st[ce->c.c->sp],
                            SoupSize - size), MalLimit)) < 0)
                if (reaper(1, sad, REAP_SOUP_FULL))
                    break;
            break;
        }
        case 6:    /* preference for suggested address (*sug_addr) */
        {   while ((padr = MemAlloc(size, sad = mo(*sug_addr, SoupSize - size),
                        MalLimit)) < 0)
                if (reaper(1, sad, REAP_SOUP_FULL))
                    break;
            break;
        }
        case 1:
        default:   /* better fit */
        {   while ((padr = MemAlloc(size, -1, 0)) < 0)
                if (reaper(1, -1, REAP_SOUP_FULL))
                    break;
            break;
        }
    }
    if (padr < 0 || padr >= SoupSize || !size)
        return 0;

    /* got a block, pass location (sug_addr) and size back  */

    ce->md.p = ad(padr);
    *sug_addr = ce->md.p;
    ce->md.s = size;
    ce->c.c->fl.E = 0;
    DownReperIf(ce);
    return size;
}

/*
 * InitDeadMem - initialize dead memory
 *
 * start - starting soup address
 * size  - size of block to initialize
 * type  - type of initialization
 *        0 = no change
 *        1 = zero
 *        2 = randomize
 */
void InitDeadMem(start, size, type)
I32s start, size, type;
{   I32s i, end=start+size;
#if PLOIDY > 1
    I32s j;
#endif /* PLOIDY > 1 */

    if(type)
        for(i=start; i<end; i++)
#if PLOIDY == 1
            soup[i]=(Instruction)((type==1)?0:
                (tirand()%((I32s)InstNum)));
#else /* PLOIDY > 1 */
            for(j=0; j<PLOIDY; j++)
                soup[i][j]=(Instruction)((type==1)?0:
                    (tirand()%((I32s)InstNum)));
#endif /* PLOIDY > 1 */
}
