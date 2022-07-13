/* queues.c 30-3-98 queueing functions for the Tierra Simulator */
/* Tierra Simulator V5.0: Copyright (c) 1991 - 1998 */
/* Tom Ray & Virtual Life */
/* MemProf labels: #13XX, in use: none */
/* FEError labels: #13XX, in use: 1300-1306 */

#include "license.h"
#include "tierra.h"
#include "globals.h"

#ifdef TIERRA

/*
 * The slicer uses a circular queue.  The reaper uses a linear queue.
 *
 * The first cell in the queues will look like this:
 *   htis = {a = 0, i = 2}
 * n_time = {a = 0, i = 2}
 * p_time = {a = 0, i = 2}
 * n_reap = {a = 0, i = 1}  below last cell to be reaped
 * p_reap = {a = 0, i = 0}  above next cell to be reaped
 *
 * after the second cell enters both queues,
 * the first two cells will look like:
 *
 * first cell:
 *   htis = {a = 0, i = 2}
 * n_time = {a = 0, i = 3}
 * p_time = {a = 0, i = 3}
 * n_reap = {a = 0, i = 3}
 * p_reap = {a = 0, i = 0}
 *
 * second cell:
 *   htis = {a = 0, i = 3}
 * n_time = {a = 0, i = 2}
 * p_time = {a = 0, i = 2}
 * n_reap = {a = 0, i = 1}
 * p_reap = {a = 0, i = 2}
 *
 * note that the slicer queue does not use {a = 0, i = 0} or {a = 0, i = 1}
 *
 * note that the reaper queue reserves {a = 0, i = 0} and {a = 0, i = 1}
 * for a special purpose: {a = 0, i = 0} is the top of the queue (it sits
 * above the next creature to be reaped), while {a = 0, i = 1} is the
 * bottom of the queue (it sits below the creature that is furthest from
 * being reaped).
 */

/*
 * IncrSliceQueue - select next cell from slicer queue
 */
void IncrSliceQueue()
{   ce = &cells[ce->q.n_time.a][ce->q.n_time.i];
    /* get dead or dormant cells out of queue */
    while (!ce->ld)
        RmvFrmSlicer(ce);
}

/*
 * DecrSliceQueue - select previous cell from slicer queue
 */
void DecrSliceQueue()           /* decrement slicer queue */
{   ce = &cells[ce->q.p_time.a][ce->q.p_time.i];
    while (!ce->ld)  /* get dead cells out of queue */
        RmvFrmSlicer(ce);
}

/*
 * EntBotSlicer - add cell to bottom of slicer queue
 *
 * nc - pointer to cell to be added to bottom of slicer queue
 */
void EntBotSlicer(nc)
    Pcells nc; /* new cell being added to bottom of slicer queue */
{   Pcells tc; /* ThisSlice cell (ce) */
    Pcells pc; /* previous slice cell */

    tc = ce;
    pc = &cells[tc->q.p_time.a][tc->q.p_time.i];
#ifdef ERRORTIE
    if ((nc->q.n_time.a != nc->q.p_time.a || nc->q.n_time.i != nc->q.p_time.i)
        && NumCells > 1)
        porterrmsg(1300,
            "Tierra EntBotSlicer() error: cell already in slicer queue",1);
#endif /* ERRORTIE */
    pc->q.n_time = nc->q.htis;  /* previous slice points to nc */
    nc->q.p_time = tc->q.p_time;  /* nc points back at previous slice */
    nc->q.n_time = ce->q.htis;  /* nc points to this slice */
    tc->q.p_time = nc->q.htis;  /* this slice points back at nc */
}

/*
 * UpReaper - move cell up the reaper queue
 *
 * cp - pointer to cell to be moved up in reaper
 */
void UpReaper(cp)
    Pcells cp;
{   Pcells pe2, pe, ne;

    if (cp == TopReap)
        return;
#ifdef ERRORTIE
    if (cp->q.n_reap.a == cp->q.htis.a && cp->q.n_reap.i == cp->q.htis.i
        && cp->q.htis.a == cp->q.p_reap.a && cp->q.htis.i == cp->q.p_reap.i
        && NumCells > 1)
        porterrmsg(1301,"Tierra UpReaper() error: cell not in reaper queue",1);
#endif /* ERRORTIE */
    ne = &cells[cp->q.n_reap.a][cp->q.n_reap.i];  /*ne is next cell in reaper */
    pe = &cells[cp->q.p_reap.a][cp->q.p_reap.i];
    /* pe is previous cell in reaper */
    pe2 = &cells[pe->q.p_reap.a][pe->q.p_reap.i];
    /* pe2 is 2nd previous cell in reaper */
    ne->q.p_reap = pe->q.htis;  /* ne points back to pe */
    cp->q.p_reap = pe2->q.htis;  /* cp points back to pe2 */
    cp->q.n_reap = pe->q.htis;  /* cp points ahead to pe */
    pe->q.p_reap = cp->q.htis;  /* pe points back to cp */
    pe->q.n_reap = ne->q.htis;  /* pe points ahead to ne */
    pe2->q.n_reap = cp->q.htis;  /* pe2 points ahead to cp */
    if (cp == BottomReap)
        BottomReap = pe;
    if (pe == TopReap)
        TopReap = cp;
}

/*
 * DownReaper - move cell down in reaper queue
 *
 * cp - pointer to cell to be moved down in reaper queue
 */
void DownReaper(cp)
    Pcells cp;
{   Pcells pe, ne, ne2;

    if (cp == BottomReap)
        return;
#ifdef ERRORTIE
    if (cp->q.n_reap.a == cp->q.htis.a && cp->q.n_reap.i == cp->q.htis.i
        && cp->q.htis.a == cp->q.p_reap.a && cp->q.htis.i == cp->q.p_reap.i
        && NumCells > 1)
        porterrmsg(1302,
            "Tierra DownReaper() error: cell not in reaper queue",1);
#endif /* ERRORTIE */
    ne = &cells[cp->q.n_reap.a][cp->q.n_reap.i];/*ne is next cell in reaper */
    ne2 = &cells[ne->q.n_reap.a][ne->q.n_reap.i];
    /* ne2 is 2nd next cell in reaper */
    pe = &cells[cp->q.p_reap.a][cp->q.p_reap.i];
    /* pe is previous cell in reaper */
    pe->q.n_reap = ne->q.htis;  /* pe points ahead to ne */
    cp->q.n_reap = ne2->q.htis;  /* cp points ahead to ne2 */
    cp->q.p_reap = ne->q.htis;  /* cp points back to ne */
    ne->q.n_reap = cp->q.htis;  /* ne points ahead to cp */
    ne->q.p_reap = pe->q.htis;  /* ne points back to pe */
    ne2->q.p_reap = cp->q.htis;  /* ne2 points back to cp */
    if (cp == TopReap)
        TopReap = ne;
    if (ne == BottomReap)
        BottomReap = cp;
}

/*
 * UpRprIf - if given cell has an error count equal to or greater than that
 *           of the upper cell in the reaper queue, move the given cell
 *           above the upper cell
 *
 * cp - given cell
 */
void UpRprIf(cp)
    Pcells cp;
{   if (cp->d.flags >= cells[cp->q.p_reap.a][cp->q.p_reap.i].d.flags)
        UpReaper(cp);
}

/*
 * UpRprIf - if given cell has an error count less than or equal to that
 *           of the lower cell in the reaper queue, move the given cell
 *           below the lower cell
 *
 * cp - given cell
 */
void DownReperIf(cp)
    Pcells cp;
{   if (cp->d.flags <= cells[cp->q.n_reap.a][cp->q.n_reap.i].d.flags)
        DownReaper(cp);
}

/*
 * EntBotReaper - add cell to bottom of reaper queue
 *
 * nc - pointer to cell to be added to bottom of reaper queue
 */

void EntBotReaper(cp)
    Pcells cp;  /* cell to be added to the bottom of reaper queue */
{   Pcells be = BottomReap;  /* cell presently at bottom of reaper queue */

#ifdef ERRORTIE
    if ((cp->q.n_reap.a != cp->q.p_reap.a || cp->q.n_reap.i != cp->q.p_reap.i)
        && NumCells > 1)
        porterrmsg(1303,
            "Tierra EntBotReaper() error: cell already in reaper queue",1);
#endif /* ERRORTIE */
    if (NumCells == 1)
    {   cp->q.p_reap = TopDummy->q.htis;
        cp->q.n_reap = BottomDummy->q.htis;
        TopDummy->q.n_reap = cp->q.htis;
        BottomDummy->q.p_reap = cp->q.htis;
        TopReap = cp;  /* TopReap is now cp */
    }
    else
    {   cp->q.p_reap = BottomReap->q.htis;
            /* new cell points back to old BottomReap */
        cp->q.n_reap = BottomDummy->q.htis;
            /* new cell points ahead to dummy bottom */
        be->q.n_reap = cp->q.htis;/* old BottomReap now points ahead to cp */
        BottomDummy->q.p_reap = cp->q.htis;
    }
    BottomReap = cp;  /* BottomReap is now cp */
}

/*
 * RmvFrmReaper - remove cell from reaper queue
 *
 * cp - pointer to cell to be removed from reaper queue
 */
void RmvFrmReaper(cp)
    Pcells cp; /* cell to be removed from reaper queue */
{   Pcells nc; /* next cell in reaper queue */
    Pcells pc; /* previous cell in reaper queue */

    nc = &cells[cp->q.n_reap.a][cp->q.n_reap.i];
    pc = &cells[cp->q.p_reap.a][cp->q.p_reap.i];
#ifdef ERRORTIE
    if (cp->q.n_reap.a == cp->q.htis.a && cp->q.n_reap.i == cp->q.htis.i
        && cp->q.htis.a == cp->q.p_reap.a && cp->q.htis.i == cp->q.p_reap.i
        && NumCells > 2)
        porterrmsg(1304,
            "Tierra RmvFrmReaper() error: cell not in reaper queue",1);
#endif /* ERRORTIE */
    if (cp == TopReap)  /* TopReap changed to next cell in queue */
        TopReap = &cells[cp->q.n_reap.a][cp->q.n_reap.i];
    if (cp == BottomReap)  /* BottomReap changed to previous cell in queue */
        BottomReap = &cells[cp->q.p_reap.a][cp->q.p_reap.i];
    /* previous cell points ahead to next cell: */
    pc->q.n_reap = cp->q.n_reap;
    /* next cell points back to previous cell: */
    nc->q.p_reap = cp->q.p_reap;
    cp->q.p_reap = cp->q.n_reap = cp->q.htis;
    /* initialize reap queue this cell */
#ifdef ERRORTIE
    if ((cp->q.n_time.a != cp->q.p_time.a || cp->q.n_time.i != cp->q.p_time.i)
        && NumCells > 1)
        porterrmsg(1305,
            "Tierra RmvFrmReaper() error: cell still in slicer queue",1);
#endif /* ERRORTIE */
}

/*
 * RmvFrmSlicer - remove cell from slicer queue
 *
 * rc - pointer to cell to be removed from slicer queue
 */
void RmvFrmSlicer(rc)
    Pcells rc; /* cell to be removed from slicer queue */
{   Pcells nc; /* next cell in slicer queue */
    Pcells pc; /* previous cell in slicer queue */
    nc = &cells[rc->q.n_time.a][rc->q.n_time.i];
    pc = &cells[rc->q.p_time.a][rc->q.p_time.i];
#ifdef ERRORTIE
    if (rc->q.n_time.a == rc->q.htis.a && rc->q.n_time.i == rc->q.htis.i
        && rc->q.htis.a == rc->q.p_time.a && rc->q.htis.i == rc->q.p_time.i
        && NumCells > 2)
        porterrmsg(1306,
            "Tierra RmvFrmSlicer() error: cell not in slicer queue",1);
#endif /* ERRORTIE */
    /* previous cell points ahead to next cell: */
    pc->q.n_time = rc->q.n_time;
    /* next cell points back to previous cell: */
    nc->q.p_time = rc->q.p_time;
    if (rc == ce)
        ce = &cells[rc->q.n_time.a][rc->q.n_time.i];
    rc->q.n_time = rc->q.p_time = rc->q.htis;
    /* initialize slice queue this cell */
}
#endif /* TIERRA */

#if defined(TIERRA)||defined(ARGTIE)||\
    defined(GENALIGN)||defined(THREADTREE)||\
    defined(PROBE)||defined(CLSTRSRVR)||\
    defined(SOUPUPDTRC)

/**********************************************************************
 * InitListHead - initialize empty ListHead structure
 * input
 *    listheadbuf - pointer to ListHead structure.
 **********************************************************************/
void InitListHead(listheadbuf)
ListHead *listheadbuf;
{   listheadbuf->lsh_first=(ListNode *)(&(listheadbuf->lsh_firstlast));
    listheadbuf->lsh_firstlast=0;
    listheadbuf->lsh_last=(ListNode *)(&(listheadbuf->lsh_first));
}

/**********************************************************************
 * AddListNode - add node to list
 * input
 *    prevnode - pointer to previous node
 *    addnode - pointer to node to be added
 **********************************************************************/
void AddListNode(prevnode, addnode)
ListNode *prevnode, *addnode;
{   prevnode->lnd_next->lnd_prev=addnode;
    addnode->lnd_next=prevnode->lnd_next;
    prevnode->lnd_next=addnode;
    addnode->lnd_prev=prevnode;
}

/**********************************************************************
 * RemListNode - remove node from list
 * input
 *    remnode - pointer to node to be removed
 **********************************************************************/
void RemListNode(remnode)
ListNode *remnode;
{   remnode->lnd_prev->lnd_next=remnode->lnd_next;
    remnode->lnd_next->lnd_prev=remnode->lnd_prev;
}

/**********************************************************************
 * RemHeadNode - remove node from list head
 * input
 *    listheadbuf - pointer to list header.
 **********************************************************************/
void RemHeadNode(listheadbuf)
ListHead *listheadbuf;
{   RemListNode(listheadbuf->lsh_first);
}

/**********************************************************************
 * AddTailNode - add node to list tail.
 * input
 *    listheadbuf - pointer to list header.
 **********************************************************************/
void AddTailNode(listheadbuf, listnodebuf)
ListHead *listheadbuf;
ListNode *listnodebuf;
{   AddListNode((ListNode *)listheadbuf->lsh_last, listnodebuf);
}

#endif /* defined(TIERRA)||defined(ARGTIE)||
          defined(GENALIGN)||defined(THREADTREE)||
          defined(PROBE)||defined(CLSTRSRVR)||
          defined(SOUPUPDTRC) */
