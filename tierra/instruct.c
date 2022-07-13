/* @(#)instruct.c       1.34 2/13/95 */
/* instruct.c 30-3-98 instruction set for the Tierra Simulator */
/* Tierra Simulator V5.0: Copyright (c) 1991 - 1998 */
/* Tom Ray & Virtual Life */
/* MemProf labels: #07XX, in use: 0700-0708 */
/* FEError labels: #07XX, in use: 0706-0707 710-714 0717-0731 */

#include "license.h"
#include "tierra.h"
#include "globals.h"

#ifdef BGL
#include "tbgl.h"
#endif /* BGL */

#if PLOIDY > 1
static void JumpTrack P_((void));
#endif /* PLOIDY > 1 */
static void adrfindtmp P_((I32s tmpcall));

/*
 * DoRPNd - downshift RPN register stack
 */
void DoRPNd()
{   I32s i;

    if (ce->c.d->idf.P)
        for (i = 1; i < NUMREG - 1; i++)
            ce->c.c->re[i] = ce->c.c->re[i + 1] + flaw();
}

/*
 * DoRPNu - upshift RPN register stack
 */
void DoRPNu()
{   I32s i;

    if (ce->c.d->idf.P)
        for (i = NUMREG - 1; i > 0; i--)
            ce->c.c->re[i] = ce->c.c->re[i - 1] + flaw();
}

/*
 * DoFlags - update cpu register flags
 */
void DoFlags()
{   ce->c.c->fl.E = ce->c.c->fl.S = ce->c.c->fl.Z = 0;
    if (*(is.dreg) == 0)
        ce->c.c->fl.Z = 1;
    if (*(is.dreg) < 0)
        ce->c.c->fl.S = 1;
}

/*
 * DoMods - subject *(is.dreg) to specified modulus if given
 *          otherwise if a range limit is given, and *(is.dreg) is outside
 *             the specified range, then zero *(is.dreg)
 *          clear modulus and range limit after use
 *
 * is.dreg - destination before modulus and range check
 * is.dmod - modulus or zero if none
 * is.dran - range limit or zero if none
 */
void DoMods()
{   if (is.dmod)
        *(is.dreg) = mo(*(is.dreg), is.dmod);
    else if (is.dran && (*(is.dreg) > is.dran || *(is.dreg) < -is.dran))
        *(is.dreg) = 0;
    is.dmod = is.dran = 0;
}

/*
 * DoMods2 - subject *(is.dreg) to specified modulus if given
 *          otherwise if a range limit is given, and *(is.dreg) is outside
 *             the specified range, then zero *(is.dreg)
 *          clear modulus and range limit after use
 *
 * is.dreg2 - destination before modulus and range check
 * is.dmod2 - modulus or zero if none
 * is.dran2 - range limit or zero if none
 */
void DoMods2()
{   if (is.dmod2)
        *(is.dreg2) = mo(*(is.dreg2), is.dmod2);
    else if (is.dran2 && (*(is.dreg2) > is.dran2 || *(is.dreg2) < -is.dran2))
        *(is.dreg2) = 0;
    is.dmod2 = is.dran2 = 0;
}

/*
 * DoMods3 - subject *(is.dreg) to specified modulus if given
 *          otherwise if a range limit is given, and *(is.dreg) is outside
 *             the specified range, then zero *(is.dreg)
 *          clear modulus and range limit after use
 *
 * is.dreg3 - destination before modulus and range check
 * is.dmod3 - modulus or zero if none
 * is.dran3 - range limit or zero if none
 */
void DoMods3()
{   if (is.dmod3)
        *(is.dreg3) = mo(*(is.dreg3), is.dmod3);
    else if (is.dran3 && (*(is.dreg3) > is.dran3 || *(is.dreg3) < -is.dran3))
        *(is.dreg3) = 0;
    is.dmod3 = is.dran3 = 0;
}

/*
 * nop - no operation
 * takes no arguments, returns no values
 * clears E, S, and Z cpu flags
 *
 */
void nop()                      /* no operation */
{   ce->c.c->fl.E = ce->c.c->fl.S = ce->c.c->fl.Z = 0;
}

#ifdef SHADOW

/* void regorder() adjust register order
 * is.sval  = the number of the register that will go to the top
 */
void regorder()                 /* adjust register order */
{   pushst();
    ce->c.c->re[NUMREG] = is.sval;
    ce->c.c->fl.E = ce->c.c->fl.S = ce->c.c->fl.Z = 0;
}

/*
 * pushst - push shadow register stack up once
 *          called by regorder()
 */
void pushst()
{   I32s i;

    for (i = (2 * NUMREG) - 1; i > NUMREG; i--)
        ce->c.c->re[i] = ce->c.c->re[i - 1];
}

#endif /* SHADOW */

/*
 * rollu - roll RPN register stack up once
 */
void rollu()
{   I32s tvar;
    I32s i;

    tvar = ce->c.c->re[i = NUMREG - 1] + flaw();
    for (; i > 0; i--)
        ce->c.c->re[i] = ce->c.c->re[i - 1] + flaw();
    ce->c.c->re[0] = tvar;
    ce->c.c->fl.E = ce->c.c->fl.S = ce->c.c->fl.Z = 0;
}

/*
 * rolld - roll RPN register stack down once
 */
void rolld()
{   I32s tvar;
    I32s i;

    tvar = ce->c.c->re[0] + flaw();
    for (i = 0; i < NUMREG - 1; i++)
        ce->c.c->re[i] = ce->c.c->re[i + 1] + flaw();
    ce->c.c->re[NUMREG - 1] = tvar;
    ce->c.c->fl.E = ce->c.c->fl.S = ce->c.c->fl.Z = 0;
}

/*
 * enter - RPN stack "enter" operation
 */
void enter()
{
    I32s i;

    for (i = NUMREG - 1; i > 0; i--)
        ce->c.c->re[i] = ce->c.c->re[i - 1] + flaw();
    ce->c.c->re[0] += flaw();
    ce->c.c->fl.E = ce->c.c->fl.S = ce->c.c->fl.Z = 0;
}

/*
 * exch - "execute" function for "register value exchange" instruction
 */
/* takes no arguments, returns no values (except clears flag) */
void exch()
{   *(is.dreg) = is.sval2;
    DoMods();
    *(is.dreg2) = is.sval;
    DoMods2();
    ce->c.c->fl.E = ce->c.c->fl.S = ce->c.c->fl.Z = 0;
}

/* void not() *(is.dreg) = ~(is.sval) + flaw();
 * is.dreg = destination register
 * is.sval = value whose bits will be flipped and put in dest reg
 * is.dmod = value by which to modulus destination register
 * is.dran = range within which to contain destination register
 */
void not()                      /* flip all bits of destination register */
{   *(is.dreg) = ~(*(is.dreg)) + flaw();
    DoMods();
    DoFlags();
}

/* void notl() *(is.dreg) = !(is.sval) + flaw();
 * is.dreg = destination register
 * is.sval = value whose logical not will be put in dest reg
 */
void notl()                     /* logical not of source value */
{   if (!flaw())
        *(is.dreg) = !(*(is.dreg));
    DoFlags();
}

/* void not0() *(is.dreg) ^= (1 + flaw());
 * is.dreg = destination register, whose bit will be flipped
 * is.dmod = value by which to modulus destination register
 * is.dran = range within which to contain destination register
 */
void not0()           /* flip low order bit of destination register */
{   *(is.dreg) ^= (1 + flaw());
    DoMods();
    DoFlags();
}

#ifdef FUTURE

/* BEGIN GLOBAL INTER-PROCESSOR COMMUNICATIONS */

void bcs()                      /* broadcast send */
{   }

void bcr()                      /* broadcast receive */
{   }

void bsu()                      /* broadcast sum */
{   }

void togpar()                   /* toggle participation */
{   }

void gloor()                    /* global or */
{   }

void pscan()                    /* plus scan of cpus */
{   }

/* END GLOBAL INTER-PROCESSOR COMMUNICATIONS */

/* BEGIN POINT TO POINT INTER-PROCESSOR COMMUNICATIONS */

void ppsend()       /* send IOS data to specific other processor */
{   }

void ppreceive()    /* receive IOS data to specific other processor */
{   }

void setag()        /* set tag, otherwise, tag is -1 */
{   }

void setsrc()       /* set processor to receive from, -1 means any proc */
{   }

void setdst()       /* set processor to send to */
{   }

void ifsfl()        /* if send flag is set, do next inst, otherwise skip */
{   }

/* END POINT TO POINT INTER-PROCESSOR COMMUNICATIONS */

#endif /* FUTURE */

/*
 * n a m e
 *
 * copy data from IOS struct to soup
 *
 * I32s   dest = offset into cell where data is written
 * IOS *  ios  = pointer to the IOS structure
 * I32s * size = size of the data written, return data
 *
 * Returns: 1 on success, 0 on failure
 */
I8s WriteIOSToSoup(cp, dest, ios, size)
Pcells  cp;
I32s  dest, *size;
IOS * ios;
{   I8u *buf;
    I8s ok = 1,svchr;
    I32s i, tadr, nethdrlen;
    MemThrdAnaDat *memthrdanadat=&(cp->cell_thrdanadat);
    Pcells *clist;
    I32s nclist;

#ifdef NET
    nethdrlen=20;
#else /* NET */
    nethdrlen=8;
#endif /* NET */
    /* create char buffer, copy tag, source address and data to buffer */
    /* tag = 4, source address = 16, data = len bytes */
    buf = (I8u *) tcalloc(nethdrlen + ios->d.len, sizeof(I8u), 701);
    *((Intu *) (buf + 0)) = ios->t;
#ifdef NET
    *((Intu *) (buf + 4)) = ios->s.address.node;
    *((Intu *) (buf + 8)) = (I32u) ios->s.address.portnb;
    *((Ints *) (buf + 12)) = ios->s.ranID;
    *((Ints *) (buf + 16)) = ios->d.len;
#else /* NET */
    *((Ints *) (buf + 4)) = ios->d.len;
#endif /* NET */
    memcpy(buf+nethdrlen, ios->d.d, ios->d.len);

    nclist=0;
    clist=NULL;
    for(i=0;i<(nethdrlen+(ios->d.len));i++)
    {   tadr = ad(dest + i);
#ifdef WRITEPROT
        if (PrivWrite(cp, tadr))
        {
#endif /* WRITEPROT */
            svchr=*((I8u *)(soup+tadr));
            *((I8u *)(soup+tadr))=*((I8u *)(buf+i));
            if(GeneBnker&&((*((I8u *)(soup+tadr)))!=svchr))
                MutBookeep(tadr,1,&clist,&nclist);
            AllocSoupUpdEvent(memthrdanadat, -1, tadr);
#ifdef WRITEPROT
        }
        else
            ok=0;
#endif /* WRITEPROT */
    }
    CellMutBookeep(clist,nclist);
    tfree(buf, (nethdrlen+ios->d.len)*sizeof(I8u), 701);
    *size = nethdrlen+ios->d.len;
    return ok;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
void InitIOS(ios)               /* initialize IOS structure */
    IOS *ios;
{   ios->t = 0;

#ifdef NET
    ios->e.address.node = ios->s.address.node = 0;
    ios->e.address.portnb = ios->s.address.portnb = (N16u)0;
    ios->e.ranID = ios->s.ranID = 0;
#endif /* NET */

    if (ios->d.d)
        tfree(ios->d.d, ios->d.len * sizeof(I8s), 702);
    ios->d.d = NULL;
    ios->d.len = 0;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I8s WriteIOS(cp, sou)   /* copy data from soup to IOS buffer */
    Pcells cp;
    I32s sou;           /* address in soup where data will be copied from */
{   IOS *ios;
    I8u *buf = (I8u *) soup + sou;
    I32s nethdrlen;
    I8s rtnval=0;

    ios = GetIOSOut(cp);
    InitIOS(ios);
    /* tag = 4, source address = 16, data = 4 + len bytes */
#ifdef READPROT
    if(PrivReadRange(cp,sou,sou+3))
    {
#endif /* READPROT */
        ios->t=(I32s)(*((Ints *)(buf)));
#ifdef READPROT
        if(PrivReadRange(cp,sou+4,sou+7))
        {
#endif /* READPROT */
        
#ifdef NET
            ios->s.address.node=(N32u)(*((Intu *)(buf+4)));
#ifdef READPROT
            if(PrivReadRange(cp,sou+8,sou+11))
            {
#endif /* READPROT */
                ios->s.address.portnb=(N16u)(*((Ints *)(buf+8)));
#ifdef READPROT
                if(PrivReadRange(cp,sou+12,sou+15))
                {
#endif /* READPROT */
                    ios->s.ranID=(I32s)(*((Ints *)(buf+12)));
#ifdef READPROT
                    if(PrivReadRange(cp,sou+16,sou+19))
                    {
#endif /* READPROT */
                        ios->d.len=(I32s)(*((Ints *)(buf+16)));
                        nethdrlen=20;
#else /* NET */
                        ios->d.len=(I32s)(*((Ints *)(buf+4)));
                        nethdrlen=8;
#endif /* NET */
                    
#ifdef READPROT
                        if(PrivReadRange(cp,sou+nethdrlen,
                            sou+nethdrlen+ios->d.len))
                        {
#endif /* READPROT */
                            ios->d.d=(I8s *)tcalloc(
                                ios->d.len,sizeof(I8s),702);
                            memcpy(ios->d.d, buf+
                                nethdrlen, ios->d.len);
                            rtnval=1;
#ifdef READPROT
                        }
                    }
                }
            }
        }
    }
#endif /* READPROT */
    if(!rtnval)
        InitIOS(ios);
    return rtnval;
}

/*
 * GetIOSOut - get address of IOS output structure for given cell
 *             create if non-existent
 *
 * cp - pointer to given cell
 */
IOS *GetIOSOut(cp)              /* get address of IOS output structure */
    Pcells cp;
{   if (!cp->c.io.siz)  /* if no buffer, make buffer */
    {   cp->c.io.siz = 1;
        cp->c.io.io = (IOS *) tcalloc((I32u) 1, (I32u) sizeof(IOS), 700);
    }
    return cp->c.io.io;
}
#ifdef NET

/* BEGIN NETWORK COMMUNICATIONS */

/*
 * GetFreeIOS - find an IOS to write incoming message t
 *
 * cp - cell to find IOS buffer for
 *
 * returns: pointer to free IOS buffer for incoming message to cell
 */
IOS *GetFreeIOS(cp)
    Pcells cp;
{   I32s i, nio;

    if (!cp->c.io.siz)  /* if no buffer, make buffer */
    {   cp->c.io.siz = 2;
        cp->c.io.nio = 1;
        cp->c.io.io = (IOS *) tcalloc((I32u) 2, (I32u) sizeof(IOS), 700);
    }
    if (cp->c.io.siz < 2)  /* there is an output struct, but no input buffer */
    {   cp->c.io.io = (IOS *) trecalloc((void *) cp->c.io.io,
            2 * sizeof(IOS), cp->c.io.siz * sizeof(IOS), 700);
        cp->c.io.siz = 2;
    }
    for (i = 0, nio = cp->c.io.nio; i < (cp->c.io.siz - 1); i++)
    {   if (!cp->c.io.io[nio].t)  /* if tag = 0 struct is free for use */
        {   memset((char *) &(cp->c.io.io[nio]), 0, sizeof(IOS));
            return &(cp->c.io.io[nio]);
        }
        nio++;
        if (nio >= cp->c.io.siz)
            nio = 1;  /* note: 0 struct is used for output, not input */
    }           /* note: MaxIOBufSiz counts both input and output structs */
    if (cp->c.io.siz < MaxIOBufSiz)
    {   cp->c.io.io = (IOS *) trecalloc((void *) cp->c.io.io,
            (cp->c.io.siz + 1) * sizeof(IOS), cp->c.io.siz * sizeof(IOS), 700);
     /* this loop moves the input buffers above nio to the end of the array */
        /* so that we put the fresh struct where nio points ? */
        for (i = cp->c.io.siz; i > cp->c.io.nio; i--)
            cp->c.io.io[i] = cp->c.io.io[i - 1];
        ++(cp->c.io.siz);
    }
    memset((char *) &(cp->c.io.io[cp->c.io.nio]), 0, sizeof(IOS));
    return &(cp->c.io.io[cp->c.io.nio]);
}

/*
 * GetTagIOS - find next IOS with specified tag, to read
 *
 * cp - pointer to cell in which to search
 * tag - IOS tag to search for
 *
 * returns: pointer to IOS buffer with given tag, or NULL if not found
 */
IOS *GetTagIOS(cp, tag)         /* find next IOS with specified tag, to read */
    Pcells cp;
    I32s tag;
{   I32s i, nio;

    if (cp->c.io.siz < 2)
        return NULL;
    for (i = 0, nio = cp->c.io.nio; i < (cp->c.io.siz - 1); i++)
    {   if (cp->c.io.io[nio].t == tag)
            return &(cp->c.io.io[nio]);
        nio++;
        if (nio >= cp->c.io.siz)
            nio = 1;  /* note: 0 struct is used for output, not input */
    }
    return NULL;
}

/*
 * IncrIOSnio - increment cp->c.io.nio and roll over at cp->c.io.siz
 *
 * cp - pointer to cell
 */
void IncrIOSnio(cp)
    Pcells cp;
{   (cp->c.io.nio)++;
    if (cp->c.io.nio >= cp->c.io.siz)
        cp->c.io.nio = 1;  /* note: 0 struct is used for output, not input */
}

/*
 * n a m e : ReadIOS
 *
 * copy data from IOS buffer to soup
 *
 * Pcells cp
 * I32s   tag    tag for type of message to be read
 * I32s   dest   address in soup where data will be written
 *
 * Returns: returns 1 on success, 0 on failure
 */
I8s ReadIOS(cp, tag, dest, size)
Pcells  cp;
I32s  tag, dest, *size;
{   IOS *ios;
    I8s ok;

    ios = GetTagIOS(cp, tag);
    if (ios == NULL)
        return 0;
    ok = WriteIOSToSoup(cp, dest, ios, size);
    InitIOS(ios);
    return ok;
}

#endif /* NET */

/* END NETWORK COMMUNICATIONS */

#ifdef IO

/* 
 * Preg GetFreeIOBuf - find get buf reg to write incoming message to
 *
 * cp - pointer to cell to receive incoming message
 * 
 * return: pointer to message buffer
 *
 * cp->c.pg.pb[cp->c.pg.gsiz]     == index of next output value to be read
 * cp->c.pg.pb[cp->c.pg.gsiz + 1] == index of next output value to be written
 */
Preg GetFreeIOBuf(putgetbuf, maxbufsize) /* find free PGbf element */
PGbf *putgetbuf;
I32s maxbufsize;
{   I32s i, npg, siz=putgetbuf->pgbf_siz;

    if (!siz)   /* if no buffer, make buffer */
    {   putgetbuf->pgbf_siz=1;
        putgetbuf->pgbf_buf=(Preg)tcalloc(3, sizeof(Reg), 703);
        return putgetbuf->pgbf_buf;
    }
    npg=(I32s)((putgetbuf->pgbf_buf)[siz+1]);
    if(npg<siz)  /* next location is within allocated buffer */
        return &((putgetbuf->pgbf_buf)[npg]);
    if(npg>=maxbufsize)
        FEError(-706, EXIT, WRITE,
            "Tierra GetFreeIOBuf() error in npg value");
    putgetbuf->pgbf_buf=(Preg)trecalloc(
        (void *)(putgetbuf->pgbf_buf),
        (siz+3)*sizeof(Reg), (siz+2)*sizeof(Reg), 703);
    for(i=siz+2; i>siz; i--)
        (putgetbuf->pgbf_buf)[i]=(putgetbuf->pgbf_buf)[i-1];
    siz=++(putgetbuf->pgbf_siz);
    if (npg >= siz)
        FEError(-707, EXIT, WRITE,
            "Tierra GetFreeIOBuf() error 2 in npg value");
    return &((putgetbuf->pgbf_buf)[npg]);
}

/* 
 * Preg GetFreePutBuf - find put buf reg to write incoming message to
 *
 * cp - pointer to cell to receive incoming message
 *
 * return: pointer to "put" buffer
 *
 * cp->c.pg.pb[cp->c.pg.psiz]     == index of next output value to be read
 * cp->c.pg.pb[cp->c.pg.psiz + 1] == index of next output value to be written
 */
void FreePGbf(pgbf)
PGbf *pgbf;
{   if(pgbf->pgbf_buf)
    {   tfree((void *)(pgbf->pgbf_buf),
            (pgbf->pgbf_siz)*sizeof(Reg), 703);
        memset((char *)pgbf, 0, sizeof(PGbf));
    }
}

void SavePGbf(pgbf,ouf)
PGbf *pgbf;
FILE *ouf;
{   if(pgbf->pgbf_buf)
        tfwrite((I8s Fp)(pgbf->pgbf_buf),
            sizeof(Reg), (pgbf->pgbf_siz)+2, ouf);
}

void ReadPGbf(pgbf,inf)
PGbf *pgbf;
FILE *inf;
{   if(pgbf->pgbf_buf)
    {   (pgbf->pgbf_buf)=(Preg)
            tcalloc((pgbf->pgbf_siz)+2, sizeof(Reg), 704);
        tfread((I8s Fp)(pgbf->pgbf_buf),
            sizeof(Reg), (pgbf->pgbf_siz)+2, inf);
    }
}

/* 
 * put - write a value to the output buffer (prayer)
 * ce->c.pg.pb[ce->c.pg.psiz]     == index of next output value to be read
 * ce->c.pg.pb[ce->c.pg.psiz + 1] == index of next output value to be written
 *
 * is.dreg = destination "register" in the put buffer
 * is.sval = value to be placed in the dest reg
 * is.dmod = value by which to modulus destination register
 * is.dran = range within which to contain destination register
 */
void put()   /* write a value to the output buffer (prayer) */
{   if((is.dreg)==(&BitBucket))
    {   ce->c.c->fl.E = 1;
        return;
    }
    movdd();
    WritePGbfVal(&(ce->c.pg.pgb_put), MaxPutBufSiz);
    ce->c.c->fl.E = ce->c.c->fl.S = ce->c.c->fl.Z = 0;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
/* void put() write a value to the output buffer
 * ce->c.pg.pb[ce->c.pg.psiz]     == index of next output value to be read
 * ce->c.pg.pb[ce->c.pg.psiz + 1] == index of next output value to be written
 *
 * is.dcel  = destination cell, in whose buffer the value will be put
 * is.dreg  = destination "register" in the put buffer
 * is.dval3 = destination for address returned by adr()
 * is.mode3 = 0 = broadcast to other cells' get buffer
 *            1 = write to other cell's get buffer
 *
 * void adr() find address of a template
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
 */
void puticc()          /* write a value to the output buffer */
{   Pcells dc = is.dcel;
    I32s ta, lpl = Put_limit;
    I8s tmode2 = is.mode2;

    if (is.dreg == &BitBucket)
    {   ce->c.c->fl.E = 1;
        return;
    }
    if (is.mode3)  /* write to other cell's get buffer */
        Write2Get(dc, is.sval);
    else
    {   adr();
        if (!ce->c.c->fl.E)
            do  /* broadcast to other cells' get buffer */
            {   if (is.mode2 == 1 || is.mode2 == 3)
                {   ta = is.dval - 1;
                    if ( (is.dcel = FindPutCell(ce, ad(ta)))!=0 )
                    {   Write2Get(is.dcel, is.sval);
                    }
                }
                if (is.mode2 == 2 || is.mode2 == 3)
                {   ta = is.dval2 - 1;
                    if ( (is.dcel = FindPutCell(ce, ad(ta)))!=0 )
                    {   Write2Get(is.dcel, is.sval);
                    }
                }
                is.mode2 = tmode2;
                lpl -= is.sval3;
                is.sval3 = lpl;
                is.dval++;
                is.dval2--;
                adr();
            }
            while (!ce->c.c->fl.E);
    }
    ce->c.c->fl.E = ce->c.c->fl.S = ce->c.c->fl.Z = 0;
}

/*
 * FindPutCell - find destination cell at given address for "put" message
 *
 * cp - pointer to cell with same execution track
 * adre - given address
 *
 * return - pointer to structure of cell at given address
 *
 */
Pcells FindPutCell(cp, adre)
    Pcells cp;
    I32s adre;
{   Pcells lcp;
    I8s md;

#if PLOIDY == 1
    if (strcmp((const char *)(&(id[
        soup[adre%InstNum]].mn[0])), (const char *)"get")
#else /* PLOIDY > 1 */
    if (strcmp((const char *)(&(id[soup[
        adre%InstNum][cp->c.c->ex]].mn[0])), (const char *)"get")
#endif /* PLOIDY > 1 */
        || adre < 0 || adre >= SoupSize || IsFree(adre))
        lcp = NULL;
    else
        WhichCell(adre, &lcp, &md);
    return lcp;
}

/*
 * ReadFPut - for god to read the data in the output buffer
 *
 * cp - pointer to cell with message to be read from the output buffer
 *
 * return: *value - value from output buffer
 *         0-failure, 1=success
 *
 * cp->c.pg.pb[cp->c.pg.psiz]     == index of next output value to be read
 * cp->c.pg.pb[cp->c.pg.psiz + 1] == index of next output value to be written
 *
 */
/* for god to read the data in the output buffer */
I8s ReadFPut(cp, value)
    Pcells cp;
    I32s *value;
{   return ReadPGbfVal(&(cp->c.pg.pgb_put), value);
}

I8s ReadPGbfVal(putgetbuf, value)
PGbf *putgetbuf;
I32s *value;
{   Preg pgbf_buf=putgetbuf->pgbf_buf;
    I32s pgbf_siz=putgetbuf->pgbf_siz;
    I32s wi, ri;

    ri=pgbf_siz;
    wi=ri+1;
    if(!pgbf_siz)    /* there is no buffer */
        return 0;
    if(pgbf_buf[ri]==pgbf_buf[wi])
        return 0;
    (*value)=pgbf_buf[pgbf_buf[ri]];
    pgbf_buf[ri]=(++(pgbf_buf[ri]))%pgbf_siz;
    return 1;
}

void WritePGbfVal(putgetbuf, maxpgbufsize)
PGbf *putgetbuf;
I32s maxpgbufsize;
{   Preg pgbf_buf=putgetbuf->pgbf_buf;
    I32s pgbf_siz=putgetbuf->pgbf_siz;
    I32s wi, ri;

    ri=pgbf_siz;
    wi=ri+1;
    if((pgbf_siz>1)&&(pgbf_buf[wi]==pgbf_buf[ri]))
        (pgbf_buf)[ri]=(++(pgbf_buf[ri]))%maxpgbufsize;
    pgbf_buf[wi]=(++(pgbf_buf[wi]))%maxpgbufsize;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
/* void get() read a value from the input buffer
 * ce->c.pg.gb[ce->c.pg.gsiz]     == index to next input value to be read
 * ce->c.pg.gb[ce->c.pg.gsiz + 1] == index to next input value to be written
 *
 * also specify the values used by movdd():
 * is.dreg = destination register, where moved value will be placed
 * is.sval = value to be placed in the dest reg
 * is.dmod = value by which to modulus destination register
 *
 * Note: As long as anything has ever been written to the get buffer,
 * we are allowed to read.  This may cause the re-reading of old data.
 *
 */
/* read a value from the input buffer */
void get()
{   I32s value;

    if(!ReadPGbfVal(&(ce->c.pg.pgb_get), &value))
    {   ce->c.c->fl.E = 1;
        return;
    }
    movdd();
    DoFlags();
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
/* void Write2Get(cp, value)  place value in input buffer of cell cp
 * cp->c.pg.pb[cp->c.pg.gsiz]     == index of next output value to be read
 * cp->c.pg.pb[cp->c.pg.gsiz + 1] == index of next output value to be written
 */
/* place value in input buffer of cell cp */
void Write2Get(cp, value)
Pcells cp;
I32s value;
{   Preg greg;
    greg=GetFreeIOBuf(&(cp->c.pg.pgb_get), MaxGetBufSiz);
    *greg=value;
    WritePGbfVal(&(cp->c.pg.pgb_get), MaxGetBufSiz);
}

/*
 * Broad2Get - broadcast value to input buffer of all cells
 *
 * value - value to be broadcast
 */

void Broad2Get(value)
    I32s value;
{   Pcells tce = ce;

    do
    {   Write2Get(tce, value);
        tce = &cells[tce->q.n_time.a][tce->q.n_time.i];
    }
    while (tce != tce);
}

#endif /* IO */

/*
 * clrf 0 clear all cpu flags
 */
void clrf()
{   ce->c.c->fl.E = ce->c.c->fl.S = ce->c.c->fl.Z =
        ce->c.c->fl.B = ce->c.c->fl.D = 0;
}

/*
 * clrfi - clear cpu flags associated with CPU cycles
 */
void clrfi()
{   ce->c.c->fl.E = ce->c.c->fl.S = ce->c.c->fl.Z = 0;
}

/*
 * clrrf - clear all cpu CRflags
 */
void clrrf()
{   ce->c.c->cf.Se.t = 0;
    ce->c.c->cf.Se.i = IDregs.Se.r[0];
    ce->c.c->cf.De.t = 0;
    ce->c.c->cf.De.i = IDregs.De.r[0];
    ce->c.c->cf.So.t = 0;
    ce->c.c->cf.So.i = IDregs.So.r[0];
    ce->c.c->fl.E = ce->c.c->fl.S = ce->c.c->fl.Z = 0;
}

void togbf()                    /* toggle bits flag */
{   ce->c.c->fl.B = (ce->c.c->fl.B)++ % 3;
    ce->c.c->fl.E = ce->c.c->fl.S = ce->c.c->fl.Z = 0;
}

void togdf()                    /* toggle direction flag */
{   (ce->c.c->fl.D)++;
    ce->c.c->fl.E = ce->c.c->fl.S = ce->c.c->fl.Z = 0;
}

void toger()                    /* toggle segment registers */
{   ++(ce->c.c->cf.Se.t);
    ce->c.c->cf.Se.t %= IDregs.Se.n;
    ce->c.c->cf.Se.i = IDregs.Se.r[(I32s)(ce->c.c->cf.Se.t)];
    ce->c.c->fl.E = ce->c.c->fl.S = ce->c.c->fl.Z = 0;
}

void togdr()                    /* toggle destination registers */
{   ++(ce->c.c->cf.De.t);
    ce->c.c->cf.De.t %= IDregs.De.n;
    ce->c.c->cf.De.i = IDregs.De.r[(I32s)(ce->c.c->cf.De.t)];
    ce->c.c->fl.E = ce->c.c->fl.S = ce->c.c->fl.Z = 0;
}

void togsr()                    /* toggle source registers */
{   ++(ce->c.c->cf.So.t);
    ce->c.c->cf.So.t %= IDregs.So.n;
    ce->c.c->cf.So.i = IDregs.So.r[(I32s)(ce->c.c->cf.So.t)];
    ce->c.c->fl.E = ce->c.c->fl.S = ce->c.c->fl.Z = 0;
}

/* 
 * ttime - *(is.dreg) = time
 *
 * is.dreg = destination register, where time will be placed
 *
 */
void ttime()
{   struct timeval now;

    tgettimeofday(&now, NULL);
    *(is.dreg) = now.tv_sec;
    DoFlags();
}

/* 
 * shl - shift left one bit
 *       *(is.dreg) <<= (Reg) (1 + flaw())
 *
 * is.dreg = destination register, whose bits will be shifted left
 * is.dmod = value by which to modulus destination register
 * is.dran = range within which to contain destination register
 *
 */
void shl()
{   *(is.dreg) <<= (Reg) (1 + flaw());
    DoMods();
    DoFlags();
}

/* shr - shift right one bit
 *       *(is.dreg) >>= (Reg) (1 + flaw());
 *
 * is.dreg = destination register, whose bits will be shifted right
 * is.dmod = value by which to modulus destination register
 * is.dran = range within which to contain destination register
 *
 */
void shr()                      /* shift right all bits in register */
{   *(is.dreg) >>= (Reg) (1 + flaw());
    DoMods();
    DoFlags();
}

/* 
 * skip -  if(is.sval) is.iip = 1 else is.iip = is.sval2 = 2;
 *
 * is.sval  = value to test
 * is.sval2 = amount to increment IP (2) if is.sval == 0
 * is.iip   = amount to increment IP (1) if is.sval == 1
 *
 */
void skip()  /* execute or skip next instruction, if is.sval == 0 */
{   if (!(is.sval))
        is.iip = (I8s)is.sval2;  /* is.sval2 = 2 to skip next instruction */
    ce->c.c->fl.E = ce->c.c->fl.S = ce->c.c->fl.Z = 0;
}

/* issignal(sig) returns 1 if the signal is found, 0 if not found
 * The entire signal buffer is scanned for a match, returning 1 if a match
 * is found, 0 otherwise
 */
I8s issignal(sig)
    I32s sig;
{   I32s i;

    for (i = 0; i < ce->c.sig.siz; i++)
        if (ce->c.sig.sig[i] == sig)
            return (I8s) 1;
    return (I8s) 0;
}

/* void addsignal(sig) add a signal to the cell's signal buffer
 * The value 0 indicates an empty location in the buffer.
 * The buffer is scanned for the first empty location, if one is
 * found, the new signal is places there.  If there are no empty
 * locations, and the buffer is less than MaxSigBufSiz, the buffer
 * is enlarged by one, and the signal is placed at the end of the buffer.
 * If the buffer is already MaxSigBufSiz, then the all the buffer elements
 * are slid down one position.  The first (oldest) buffer entry is lost
 * and the new signal is added to the end of the buffer.
 */
void addsignal(sig, cp)
    I32s sig;
    Pcells cp;
{   I32s i;

    for (i = 0; i < cp->c.sig.siz; i++)
        if (cp->c.sig.sig[i] == 0)
        {   cp->c.sig.sig[i] = (I8s)sig;
            return;
        }
    if (cp->c.sig.siz >= MaxSigBufSiz)
    {   for (i = 0; i < cp->c.sig.siz - 1; i++)
            cp->c.sig.sig[i] = cp->c.sig.sig[i + 1];
    }
    else
    {   cp->c.sig.sig = (I8s *) trecalloc((void *) cp->c.sig.sig,
            (cp->c.sig.siz + 1) * sizeof(I8s), cp->c.sig.siz * sizeof(I8s),
            706);
        (cp->c.sig.siz)++;
    }
    cp->c.sig.sig[cp->c.sig.siz - 1] = (I8s)sig;
}

/* void remsignal(sig) remove a signal from the cell's signal buffer
 * Arrival order in the buffer is maintained.  The first signal of
 * value sig is deleted from the buffer, and later arriving signals
 * are slid down the buffer to fill it's space, leaving a new empty
 * place at the end of the buffer.  If the named signal is not found
 * an error is reported.
 */
void remsignal(sig, cp)
    I32s sig;
    Pcells cp;
{   I32s i, found = 0;

    for (i = 0; i < cp->c.sig.siz; i++)
    {   if (!found && (cp->c.sig.sig[i] == sig))
        {   cp->c.sig.sig[i] = 0;
            found = 1;
        }
        if (found && (i < cp->c.sig.siz - 1))
        {   cp->c.sig.sig[i] = cp->c.sig.sig[i + 1];
            cp->c.sig.sig[i + 1] = 0;
        }
    }
    if (!found)
    {   tsprintf((char *)(&(Fbuf[0])),
            "Tierra remsignal() error, signal %d not in signal buffer", sig);
        FEError(-721, NOEXIT, NOWRITE, (char *)(&(Fbuf[0])));
    }
}

/* split - create a new cpu
 *
 * is.sval = source value for cpu number
 * is.dval = destination register # for new cpu number
 *
 */
void split()                    /* create a new processor */
{   Cpu *ncpu;

    ce->c.c->fl.E = ce->c.c->fl.S = ce->c.c->fl.Z = 0;
    if (ce->c.n >= MaxCpuPerCell)
    {   ce->c.c->fl.E = 1;
        return;
    }
    ce->c.ar = (Cpu *) trecalloc((void *) (ce->c.ar),
        (I32u) sizeof(Cpu) * (I32u) (ce->c.n + 1),
        (I32u) sizeof(Cpu) * (I32u) ce->c.n, 1902);
    ++(ce->c.n);
    if ((!ce->d.flaw && !ce->d.nonslfmut) && (ce->d.max_cpus < ce->c.n))
        ce->d.max_cpus = ce->c.n;
    ncpu = &(ce->c.ar[ce->c.n - 1]);
    ce->c.c = &(ce->c.ar[ce->c.ac]);
    *(ncpu) = *(ce->c.c);
    InitCpu(ncpu);
    ncpu->parthrdid=ce->c.c->threadid;
    (ncpu->ip)++;
    ncpu->ip = ad(ncpu->ip);
    ncpu->curcallvlcell=ce->c.c->curcallvlcell;
    ce->c.c->re[is.dval] = is.sval * 2;
#ifdef SHADOW
    if (ce->c.d->idf.H)
        ncpu->re[ncpu->re[NUMREG + is.dval]] = (is.sval * 2) + 1;
    else
#endif /* SHADOW */
        ncpu->re[is.dval] = (is.sval * 2) + 1;
    if (ce->c.c->sy.G > -1)
    {   ce->c.sy.sy[ce->c.c->sy.G].ncpu++;
        if (ce->c.sy.sy[ce->c.c->sy.G].sync)
            ce->c.sy.sy[ce->c.c->sy.G].sync++;
    }
}

void slicexit()
{   ce->c.c->slicexit = 1;
    ce->c.c->fl.E = ce->c.c->fl.S = ce->c.c->fl.Z = 0;
}

/*
 * The csync instruction permits synchronization of groups of
 * CPUs of an organism.  If a CPU is not yet a member of a
 * synchronization group, then the first time that it executes
 * csync, it will create a new sync group.  The sync group
 * identity is inherited when new CPUs are created by the split
 * instruction.  The only way to add CPUs to a sync group is by
 * splitting of the CPUs, and membership in a sync group is
 * permanent for the life of the CPU.  Once defined, the sync
 * group identity of a CPU can not be changed.
 *
 * Once it is already a member of a sync group, when a CPU
 * executes csync, it will stop executing and wait (losing its
 * time slice(s)), until all of the CPUs of its sync group have
 * also executed csync.  When all of the CPUs of the group have
 * executed csync, then they will all simultaneously begin
 * executing again.  Individual sync groups disappear when all
 * CPUs of the group halt (or join).
 *
 * Returns:
 */
void csync()                    /* synchronize cpus within a cell */
{   I32s i, group = -1;

    /* if no CPU in this cell has ever synchronized */
    if (!ce->c.sy.n)
    {   ce->c.sy.n = 1;
        ce->c.sy.sy = (Sync *) tcalloc(ce->c.sy.n, sizeof(Sync), 707);
    }

    /* if this CPU is not a member of a sync group */
    if (ce->c.c->sy.G < 0)
    {   /* make and assign new sync group */
        for (i = 0; i < ce->c.sy.n; i++)
            if (!ce->c.sy.sy[i].ncpu)
            {   group = ce->c.c->sy.G = i;
                break;
            }
        if (ce->c.c->sy.G < 0)
        {   ce->c.sy.sy = (Sync *) trecalloc(ce->c.sy.sy,
                (ce->c.sy.n + 1) * sizeof(Sync),
                ce->c.sy.n * sizeof(Sync), 707);
            group = ce->c.c->sy.G = ce->c.sy.n;
            (ce->c.sy.n)++;
        }
        ce->c.sy.sy[group].ncpu = 1;
        return;
    }

    group = ce->c.c->sy.G;

    /* if there is only one CPU in this sync group it can not start a sync */
    if (ce->c.sy.sy[group].ncpu < 2)
        return;

    /* start synchronization */
    if (!(ce->c.c->sy.sync) && !(ce->c.sy.sy[group].sync))
    {   ce->c.c->sy.sync = 1;
        ce->c.sy.sy[group].sync = ce->c.sy.sy[group].ncpu - 1;
    }
    /* if a synchronization has begun, but this CPU has not synched... */
    else if (!(ce->c.c->sy.sync) && ce->c.sy.sy[group].sync)
    {   (ce->c.sy.sy[group].sync)--;
        if (ce->c.sy.sy[group].sync) /* if not all CPU have synchronized yet */
            ce->c.c->sy.sync = 1;
        else /* if this is the last CPU to synchronize */
            for (i = 0; i < ce->c.n; i++)
                if (ce->c.ar[i].sy.G == group)
                    ce->c.ar[i].sy.sync = 0;
    }
    /* the following condition must be an error condition
       it should never occur
     */
    else if (ce->c.c->sy.sync && !(ce->c.sy.sy[group].sync))
    {   ce->c.c->sy.sync = 0;
        tsprintf((char *)(&(Fbuf[0])),
            "Tierra csync(): ce->c.c->sy.sync && !(ce->c.sy.sy[group].sync)");
        FEError(-730, NOEXIT, NOWRITE, (char *)(&(Fbuf[0])));
    }
    /* This condition is an already synchronized CPU trying to sync again,
       it is ignored.  It should never happen
     */
    else if (ce->c.c->sy.sync && ce->c.sy.sy[group].sync)
    {   tsprintf((char *)(&(Fbuf[0])),
            "Tierra csync(): ce->c.c->sy.sync && ce->c.sy.sy[group].sync");
        FEError(-731, NOEXIT, NOWRITE, (char *)(&(Fbuf[0])));
    }
    ce->c.c->fl.E = ce->c.c->fl.S = ce->c.c->fl.Z = 0;
}

/*
 * halt - destroy  the current cpu, if it is the only cpu, the cell dies
 */
void halt()
{   I32s i, j, group=-1;
    Cpu *cpu;

    if (ce->c.n == 1 && NumCells <= NumCellsMin)
        return;
#ifdef MICRO
    if (ce->c.c->threadid == BrkTrapCondBuf.curbrkdat.MicroCpuI)
    {   GoBrkClr();
        MC_step=1;
        tsprintf((char *)(&(Fbuf[0])),
            "cpu being monitored has halted");
        FEError(-719, NOEXIT, NOWRITE, (char *)(&(Fbuf[0])));
        FEBanner();
        FEStats();
    }
#endif /* MICRO */

    GenExExe(is.oip);

    cpu = ce->c.ar;
#ifdef ERRORTIE
    if (ce->c.n < 1)
        FEError(-712, EXIT, WRITE, "Tierra halt() error: ce->c.n < 1");
#endif /* ERRORTIE */
    if (ce->c.n == 1)  /* cell must die */
    {   ce->c.c->fl.E=0;
        AllocThrdAnaEvent(&(ce->cell_thrdanadat));
        CellHalt = 1;
        ThrdAnaDatColl(1,0, is.oip);
#ifdef NET
#ifdef BGL
        if (ce->d.BglTrack.node) /* this cell currently being tracked */
            MigrDeathMesg(ce);
#endif /* BGL */
#endif /* NET */
        ReapCell(ce, REAP_HALT);
        return;
    }
    /* if a synchronization is in progress, deal with it before halting */
    group = ce->c.c->sy.G;
    if ((group > -1) && ce->c.sy.sy)
    {   ce->c.sy.sy[group].ncpu--;
        if (!(ce->c.c->sy.sync) && ce->c.sy.sy[group].sync)
        {   ce->c.sy.sy[group].sync--;
            if (ce->c.sy.sy[group].sync) /* if not all CPU have synched yet */
                ce->c.c->sy.sync = 1;
            else /* if this is the last CPU to synchronize */
                for (i = 0; i < ce->c.n; i++)
                    if (ce->c.ar[i].sy.G == group)
                        ce->c.ar[i].sy.sync = 0;
        }
    }
    ce->c.c->fl.E=0;
    AllocThrdAnaEvent(&(ce->cell_thrdanadat));
    extthdcallvlintrvl(ce, ce->c.c);

    /* the following code removes one cpu from the cell */
    ce->c.ar = (Cpu *) tcalloc((I32u) (ce->c.n - 1), (I32u) sizeof(Cpu), 1902);
    for (i = 0, j = 0; i < ce->c.n - 1; i++, j++)
    {   if (i == ce->c.ac)
            j++;
        ce->c.ar[i] = cpu[j];
    }
    tfree((void *) cpu, ce->c.n * sizeof(Cpu), 1902);
    if((BrkTrapCondBuf.NewCpuI)>(ce->c.ac))
        (BrkTrapCondBuf.NewCpuI)--;
    --(ce->c.n);
    --(ce->c.ac);
    if (ce->c.ac < 0)
        ce->c.ac = ce->c.n - 1;
    ce->c.c = &(ce->c.ar[ce->c.ac]);
    CpuHalt = 1;
}

/*
 * join - destroy a cpu, but never cpu 0, 0 waits for others to exit
 */
void join()
{   I32s i, j, group;
    Cpu *cpu;

    if (!(ce->c.ac))  /* you are cpu 0 */
    {   ce->c.c->fl.E = ce->c.c->fl.S = ce->c.c->fl.Z = 0;
        if (ce->c.n == 1)  /* there is only one cpu, so you are finished */
        {   TierraWait = 0;
            return;
        }
        if (ce->c.n > 1)  /* must wait */
        {   TierraWait = 1;
            return;
        }
#ifdef ERRORTIE
        FEError(-710, EXIT, WRITE, "Tierra join() error: ce->c.n < 1");
#endif /* ERRORTIE */
    }
#ifdef MICRO
    if (ce->c.c->threadid == BrkTrapCondBuf.curbrkdat.MicroCpuI)
    {   GoBrkClr();
        MC_step=1;
        tsprintf((char *)(&(Fbuf[0])),
            "cpu being monitored has joined");
        FEError(-711, NOEXIT, NOWRITE, (char *)(&(Fbuf[0])));
        FEBanner();
        FEStats();
    }
#endif /* MICRO */
    cpu = ce->c.ar;
#ifdef ERRORTIE
    if (ce->c.n <= 1)
        FEError(-713, EXIT, WRITE, "Tierra join() error: ce->c.n <= 1");
#endif /* ERRORTIE */
    /* if a synchronization is in progress, deal with it before joining */
    group = ce->c.c->sy.G;
    if ((group > -1) && ce->c.sy.sy)
    {   ce->c.sy.sy[group].ncpu--;
        if (!(ce->c.c->sy.sync) && ce->c.sy.sy[group].sync)
        {   ce->c.sy.sy[group].sync--;
            if (ce->c.sy.sy[group].sync) /* if not all CPU have synched yet */
                ce->c.c->sy.sync = 1;
            else /* if this is the last CPU to synchronize */
                for (i = 0; i < ce->c.n; i++)
                    if (ce->c.ar[i].sy.G == group)
                        ce->c.ar[i].sy.sync = 0;
        }
    }
    ce->c.c->fl.E=0;
    AllocThrdAnaEvent(&(ce->cell_thrdanadat));
    /* the following code removes one cpu from the cell */
    ce->c.ar = (Cpu *) tcalloc((I32u) (ce->c.n - 1), (I32u) sizeof(Cpu), 1902);
    for (i = 0, j = 0; i < ce->c.n - 1; i++, j++)
    {   if (i == ce->c.ac)
            j++;
        ce->c.ar[i] = cpu[j];
    }
    tfree((void *) cpu, ce->c.n * sizeof(Cpu), 1902);
    if((BrkTrapCondBuf.NewCpuI)>(ce->c.ac))
        (BrkTrapCondBuf.NewCpuI)--;
    --(ce->c.n);
    TierraWait = 1;
}

/* 
 * add - add two numbers and store the result in a register
 *       *(is.dreg) = is.sval + is.sval2;
 * is.dreg  = destination register, where calculation will be stored
 * is.sval  = a value that will be added to is.sval2 and placed in dest reg
 * is.sval2 = a value that will be added to is.sval  and placed in dest reg
 * is.dmod  = value by which to modulus destination register
 * is.dran  = range within which to contain destination register
 *
 */
void add()
{   *(is.dreg) = is.sval + is.sval2;
    DoMods();
    DoFlags();
    DoRPNd();
}

/*
 * mul - multiply two numbers and store the result in a register 
 *       *(is.dreg) = is.sval * is.sval2;
 *
 * is.dreg  = destination register, where calculation will be stored
 * is.sval  = value that will be multiplied by is.sval2 and placed in dest reg
 * is.sval2 = value that will be multiplied by is.sval  and placed in dest reg
 * is.dmod  = value by which to modulus destination register
 * is.dran  = range within which to contain destination register
 */
void mul()
{   *(is.dreg) = is.sval * is.sval2;
    DoMods();
    DoFlags();
    DoRPNd();
}

/* 
 * idiv - divide two numbers and store the result in a register
 *        *(is.dreg) = is.sval / is.sval2;
 *
 * is.dreg  = destination register, where calculation will be stored
 * is.sval  = value that will be divided by is.sval2 and placed in dest reg
 * is.sval2 = value that will be divided into is.sval and placed in dest reg
 * is.dmod  = value by which to modulus destination register
 * is.dran  = range within which to contain destination register
 *
 */
void idiv()
{   I32s a=~(((I32u)(-1))>>1);
    I32s b=-1;

    if (!is.sval2)
    {   ce->c.c->fl.E = 1;
        *(is.dreg) = 2 * is.sval;
        return;
    }
    (*(is.dreg))=(((is.sval)==a)&&((is.sval2)==b))?
        (is.sval):((is.sval)/is.sval2);
    DoMods();
    DoFlags();
    DoRPNd();
}

/* and - bitwise "and" two numbers and store the result in a register 
 *       *(is.dreg) = is.sval & is.sval2;
 *
 * is.dreg  = destination register, where calculation will be stored
 * is.sval  = value that will be anded by is.sval2 and placed in dest reg
 * is.sval2 = value that will be anded into is.sval and placed in dest reg
 * is.dmod  = value by which to modulus destination register
 * is.dran  = range within which to contain destination register
 */
void and()
{/* if (!is.sval2)
    {   ce->c.c->fl.E = 1;
        return;
    }
 */
    *(is.dreg) = is.sval & is.sval2;
    DoMods();
    DoFlags();
    DoRPNd();
}

/* ior - bitwise inclusive or two numbers and store them in register
 *       *(is.dreg) = is.sval | is.sval2;
 *
 * is.dreg  = destination register, where calculation will be stored
 * is.sval  = value that will be ored by is.sval2 and placed in dest reg
 * is.sval2 = value that will be ored into is.sval and placed in dest reg
 * is.dmod  = value by which to modulus destination register
 * is.dran  = range within which to contain destination register
 */
void ior()
{   *(is.dreg) = is.sval | is.sval2;
    DoMods();
    DoFlags();
    DoRPNd();
}

/* xor - bitwise exclusive or two numbers and store the result in a register
 *       *(is.dreg) = is.sval ^ is.sval2;
 * is.dreg  = destination register, where calculation will be stored
 * is.sval  = value that will be ored by is.sval2 and placed in dest reg
 * is.sval2 = value that will be ored into is.sval and placed in dest reg
 * is.dmod  = value by which to modulus destination register
 * is.dran  = range within which to contain destination register
 */
void xor()
{   *(is.dreg) = is.sval ^ is.sval2;
    DoMods();
    DoFlags();
    DoRPNd();
}

/* offset - multiply two numbers, add them to another,
 *          and store the result in a register
 *
 *          *(is.dreg) = is.sval + (is.sval2 * is.sval3) + flaw();
 *
 * is.dreg  = destination register, where calculation will be stored
 * is.sval  = will be added to (is.sval2 * is.sval3) and placed in dest reg
 * is.sval2 = will be multiplied by is.sval3, and added to is.sval
 * is.sval3 = will be multiplied by is.sval2, and added to is.sval
 * is.dmod  = value by which to modulus destination register
 * is.dran  = range within which to contain destination register
 *
 */
void offset()
{   I32s dreg;
    Pcells cp;

    *(is.dreg) = is.sval + (is.sval2 * is.sval3) + flaw();
    dreg=*(is.dreg);
    is.odstoffsettype=(I8u)WhoIs(&cp, dreg, &(is.odstoffset));
    DoMods();
    DoFlags();
}

/* push - push a value onto the stack
 * is.sval = value to be pushed onto the stack
 */
void push()
{   ce->c.c->sp = ++ce->c.c->sp % STACK_SIZE;
    ce->c.c->st[ce->c.c->sp] = is.sval + flaw();
    ce->c.c->fl.E = ce->c.c->fl.S = ce->c.c->fl.Z = 0;
}

/* pop - pop a value from the stack into a register
 * is.dreg = destination register, where value popped off stack will go
 * is.dmod = value by which to modulus destination register
 * is.dran = range within which to contain destination register
 */
void pop()
{   I32s adr1;

    DoRPNu();
    adr1=ce->c.c->st[ce->c.c->sp]+flaw();

    if((is.dreg)==(&(ce->c.c->ip)))
    {   *(is.dreg)=ad(adr1);
        ce->c.c->retins=1;
    }
    else
        *(is.dreg)=adr1;
    if (!ce->c.c->sp)
        ce->c.c->sp = STACK_SIZE - 1;  /* decr stack pointer */
    else
        --ce->c.c->sp;
    DoMods();
    DoFlags();
#if PLOIDY > 1
    JumpTrack();
#endif /* PLOIDY > 1 */
}

/*
 * stup()
 *
 * moves stack pointer up the stack, one place
 *
 * Returns: void
 */
void stup()
{   ce->c.c->sp = ++ce->c.c->sp % STACK_SIZE;
    ce->c.c->fl.E = ce->c.c->fl.S = ce->c.c->fl.Z = 0;
}

/*
 * stdn()
 *
 * moves stack pointer down the stack, one place
 *
 * Returns: void
 */
void stdn()
{   if (!ce->c.c->sp)
        ce->c.c->sp = STACK_SIZE - 1;  /* decr stack pointer */
    else
        --ce->c.c->sp;
    ce->c.c->fl.E = ce->c.c->fl.S = ce->c.c->fl.Z = 0;
}

void tcall()                    /* call template */
{   adrfindtmp(1);
    if(!(ce->c.c->fl.E))
    {   if(is.sval2)
        {   (ce->c.c->callins)=1;
            (ce->c.c->calltcmpstr)=(*(is.dreg))-(is.sval2);
            (ce->c.c->calltcmplen)=is.sval2;
        }
        push();
    }
}

void call()                     /* call address */
{   movdd();
    if(!(ce->c.c->fl.E))
    {   (ce->c.c->callins)=1;
        push();
    }
}

/* void mov() move some data
 * is.mode = form of mov to use
 * see specific movs below for other passed values
 */
void mov()                      /* move some data */
{   switch (is.mode)
    {   case 0:
            movdd();
            break;  /*   direct destination,   direct source */
        case 1:
            movdi();
            break;  /*   direct destination, indirect source */
        case 2:
            movid();
            break;  /* indirect destination,   direct source */
        case 3:
            movii();
            break;  /* indirect destination, indirect source */
    }
}

/* movdd() - inter-register move
 *           *(is.dreg) = is.sval + flaw();
 * is.dreg = destination register, where moved value will be placed
 * is.sval = value to be placed in the dest reg
 * is.dmod = value by which to modulus destination register
 * is.dran = range within which to contain destination register
 */
void movdd()
{   DoRPNu();
    *(is.dreg) = is.sval + flaw();
    DoMods();
    DoFlags();
}

/* movid() - move from register to soup
 *           *(is.dins) = is.sval + flaw();
 *
 * is.dval  = address of destination instruction
 * is.dins  = pointer to destination instruction
 * is.sval  = value to be moved to destination instruction
 * is.sval2 = original value of destination instruction
 * is.dtra  = track of destination instruction
 */
void movid()
{   I8s  i8,  *pi8s=0, *pi8d;
    I16s i16;
    I32s NumBytes=0, i, coffset;
    Ints i32;
    MemThrdAnaDat *memthrdanadat=&(ce->cell_thrdanadat);
    Pcells *clist;
    I32s nclist;

    ce->c.c->fl.E = ce->c.c->fl.S = ce->c.c->fl.Z = 0;
    switch (ce->c.d->mn[5])
    {   case 0:    /* movid() */
        {   if (ce->c.d->idf.B)
            {   switch (ce->c.c->fl.B)
                {   case 0:
                        goto byte4;
                    case 1:
                        goto byte2;
                    /* case 2: default to byte1; */
                }
            }
            NumBytes = 1;
            i8 = (I8s)(is.sval + flaw());
            pi8s = &i8;
            break;
        }
        case '2':  /* movid2() */
        {   byte2:
            NumBytes = 2;
            i16 = (I16s)(is.sval + flaw());
            pi8s = (I8s *) &i16;
            break;
        }
        case '4':  /* movid4() */
        {   byte4:
            NumBytes = 4;
            i32 = (Ints)(is.sval + flaw());
            pi8s = (I8s *) &i32;
            break;
        }
    }
    nclist=0;
    clist=NULL;
    for (i = 0; i < NumBytes; i++)
    {   if ((0 <= is.dval && is.dval < SoupSize)
#ifdef WRITEPROT
            && PrivWrite(ce, is.dval)
#endif /* WRITEPROT */
            )
        {   pi8d = (I8s *) is.dins;   /* byte-wise movement is necessary */
            *pi8d = *(pi8s + i);      /* to avoid alignment problems */
            if ((ce->md.s)&&(((is.dval)>=(ce->md.p))&&
                ((is.dval)<((ce->md.p)+(ce->md.s)))))
            {   coffset = is.dval - ce->md.p;
                if (!ce->d.mov_daught)
                    ce->d.MovOffMin = ce->d.MovOffMax = coffset;
                if (coffset < ce->d.MovOffMin)
                    ce->d.MovOffMin = coffset;
                if (coffset > ce->d.MovOffMax)
                    ce->d.MovOffMax = coffset;
                ce->d.mov_daught++;
#ifdef ERRORTIE
                if (ce->d.MovOffMin < 0)
                {   tsprintf((char *)(&(Fbuf[0])),
                        "Tierra movid() error: ce->d.MovOffMin = %d",
                        ce->d.MovOffMin);
                    FEError(-722, EXIT, WRITE, (char *)(&(Fbuf[0])));
                }
                if (ce->d.MovOffMin >= ce->md.s)
                {   tsprintf((char *)(&(Fbuf[0])),
                "Tierra movid() error: ce->d.MovOffMin = %d  ce->md.s = %d",
                    ce->d.MovOffMin, ce->md.s);
                    FEError(-723, EXIT, WRITE, (char *)(&(Fbuf[0])));
                }
                if (ce->d.MovOffMax < 0)
                {   tsprintf((char *)(&(Fbuf[0])),
                "Tierra movid() error: ce->d.MovOffMax = %d",
                    ce->d.MovOffMax);
                    FEError(-724, EXIT, WRITE, (char *)(&(Fbuf[0])));
                }
                if (ce->d.MovOffMax >= ce->md.s)
                {   tsprintf((char *)(&(Fbuf[0])),
                "Tierra movid() error: ce->d.MovOffMax = %d  ce->md.s = %d",
                        ce->d.MovOffMax, ce->md.s);
                    FEError(-725, EXIT, WRITE, (char *)(&(Fbuf[0])));
                }
#endif /* ERRORTIE */
            }
            else if(GeneBnker&&((*(is.dins))!=(is.sval2)))
                MutBookeep(is.dval,1,&clist,&nclist);
            AllocSoupUpdEvent(memthrdanadat, -1, is.dval);
        }
        else
            ce->c.c->fl.E = 1;
        is.dval++;
        is.dval = ad(is.dval);
#if PLOIDY == 1
        is.dins = &soup[is.dval];
#else /* PLOIDY > 1 */
        is.dtra = ce->c.c->de;
        is.dins = &soup[is.dval][is.dtra];
#endif /* PLOIDY > 1 */
        is.sval2 = *(is.dins);
    }
    CellMutBookeep(clist,nclist);
}

/* movdi - move from soup to register
 *         *(is.dreg) = *(is.sins) + flaw();
 *
 * is.sins = pointer to source instruction
 * is.sval = soup address of source instruction
 * is.dreg = destination register, where moved value will be placed
 * is.dmod = value by which to modulus destination register
 * is.dran = range within which to contain destination register
 * is.stra  = track of source instruction
 */
void movdi()
{   I8s  i8, *pi8d;
    I16s i16;
    I32s NumBytes, i, j, src, sizcls;
    Ints i32;
    Pcells cp;

    src=is.sval;
    ce->c.c->fl.E = ce->c.c->fl.S = ce->c.c->fl.Z = 0;
    switch(ce->c.d->mn[5])
    {   case '2':
            sizcls=1;
            break;
        case '4':
            sizcls=0;
            break;
        default:
        {   sizcls=(ce->c.d->idf.B)?(ce->c.c->fl.B):2;
            break;
        }
    }
    switch(sizcls)
    {   case 0:
        {   NumBytes=sizeof(i32);
            pi8d=(I8s *)&i32;
            break;
        }
        case 1:
        {   NumBytes = sizeof(i16);
            pi8d=(I8s *)&i16;
            break;
        }
        default:
        {   NumBytes=sizeof(i8);
            pi8d=(I8s *)&i8;
            break;
        }
    }
    for(i=0,j=0; i<NumBytes; i++,j++)
    {
#ifdef READPROT
        if (PrivRead(ce, is.sval))
#endif /* READPROT */
            pi8d[j]=(I8s)((*((I8s *)is.sins))+flaw());
#ifdef READPROT
        else
        {   ce->c.c->fl.E=1;
            break;
        }
#endif /* READPROT */
        (is.sval)++;
        is.sval=ad(is.sval);
#if PLOIDY == 1
        is.sins=&soup[is.sval];
#else /* PLOIDY > 1 */
        is.stra=ce->c.c->so;
        is.sins=&soup[is.sval][is.stra];
#endif /* PLOIDY > 1 */
    }
    if(!(ce->c.c->fl.E))
    {   if(NumBytes==1)
            *(is.dreg)=i8;
        else if(NumBytes==2)
            *(is.dreg)=i16;
        else if(NumBytes==4)
        {   *(is.dreg)=(I32s)i32;
            is.oi32a=(N32u)i32;
            is.odstoffsettype=(I8u)WhoIs(&cp, src, &(is.odstoffset));
        }
        DoFlags();
        DoMods();
    }
}

/* movii - move data from soup to soup
 *         *(is.dins) = *(is.sins);
 *
 * is.dval  = address of destination instruction
 * is.dins  = pointer to destination instruction
 * is.sval  = address of source instruction
 * is.sins  = pointer to source instruction
 * is.dtra  = track of destination instruction
 * is.stra  = track of source instruction
 * is.sval2 = original value of destination instruction
 */
void movii()
{   I32s i, NumBytes=0, coffset;
    I32s srccpysegofst, dstcpysegofst;
    MemThrdAnaDat *memthrdanadat=&(ce->cell_thrdanadat);
    Pcells *clist;
    I32s nclist;

    ce->c.c->fl.E = ce->c.c->fl.S = ce->c.c->fl.Z = 0;
    switch (ce->c.d->mn[5])
    {   case 0:    /* movii() */
        {   NumBytes = 1;
            break;
        }
        case '2':  /* movii2() */
        {   NumBytes = 2;
            break;
        }
        case '4':  /* movii4() */
        {   NumBytes = 4;
            break;
        }
    }
    nclist=0;
    clist=NULL;
    for (i = 0; i < NumBytes; i++)
    {   if ((is.dval != is.sval)
#ifdef WRITEPROT
            && PrivWrite(ce, is.dval)
#endif /* WRITEPROT */
#ifdef READPROT
            && PrivRead(ce, is.sval)
#endif /* READPROT */
            && (0 <= is.dval && is.dval < SoupSize)
            && (0 <= is.sval && is.sval < SoupSize))
        {   *(is.dins) = *(is.sins);
            if (RateMovMut && ++CountMovMut >= RateMovMut)
            {   mut_site(soup + ad(is.dval), is.dtra);
                CountMovMut = tlrand() % RateMovMut;
                TotMovMut++;
                ce->d.nonslfmut=1;
            }
#if PLOIDY > 1
            if (RateJmpSou && ++CountJmpSou >= RateJmpSou)
            {   ce->d.srctrksw=1;
                ChangeTrack(&(ce->c.c->so));
                CountJmpSou = tlrand() % RateJmpSou;
            }
#endif /* PLOIDY > 1 */

            srccpysegofst=(is.sval)-(ce->mm.p);
            dstcpysegofst=(is.dval)-(ce->mm.p);
            ThrdAnaDatCollMov(srccpysegofst, is.sval, is.dval);
            if (GeneBnker && WatchMov)
                GenExMov(is.dval, is.sval,
                    srccpysegofst, dstcpysegofst);
            AllocSoupUpdEvent(memthrdanadat, is.sval, is.dval);

            if((ce->md.s)&&((is.dval)>=(ce->md.p))&&
                ((is.dval)<((ce->md.p)+(ce->md.s))))
            {   coffset = is.dval - ce->md.p;
                if (!ce->d.mov_daught)
                    ce->d.MovOffMin = ce->d.MovOffMax = coffset;
                if (coffset < ce->d.MovOffMin)
                    ce->d.MovOffMin = coffset;
                if (coffset > ce->d.MovOffMax)
                    ce->d.MovOffMax = coffset;
                ce->d.mov_daught++;
#ifdef ERRORTIE
                if (ce->d.MovOffMin < 0)
                {   tsprintf((char *)(&(Fbuf[0])),
                        "Tierra movid() error: ce->d.MovOffMin = %d",
                        ce->d.MovOffMin);
                    FEError(-726, EXIT, WRITE, (char *)(&(Fbuf[0])));
                }
                if (ce->d.MovOffMin >= ce->md.s)
                {   tsprintf((char *)(&(Fbuf[0])),
                "Tierra movid() error: ce->d.MovOffMin = %d  ce->md.s = %d",
                        ce->d.MovOffMin, ce->md.s);
                    FEError(-727, EXIT, WRITE, (char *)(&(Fbuf[0])));
                }
                if (ce->d.MovOffMax < 0)
                {   tsprintf((char *)(&(Fbuf[0])),
                "Tierra movid() error: ce->d.MovOffMax = %d",
                        ce->d.MovOffMax);
                    FEError(-728, EXIT, WRITE, (char *)(&(Fbuf[0])));
                }
                if (ce->d.MovOffMax >= ce->md.s)
                {   tsprintf((char *)(&(Fbuf[0])),
                "Tierra movid() error: ce->d.MovOffMax = %d  ce->md.s = %d",
                        ce->d.MovOffMax, ce->md.s);
                    FEError(-729, EXIT, WRITE, (char *)(&(Fbuf[0])));
                }
#endif /* ERRORTIE */
            }
            else if (GeneBnker&&((*(is.dins))!=(is.sval2)))
                MutBookeep(is.dval,1,&clist,&nclist);
        }
        else
            ce->c.c->fl.E = 1;
        is.dval++;
        is.sval++;
        is.dval = ad(is.dval);
        is.sval = ad(is.sval);
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
    }           /* end of for loop */
    CellMutBookeep(clist,nclist);
}

/* adr - find address of a template
 * is.mode  = search mode: 1 = forward, 2 = backward, 0 = outward
 * is.mode2 =  preference: 1 = forward, 2 = backward, and return for
 *        direction found: 1 = forward, 2 = backward, 3 = both, 0 = none
 * is.dval  = starting address for forward search
 *                and return for finish address
 * is.dval2 = starting address for backward search
 *                and return for finish address
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
void adr() /* find address of a template */
{   adrfindtmp(0);
}

/* adrfindtmp - find address of a template
 *
 * tmpcall - 1 = called for "call" with template
 */
static void adrfindtmp(tmpcall) /* find address of a template */
I32s tmpcall;
{   I32s adrt=0, tval;

    if (!is.sval2)
    {   *(is.dreg) = is.sval;  /* source template missing */
        is.dmod = is.dran = 0;
        is.dmod2 = is.dran2 = 0;
        is.dmod3 = is.dran3 = 0;
        return;
    }
    if (ce->c.d->idf.P)
    {   tval = 3 + flaw();
        ce->c.c->re[mo(tval, NUMREG)] = ce->c.c->re[0] + flaw();
    }
    if (!is.mode)  /* outward search */
        adrt = ctemplate(&is.dval, &is.dval2, &is.sval3,
            &is.mode2, is.sval2, 'o', tmpcall);
    else if (is.mode == 1)  /* forward search */
        adrt = ctemplate(&is.dval, &is.dval2, &is.sval3,
            &is.mode2, is.sval2, 'f', tmpcall);
    else if (is.mode == 2)  /* backward search */
        adrt = ctemplate(&is.dval, &is.dval2, &is.sval3,
            &is.mode2, is.sval2, 'b', tmpcall);
    if (adrt < 0)  /* target template not found */
    {   is.iip = is.sval2 + 1;  /* skip IP over source template */
        ce->c.c->fl.E = 1;
        is.dmod = is.dran = 0;
        is.dmod2 = is.dran2 = 0;
        is.dmod3 = is.dran3 = 0;
        return;
    }
    *(is.dreg3) = is.sval3;
    DoMods3();
    *(is.dreg2) = is.sval2;
    DoMods2();
    *(is.dreg) = adrt;
    DoMods();
#if PLOIDY > 1
    JumpTrack();
#endif /* PLOIDY > 1 */

    ce->c.c->fl.E = ce->c.c->fl.S = ce->c.c->fl.Z = 0;
    return;
}

/* malchm - allocate space and protect it
 * is.dreg  = destination register where allocated address is stored
 * is.sval  = requested size of block for mal()
 * is.sval2 = flawed size of block
 * is.sval3 = suggested address, and allocated address
 * is.mode  = memory protection mode (rwx), probably MemModeProt
 * is.mode2 = memory allocation mode for mal()
 */
void malchm()                   /* allocate space and protect it */
{   /* is.sval = requested size of block, is.sval2 = flawed size of block */
    /* is.sval3 = suggested address, & returned address */
    /* is.dreg = location of block, is.mode2 = allocation style */

    DoRPNu();
    if (is.sval < MinCellSize || is.sval >= SoupSize)
    {   ce->c.c->fl.E = 1;
        return;
    }
    if (!(is.sval2 = mal(&(is.sval3), is.sval, is.mode2)))
    {   ce->c.c->fl.E = 1;
        return;
    }
    *(is.dreg) = is.sval3;

    /* is.sval3 = location of chmoded block */
    /* is.sval2 = size of chmoded block */
    /* is.mode  = chmod mode, unix notation, e.g. 7 = full protection */

    ce->c.c->fl.E = ce->c.c->fl.S = ce->c.c->fl.Z = 0;
}

/* divide - cell division
 * is.sval  = eject genome from soup = !0, 0 = leave in soup, in network
 *            version, daughter goes to IP address specified by is.sval
 * is.sval2 = offset of IP into daughter's genome NOTE currently disabled
 * is.sval3 = network version: destination port
 * is.mode  = divide mode (3 steps)
 */
void divide()                   /* cell division */
{   Pcells nc;                  /* pointer to the new cell */
    I32s i, DGenMemSiz, Thresh, index = -1, divclnup=0, cpu;

    DGenMemSiz = ce->d.MovOffMax - ce->d.MovOffMin + 1;
    Thresh = (I32s) (ce->md.s * MovPropThrDiv * PLOIDY);
    if (ce->md.s < MinCellSize || DGenMemSiz < MinGenMemSiz ||
        DGenMemSiz < Thresh || ce->d.mov_daught < Thresh)
    {   ce->c.c->fl.E = 1;
        return;
    }

    if (DivSameSiz)
    {   if (ce->mm.s != ce->md.s)
        {   ce->c.c->fl.E = 1;
            return;
        } /* in tsetup.c: if (DivSameGen) DivSameSiz = 1; */
        if ((DivSameGen) &&
            ((ce->d.mg.s != DGenMemSiz) ||
            (ce->d.mg.p != ce->d.MovOffMin) ||
            (!IsSameGen(ce->d.mg.s, soup + ce->md.p + ce->d.MovOffMin,
            soup + ce->mm.p + ce->d.mg.p))))
        {   ce->c.c->fl.E = 1;
            return;
        }
    }
#ifdef NET
    if (is.mode == 2 && (is.sval || (EjectRate && !(tlrand() % EjectRate))))
    {   index = ChooseIP(is.sval, is.sval3, 0, StrictIP);
        if (index == -1)
        {   ce->c.c->fl.E = 1;
            return;
        }
    }
#endif /* NET */

    switch (is.mode)
    {   case 0:  /* create cpu */
        {   if ((ce->d.ne.a == ce->q.htis.a) &&
                (ce->d.ne.i == ce->q.htis.i))
                /* if there is no cpu (first call to div 0) */
            {   nc = GetFreeCell();
                nc->ld = 1;
                nc->mm = ce->md;
                nc->d.dm = 1;
                nc->d.genome = soup + nc->mm.p;
                ce->d.ne = nc->q.htis;
                for (cpu = 0; cpu < nc->c.n; cpu++)
                {   nc->c.c = &(nc->c.ar[cpu]);
                    nc->c.c->ip = nc->mm.p;
                    if (!(ce->c.d->idf.C))
                    {   /* nc->c.c->ip += mo(is.sval2, nc->mm.s);
                           nc->c.c->ip = ad(nc->c.c->ip); */
                        for (i = 0; i < NUMREG; i++)
                            nc->c.c->re[i] = ce->c.c->re[i];
                    }
                }
            }
            else
                /* if there is a cpu (second call to div 0) */
            {   nc = &cells[ce->d.ne.a][ce->d.ne.i];
                if (nc->d.is)  /* call to div 0 after call to div 1 */
                {   RmvFrmSlicer(nc);
                    nc->d.is = 0;
                }
                else /* two sequential calls to div 0, error */
                {   ce->c.c->fl.E = 1;
                    return;
                }
            }
            break;
        }
        case 1:  /* start cpu */
        {   if ((ce->d.ne.a == ce->q.htis.a) &&
                (ce->d.ne.i == ce->q.htis.i))
                /* if there is no cpu, div 1 before div 0 */
            {   nc = GetFreeCell();
                nc->ld = 1;
                nc->mm = ce->md;
                nc->d.dm = 1;
                nc->d.genome = soup + nc->mm.p;
                ce->d.ne = nc->q.htis;
                for (cpu = 0; cpu < nc->c.n; cpu++)
                {   nc->c.c = &(nc->c.ar[cpu]);
                    nc->c.c->ip = nc->mm.p;
                    if (!(ce->c.d->idf.C))
                    {/* nc->c.c->ip += mo(is.sval2, nc->mm.s);
                        nc->c.c->ip = ad(nc->c.c->ip); */
                        for (i = 0; i < NUMREG; i++)
                            nc->c.c->re[i] = ce->c.c->re[i];
                    }
                }
            }
            else  /* if there is already a cpu, make pointers to it */
                nc = &cells[ce->d.ne.a][ce->d.ne.i];
            if (nc->d.is)  /* 2nd call to div 1, cpu is already started */
            {   RmvFrmSlicer(nc);
                nc->d.is = 0;
            }
            else
                /* not 2nd call to div 1, cpu is not already started */
            {   EntBotSlicer(nc);
                nc->d.is = 1;
            }
            break;
        }
        case 2:  /* split cells */
        {   GeneticOps();
            DGenMemSiz = ce->d.MovOffMax - ce->d.MovOffMin + 1;
            if (ce->md.s < MinCellSize || DGenMemSiz < MinGenMemSiz)
            {   ce->c.c->fl.E = 1;
                return;
            }

#ifdef NET
            if((GeneBnker)&&(is.sval))
                if(index>=0)
                {   (ce->d.sensattempt)++;
                    if ((IPMapC[index].address.node != LocalNetAddr.node)||
                        (IPMapC[index].address.portnb != LocalNetAddr.portnb))
                    {   (sl[ce->d.gen.size]->g[ce->d.origgi]->gennsl)++;
                        (sl[ce->d.gen.size]->siznsl)++;
                    }
                }
#endif /* NET */

            if ((ce->d.ne.a == ce->q.htis.a) &&
                (ce->d.ne.i == ce->q.htis.i))
                /* if there is no cpu, div 2 before div 0 */
            {   nc = GetFreeCell();
                divclnup=1;
                nc->ld = 1;
                nc->mm = ce->md;
                nc->d.mg.p = ce->d.MovOffMin;
                nc->d.mg.s = DGenMemSiz;
                nc->d.genome = soup + nc->mm.p;
                for (cpu = 0; cpu < nc->c.n; cpu++)
                {   nc->c.c = &(nc->c.ar[cpu]);
                    nc->c.c->ip = nc->mm.p;
                    if (!(ce->c.d->idf.C))
                    {/* nc->c.c->ip += mo(is.sval2, nc->mm.s);
                        nc->c.c->ip = ad(nc->c.c->ip); */
                        for (i = 0; i < NUMREG; i++)
                            nc->c.c->re[i] = ce->c.c->re[i];
                    }
                }
            }
            else
                nc = &cells[ce->d.ne.a][ce->d.ne.i];
#ifdef NET
            if(index>=0)
            {   if(EmigSubNetChk(TieSubNet,
                    IPMapC[index].PrivPingData.subnet, 0))
                {   if(((IPMapC[index].address.node)==
                        (LocalNetAddr.node))&&
                        ((IPMapC[index].address.portnb)==
                        (LocalNetAddr.portnb)))
                    {   index = -1;
                        EjectToSelf++;
                    }
                    else
                        nc->ld=2;
                }
                else
                {   /* clean up daughter memory */
                    MemDealloc(ce->md.p, ce->md.s);
                    InitDeadMem(ce->md.p, ce->md.s, DeadMemInit);
                    ce->md.p=0;
                    ce->md.s=0;
                    ce->d.repinst=0;
                    ce->d.mov_daught=0;
                    ce->d.mut=0;
                    ce->d.MovOffMin=0;
                    ce->d.MovOffMax=0;
                    if(divclnup)
                    {   NumCells--;
                        nc->ld=0;
                    }
                    ce->c.c->fl.E=0;
                    return;
                }
            }
#endif /* NET */
            if (!nc->d.is)  /* no slicer, div 2 before div 1 */
            {   EntBotSlicer(nc);
                EntBotReaper(nc);
                nc->d.is = 1;
            }
            ce->md.s = ce->md.p = 0;
            ce->d.ne = ce->q.htis;  /* clean up if div 0 or 1 before 2 */
            nc->d.dm = 0;

            DownReperIf(ce);
            DivideBookeep(ce, nc, index);

#ifdef EXEINSTSIZTIMSLC
            nc->d.PExeSegSiz=ce->d.ExeSegSiz;
#endif /* EXEINSTSIZTIMSLC */

#ifdef NET
            is.onodetype=0;
            if (index != -1)
            {   is.onodetype=1;
                is.oi32a=IPMapC[index].address.node;
                is.oport=IPMapC[index].address.portnb;
                NEject(nc, index, 0);
             /* NEject(nc, RandIP(), 0); random ver */
                ReapCell(nc, REAP_DIVIDE);
            }
#ifdef EXEINSTSIZTIMSLC
            else
                AllocCellGenBits(nc);
#endif /* EXEINSTSIZTIMSLC */

#else /* NET */
            if((is.mode==2)&&(EjectRate)&&(!(tlrand()%EjectRate)))
                ReapCell(nc, REAP_NON_NET_EJECT);
#ifdef EXEINSTSIZTIMSLC
            else
                AllocCellGenBits(nc);
#endif /* EXEINSTSIZTIMSLC */
#endif /* NET */
            is.divide=1;
        }
    }       /* switch */

#ifdef NET
    if((GeneBnker)&&(is.sval))
        if(index>=0)
            if ((IPMapC[index].address.node != LocalNetAddr.node)||
                (IPMapC[index].address.portnb != LocalNetAddr.portnb))
            {   (sl[ce->d.gen.size]->g[ce->d.origgi]->sucgennsl)++;
                (sl[ce->d.gen.size]->sucsiznsl)++;
                if(((ce->c.io.ipic1)>=0)&&((ce->c.io.ipic1)<(I32s)NumNodesC))
                    if((SensMapAtt<0)||((ce->d.sensattempt)==SensMapAtt))
                    {   (IPMapC[ce->c.io.ipic1].ipi1)++;
                        is.oipic1=ce->c.io.ipic1;
                        (ce->c.io.ipic1)=(-1);
                    }
            }
#endif /* NET */

    ce->c.c->fl.E = ce->c.c->fl.S = ce->c.c->fl.Z = 0;
}

#ifdef NET
/* migrate - send genome to remote node
 *
 * is.sval = IP address of the remote node to which creature is to be sent
 * is.sval2 = port of the remote node to which creature is to be sent
 * is.sval3 : 0 = local cluster migration, 1 = remote cluster migration
 */
void migrate()
{   I32s  index, NumNodes, *ipi1, *oipi1;
    MapNode  *IPMap;

    if (NumCells <= NumCellsMin || !ce->d.fecundity)
    {   ce->c.c->fl.E = 1;
        return;
    }

    if (is.sval3)
    {   IPMap=IPMapS;
        NumNodes=NumNodesS;
        ipi1=&(ce->c.io.ipis1);
        oipi1=&(is.oipis1);
    }
    else
    {   IPMap=IPMapC;
        NumNodes=NumNodesC;
        ipi1=&(ce->c.io.ipic1);
        oipi1=&(is.oipic1);
    }
    if(OnLineStat)
    {   index = ChooseIP(is.sval, is.sval2, is.sval3, StrictIP);
        if (index == -1)
        {   ce->c.c->fl.E = 1;
            return;
        }

        if(is.sval3 &&
            (IPMap[index].address.node == LocalClusterAddr.node)&&
            (IPMap[index].address.portnb == LocalClusterAddr.portnb))
            return; /* no cluster migration to the local cluster */

        if(GeneBnker)
        {   (ce->d.sensattempt)++;
            if ((IPMap[index].address.node != LocalNetAddr.node)||
                (IPMap[index].address.portnb != LocalNetAddr.portnb))
            {   (sl[ce->d.gen.size]->g[ce->d.origgi]->gennsl)++;
                (sl[ce->d.gen.size]->siznsl)++;
            }
        }

        if(EmigSubNetChk(TieSubNet, IPMap[index].
            PrivPingData.subnet, is.sval3))
        {   if((IPMap[index].address.node!=LocalNetAddr.node)||
               (IPMap[index].address.portnb!=LocalNetAddr.portnb))
            {   if(GeneBnker)
                {   (sl[ce->d.gen.size]->g[ce->d.origgi]->sucgennsl)++;
                    (sl[ce->d.gen.size]->sucsiznsl)++;
                    if(((*ipi1)>=0)&&((*ipi1)<NumNodes))
                        if((SensMapAtt<0)||((
                            ce->d.sensattempt)==SensMapAtt))
                        {   (IPMap[*ipi1].ipi1)++;
                            (*oipi1)=(*ipi1);
                            (*ipi1)=(-1);
                        }
                }

                /* if sufficient remaining bandwidth capacity */
                ce->c.c->fl.E=0;
                if(NEject(ce, index, is.sval3))
                {
#ifdef BGL
                    /* this cell currently being tracked */
                    if (ce->d.BglTrack.node)
                        MigrMigrateMesg(ce, index);
#endif /* BGL */
                    is.oi32a=IPMap[index].address.node;
#ifdef USE_PORT
                    is.oport=IPMap[index].address.portnb;
#endif /* USE_PORT */
                    GenExExe(is.oip);
                    AllocThrdAnaEvent(&(ce->cell_thrdanadat));
                    CellHalt = 1;
                    ThrdAnaDatColl(1,1, is.oip);
                    ReapCell(ce, REAP_SURF);
                }
            }
        }
        else
        {
            ce->c.c->fl.E=0;
#ifdef BGL
            /* this cell currently being tracked */
            if(ce->d.BglTrack.node)
                MigrDeathMesg(ce);
#endif /* BGL */
            is.oi32a=IPMap[index].address.node;
#ifdef USE_PORT
            is.oport=IPMap[index].address.portnb;
#endif /* USE_PORT */
            GenExExe(is.oip);
            AllocThrdAnaEvent(&(ce->cell_thrdanadat));
            CellHalt = 1;
            ThrdAnaDatColl(1,1, is.oip);
            ReapCell(ce, REAP_SUBNET);
            return;
        }
    }
    return;
}

/*
 * void tpingsnd()
 *
 * send a tping
 * is.sval = IP address of the remote node to which the ping is to be sent
 * is.sval2 = port of the remote node to which the ping is to be sent
 *
 * Returns:
 */
void tpingsnd()
{   ComAddr cell_addr;
    I32s  index;

    index = ChooseIP(is.sval, is.sval2, 0, StrictIP);
    if (index == -1)
    {   ce->c.c->fl.E = 1;
        return;
    }
    if ((IPMapC[index].address.node != LocalNetAddr.node)||
       (IPMapC[index].address.portnb != LocalNetAddr.portnb))
    {   SelfID(ce, &cell_addr);  /* fill in the source cell address */
        if(OnLineStat)
            TPing_send(index, &cell_addr, 0);  /* send the tping */
    }
    else
    {   IPMapC[index].data.TransitTime = 0;
        GetTPingData(&(IPMapC[index].data),
            &(IPMapC[index].PrivPingData), -1);
        IPMapC[index].data.TransitTime = 0;
    }
    ce->c.c->fl.E = ce->c.c->fl.S = ce->c.c->fl.Z = 0;
    if (issignal(1))
        ce->c.c->fl.E = 1;
}

/* tpingrec - receive a tping response
 * is.dreg = points to location in soup where data will be written
 */
void tpingrec()
{   I32s  size;
    I32s dest = *(is.dreg);

    if (ReadIOS(ce, TPINGREPTAG, dest, &size))
    {   remsignal(TPINGREPTAG, ce);
        ce->c.c->fl.E = 0;
    }
    else
        ce->c.c->fl.E = 1;
    ce->c.c->fl.S = ce->c.c->fl.Z = 0;
}

/*
 * n a m e
 *
 * get an TPing data from the map file, put in soup 
 * The first call will return the TPing data for the local machine
 * or cluster.  After the first call the pointer into the list is
 * randomized
 *
 * is.dreg = the register containing address in soup where data will go
 * is.sval : 0 = local cluster data, 1 = remote cluster server data
 *
 * Returns: void
 */
void getipp()
{   IOS  ios;
    I32s size;
    I32s dest = *(is.dreg);
    I32s index, NumNodes;
    I8s  ok;
    MapNode *IPMap, clrmapnode, *mapnodeptr;
    Pcells cp;

    if (is.sval)
    {   NumNodes=NumNodesS;
        IPMap=IPMapS;
        if (ce->c.io.ipis == -1)
        {   if(GetIPPStrtIdxS==(-2))
                index=(NumNodes)?(tlrand()%NumNodes):0;
            else
                if(GetIPPStrtIdxS==(-1))
                    index=ChooseIP(LocalClusterAddr.node,
                        LocalClusterAddr.portnb, is.sval, 1);
                else
                    index=GetIPPStrtIdxS;
            index=(index<0)?0:((index>NumNodes)?NumNodes:index);
            (ce->c.io.ipis1)=index;
            (ce->c.io.ipis)=index+1;
        }
        else
        {   index=ce->c.io.ipis;
            (ce->c.io.ipis)++;
        }
        if (ce->c.io.ipis >= NumNodes)
            ce->c.io.ipis=0;
    }
    else
    {   NumNodes=NumNodesC;
        IPMap=IPMapC;
        if (ce->c.io.ipic == -1)
        {   if(GetIPPStrtIdxC==(-2))
                index=(NumNodes)?(tlrand()%NumNodes):0;
            else
                if(GetIPPStrtIdxC==(-1))
                    index=ChooseIP(LocalNetAddr.node,
                        LocalNetAddr.portnb, is.sval, 1);
                else
                    index=GetIPPStrtIdxC;
            index=(index<0)?0:((index>NumNodes)?NumNodes:index);
            (ce->c.io.ipic1)=index;
            (ce->c.io.ipic)=index+1;
        }
        else
        {   index=ce->c.io.ipic;
            (ce->c.io.ipic)++;
        }
        if (ce->c.io.ipic >= NumNodes)
            ce->c.io.ipic=0;
    }
#ifdef ERRORTIE
    if (index == -1)
        FEError(-714, EXIT, WRITE, "Local Node or Server not in MapFile");
#endif /* ERRORTIE */

    ce->c.c->fl.E = ce->c.c->fl.S = ce->c.c->fl.Z = 0;
    memset((char *) &(ios), 0, sizeof(IOS));
    ios.t = TPINGREPTAG;
    ios.s.address = IPMap[index].address;
    ios.d.len = sizeof(TPingData);
    ios.d.d = (I8s *) tcalloc(ios.d.len, sizeof(I8s), 702);

    memset(&clrmapnode, 0, sizeof(MapNode));

    clrmapnode.address=IPMap[index].address;
    clrmapnode.PrivPingData.subnet=
        IPMap[index].PrivPingData.subnet;

    mapnodeptr=(!EmigSubNetChk(TieSubNet,
        IPMap[index].PrivPingData.subnet, is.sval))?
        (&clrmapnode):(&(IPMap[index]));
    memcpy(ios.d.d, &(mapnodeptr->data), ios.d.len);

    if (!(ok=WriteIOSToSoup(ce, dest, &ios, &size)))
        ce->c.c->fl.E = 1;
    if(ok)
    {   is.oi32a=mapnodeptr->address.node;
#ifdef USE_PORT
        is.oport=mapnodeptr->address.portnb;
#endif /* USE_PORT */
        is.odstoffsettype=(I8u)WhoIs(&cp, dest, &(is.odstoffset));
    }
    tfree(ios.d.d, (ios.d.len) * sizeof(I8s), 702);
}

#ifdef COMMENT

This is a description of the data written to the soup by getipp():

    struct NetAddr   /* internet address */
    {   I32u node;   /* internet node address */
        I16u portnb; /* socket port number, identifies process within a node */
    };

    struct ComAddr /* structure for communication addresses */
    {   struct NetAddr address; /* internet address */
        I32s ranID;    /* random ID # of cell */
    /*  I32s soup; */  /* soup within a process, as on parallel machines */
    };

    typedef struct   /* structure for communications data */
    {   I32s len;    /* length of message */
        I8s *d;      /* array for data communications */
    } ComData;

    typedef struct  /* structure for IO for communication */
    {   I32s t;     /* tag for type of message 0 reserved for free struct */
        ComAddr s;  /* source address */
        ComAddr e;  /* destination address */
        ComData d;  /* data */
    } IOS;

    struct Event  /* structure for time measured in executed instructions */
    {   I32s m;   /* count of millions of instructions */
        I32s i;   /* count of instructions */
    };

struct TPingData {
    I32s FecundityAvg; /* average fecundity at death, over last million */
    I32s Speed; /* average instructions/second during last million */
    I32s NumCells;
    I32s AgeAvg; /* average inst age at death, over last million */
    I32s SoupSize;
    I32u TransitTime; /* in milliseconds */
    I32u Fresh; /* local clock time at last refresh of this data struct */
    I32u Time; /* clock time at node */
    I32s InstExec;
    I32s InstExecConnect;
    I32s OS; /* operating system tag */
};

    /* create char buffer, copy tag, source address and data to buffer */
    /* tag = 4, source address = 16, data = len bytes */
buf = (I8u *) tcalloc(20 + ios->d.len, sizeof(I8u), 701);
*((I32s *) (buf + 0)) = ios->t;
*((I32u *) (buf + 4)) = ios->s.address.node;
*((I32u *) (buf + 8)) = (I32u) ios->s.address.portnb;
*((I32s *) (buf + 12)) = ios->s.ranID;
*((I32s *) (buf + 16)) = ios->s.len;

In the case of getipp(), the ios->d.d is the TPingData data structure

000000  0 -  3 4 I32s t
000100  4 -  7 4 I32u address.node
001000  8 - 11 4 I32u address.portnb
001100 12 - 15 4 I32s ranID
010000 16 - 19 4 I32s len;
memcpy(buf + 20, ios->d.d, ios->d.len);
010100 20 - 23 4 I32s FecundityAvg; /* avg fecundity at death, last million */
011000 24 - 27 4 I32s Speed; /* average inst/second during last million */
011100 28 - 31 4 I32s NumCells;
100000 32 - 35 4 I32s AgeAvg; /* average inst age at death, last million */
100100 36 - 39 4 I32s SoupSize;
101000 40 - 43 4 I32u TransitTime; /* in milliseconds */
101100 44 - 47 4 I32u Fresh; /* local clock time at last refresh this data */
110000 48 - 51 4 I32u Time; /* clock time at node */
110100 52 - 55 4 I32s InstExec;
111000 56 - 59 4 I32s InstExecConnect;
111100 60 - 63 4 I32s OS; /* operating system tag */

#endif /* COMMENT */

/* getip - get an IP address from the map file
 * is.dreg = the register where the IP address will be placed
 */
void getip()
{   *(is.dreg) = ReadIPMap(ce);
    ce->c.c->fl.E = ce->c.c->fl.S = ce->c.c->fl.Z = 0;
}

/* ReadIPMap - read node address from IP Map
 * cp - cell to supply next index in IPMap
 */
I32u ReadIPMap(cp)
    Pcells cp;
{   I32s  index;

    if (cp->c.io.ipic == -1)
    {   if(GetIPPStrtIdxC==(-2))
            index=(NumNodesC)?(tlrand()%NumNodesC):0;
        else
            if(GetIPPStrtIdxC==(-1))
                index=ChooseIP(LocalNetAddr.node,
                    LocalNetAddr.portnb, is.sval, 1);
            else
                index=GetIPPStrtIdxC;
        index=(index<0)?0:((index>((I32s)NumNodesC))?NumNodesC:index);
        (cp->c.io.ipic1)=index;
        (cp->c.io.ipic)=index+1;
    }
    else
    {   index=cp->c.io.ipic;
        (cp->c.io.ipic)++;
    }
    if (cp->c.io.ipic >= (I32s)NumNodesC)
        cp->c.io.ipic = 0;
    return (I32u) (IPMapC[index].address.node);
}

/*
 * NEject - Eject cell from soup
 *          for network model, send cell to remote node
 *          for non-network model, kill cell
 *
 * cr - pointer to cell structure of migrating cell
 * index - index of target node into the IPMap array
 * whichmap - 0 - local cluster map (IPMapC) 
 *            1 - cluster server map (IPMapS) 
 *
 * Returns:
 */
I32s NEject(cr, index, whichmap)
Pcells  cr;
I32s  index;
I32s  whichmap;
{
#if PLOIDY != 1
    I32u j;
#endif /* PLOIDY != 1 */
    I32s gen = cr->mm.p;      /* offset into soup of genome */
    I32s siz = cr->mm.s;      /* size of cell */
    FpInst p = soup + gen;
    TMessage mesg;
    struct TieCommID dstcommid;
    MapNode  *IPMap;
    I32s sufrembndcap=1,addrmod;

#ifdef ERRORTIE
    if (!siz)
        FEError(-716, EXIT, WRITE, "Tierra NEject() error: size = 0");
    if (gen < 0 || gen + siz > SoupSize)
    {   tsprintf((char *)(&(Fbuf[0])),
            "Tierra NEject() error: genome gen: %d siz: %d not in soup",
                siz, gen);
        FEError(-717, EXIT, WRITE, (char *)(&(Fbuf[0])));
    }
#endif /* ERRORTIE */

    IPMap=(whichmap)?IPMapS:IPMapC;

    if(OnLineStat)
    {   memset((char *) &mesg, 0, sizeof(TMessage));
        mesg.dstmap=whichmap;
        mesg.srcmap=0;
        mesg.info.tag = EMIGRATETAG;
        mesg.info.Data_u.Creature.dest.node=
            IPMap[index].address.node;
        mesg.info.Data_u.Creature.dest.portnb=
            IPMap[index].address.portnb;
        mesg.info.Data_u.Creature.BglTrack=cr->d.BglTrack;
        mesg.info.Data_u.Creature.ranID=cr->d.RanID;
        mesg.info.Data_u.Creature.genome_siz=siz;

        /* genome len in 32 bit words */
        mesg.info.Data_u.Creature.genome.genome_len=
            InstrXdrWrdSize(mesg.info.Data_u.Creature.genome_siz);

        if((addrmod=(cr->mm.p)%sizeof(*(mesg.info.Data_u.
            Creature.genome.genome_val))))
        {   mesg.info.Data_u.Creature.genome.genome_val=
                (xdrwrd *)thcalloc(mesg.info.Data_u.
                Creature.genome.genome_len, sizeof(xdrwrd), 708);
            memcpy(mesg.info.Data_u.Creature.genome.genome_val,
                p, (mesg.info.Data_u.Creature.genome.
                genome_len)*sizeof(xdrwrd));
        }
        else
            mesg.info.Data_u.Creature.genome.genome_val=(xdrwrd *)p;

        mesg.info.Data_u.Creature.mg = cr->d.mg;

#ifdef EXEINSTSIZTIMSLC
        mesg.info.Data_u.Creature.PExeSegSiz=cr->d.PExeSegSiz;
#endif /* EXEINSTSIZTIMSLC */

        mesg.PingData.TransitTime = 0;
        GetTPingData(&(mesg.PingData), &(mesg.PrivPingData), -1);

        if ((IPMap[index].address.node)
            && (IPMap[index].address.portnb))
        {   dstcommid.tiec_node=IPMap[index].address.node;
            dstcommid.tiec_port=IPMap[index].address.portnb;
            if(!TieCommSend(&globtcibuf, &mesg,
                dstcommid, &sufrembndcap))
            {   

#ifdef TIERRA
#if FRONTEND == BASIC
                if(!HistoInEffect())
                {
#endif /* FRONTEND == BASIC */
#endif /* TIERRA */
                    if (GeneBnker)
                    {   tsprintf((char *)(&(Fbuf[0])),
                    "%s:Send Genome %.4d%3s to %.2d.%.2d.%.2d.%.2d:%d",
                            LocalHostName, siz, "---",
                            ((I8u *)(&(IPMap[index].address.node)))[0],
                            ((I8u *)(&(IPMap[index].address.node)))[1],
                            ((I8u *)(&(IPMap[index].address.node)))[2],
                            ((I8u *)(&(IPMap[index].address.node)))[3],
                            ntohs(IPMap[index].address.portnb));
                    }
                    else
                    {   tsprintf((char *)(&(Fbuf[0])),
                        "%s:Send Genome %.4d to %.2d.%.2d.%.2d.%.2d:%d\n",
                            LocalHostName, siz,
                            ((I8u *)(&(IPMap[index].address.node)))[0],
                            ((I8u *)(&(IPMap[index].address.node)))[1],
                            ((I8u *)(&(IPMap[index].address.node)))[2],
                            ((I8u *)(&(IPMap[index].address.node)))[3],
                            ntohs(IPMap[index].address.portnb));
                    }
                    FEPrintf(MSG_X, MSG_Y, 0, 0, (char *)(&(Fbuf[0])));
#ifdef BGL
                    TBgl_make_femessages(&(Fbuf[0]),
                        strlen((const char *)(&(Fbuf[0]))));
#endif /* BGL */

#ifdef TIERRA
#if FRONTEND == BASIC
                }
#endif /* FRONTEND == BASIC */
#endif /* TIERRA */
            }
        }
        if(addrmod)
            tfree(mesg.info.Data_u.Creature.
                genome.genome_val, InstrXdrWrdSize(
                (mesg.info.Data_u.Creature.genome.
                genome_len))*sizeof(xdrwrd), 708);
    }

/* the following code erases the genome from the soup */

    /* if no TieCommSend failure due to insufficient remaining */
    /*     bandwidth capacity */
    if(sufrembndcap)
        InitDeadMem(gen, siz, DeadMemInit);
    return sufrembndcap;
}

#endif /* NET */

/*
 * GetFreeCell - gets a free cell structure from the cells array
 *
 * Returns: pointer to the new free cell structure
 */
Pcells GetFreeCell()
{   Pcells fc=0;
    I32s i, j=0, found = 0;

    if (++NumCells > CellsSize - 2)
        CheckCells();
    for (i = 0; i < NumCelAr; i++)  /* find unoccupied cell struct */
    {   for (j = 0; j < CelArSiz; j++)
        {
#ifdef ERRORTIE
            if (i * j >= CellsSize)
                FEError(-718, EXIT, WRITE,
                    "Tierra GetFreeCell() error, exiting");
#endif
            if (!cells[i][j].ld)
            {   found = 1;
                fc = &cells[i][j];
                break;
            }
        }
        if (found)
            break;
    }
    InitCell(i, j, fc);
    return fc;
}

/*
 * CheckCells - check and adjust cell structure memory allocation if necessary
 */
void CheckCells()
{   I32s j;
    CellInd htis, mtis;

#ifdef TIERRA
#if FRONTEND == BASIC
    if(!HistoInEffect())
    {   FEClrmsg(MSG_Y,
            ((OPSYS == BCDOS) || (OPSYS == DJGPPDOS) ||
            ((OPSYS == AMIGADOS) && (FRONTEND == BASIC))
            || (OPSYS == MACTIE) || (OPSYS == WIN32TIE) ?(I8s *)
"                       Press Any Key for Menu ...                           "
            :(I8s *)
"                    Press Interrupt Key for Menu ...                        "
        ));
        FEStats();
    }
#endif /* FRONTEND == BASIC */
#endif /* TIERRA */

#ifdef ERRORTIE
#ifdef TIERRA
#if FRONTEND == BASIC
    if(!HistoInEffect())
    {
#endif /* FRONTEND == BASIC */
#endif /* TIERRA */
        tsprintf((char *)(&(Buff[0])),"%s%d%s%d\n",
            "in_div CheckCells: recalloc, NumCells = ",
            NumCells,"    old CellsSize = ",CellsSize);
        FEPrintf(MSG_X, MSG_Y, 0, 0, (char *)(&(Buff[0])));
#ifdef TIERRA
#if FRONTEND == BASIC
    }
#endif /* FRONTEND == BASIC */
#endif /* TIERRA */
#endif /* ERRORTIE */

    htis.a = ce->q.htis.a;
    htis.i = ce->q.htis.i;
    if(BrkTrapCondBuf.curbrkdat.MicroSlice)
    {   mtis.a=BrkTrapCondBuf.curbrkdat.MicroSlice->q.htis.a;
        mtis.i=BrkTrapCondBuf.curbrkdat.MicroSlice->q.htis.i;
    }

    NumCelAr++;
    cells = (Pcells Fp) trecalloc((void *) cells,
        (I32u) sizeof(Pcells) * (I32u) NumCelAr,
        (I32u) sizeof(Pcells) * (I32u) (NumCelAr - 1), 1900);
    CellsSize = NumCelAr * CelArSiz;
    cells[NumCelAr - 1] = (Pcells) tcalloc(CelArSiz, sizeof(TCell), 1901);

#ifdef ERRORTIE
#ifdef __TURBOC__
#ifdef TIERRA
#if FRONTEND == BASIC
    if(!HistoInEffect())
    {
#endif /* FRONTEND == BASIC */
#endif /* TIERRA */
        tsprintf((char *)(&(Fbuf[0])),
            "%s new CellsSize = %d\n", Buff, CellsSize);
        FEPrintf(MSG_X, MSG_Y + 1, 0, 0, (char *)(&(Buff[0])));
#ifdef TIERRA
#if FRONTEND == BASIC
    }
#endif /* FRONTEND == BASIC */
#endif /* TIERRA */
#ifndef __GUI__
#ifdef TIERRA
#if FRONTEND == BASIC
    if(!HistoInEffect())
    {
#endif /* FRONTEND == BASIC */
#endif /* TIERRA */
        tsprintf((char *)(&(Fbuf[0])),
            "coreleft = %u  divide (cells)\n", coreleft());
        FEPrintf(MSG_X, MSG_Y + 2, 0, 0, (char *)(&(Fbuf[0])));
#ifdef TIERRA
#if FRONTEND == BASIC
    }
#endif /* FRONTEND == BASIC */
#endif /* TIERRA */
#endif /* __GUI__ */
#else /* __TURBOC__ */
#ifdef TIERRA
#if FRONTEND == BASIC
    if(!HistoInEffect())
    {
#endif /* FRONTEND == BASIC */
#endif /* TIERRA */
        tsprintf((char *)(&(Fbuf[0])),
            "%s new CellsSize = %d\n", Buff, CellsSize);
        FEPrintf(MSG_X, MSG_Y + 1, 0, 0, (char *)(&(Fbuf[0])));
#ifdef TIERRA
#if FRONTEND == BASIC
    }
#endif /* FRONTEND == BASIC */
#endif /* TIERRA */
#endif /* __TURBOC__ */
#endif /* ERRORTIE */

    for (j = 0; j < CelArSiz; j++)
        InitCell(NumCelAr - 1, j, &cells[NumCelAr - 1][j]);

    ce = &cells[htis.a][htis.i];
    if(BrkTrapCondBuf.curbrkdat.MicroSlice)
        BrkTrapCondBuf.curbrkdat.MicroSlice=&cells[mtis.a][mtis.i];
#ifdef TIERRA
#if FRONTEND == BASIC
    if(!HistoInEffect())
    {   FEClrmsg(MSG_Y,
            ((OPSYS == BCDOS) || (OPSYS == DJGPPDOS) ||
            ((OPSYS == AMIGADOS) && (FRONTEND == BASIC))
            || (OPSYS == MACTIE) || (OPSYS == WIN32TIE) ?(I8s *)
"                       Press Any Key for Menu ...                           "
            :(I8s *)
"                    Press Interrupt Key for Menu ...                        "
        ));
        FEStats();
    }
#endif /* FRONTEND == BASIC */
#endif /* TIERRA */
}

/*
 * flaw - introduce instruction operation flaw at appropriate intervals
 *        returns  1 if randomly generated integer is even - for increment
 *                -1 if randomly generated integer is odd  - for decrement
 */
I32s flaw()
{   if (RateFlaw && ++CountFlaw >= RateFlaw)
    {   CountFlaw = tlrand() % RateFlaw;
        TotFlaw++;
        ce->d.flaw++;
        ce->d.nonslfmut=1;
        if (tcrand() % 2)
            return 1;
        return -1;
    }
    return 0;
}

/* ctemplate - search in specified direction for nop template return 
 *             address, 
 *
 * NOTE: ce->c.c->ip must point to the instruction being executed
 *
 * f    - starting address for forward search
 * b    - starting address for backward search
 * slim - search limit
 * mode - preference for forward (1) or backward (2) target if both
 *        are found
 * tz   - template size
 * dir  - direction of search, f = forward, b = backward, o = out
 * tmpcall - 1 = called for "call" with template
 *
 * returns: address of instruction following target template
 *          *f    - ending address for forward search
 *          *b    - ending address for backward search
 *          *slim - distance searched
 *          *mode - 0 not found
 *                - 1 fwd match found
 *                - 2 bkwd match found
 *                - 3 fwd and bkwd matches found ?
 */
I32s ctemplate(f, b, slim, mode, tz, dir, tmpcall)
    I32s *f,*b, *slim, tz, dir, tmpcall;
    I8s *mode;  /* preference for forward (1) or backward (2) target */
{   I32s o, l = 1, adrt;
    I32s i = 0, fmatch = 0, bmatch = 0;
    I8s df=0, db=0;

    if ((tz < MinTemplSize) || (tz > SoupSize))
    {   adrt = -1;
        *mode = 0;
        goto finish;
    }
    if ((I8s) dir == 'o')  /* both directions */
        df = db = 1;
    else if ((I8s) dir == 'f')  /* forward only */
    {   df = 1;
        db = 0;
    }
    else if ((I8s) dir == 'b')  /* backwards only */
    {   df = 0;
        db = 1;
    }
    o = ad(ce->c.c->ip + 1);
    while (1)
    {   while (1) /* this skips code that are not templates (NOPs) */
        {
#if PLOIDY == 1
            if ((df &&  /* forward */
                    (soup[*f] == Nop0 || soup[*f] == Nop1)
#ifdef READPROT
                    && PrivRead(ce, *f)
#endif /* READPROT */
                )
                || (db &&  /* backward */
                    (soup[*b] == Nop0 || soup[*b] == Nop1)
#ifdef READPROT
                    && PrivRead(ce, *b)
#endif /* READPROT */
                ))
#else /* PLOIDY > 1 */
            if ((df &&  /* forward */
                    (soup[*f][is.dtra] == Nop0 || soup[*f][is.dtra] == Nop1)
#ifdef READPROT
                    && PrivRead(ce, *f)
#endif /* READPROT */
                )
                || (db &&  /* backward */
                    (soup[*b][is.dtra] == Nop0 || soup[*b][is.dtra] == Nop1)
#ifdef READPROT
                    && PrivRead(ce, *b)
#endif /* READPROT */
                ))
#endif /* PLOIDY > 1 */
                break;
            if (df)
            {   (*f)++;
                *f = ad(*f);
            }
            if (db)
            {   (*b)--;
                *b = ad(*b);
            }
            l++;
            if (l > *slim)  /* if we exceed the search limit abort */
            {   adrt = -1;
                *mode = 0;
                goto finish;
            }
        } /* inner while(1) this skips code that are not templates */

        /* forward */
#if PLOIDY == 1
        if (df && (soup[*f] == Nop0  /* if NOPs */
                || soup[*f] == Nop1))
#else /* PLOIDY > 1 */
        if (df && (soup[*f][is.dtra] == Nop0  /* if NOPs */
                || soup[*f][is.dtra] == Nop1))
#endif /* PLOIDY > 1 */
        {   fmatch = 1;
            for (i = 0; i < tz; i++)  /* over the full template size */
            {
#if PLOIDY == 1
                if (soup[ad(o + i)] +
                    soup[ad(*f + i)] - NopS
#ifdef READPROT
                    || !PrivRead(ce, ad(*f + i))
#endif /* READPROT */
#else /* PLOIDY > 1 */
                if (soup[ad(o + i)][is.stra] +
                    soup[ad(*f + i)][is.dtra] - NopS
#ifdef READPROT
                    || !PrivRead(ce, ad(*f + i))
#endif /* READPROT */
#endif /* PLOIDY > 1 */
                    )
                {   fmatch = 0;
                    break;
                }
            }
        }
        else
            fmatch = 0;

        /* backward */
#if PLOIDY == 1
        if (db && (soup[*b] == Nop0  /* if NOPs */
                || soup[*b] == Nop1))
#else /* PLOIDY > 1 */
        if (db && (soup[*b][is.dtra] == Nop0  /* if NOPs */
                || soup[*b][is.dtra] == Nop1))
#endif /* PLOIDY > 1 */
        {   bmatch = 1;
            for (i = 0; i < tz; i++)  /* over the full template size */
            {
#if PLOIDY == 1
                if (soup[ad(o + i)] +
                    soup[ad(*b + i)] - NopS
#ifdef READPROT
                    || !PrivRead(ce, ad(*b + i))
#endif /* READPROT */
#else /* PLOIDY > 1 */
                if (soup[ad(o + i)][is.stra] +
                    soup[ad(*b + i)][is.dtra] - NopS
#ifdef READPROT
                    || !PrivRead(ce, ad(*b + i))
#endif /* READPROT */
#endif /* PLOIDY > 1 */
                    )
                {   bmatch = 0;
                    break;
                }
            }
        }
        else
            bmatch = 0;

        if (fmatch && bmatch)
        {   if (*mode == 1)
            {   *f += flaw();
                adrt = ad(*f + tz);
                *mode = 3;
                goto finish;
            }
            else if (*mode == 2)
            {   *b += flaw();
                adrt = ad(*b + tz);
                *mode = 3;
                goto finish;
            }
        }
        else if (fmatch)
        {   *f += flaw();
            adrt = ad(*f + tz);
            *mode = 1;
            goto finish;
        }
        else if (bmatch)
        {   *b += flaw();
            adrt = ad(*b + tz);
            *mode = 2;
            goto finish;
        }
        if (db) /* increment search pointers, backward and forward */
        {   (*b)--;
            *b = ad(*b);
        }
        if (df)
        {   (*f)++;
            *f = ad(*f);
        }
        l++;
        if (l > *slim)  /* if we exceed the search limit abort */
        {   adrt = -1;
            *mode = 0;
            goto finish;
        }
    }           /* outermost while(1) */
    finish:
    *slim = l;
    if (GeneBnker && WatchTem)
    {   if (adrt >= 0)
            GenExTemp(ad(adrt - tz), tz, tmpcall, 0);
    }
    return adrt;  /* address of instruction following target template */
}

/*
 * getregs - copy registers of other cpu into current cpu
 * is.sval - 0 - previous cpu
 *           1 - next cpu
 *           2 - random cpu
 */
void getregs()
{   I32s i, s=0, d = ce->c.ac;

    ce->c.c->fl.E = ce->c.c->fl.S = ce->c.c->fl.Z = 0;
    if (ce->c.n < 2)
    {
#ifdef ERRORTIE
        FEError(-720, EXIT, WRITE, "Tierra getregs() error: ce->c.n < 2");
#endif /* ERRORTIE */
        ce->c.c->fl.E = 1;
        return;
    }
    switch (is.sval)
    {   case 0:
        {   if (d)
                s = d - 1;
            else
                s = ce->c.n - 1;
            break;
        }
    case 1:
        {   if (d < ce->c.n - 1)
                s = d + 1;
            else
                s = 0;
            break;
        }
    case 2:
        {   do
            {   s = (I8s) tcrand() % ce->c.n;
            }
            while (s == d);
            break;
        }
    default:
        break;
    }
    for (i = 0; i < ALOC_REG; i++)
        ce->c.c->re[i] = ce->c.ar[s].re[i];
}

#if PLOIDY > 1

/*
 * trex - switch track of execution
 */
void trex()
{   if (!flaw())
        ChangeTrack(&(ce->c.c->ex));
    ce->c.c->fl.E = ce->c.c->fl.S = ce->c.c->fl.Z = 0;
}

/*
 * trso - switch track of source
 */
void trso()
{   if (!flaw())
        ChangeTrack(&(ce->c.c->so));
    ce->c.c->fl.E = ce->c.c->fl.S = ce->c.c->fl.Z = 0;
}

/*
 * trde - switch track of destination
 */
void trde()
{   if (!flaw())
        ChangeTrack(&(ce->c.c->de));
    ce->c.c->fl.E = ce->c.c->fl.S = ce->c.c->fl.Z = 0;
}

/*
 * JumpTrack - switch current execution track to randomly selected one
 */
void JumpTrack()
{   if(((is.dreg)==(&(ce->c.c->ip)))&&(tdrand()<JumpTrackProb))
        ChangeTrack(&(ce->c.c->ex));
    ce->c.c->fl.E = ce->c.c->fl.S = ce->c.c->fl.Z = 0;
}

/*
 * ChangeTrack - change given track
 *
 * track - pointer to given track
 */
void ChangeTrack(track)
I8s *track;
{
#if PLOIDY > 2
    I8s ttr=(*track);
#endif
#if PLOIDY == 2
    (*track)=!(*track);
#else /* PLOIDY == 2 */
    while((*track)==ttr)
        (*track)=(I8s)(tcrand()%PLOIDY);
#endif /* PLOIDY == 2 */
}

#endif /* PLOIDY > 1 */
