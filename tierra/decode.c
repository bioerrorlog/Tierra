/* @(#)decode.c 1.9 1/13/95 */
/* decode.c 30-3-98 decode functions for the Tierra Simulator */
/* Tierra Simulator V5.0: Copyright (c) 1991 - 1998 */
/* Tom Ray & Virtual Life */
/* MemProf labels: #03XX, in use: none */
/* FEError labels: #03XX, in use: none */

#include "license.h"
#include "tierra.h"
#include "globals.h"

/*
 * pnop - do nothing (for nop)
 */
void pnop()                     /* do nothing */
{   is.iip = 1;
}

/*
 * dec1s - decode one source value
 * use with: push(), regorder(), getregs(), surf(), surff(), tpings()
 *
 */
void dec1s()
{   I32s tval;
    is.iip = 1;
#if PLOIDY > 1
    is.dtra = ce->c.c->de;
    is.stra = ce->c.c->so;
#endif /* PLOIDY > 1 */
#ifdef SHADOW
    if (ce->c.d->idf.H)
        tval = ce->c.c->re[NUMREG + ce->c.d->re[0]] + flaw();
    else
#endif /* SHADOW */
    {   if (ce->c.d->idf.So)  /* using source toggle register */
            tval = flaw() + ce->c.c->cf.So.i;
        else    /* using fixed source register provided by opcode.map */
            tval = flaw() + ce->c.d->re[0];
    }
    is.sval = ce->c.c->re[mo(tval, NUMREG)] + flaw();
    switch (ce->c.d->mn[0])
    {   case 'g':  /* getregs() */
        {   is.sval = ce->c.d->re[0];  /* supply one of 3 direction modes */
            break;
        }
#ifdef SHADOW
        case 'A':
        case 'B':
        case 'C':
        case 'D':  /* regorder() */
        {   is.sval = flaw() + ce->c.d->re[0];  /* supply new top register */
            is.sval = mo(is.sval, NUMREG);
            break;
        }
#endif /* SHADOW */
        case 's':  /* surf(), surff() */
        {   if (!strcmp((const char *)(
                &(ce->c.d->mn[0])), (const char *)"surf"))
                is.sval3=0;
            else if (!strcmp((const char *)(
                &(ce->c.d->mn[0])), (const char *)"surff"))
                is.sval3=1;
            break;
        }
        default:   /* push(), tpingsnd() ok as is */
        break;
    }
}

/*
 * dec2s - decode two source values
 *         use with: ifz, ifE, ifS, ifZ, ifequal, ifless, ifgreater,
 *                   exch, surf
 *
 */

void dec2s()
{   I32s tval0, tval1;

    is.iip = 1;
#if PLOIDY > 1
    is.dtra = ce->c.c->de;
    is.stra = ce->c.c->so;
#endif /* PLOIDY > 1 */
#ifdef SHADOW
    if (ce->c.d->idf.H)
    {   tval0 = ce->c.c->re[NUMREG + ce->c.d->re[0]] + flaw();
        tval1 = ce->c.c->re[NUMREG + ce->c.d->re[1]] + flaw();
    }
    else
#endif /* SHADOW */
    {   if (ce->c.d->idf.De)  /* using destination toggle registers */
            tval0 = flaw() + ce->c.c->cf.De.i;
        else    /* using fixed destination registers provided by opcode.map */
            tval0 = flaw() + ce->c.d->re[0];
        if (ce->c.d->idf.So)  /* using source toggle registers */
            tval1 = flaw() + ce->c.c->cf.So.i;
        else    /* using fixed source registers provided by opcode.map */
            tval1 = flaw() + ce->c.d->re[1];
    }
    is.sval = ce->c.c->re[mo(tval0, NUMREG)] + flaw();
    is.sval2 = ce->c.c->re[mo(tval1, NUMREG)] + flaw();
    switch (ce->c.d->mn[2])
    {   case 'z':  /* ifz() */
        {   is.sval = (is.sval == 0);
            is.sval2 = 2;
            break;
        }
        case 'e':  /* ifequal() */
        {   is.sval = (is.sval == is.sval2);
            is.sval2 = 2;
            break;
        }
        case 'l':  /* ifless() */
        {   is.sval = (is.sval < is.sval2);
            is.sval2 = 2;
            break;
        }
        case 'g':  /* ifgrtr() */
        {   is.sval = (is.sval > is.sval2);
            is.sval2 = 2;
            break;
        }
        case 's':  /* ifsignal() */
        {   is.sval = issignal(is.sval);
            is.sval2 = 2;
            break;
        }
        case 'E':  /* ifE() */
        {   is.sval = ce->c.c->fl.E + flaw();
            is.sval2 = 2;
            break;
        }
        case 'S':  /* ifS() */
        {   is.sval = ce->c.c->fl.S + flaw();
            is.sval2 = 2;
            break;
        }
        case 'Z':  /* ifZ() */
        {   is.sval = ce->c.c->fl.Z + flaw();
            is.sval2 = 2;
            break;
        }
        case 'v':  /* divide() */
        {   is.mode = 2;
            if (ce->c.d->idf.C)
            {   is.sval = 0;  /* ce->c.d->re[0] + flaw(); */
                /*  is.sval2 = 0; */
            }
            break;
        }
        case 'c':  /* exch() */
        {   is.dreg = &(ce->c.c->re[mo(tval0, NUMREG)]);
            is.dreg2 = &(ce->c.c->re[mo(tval1, NUMREG)]);
            break;
        }
        case 'r':  /* surf(), surff() */
        {   if (!strcmp((const char *)(
                &(ce->c.d->mn[0])), (const char *)"surf"))
                is.sval3 = 0;
            else if (!strcmp((const char *)(
                &(ce->c.d->mn[0])), (const char *)"surff"))
                is.sval3 = 1;
            break;
        }
        default:   /* ok as is */
        break;
    }
}

void dec3s() /* decode three source values */
{   I32s tval0, tval1, tval2;  /* use with: offset(), mal(), divide */

    is.iip = 1;
#if PLOIDY > 1
    is.dtra = ce->c.c->de;
    is.stra = ce->c.c->so;
#endif /* PLOIDY > 1 */
#ifdef SHADOW
    if (ce->c.d->idf.H)
    {   tval0 = ce->c.c->re[NUMREG + ce->c.d->re[0]] + flaw();
        tval1 = ce->c.c->re[NUMREG + ce->c.d->re[1]] + flaw();
        tval2 = ce->c.c->re[NUMREG + ce->c.d->re[2]] + flaw();
    }
    else
#endif /* SHADOW */
    {   if (ce->c.d->idf.So)  /* using source toggle registers */
        {   tval0 = flaw() + ce->c.c->cf.So.i;
            tval1 = (ce->c.c->cf.So.t + 1) % IDregs.So.n;
            tval1 = flaw() + IDregs.So.r[tval1];
            tval2 = (ce->c.c->cf.So.t + 2) % IDregs.So.n;
            tval2 = flaw() + IDregs.So.r[tval2];
        }
        else
            /* using fixed source registers provided by opcode.map */
        {   tval0 = flaw() + ce->c.d->re[0];
            tval1 = flaw() + ce->c.d->re[1];
            tval2 = flaw() + ce->c.d->re[2];
        }
    }
    is.sval = ce->c.c->re[mo(tval0, NUMREG)];
    is.sval2 = ce->c.c->re[mo(tval1, NUMREG)];
    is.sval3 = ce->c.c->re[mo(tval2, NUMREG)];
    switch (ce->c.d->mn[2])
    {   case 'v':  /* divide() */
        {   is.mode = 2;
            if (ce->c.d->idf.C)
            {   is.sval = 0;  /* ce->c.d->re[0] + flaw(); */
                /*  is.sval2 = 0; */
            }
            break;
        }
        default:   /* ok as is */
        break;
    }
}

void dec4s() /* decode four source values */
{   I32s tval0, tval1, tval2, tval3;  /* use with: divide */

    is.iip = 1;
#if PLOIDY > 1
    is.dtra = ce->c.c->de;
    is.stra = ce->c.c->so;
#endif /* PLOIDY > 1 */
#ifdef SHADOW
    if (ce->c.d->idf.H)
    {   tval0 = ce->c.c->re[NUMREG + ce->c.d->re[0]] + flaw();
        tval1 = ce->c.c->re[NUMREG + ce->c.d->re[1]] + flaw();
        tval2 = ce->c.c->re[NUMREG + ce->c.d->re[2]] + flaw();
        tval3 = ce->c.c->re[NUMREG + ce->c.d->re[3]] + flaw();
    }
    else
#endif /* SHADOW */
    {   if (ce->c.d->idf.So)  /* using source toggle registers */
        {   tval0 = flaw() + ce->c.c->cf.So.i;
            tval1 = (ce->c.c->cf.So.t + 1) % IDregs.So.n;
            tval1 = flaw() + IDregs.So.r[tval1];
            tval2 = (ce->c.c->cf.So.t + 2) % IDregs.So.n;
            tval2 = flaw() + IDregs.So.r[tval2];
            tval3 = (ce->c.c->cf.So.t + 3) % IDregs.So.n;
            tval2 = flaw() + IDregs.So.r[tval3];
        }
        else
            /* using fixed source registers provided by opcode.map */
        {   tval0 = flaw() + ce->c.d->re[0];
            tval1 = flaw() + ce->c.d->re[1];
            tval2 = flaw() + ce->c.d->re[2];
            tval3 = flaw() + ce->c.d->re[3];
        }
    }
    is.sval = ce->c.c->re[mo(tval0, NUMREG)];
    is.sval2 = ce->c.c->re[mo(tval1, NUMREG)];
    is.sval3 = ce->c.c->re[mo(tval2, NUMREG)];
    is.sval4 = ce->c.c->re[mo(tval3, NUMREG)];
    switch (ce->c.d->mn[2])
    {   case 'v':  /* divide() */
        {   is.mode = 2;
            if (ce->c.d->idf.C)
            {   is.sval = 0;  /* ce->c.d->re[0] + flaw(); */
                /*  is.sval2 = 0; */
            }
            break;
        }
        default:   /* ok as is */
        break;
    }
}

/*
 * dec1d - decode one destination register
 *         use with: shl, shr, not, notl, not0, pop, ret, tpingrec,
 *         getip, getipp, getippf
 *
 * Returns:
 */
void dec1d()
{   I32s tval;

    is.iip = 1;
#if PLOIDY > 1
    is.dtra = ce->c.c->de;
    is.stra = ce->c.c->so;
#endif /* PLOIDY > 1 */
#ifdef SHADOW
    if (ce->c.d->idf.H)
        tval = ce->c.c->re[NUMREG + ce->c.d->re[0]] + flaw();
    else
#endif /* SHADOW */
    {   if (ce->c.d->idf.De)  /* using destination toggle registers */
            tval = flaw() + ce->c.c->cf.De.i;
        else    /* using fixed destination registers provided by opcode.map */
            tval = flaw() + ce->c.d->re[0];
    }
    is.dreg = &(ce->c.c->re[mo(tval, NUMREG)]);
    switch (ce->c.d->mn[0])
    {   case 'r':  /* ret() */
        {   is.dreg = &(ce->c.c->ip);
            is.iip = 0;
            break;
        }
        case 'g':  /* getipp(), getippf() */
        {   if (!strcmp((const char *)(
                &(ce->c.d->mn[0])), (const char *)"getipp"))
                is.sval = 0;
            else if (!strcmp((const char *)(
                &(ce->c.d->mn[0])), (const char *)"getippf"))
                is.sval = 1;
            break;
        }
        default:   /* ok as is: ttime, shl, shr, not, notl, not0, pop,
                   tpingrec, getip */
        break;
    }
}

/*
 * dec1d1s - decode one destination register, one source value
 *           use with: inc(), dec(), decC(), dec2(), dec4(),
 *                     rand(), put(), zero(), movdd(), get(), split()
 *
 */
void dec1d1s()
{   I32s a, tval0, tval1;
    is.iip = 1;
#if PLOIDY > 1
    is.dtra = ce->c.c->de;
    is.stra = ce->c.c->so;
#endif /* PLOIDY > 1 */
#ifdef SHADOW
    if (ce->c.d->idf.H)
    {   tval0 = ce->c.c->re[NUMREG + ce->c.d->re[0]] + flaw();
        tval1 = ce->c.c->re[NUMREG + ce->c.d->re[1]] + flaw();
    }
    else
#endif /* SHADOW */
    {   if (ce->c.d->idf.De)  /* using destination toggle registers */
            tval0 = flaw() + ce->c.c->cf.De.i;
        else    /* using fixed destination registers provided by opcode.map */
            tval0 = flaw() + ce->c.d->re[0];
        if (ce->c.d->idf.So)  /* using source toggle registers */
            tval1 = flaw() + ce->c.c->cf.So.i;
        else    /* using fixed source registers provided by opcode.map */
            tval1 = flaw() + ce->c.d->re[1];
    }           /* destination register # */
    is.dval = mo(tval0, NUMREG);        /* only used for split() */
    is.dreg = &(ce->c.c->re[is.dval]);  /* careful, dval used by put() */
    is.sval = ce->c.c->re[mo(tval1, NUMREG)] + flaw();
    switch (ce->c.d->mn[0])
    {   case 'i':  /* inc() */
        {   is.sval2 = 1;
            break;
        }
        case 'd':  /* dec(), decC(), dec2(), dec4() */
        {   switch (ce->c.d->mn[3])
            {   case '4':  /* dec4() */
                {   is.sval2 = -4;
                    break;
                }
                case '2':  /* dec2() */
                {   is.sval2 = -2;
                    break;
                }
                default:  /* decC(), dec() */
                {   is.sval2 = -1;
                    break;
                }
            }
            break;
        }
        case 'r':  /* rand() */
        {   is.sval = tlrand();
            is.dran = 0;
            break;
        }
        case 'z':  /* zero() */
        {   is.sval = 0;
            break;
        }
#ifdef IO
        case 'p':  /* put() */
        {   is.dreg = GetFreeIOBuf(&(ce->c.pg.pgb_put), MaxPutBufSiz);
            break;
        }
        case 'g':  /* get() */
        {   if(ce->c.pg.pgb_get.pgbf_siz)
                is.sval=(ce->c.pg.pgb_get.pgbf_buf)[
                    (ce->c.pg.pgb_get.pgbf_buf)[
                    ce->c.pg.pgb_get.pgbf_siz]];
            else
                break;
            /* a = address of start of template */
            a = ad((ce->c.c->ip)+1);
            while (1)  /* find size of template, s = size */
            {
#if PLOIDY == 1
                if (soup[ad(a)] != Nop0 &&
                    soup[ad(a)] != Nop1)
#else /* PLOIDY > 1 */
                if (soup[ad(a)][is.stra] != Nop0 &&
                    soup[ad(a)][is.stra] != Nop1)
#endif /* PLOIDY > 1 */
                    break;
                a++;
                is.iip++;
            }
            break;
        }
#endif /* IO */
        default:   /* movdd(), ok as is */
        break;
    }
}

/*
 * dec1d2s - decode one destination register, two source values
 *           use with: add2, sub2, mul2, div2, and2, ior2, xor2; 2 registers
 *                     add,  sub,  mul,  div,  and,  ior,  xor ; 3 registers
 *
 */
void dec1d2s() /* decode one destination register, two source values */
{   I32s tval0, tval1=0, tval2;

    is.iip = 1;
#if PLOIDY > 1
    is.dtra = ce->c.c->de;
    is.stra = ce->c.c->so;
#endif /* PLOIDY > 1 */
#ifdef SHADOW
    if (ce->c.d->idf.H)
    {   tval0 = ce->c.c->re[NUMREG + ce->c.d->re[0]] + flaw();
        tval1 = ce->c.c->re[NUMREG + ce->c.d->re[1]] + flaw();
        tval2 = ce->c.c->re[NUMREG + ce->c.d->re[2]] + flaw();
    }
    else
#endif /* SHADOW */
    {   if (ce->c.d->idf.De)  /* using destination toggle registers */
        {   tval0 = flaw() + ce->c.c->cf.De.i;
            if (ce->c.d->mn[3] == '2')  /* 1 dest reg, 1 source reg */
                tval1 = flaw() + ce->c.c->cf.De.i;
        }
        else
            /* using fixed destination registers provided by opcode.map */
        {   tval0 = flaw() + ce->c.d->re[0];
            tval1 = flaw() + ce->c.d->re[1];
        }
        if (ce->c.d->idf.So)  /* using source toggle registers */
        {   if (ce->c.d->mn[3] != '2')  /* 1 dest reg, 2 source regs */
            {   tval1 = flaw() + ce->c.c->cf.So.i;
                tval2 = (ce->c.c->cf.So.t + 1) % IDregs.So.n;
                tval2 = flaw() + IDregs.So.r[tval2];
            }
            tval2 = flaw() + ce->c.c->cf.So.i;
        }
        else    /* using fixed source registers provided by opcode.map */
            tval2 = flaw() + ce->c.d->re[2];
    }
    is.dreg = &(ce->c.c->re[mo(tval0, NUMREG)]);
    is.sval = ce->c.c->re[mo(tval1, NUMREG)] + flaw();
    is.sval2 = ce->c.c->re[mo(tval2, NUMREG)];
    switch (ce->c.d->mn[0])
    {   case 's':  /* sub() */
        {   is.sval2 = -is.sval2;
            break;
        }
        default:   /* add(), mul(), div(), and(), ior(), xor() ok as is */
        break;
    }
}

/*
 * dec2d2s - decode 2 destination registers, 2 source values
 *
 */
void dec2d2s()
{   I32s tval0, tval1, tval2, tval3;

    is.iip = 1;
#if PLOIDY > 1
    is.dtra = ce->c.c->de;
    is.stra = ce->c.c->so;
#endif /* PLOIDY > 1 */
#ifdef SHADOW
    if (ce->c.d->idf.H)
    {   tval0 = ce->c.c->re[NUMREG + ce->c.d->re[0]] + flaw();
        tval1 = ce->c.c->re[NUMREG + ce->c.d->re[1]] + flaw();
        tval2 = ce->c.c->re[NUMREG + ce->c.d->re[2]] + flaw();
        tval3 = ce->c.c->re[NUMREG + ce->c.d->re[3]] + flaw();
    }
    else
#endif /* SHADOW */
    {   if (ce->c.d->idf.De)  /* using destination toggle registers */
        {   tval0 = flaw() + ce->c.c->cf.De.i;
            tval1 = (ce->c.c->cf.De.t + 1) % IDregs.De.n;
            tval1 = flaw() + IDregs.So.r[tval1];
        }
        else    /* using fixed destination registers provided by opcode.map */
        {   tval0 = flaw() + ce->c.d->re[0];
            tval1 = flaw() + ce->c.d->re[3];
        }
        if (ce->c.d->idf.So)  /* using source toggle registers */
        {   tval2 = flaw() + ce->c.c->cf.So.i;
            tval3 = (ce->c.c->cf.So.t + 1) % IDregs.So.n;
            tval3 = flaw() + IDregs.So.r[tval3];
        }
        else
            /* using fixed source registers provided by opcode.map */
        {   tval2 = flaw() + ce->c.d->re[2];
            tval3 = flaw() + ce->c.d->re[3];
        }
    }
    is.dreg = &(ce->c.c->re[mo(tval0, NUMREG)]);
    is.dreg2= &(ce->c.c->re[mo(tval1, NUMREG)]);
    is.sval = ce->c.c->re[mo(tval2, NUMREG)];
    is.sval2 = ce->c.c->re[mo(tval3, NUMREG)];
    is.dran = SoupSize;
    if(!strcmp((const char *)(&(ce->c.d->mn[0])),
        (const char *)"omal"))
    {   is.mode = (I8s)MemModeProt;
        is.sval=ad(is.sval);
    }
}

/*
 * dec3d2s - decode 3 destination registers, 2 source values
 *
 */
void dec3d2s() /* decode 3 destination registers, 2 source values */
{   I32s tval0, tval1, tval2, tval3, tval4;

    is.iip = 1;
#if PLOIDY > 1
    is.dtra = ce->c.c->de;
    is.stra = ce->c.c->so;
#endif /* PLOIDY > 1 */
#ifdef SHADOW
    if (ce->c.d->idf.H)
    {   tval0 = ce->c.c->re[NUMREG + ce->c.d->re[0]] + flaw();
        tval1 = ce->c.c->re[NUMREG + ce->c.d->re[1]] + flaw();
        tval2 = ce->c.c->re[NUMREG + ce->c.d->re[2]] + flaw();
        tval3 = ce->c.c->re[NUMREG + ce->c.d->re[3]] + flaw();
        tval4 = ce->c.c->re[NUMREG + ce->c.d->re[4]] + flaw();
    }
    else
#endif /* SHADOW */
    {   if (ce->c.d->idf.De)  /* using destination toggle registers */
        {   tval0 = flaw() + ce->c.c->cf.De.i;
            tval1 = (ce->c.c->cf.De.t + 1) % IDregs.De.n;
            tval1 = flaw() + IDregs.So.r[tval1];
            tval2 = (ce->c.c->cf.De.t + 2) % IDregs.De.n;
            tval2 = flaw() + IDregs.So.r[tval2];
        }
        else    /* using fixed destination registers provided by opcode.map */
        {   tval0 = flaw() + ce->c.d->re[0];
            tval1 = flaw() + ce->c.d->re[1];
            tval2 = flaw() + ce->c.d->re[2];
        }
        if (ce->c.d->idf.So)  /* using source toggle registers */
        {   tval3 = flaw() + ce->c.c->cf.So.i;
            tval4 = (ce->c.c->cf.So.t + 1) % IDregs.So.n;
            tval4 = flaw() + IDregs.So.r[tval4];
        }
        else
            /* using fixed source registers provided by opcode.map */
        {   tval3 = flaw() + ce->c.d->re[3];
            tval4 = flaw() + ce->c.d->re[4];
        }
    }
    is.dreg = &(ce->c.c->re[mo(tval0, NUMREG)]);
    is.dreg2= &(ce->c.c->re[mo(tval1, NUMREG)]);
    is.dreg3= &(ce->c.c->re[mo(tval2, NUMREG)]);
    is.sval = ce->c.c->re[mo(tval3, NUMREG)];
    is.sval2 = ce->c.c->re[mo(tval4, NUMREG)];
    is.dran = SoupSize;
    if(!strcmp((const char *)(&(ce->c.d->mn[0])),
        (const char *)"omal"))
    {   is.mode = (I8s)MemModeProt;
        is.sval=ad(is.sval);
    }
}

/*
 * dec1d3s - decode one destination register, three source values
 *           use with: use with: offset(), mal()
 *
 */
void dec1d3s()
{   I32s tval0, tval1, tval2, tval3;

    is.iip = 1;
#if PLOIDY > 1
    is.dtra = ce->c.c->de;
    is.stra = ce->c.c->so;
#endif /* PLOIDY > 1 */
#ifdef SHADOW
    if (ce->c.d->idf.H)
    {   tval0 = ce->c.c->re[NUMREG + ce->c.d->re[0]] + flaw();
        tval1 = ce->c.c->re[NUMREG + ce->c.d->re[1]] + flaw();
        tval2 = ce->c.c->re[NUMREG + ce->c.d->re[2]] + flaw();
        tval3 = ce->c.c->re[NUMREG + ce->c.d->re[3]] + flaw();
    }
    else
#endif /* SHADOW */
    {   if (ce->c.d->idf.De)  /* using destination toggle registers */
            tval0 = flaw() + ce->c.c->cf.De.i;
        else    /* using fixed destination registers provided by opcode.map */
            tval0 = flaw() + ce->c.d->re[0];
        if (ce->c.d->idf.So)  /* using source toggle registers */
        {   tval1 = flaw() + ce->c.c->cf.So.i;
            tval2 = (ce->c.c->cf.So.t + 1) % IDregs.So.n;
            tval2 = flaw() + IDregs.So.r[tval2];
            tval3 = (ce->c.c->cf.So.t + 2) % IDregs.So.n;
            tval3 = flaw() + IDregs.So.r[tval3];
        }
        else
            /* using fixed source registers provided by opcode.map */
        {   tval1 = flaw() + ce->c.d->re[1];
            tval2 = flaw() + ce->c.d->re[2];
            tval3 = flaw() + ce->c.d->re[3];
        }
    }
    is.dreg = &(ce->c.c->re[mo(tval0, NUMREG)]);
    is.sval = ce->c.c->re[mo(tval1, NUMREG)];
    is.sval2 = ce->c.c->re[mo(tval2, NUMREG)];
    is.sval3 = ce->c.c->re[mo(tval3, NUMREG)];
    is.dran = SoupSize;

    if((!strcmp((const char *)(&(ce->c.d->mn[0])),
        (const char *)"mal"))||
        (!strcmp((const char *)(&(ce->c.d->mn[0])),
        (const char *)"imal")))
    {   is.mode2=(I8s)(((MalMode==6)&&(is.sval3<0))?1:MalMode);
        is.mode=(I8s)MemModeProt;
    }
}

/* void puticc() write a value to the input buffer of other cell
 * ce->c.pg.pb[ce->c.pg.psiz]     == index of next output value to be read
 * ce->c.pg.pb[ce->c.pg.psiz + 1] == index of next output value to be written
 *
 * is.dcel  = destination cell, in whose buffer the value will be put
 * is.dreg  = destination "register" in the put buffer
 * is.dval3 = destination for address returned by adr()
 * is.mode3 = 0 = broadcast to other cells' get buffer
 *            1 = write to other cell's get buffer
 * is.sval  = value to be placed in the dest reg
 *
 * void adr(ce) find address of a template
 * is.mode  = search mode: 1 = forward, 2 = backward, 0 = outward
 * is.mode2 =  preference: 1 = forward, 2 = backward, and return for
 *        direction found: 1 = forward, 2 = backward, 3 = both, 0 = none
 * is.dval  = starting address for forward search
 * is.dval2 = starting address for backward search
 * is.dreg  = destination register where target address will be stored
 * is.dreg2 = destination register where template size will be stored
 * is.dreg3 = destination register where offset of target will be stored
 * is.sval  = return address if template size = 0
 * is.sval2 = template size, 0 = no template
 * is.sval3 = search limit, and return for distance actually searched
 * is.dmod  = modulus value for is.dreg
 * is.dmod2 = modulus value for is.dreg2
 * is.dmod3 = modulus value for is.dreg3
 * is.dran  = range to maintain for is.dreg
 * is.dran2 = range to maintain for is.dreg2
 * is.dran3 = range to maintain for is.dreg3
 *
 * write AX to output port; or if put template, put to input
 * port of creature(s) with complementary get template
 */
void pputicc()
{   I32s a, s = 0, tval;
    I8s md;

    is.iip = 1;
#if PLOIDY > 1
    is.dtra = ce->c.c->de;
    is.stra = ce->c.c->so;
#endif /* PLOIDY > 1 */
    a = ad(ce->c.c->ip + 1);  /* a = address of start of template */
    while (1)   /* find size of template, s = size */
    {
#if PLOIDY == 1
        if (soup[ad(a + s)] != Nop0 &&
            soup[ad(a + s)] != Nop1)
#else /* PLOIDY > 1 */
        if (soup[ad(a + s)][is.stra] != Nop0 &&
            soup[ad(a + s)][is.stra] != Nop1)
#endif /* PLOIDY > 1 */
            break;
        s++;
    }
#ifdef SHADOW
    if (ce->c.d->idf.H)
        tval = ce->c.c->re[NUMREG + ce->c.d->re[0]] + flaw();
    else
#endif /* SHADOW */
    {   if (ce->c.d->idf.So)  /* using source toggle registers */
            tval = flaw() + ce->c.c->cf.So.i;
        else    /* using fixed source registers provided by opcode.map */
            tval = flaw() + ce->c.d->re[0];
    }
    is.sval = ce->c.c->re[mo(tval, NUMREG)];
    if (s)
    {   is.dreg = &is.dval3;  /* dest register for address */
        is.dreg2 = &BitBucket;  /* dest reg for template size */
        is.dreg3 = &BitBucket;  /* dest reg for offset */
        is.sval2 = s;
        is.sval3 = Put_limit;
        is.dval = ce->mm.p + ce->mm.s;
        is.dval2 = ce->mm.p - 1;
        is.mode = 0;
        is.mode2 = 1;
        is.mode3 = 0;
        is.iip = s + 1;
    }
    else
    {
#ifdef SHADOW
        if (ce->c.d->idf.H)
            tval = ce->c.c->re[NUMREG + ce->c.d->re[1]] + flaw();
        else
#endif /* SHADOW */
        {   if (ce->c.d->idf.De)  /* using destination toggle registers */
                tval = flaw() + ce->c.c->cf.De.i;
            else  /* fixed destination registers provided by opcode.map */
                tval = flaw() + ce->c.d->re[1];
        }
        tval = ce->c.c->re[mo(tval, NUMREG)];
        tval = ad(tval);
        if (!IsFree(tval))
            WhichCell(tval, &is.dcel, &md);
        else
            is.dreg = &BitBucket;
        is.mode3 = 1;
    }
}

/* pvoid movid() - *(is.dins) = is.sval + flaw();
 * is.dval  = address of destination instruction
 * is.dins  = pointer to destination instruction
 * is.sval  = value to be moved to destination instruction
 * is.sval2 = original value of destination instruction
 */
void pmovid() /* soup [R0] = R1 */
{   I32s  tval0, tval1, tval2;

    is.iip = 1;
#if PLOIDY > 1
    is.dtra = ce->c.c->de;
    is.stra = ce->c.c->so;
#endif /* PLOIDY > 1 */
#ifdef SHADOW
    if (ce->c.d->idf.H)
    {   tval0 = ce->c.c->re[NUMREG + ce->c.d->re[0]] + flaw();
        tval1 = ce->c.c->re[NUMREG + ce->c.d->re[1]] + flaw();
        tval2 = ce->c.c->re[NUMREG + ce->c.d->re[2]] + flaw();
    }
    else
#endif /* SHADOW */
    {   if (ce->c.d->idf.De)  /* using destination toggle registers */
            tval0 = flaw() + ce->c.c->cf.De.i;
        else    /* using fixed destination registers provided by opcode.map */
            tval0 = flaw() + ce->c.d->re[0];
        if (ce->c.d->idf.So)  /* using source toggle registers */
            tval1 = flaw() + ce->c.c->cf.So.i;
        else    /* using fixed source registers provided by opcode.map */
            tval1 = flaw() + ce->c.d->re[1];
        if (ce->c.d->idf.Se)  /* using segment toggle registers */
            tval2 = flaw() + ce->c.c->cf.Se.i;
        else
            tval2 = flaw() + ce->c.d->re[2];
    }
    tval0 = ce->c.c->re[mo(tval0, NUMREG)] + flaw();
    if (ce->c.d->idf.C || (ce->c.d->idf.Se))
    {   tval2 = ce->c.c->re[mo(tval2, NUMREG)] + flaw();
        is.dval = ad(tval0 + tval2);
    }
    else
        is.dval = ad(tval0);
    is.sval = ce->c.c->re[mo(tval1, NUMREG)] + flaw();
#if PLOIDY == 1
    is.dins = &soup[is.dval];
#else /* PLOIDY > 1 */
    is.dins = &soup[is.dval][is.dtra];
#endif /* PLOIDY > 1 */
    is.sval2 = *(is.dins);
}

/* pmovdi() - *(is.dreg) = *(is.sins) + flaw();
 * is.sins = pointer to source instruction
 * is.sval = address of source instruction
 * is.dreg = destination register, where moved value will be placed
 * is.dmod = value by which to modulus destination register
 * is.dran = range within which to contain destination register
 */
void pmovdi() /* R0 = soup [R1] */
{   I32s tval0, tval1, tval2;

    is.iip = 1;
#if PLOIDY > 1
    is.dtra = ce->c.c->de;
    is.stra = ce->c.c->so;
#endif /* PLOIDY > 1 */
#ifdef SHADOW
    if (ce->c.d->idf.H)
    {   tval0 = ce->c.c->re[NUMREG + ce->c.d->re[0]] + flaw();
        tval1 = ce->c.c->re[NUMREG + ce->c.d->re[1]] + flaw();
        tval2 = ce->c.c->re[NUMREG + ce->c.d->re[2]] + flaw();
    }
    else
#endif /* SHADOW */
    {   if (ce->c.d->idf.De)  /* using destination toggle registers */
            tval0 = flaw() + ce->c.c->cf.De.i;
        else    /* using fixed destination registers provided by opcode.map */
            tval0 = flaw() + ce->c.d->re[0];
        if (ce->c.d->idf.So)  /* using source toggle registers */
            tval1 = flaw() + ce->c.c->cf.So.i;
        else    /* using fixed source registers provided by opcode.map */
            tval1 = flaw() + ce->c.d->re[1];
        if (ce->c.d->idf.Se)  /* using segment toggle registers */
            tval2 = flaw() + ce->c.c->cf.Se.i;
        else
            tval2 = flaw() + ce->c.d->re[2];
    }
    is.dreg = &(ce->c.c->re[mo(tval0, NUMREG)]);
    tval1 = ce->c.c->re[mo(tval1, NUMREG)] + flaw();
    if (ce->c.d->idf.C || ce->c.d->idf.Se)
    {   tval2 = ce->c.c->re[mo(tval2, NUMREG)] + flaw();
        is.sval = ad(tval1 + tval2);
    }
    else
        is.sval = ad(tval1);
#if PLOIDY == 1
    is.sins = &soup[is.sval];
#else /* PLOIDY > 1 */
    is.sins = &soup[is.sval][is.stra];
#endif /* PLOIDY > 1 */
}

/* pmovii() - *(is.dins) = *(is.sins);
 * is.dval  = address of destination instruction
 * is.dins  = pointer to destination instruction
 * is.sval  = address of source instruction
 * is.sins  = pointer to source instruction
 * is.dtra  = track of destination instruction
 * is.sval2 = original value of destination instruction
 */
void pmovii()
{   I32s tval0, tval1, tval2;

#ifdef SHADOW
    if (ce->c.d->idf.H)
    {   tval0 = ce->c.c->re[NUMREG + ce->c.d->re[0]] + flaw();
        tval1 = ce->c.c->re[NUMREG + ce->c.d->re[1]] + flaw();
        tval2 = ce->c.c->re[NUMREG + ce->c.d->re[2]] + flaw();
    }
    else
#endif /* SHADOW */
    {   if (ce->c.d->idf.De)  /* using destination toggle registers */
            tval0 = flaw() + ce->c.c->cf.De.i;
        else    /* using fixed destination registers provided by opcode.map */
            tval0 = flaw() + ce->c.d->re[0];
        if (ce->c.d->idf.So)  /* using source toggle registers */
            tval1 = flaw() + ce->c.c->cf.So.i;
        else    /* using fixed source registers provided by opcode.map */
            tval1 = flaw() + ce->c.d->re[1];
        if (ce->c.d->idf.Se)  /* using segment toggle registers */
            tval2 = flaw() + ce->c.c->cf.Se.i;
        else
            tval2 = flaw() + ce->c.d->re[2];
    }
    tval0 = ce->c.c->re[mo(tval0, NUMREG)] + flaw();
    tval1 = ce->c.c->re[mo(tval1, NUMREG)] + flaw();
    if (ce->c.d->idf.C || ce->c.d->idf.Se)
    {   tval2 = ce->c.c->re[mo(tval2, NUMREG)] + flaw();
        is.dval = ad(tval0 + tval2);
        is.sval = ad(tval1 + tval2);
    }
    else
    {   is.dval = ad(tval0);
        is.sval = ad(tval1);
    }
#if PLOIDY == 1
    is.dins = &soup[is.dval];
    is.sins = &soup[is.sval];
#else /* PLOIDY > 1 */
    is.dtra = ce->c.c->de;
    is.stra = ce->c.c->so;
    is.dins = &soup[is.dval][is.dtra];
    is.sins = &soup[is.sval][is.stra];
#endif /* PLOIDY > 1 */
    is.sval2 = *(is.dins);
    is.iip = 1;
}

/* void tcall() adr(); push(); */
/* void adr() find address of a template
 * is.mode  = search mode: 1 = forward, 2 = backward, 0 = outward
 * is.mode2 =  preference: 1 = forward, 2 = backward, and return for
 *        direction found: 1 = forward, 2 = backward, 3 = both, 0 = none
 * is.dval  = starting address for forward search
 * is.dval2 = starting address for backward search
 * is.dreg  = destination register where target address will be stored
 * is.dreg2 = destination register where template size will be stored
 * is.dreg3 = destination register where offset of target will be stored
 * is.sval  = return address if template size = 0
 * is.sval2 = template size, 0 = no template
 * is.sval3 = search limit, and return for distance actually searched
 * is.dmod  = modulus value for is.dreg
 * is.dmod2 = modulus value for is.dreg2
 * is.dmod3 = modulus value for is.dreg3
 * is.dran  = range to maintain for is.dreg
 * is.dran2 = range to maintain for is.dreg2
 * is.dran3 = range to maintain for is.dreg3
 */
/* void push() ce->c.c->sp = ++ce->c.c->sp % STACK_SIZE;
 *               ce->c.c->st[ce->c.c->sp] = is.sval + flaw();
 * is.sval = value to be pushed onto the stack
 */
void ptcall()                   /* push ip to stack, outward template jump */
{   I32s a, s = 0;              /* this maps to adr followed by push */
    is.iip = 1;
#if PLOIDY > 1
    is.dtra = ce->c.c->de;
    is.stra = ce->c.c->so;
#endif /* PLOIDY > 1 */
    a = ad(ce->c.c->ip + 1);  /* a = address of start of template */
    while (1)   /* find size of template, s = size */
    {
#if PLOIDY == 1
        if (soup[ad(a + s)] != Nop0 &&
            soup[ad(a + s)] != Nop1)
#else /* PLOIDY > 1 */
        if (soup[ad(a + s)][is.stra] != Nop0 &&
            soup[ad(a + s)][is.stra] != Nop1)
#endif /* PLOIDY > 1 */
            break;
        s++;
    }
    is.dreg = &(ce->c.c->ip);  /* destination register for address */
    is.dreg2 = &BitBucket;
    is.dreg3 = &BitBucket;
    is.sval = ad(ce->c.c->ip + s + 1);  /* address to be pushed onto stack */
    is.sval2 = s;  /* size of template */
    is.sval3 = Search_limit;
    is.dmod = SoupSize;
    is.dval = ad(a + s + 1);  /* start address for forward search */
    is.dval2 = ad(a - s - 1);  /* start address for backward search */
    is.mode = 0;  /* outward jump */
    is.mode2 = 1;
    is.iip = 0;
    switch (ce->c.d->mn[4])
    {   case 'o':  /* jmpo() */
        {   is.mode = 0;  /* outward jump */
            is.mode2 = 1;
            break;
        }
        case 'b':  /* jmpb() */
        {   is.mode = 2;  /* backward jump */
            is.mode2 = 2;
            break;
        }
        case 'f':  /* jmpf() */
        {   is.mode = 1;  /* forward jump */
            is.mode2 = 1;
            break;
        }
        default:   /* ok as is */
        break;
    }
}

/* void adr() find address of a template
 * is.mode  = search mode: 1 = forward, 2 = backward, 0 = outward
 * is.mode2 =  preference: 1 = forward, 2 = backward, and return for
 *        direction found: 1 = forward, 2 = backward, 3 = both, 0 = none
 * is.dval  = starting address for forward search
 * is.dval2 = starting address for backward search
 * is.dreg  = destination register where target address will be stored
 * is.dreg2 = destination register where template size will be stored
 * is.dreg3 = destination register where offset of target will be stored
 * is.sval  = return address if template size = 0
 * is.sval2 = template size, 0 = no template
 * is.sval3 = search limit, and return for distance actually searched
 * is.stra  = source track for template
 * is.dtra  = destination track for template
 * is.dmod  = modulus value for is.dreg
 * is.dmod2 = modulus value for is.dreg2
 * is.dmod3 = modulus value for is.dreg3
 * is.dran  = range to maintain for is.dreg
 * is.dran2 = range to maintain for is.dreg2
 * is.dran3 = range to maintain for is.dreg3
 *
 * return template size in cx
 * search for template, return address in ax
 */
void decadr()
{   I32s a, s = 0, tval0=0, tval1=0, tval2=0, tval3;

    is.iip = 1;
#if PLOIDY > 1
    is.dtra = ce->c.c->de;
    is.stra = ce->c.c->so;
#endif /* PLOIDY > 1 */
    a = ad(ce->c.c->ip + 1);  /* a = address of start of template */
    while (1)   /* find size of template, s = size */
    {
#if PLOIDY == 1
        if (soup[ad(a + s)] != Nop0 &&
            soup[ad(a + s)] != Nop1)
#else /* PLOIDY > 1 */
        if (soup[ad(a + s)][is.stra] != Nop0 &&
            soup[ad(a + s)][is.stra] != Nop1)
#endif /* PLOIDY > 1 */
            break;
        s++;
    }
#ifdef SHADOW
    if (ce->c.d->idf.H)
    {   if (s)
        {   tval0 = ce->c.c->re[NUMREG + ce->c.d->re[0]] + flaw();
            tval1 = ce->c.c->re[NUMREG + ce->c.d->re[1]] + flaw();
            tval2 = ce->c.c->re[NUMREG + ce->c.d->re[2]] + flaw();
        }
        tval3 = ce->c.c->re[NUMREG + ce->c.d->re[3]] + flaw();
    }
    else
#endif /* SHADOW */
/* PROBLEM: this require that 4 destination toggle register be specified */
    {   if (ce->c.d->idf.De)  /* using destination toggle registers */
        {   tval0 = flaw() + ce->c.c->cf.So.i;
            tval1 = (ce->c.c->cf.So.t + 1) % IDregs.So.n;
            tval1 = flaw() + IDregs.So.r[tval1];
            tval2 = (ce->c.c->cf.So.t + 2) % IDregs.So.n;
            tval2 = flaw() + IDregs.So.r[tval2];
            tval3 = (ce->c.c->cf.So.t + 3) % IDregs.So.n;
            tval3 = flaw() + IDregs.So.r[tval3];
        }
        else
        {   if (s)
            {   tval0 = flaw() + ce->c.d->re[0];
                tval1 = flaw() + ce->c.d->re[1];
                tval2 = flaw() + ce->c.d->re[2];
            }
            tval3 = flaw() + ce->c.d->re[3];
        }
    }
    if (s)
    {   is.dreg = &(ce->c.c->re[mo(tval0, NUMREG)]); /* dest reg for address */
        is.dreg2 = &(ce->c.c->re[mo(tval1, NUMREG)]);/* dest for templ size */
        is.dreg3 = &(ce->c.c->re[mo(tval2, NUMREG)]);/* dest reg for offset */
    }
    else
        is.dreg = is.dreg2 = is.dreg3 = &BitBucket;
    if (ce->c.d->idf.C)
    {   if (ce->c.d->re[0] < 0)
            is.dreg = &BitBucket;
        if (ce->c.d->re[1] < 0)
            is.dreg2 = &BitBucket;
        if (ce->c.d->re[2] < 0)
            is.dreg3 = &BitBucket;
        if (ce->c.d->re[3] < 0)
            tval3 = 0;
        else
            tval3 = ce->c.c->re[mo(tval3, NUMREG)];  /* start at offset */
    }
    else
        tval3 = ce->c.c->re[mo(tval3, NUMREG)];  /* start at offset */
    is.sval2 = s;  /* size of template */
    is.dmod = SoupSize;
    is.dmod3 = SoupSize;
    is.dran2 = SoupSize;
    is.dval = ad(a + s + tval3 + 1);  /* start address for forward search */
    is.dval2 = ad(a - s - tval3 - 1);  /* start address for backward search */
    is.sval3 = Search_limit - tval3;
    is.iip = s + 1;
    switch (ce->c.d->mn[3])
    {   case 'o':  /* adro() */
        {   is.mode = 0;  /* outward search */
            is.mode2 = 1;
            break;
        }
        case 'b':  /* adrb() */
        {   is.mode = 2;  /* backward search */
            is.mode2 = 2;
            break;
        }
        case 'f':  /* adrf() */
        {   is.mode = 1;  /* forward search */
            is.mode2 = 1;
            break;
        }
        default:   /* ok as is */
        break;
    }
}

/* void adr() find address of a template
 * is.mode  = search mode: 1 = forward, 2 = backward, 0 = outward
 * is.mode2 =  preference: 1 = forward, 2 = backward, and return for
 *        direction found: 1 = forward, 2 = backward, 3 = both, 0 = none
 * is.dval  = starting address for forward search
 * is.dval2 = starting address for backward search
 * is.dreg  = destination register where target address will be stored
 * is.dreg2 = destination register where template size will be stored
 * is.dreg3 = destination register where offset of target will be stored
 * is.sval  = return address if template size = 0
 * is.sval2 = template size, 0 = no template
 * is.sval3 = search limit, and return for distance actually searched
 * is.dmod  = modulus value for is.dreg
 * is.dmod2 = modulus value for is.dreg2
 * is.dmod3 = modulus value for is.dreg3
 * is.dran  = range to maintain for is.dreg
 * is.dran2 = range to maintain for is.dreg2
 * is.dran3 = range to maintain for is.dreg3
 */
void decjmp() /* template jump: jmpo(), jmpb(), jmpf() */
{   I32s a, s = 0, tval;

#if PLOIDY > 1
    is.dtra = ce->c.c->de;
    is.stra = ce->c.c->so;
#endif /* PLOIDY > 1 */
    a = ad(ce->c.c->ip + 1);  /* a = address of start of template */
    while (1)   /* find size of template, s = size */
    {
#if PLOIDY == 1
        if (soup[ad(a + s)] != Nop0 &&
            soup[ad(a + s)] != Nop1)
#else /* PLOIDY > 1 */
        if (soup[ad(a + s)][is.stra] != Nop0 &&
            soup[ad(a + s)][is.stra] != Nop1)
#endif /* PLOIDY > 1 */
            break;
        s++;
    }
#ifdef SHADOW
    if (ce->c.d->idf.H)
        tval = ce->c.c->re[NUMREG + ce->c.d->re[0]] + flaw();
    else
#endif /* SHADOW */
        tval = flaw() + ce->c.d->re[0];
    tval = ce->c.c->re[mo(tval, NUMREG)] + flaw();
    is.sval = ad(tval);  /* target for IP if s == 0 */
    is.dreg = &(ce->c.c->ip);  /* destination register for address */
    is.dreg2 = &BitBucket;  /* destination register for template size */
    is.dreg3 = &BitBucket;  /* dest reg for offset */
    is.sval2 = s;  /* size of template */
    is.sval3 = Search_limit;
    is.dval = ad(a + s + 1);  /* start address for forward search */
    is.dval2 = ad(a - s - 1);  /* start address for backward search */
    is.dmod = SoupSize;
    is.dran2 = SoupSize;
    is.iip = 0;
    switch (ce->c.d->mn[3])
    {   case 'o':  /* jmpo() */
        {   is.mode = 0;  /* outward jump */
            is.mode2 = 1;
            break;
        }
        case 'b':  /* jmpb() */
        {   is.mode = 2;  /* backward jump */
            is.mode2 = 2;
            break;
        }
        case 'f':  /* jmpf() */
        {   is.mode = 1;  /* forward jump */
            is.mode2 = 1;
            break;
        }
        default:   /* ok as is */
        break;
    }
}
