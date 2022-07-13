/* @(#)memtree.c 1.3 12/15/94 */
/* memtree.c 30-3-98 memory allocation routines for the Tierra Simulator */
/* Tierra Simulator V5.0: Copyright (c) 1991 - 1998 */
/* C. J. Stephenson & Virtual Life */
/* MemProf labels: #09XX, in use: none */
/* FEError labels: #09XX, in use: 0900-0911 */

#include "license.h"
#include "tierra.h"
#include "globals.h"

#ifdef BGL
#include "tbgl.h"
#endif /* BGL */

/* This is the file memtree.c 

   This file contains the code for:

   MemInit()    --  Initialize soup allocator
   IsFree()     --  Enquire status of soup addr
   MemAlloc()   --  Allocate an area of the soup
   MemDealloc() --  Deallocate an area of soup   */


/*  _________________________________________________
   |                                                 | 
   |        NEW  SOUP  ALLOCATOR  FOR  TIERRA        |
   |_________________________________________________|


   Program and program notes by C.J.Stephenson, Santa
   Fe, July 1992.

   *    *    *

   This soup allocator allows the program requesting 
   space to supply a preferred address for allocation,
   and also a tolerance. This permits a dividing creat-
   ure to place its offspring near itself (which may be 
   desirable for self-contained creatures), or scatter-
   ed far and wide (which may make sense for seeds and
   parasites).

   This allocator is also expected to perform better 
   than the previous version.

   *    *    *

   The `soup' begins at zero and has a size of SoupSize
   (global var). The soup size is constant during a run.

   The soup allocator possesses two main entry points,
   MemAlloc() and MemDealloc().  

   When MemAlloc() is called, it doles out some soup
   by finding an unoccupied area of adequate size.  When 
   MemDealloc() is called, it records that the specified 
   area of soup is no longer occupied (and is therefore
   available for reallocation).

   This soup allocator maintains a record of the unoc-
   cupied areas in a `cartesian' tree.  A cartesian tree
   is a binary search tree in which one property is ord-
   ered horizontally (in this case the soup addr), and
   some other property is ordered vertically (in this
   case the size of the unoccupied area), such that no
   son is `heavier' than its father. Therefore the root
   of the tree always describes the biggest unoccupied
   area.  Here is a picture of such a tree:

   _________
   |         |
   | 300,500 |
   |_________|
   /    \
   /      \
   /        \
   /          \
   /            \
   /              \
   __/______          \__________
   |         |         |          |
   | 100,100 |         | 1000,200 |
   |_________|         |__________|
   /                   /    \
   /                   /      \
   /                   /        \
   __/___            ____/___       \__________
   |      |          |        |      |          |
   | 0,50 |          | 900,50 |      | 1400,200 |
   |______|          |________|      |__________|
   \
   \
   \
   \________
   |        |
   | 960,10 |
   |________|


   A cartesian tree can be maintained by the technique 
   called `root insertion'.  It possesses several nice
   properties:

   (1)  The method offers better performance than list
   methods (and does not impose course granularity
   or possess the other restrictions of the `buddy'
   methods).  When allocating, an area of adequate
   size can be found by descending from the root 
   and examining only the areas of adequate size,
   without needing to inspect the numerous scraps
   of inadequate size (which are relegated to the 
   lower branches and the leaves).  And when deal-
   locating, the area being returned can be placed 
   correctly in the tree, and combined with its 
   neighbour(s) if any, by performing a binary 
   search, which usually requires far less work
   than searching a list.

   (Note however that the tree is not balanced, and
   cannot in general be balanced, so the worst-case
   performance is still bad, though rarely encount-
   ered in practice.  Techniques exist for alleviat-
   ing the worst case, but they are not used in this
   program.)

   (2)  The method allows several interesting allocation
   policies to be offered cheaply:

   (a) `Friendly Fit'.  The caller of MemAlloc() 
   can supply a preferred addr for allocation, and
   an acceptable tolerance. The allocator can find 
   the nearest unoccupied area (of adequate size) 
   by performing a binary search for the prefer-
   red address. If the nearest area is near enough
   (i.e. within the given tolerance), the allocator
   supplies some soup from this area; otherwise it 
   reports that there is no suitably placed soup
   available.

   First fit (or `Leftmost Fit' from the tree) can 
   be realized simply by supplying a preferred addr
   of zero and the most generous tolerance.

   (b) `Better Fit'. Alternatively the caller can
   indicate the absence of a preference, in which
   case the allocator employs a policy that tends
   to minimize fragmentation of the soup.

   Control over placement is the issue that motivated
   this new allocator.  In Tierra, proximity in the soup
   has semantic significance, since it affects the inter-
   actions between creatures.  It therefore seems desir-
   able to give Mama some control over the position of
   her offspring.

   For a description of root insertion, see:

   [1]  C.J.Stephenson, A method for constructing
   binary search trees by making insertions
   at the root, Int J Comp and Inf Sci, Vol
   9 (1980).

   For the original description of cartesian trees,
   see:

   [2]  J.Vuillemin, A unifying look at data
   structures, CACM Vol 23 (1980).

   For the application of cartesian trees to memory
   allocation, see:

   [3]  C.J.Stephenson, Fast Fits: New methods for
   dynamic storage allocation, Operating Sys
   Review (extended abstract), Vol 17 (1983).

   Note that ref [3] describes a method of memory
   allocation in which the tree is constructed in
   the unoccupied pieces themselves, i.e. in the
   same space that is being managed. In the pre-
   sent application, the tree is constructed in
   separate memory (see below), and not within
   the soup.


   THE NODE ARRAY

   The nodes for the tree are `allocated' from an
   array of nodes, and the tree `ptrs' are represented
   by the indices of the nodes in the array. This per-
   mits the entire tree to be preserved in a file (or
   restored from file) simply be writing (or reading)
   the array.  (A similar scheme was employed in the
   previous soup allocator, and carried over to this
   one.) 

   The structure that describes an indiividual node
   is named MemFr (for `Memory Frame').  It contains 4
   signed fullword integers, laid out thus:

          ___________________________ 
          |             |             |
   MemFr  | l=left ptr  | r=right ptr | 
          |_____________|_____________|
          |             |             |
          | p=soup addr |   s = size  |
          |_____________|_____________|


   where:  l,r  =  `ptrs' to (i.e. indices of) the
   left and right sons, or zero if
   corresponding son does not exist

   p,s  =  addr and size of unoccupied area
   of the soup (0 <= addr < SoupSize,
   0 < s <= SoupSize)

   The addr of the array is maintained in FreeMemry (glob-
   al var) -- and the number of nodes in the array is in 
   MaxFreeBlocks (another global var).  An initial modest
   array is allocated when the program begins, and is en-
   larged as necessary during the course of the run (with
   FreeMemry and MaxFreeBocks being adjusted accordingly).
   Here is a picture of the array:

                     ___________
                     |           |            
       FreeMemry[0]  |   MemFr   |  Node 0 is special (see below)
                     |___________|
                     |           |            
       FreeMemry[1]  |   MemFr   |  Some nodes are used for the tree
                     |___________|  
                     |           |            
       FreeMemry[2]  |   MemFr   | 
                     |___________|
                     |           |            
       FreeMemry[3]  |   MemFr   |  Some nodes are typically unused
                     |___________|
                     :           :
                     :           :
                     :           :
                     :___________:
                     |           |            
   FreeMemry[MFB-1]  |   MemFr   |  (MFB is short for MaxFreeBlocks)
                     |___________|


   The zeroth node in the array acts as an `anchor' for
   the tree -- and also for the unused nodes. It is laid
   out thus:

                 ___________________________ 
                 |             |             |
   FreeMemry[0]  | l=`liberty' |  r = `root' |  The anchor
                 |_____________|_____________|
                 |             |             |
                 |    p = 0    |    s = 0    |
                 |_____________|_____________|


   where:  l = `liberty ptr' = anchor for unused nodes
   (see below), or zero if
   no unused nodes exist

   r =  `root ptr'   = index of the tree root
   (1 <= r < MFB), or zero
   if the tree is empty


   UNUSED NODES

   There are two flavours of unused nodes, untouched
   and recycled.

   Untouched nodes are nodes that have never been used
   in the tree.  Their contents are undefined.  They form
   a contiguous group (which may be empty) at the end of
   the array.  The position of the first is identified by 
   a negative `liberty ptr' having a value of u-MFB, where
   u = index of first untouched node; note that this nat-
   urally acquires a value of zero when the last untouch-
   ed node is pressed into service.

   Recycled nodes are nodes that have been used in the
   tree, but are not currently used. They may be sprinkled
   among the nodes that are in use.  They are chained to-   
   gether, via their `l' ptrs, into a free list. 

   The liberty ptr in the anchor identifies the first
   recycled node if any (positive ptr), or the first un-
   touched node otherwise (negative ptr), or it is zero 
   if the array is `full'  If recycled nodes exist, the 
   last one identifies the first untouched node, or con-
   tains a zero ptr if there are no untouched nodes. 

   Fortunately this scheme is easier to implement
   than to describe, and has the nice property that the
   tail of the array is not touched until it is needed
   (or is never touched if never needed), which is good
   for the working set.  [Actually, at the time of writ-
   ing (1992/07), the array is allocated and reallocated  
   with calloc(), which clears the entire thing to zero  
   -- so in truth there is little benefit in postponing 
   contact with the tail.  This soup allocator does not 
   however require the array to be cleared, and malloc()
   could be used instead of calloc(), which would result 
   in slightly better performance.]


   NULL POINTERS IN THE TREE

   In the tree, a left or right ptr of zero indic-
   ates the absence of a left or right son. When examin-
   ing or manipulating the tree, it is usually necessary
   to check for such `null ptrs' explicitly, in order to 
   avoid unwittingly falling out of the tree.  In some 
   contexts, however, it is possible to omit the check, 
   and cunningly plough on regardless (without risking
   a broken arm).

   Consider for example a search for the deepest node
   of adequate size on some particular path through the
   tree.  (For the present purpose, the actual path is
   immaterial.)  If we treat a null left or right ptr as
   a valid tree ptr, we will find ourselves examining node
   0 (the anchor), where the `size' field is permanently
   set to zero. So we will conclude (correctly) that the
   node possessing the null ptr does not possess a son of
   adequate size, without needing to distinguish between
   the case of a small son and the case of a missing one.

   *    *    *

   My thanks to Tom Ray, for creating Tierra; to Tom and
   his colleague Dan Pirone for encouraging this little
   project (and for answering numerous questions); and
   to the Santa Fe Institute and IBM for providing the
   facilities and the time for the work.

   CJS, Santa Fe, July 1992.                         */




/* Declare private internal functions */

static void deletenode P_((I32s Hp pa, Pmf victim));
static void demote P_((I32s Hp pa, Pmf victim));
static void promote P_((I32s Hp pa, Pmf riser));
static I32s memnode P_((void));


/*  _________________________________________________
   |                                                 | 
   |  Function MemInit -- Initialize soup allocator  |
   |_________________________________________________|

   Call is:  MemInit ()

   On entry: FreeMemry  = addr of initial memory node array 
   MaxFreeBlocks = the size of this array (>=2)
   SoupSize = size of soup in instr slots (>0)

   Effect is to initialize memory node 0 (anchor) and 1
   (initial root), thus:

                _________________________ 
                |            |            |
   FreeMemry[0] | l=-(MFB-2) |     r=1    | Anchor for tree
                |____________|____________|
                |            |            |
                |    p=0     |     s=0    |
                |____________|____________|
                |            |            |
   FreeMemry[1] |    l=0     |     r=0    | The initial root
                |____________|____________|
                |            |            |
                |    p=0     | s=SoupSize |
                |____________|____________|
                |            |            |
                :            :            :

   where `MFB' is short for MaxFreeBlocks (array size).

   Notes:

   1.   This version of MemInit is for the new 
   soup allocator (CJS, Santa Fe, July 1992).

   2.   This routine does not bother to initialize
   fields that require an initial value of zero,
   since the memory for the node array is obtain-
   ed with calloc(), which clears the whole thing.
   This soup allocator does not however require 
   the entire array to be initialized, and the
   array could equally well be obtained with
   malloc() instead of calloc() provided this
   routine was changed to clear the following
   fields:

   FreeMemry[0].p, FreeMemry[0].s,
   FreeMemry[1].l, FreeMemry[1].r, FreeMemry[1].p.  */

void MemInit()
{

#ifdef ERRORTIE

    if (MaxFreeBlocks < 2)
        FEError(-900, EXIT, WRITE,
            "Tierra MemInit() error: memory node array too small");

    if (SoupSize <= 0)
        FEError(-901, EXIT, WRITE,
            "Tierra MemInit() error: invalid soup size");

#endif /* ERRORTIE */

    FreeMemry[0].l = -(MaxFreeBlocks - 2);  /* -ve index of untouched */
    FreeMemry[0].r = 1;  /* Initial root for tree  */
    FreeMemry[1].s = SoupSize;  /* Initial root holds all */

    return;

}

/*  ________________________________________________________
   |                                                        | 
   |  Function IsFree -- Enquire whether soup addr is free  |
   |________________________________________________________|

   Call is:  y = IsFree (x)

   where:    x = signed fullword integer containing soup addr
   y = small integer (8 bits) which is set to 0 (if 
   given soup addr is occupied) or 1 (if free)     

   Also:     FreeMemry  = addr of the memory node array
   SoupSize = size of soup in instr slots

   This version of IsFree() was written for the new soup 
   allocator; CJS, Santa Fe, July 1992.                   */


I8s IsFree(x)
    I32s x;
{

    Pmf a,                      /* Addr of memory node array */
        c;                      /* Addr of current memory node */

#ifdef ERRORTIE
    if (x < 0 || x >= SoupSize)
    {   tsprintf((char *)(&(Fbuf[0])),
        "Tierra IsFree() error: addr %d not in soup", x);
        FEError(-902, EXIT, WRITE, (char *)(&(Fbuf[0])));
    }
#endif /* ERRORTIE */

    a = FreeMemry;  /* Addr of node array */
    c = a + a->r;  /* Addr of the tree root */

    while (c != a)  /* Until I fall from tree */
        if (c->p > x)  /* If this node exceeds x */
            c = a + c->l;  /* Step down to left son */
        else if ((c->p + c->s) > x)  /* If node contains x */
            return (1);  /* Yield 1 (addr free) */
        else
            c = a + c->r;  /* Step down to right son */

    return (0); /* Yield 0 (addr occupied) */

}

/*  _____________________________________________________
   |                                                     | 
   |  Function MemAlloc -- Allocate an area of the soup  | 
   |_____________________________________________________|

   Call is:  p = MemAlloc (size,pref,tol) 

   where:    size, pref, tol and p are all signed fullword 
   integers

   and:      size = required amount of soup in instr slots 
   (0 < size <= SoupSize)

   pref = preferred soup addr (0 <= pref < SoupSize),
   or pref < 0 if the caller has no preference

   tol  = acceptable tolerance (0 <= tol < SoupSize),
   or irrel if pref < 0

   p    = allocated soup addr (normally),
   or -1 if there is no unoccupied  area
   of adequate size,
   or -2 if an area of adequate size exists,
   but not within the given tolerance

   Also:     FreeMemry        = addr of memory node array  :  May be changed
   MaxFreeBlocks    = size of memory node array  :  as side effect 
   FreeMemCurrent   = sum of unoccupied space    :  of call

   SoupSize = size of soup in instr slots      :  Not changed


   ----------------  Summary of MemAlloc()  ----------------

   1.   If the largest unoccupied area of soup has inadequate
   size, supply a dummy soup addr of -1, without allocat-
   ing any soup at all.  Otherwise proceed as follows.

   2.   If pref < 0, ignore given tolerance, and allocate 
   required soup using `Better Fit', which is sure to
   succeed (see below).  Otherwise proceed as follows.

   3.   Identify the winning unoccupied area.

   If the preferred addr lies within an unoccupied area 
   of adequate size, the winning area is the one contain-
   ing the preferred addr; otherwise it is the area of
   adequate size that possesses the minimum separation
   from the preferred addr. (The `separation' is measur-
   ed from `pref' to the beginning of the area, if pref
   precedes the area; or from the end of the area to
   `pref', if the area precedes pref.) If two adequate 
   areas possess equal separation, the one on the left
   prevails.

   4a.  If the winning unoccupied area does not contain the
   preferred addr, identify the nearer edge of the area,
   and measure the absolute difference between this edge
   and the preferred addr.  If this distance exceeds the
   given tolerance, supply a dummy soup addr of -2, with-
   out allocating any soup at all.  Otherwise allocate
   the required soup from the nearer edge.

   4b.  If the winning area contains the preferred addr,
   identify the nearer edge of the area (or define the
   left edge as the nearer if `pref' lies at the centre
   of the winning area), and measure the absolute dis-
   tance between this edge and the preferred addr.  If
   the distance does not exceed size+tol, allocate the
   required soup from this nearer edge; otherwise al-
   locate it at the preferred addr exactly (i.e. beg-
   inning at `pref' and extending to the right).

   Notes:

   1.   `Better Fit' is an allocation policy which selects
   a winning node by descending from the root, always 
   choosing the better-fitting son, until it encount-
   ers a node which possess no sons of adequate size 
   (or no sons at all). The winning area of the soup 
   is the one described by the winning node. 

   In this implementation of Better Fit:

   if both sons are adequate, and have the  
   same size, the one on the left prevails;

   if the size of the winning area exceeds 
   the required size, allocation is made 
   from its left edge.

   2.   `Leftmost Fit' from the tree (which is equivalent 
   to `First Fit' from the corresponding list) can be
   realized by supplying a preferred address of zero
   and the most generous allowable tolerance.        */


I32s MemAlloc(size, pref, tol)
    I32s size, pref, tol;
{

    Pmf a,                      /* Addr of memory node array */
        c,                      /* Addr of current memory node */
        e;                      /* Addr of new memory node */

    I32s Hp b,                  /* Addr of parental `ptr' to c */
        Hp pa=0,                /* Addr of `ptr' to best node */
        best,                   /* Distance to nearest node */
        oc,                     /* Offset of node `c' in array */
        oe,                     /* Offset of node `e' in array */
        opa,                    /* Offset of `pa' in node array */
        x,                      /* Beg soup addr of selected node */
        y,                      /* The soup addr that is allocated */
        z;                      /* End soup addr of selected node */

#ifdef ERRORTIE

    if (size <= 0 || size > SoupSize)
    {   tsprintf((char *)(&(Fbuf[0])),
            "Tierra MemAlloc error: invalid size %d", size);
        FEError(-903, EXIT, WRITE, (char *)(&(Fbuf[0])));
    }
    if (pref >= SoupSize)
    {   tsprintf((char *)(&(Fbuf[0])),
            "Tierra MemAlloc error: invalid preference %d", pref);
        FEError(-904, EXIT, WRITE, (char *)(&(Fbuf[0])));
    }
    if (pref >= 0 && (tol < 0 || tol >= SoupSize))
    {   tsprintf((char *)(&(Fbuf[0])),
            "Tierra MemAlloc error: invalid tolerance %d", tol);
        FEError(-905, EXIT, WRITE, (char *)(&(Fbuf[0])));
    }

#endif /* ERRORTIE */

    a = FreeMemry;  /* Addr of node array */
    c = a + a->r;  /* Addr of the tree root */

    if (c->s < size)  /* If biggest area is too small */
        return (-1);  /* Yield -1 (cannot supply soup) */

    if (pref < 0)  /* If caller has no preference */
        goto better;  /* Handle below (by better fit) */

    /* Arrive here if soup is to be allocated using  
       given preference and tolerance. At this point:

       a = addr of memory array
       c = addr of the root node

       size,pref,tol are as supplied by caller

       Search for the preferred addr until I reach
       a node whose son is too short.              */

    b = &a->r;  /* Addr of my parental `ptr' */
    best = I32S_MAX;  /* Best so far is pretty bad */

    do
    {           /* Execute this at least once */

        if (c->p > pref)
        {       /* If this soup addr > pref */
            if (c->p - pref <= best)
            {   /* And it is nearest so far */
                pa = b;  /* Remember the parental ptr */
                best = c->p - pref;  /* Remember closest contact */
            }
            b = &c->l;  /* Step down to the left */
        }
        else
        {   if (pref - (c->p + c->s - 1) <= best)
            {   pa = b;
                best = pref - (c->p + c->s - 1);  /* May set best < 0 */
            }
            b = &c->r;
        }

        c = a + *b;  /* Addr of the descendant node */

    }
    while (best > 0 && c->s >= size);


    /* Arrive here after finding the nearest unoccupied area
       of adequate size. See if it is near enough to his pre- 
       ference.  At this point:

       a = addr of the memory array
       pa = addr of `ptr' to best node
       best = distance separating best area from
       preferred addr, or <= 0 if preferred
       addr lies within an unoccupied area
       of adequate size

       size,pref,tol are as supplied by caller      */

    c = a + *pa;  /* Addr of winning node */

    x = c->p;   /* x = beginning soup addr */
    z = x + c->s;  /* z = ending addr for node */

    if (best > 0)
    {           /* If pref addr is not available */

        if (best > tol)  /* If distance exceeds tolerance */
            return (-2);  /* Yield -2 (cannot satisfy him) */

        if (x > pref)  /* If best is to right of pref */
            goto leftedge;  /* Allocate from left-hand edge */
        else    /* Or if the best is to the left */
            goto rightedge;  /* Allocate from right-hand edge */

    }

    /* Arrive here if the preferred addr lies within an
       unoccupied area of the soup. Allocate from the nearer
       edge -- if this will place the area within the speci-
       fied tolerance.  At this point:

       a = addr of the memory array
       c = the addr of the best node
       pa = addr of `ptr' to best node

       x = beg soup addr of best node
       z = end soup addr of best node

       size,pref,tol are as supplied by caller      */

    if (pref - x <= z - (pref + size - 1))  /* If left edge is nearer */

        if (pref - x <= tol)  /* And if it is near enough */
            goto leftedge;  /* Allocate from left edge */
        else;

    else
        /* If right edge nearer ... */ if (z - (pref + size - 1) <= tol)
        goto rightedge;
    else;

    /* Arrive here if both edges of the area containing 
       the preferred addr are too far from the preferred 
       addr.  Split the area into three, and allocate at
       the preferred addr exactly.  At this point:  

       a = addr of the memory array
       c = the addr of the best node
       pa = addr of `ptr' to best node

       x = beg soup addr of best node
       z = end soup addr of best node

       size,pref,tol are as supplied by caller      

       See note in `memnode' on relocation of ptrs.  */

    oc = c - a; /* Convert ptrs to offsets */
    opa = pa - (I32s Hp) a;

    oe = memnode();  /* Acquire new memory node */

    if (oe == 0)  /* If insufficient memory */
        goto bust;  /* Handle situation below */

    a = FreeMemry;  /* Addr of (new) node array */

    c = a + oc; /* Reset my  node ptrs */
    pa = (I32s Hp) a + opa;

    e = a + oe; /* Addr of new memory node */

    e->p = pref + size;  /* Addr of right-hand scrap */
    e->s = z - e->p;  /* Size of right-hand scrap */

    c->s = pref - x;  /* Size of left-hand scrap */

    /* Demote the larger scrap and then insert the
       smaller scrap at the correct place below it.

       a = addr of the memory array
       c = addr of node for LH scrap
       e = addr of node for RH scrap           

       pa = addr of `ptr' to LH scrap

       size,pref are as supplied by caller    */

    if (c->s >= e->s)
    {           /* If LHS is bigger */

        demote(pa, c);  /* Demote LH scrap */

        pa = &c->r;  /* Find home for RHS */
        while ((a + *pa)->s > e->s)
            pa = &(a + *pa)->l;

        e->l = 0;  /* Insert RH scrap */
        e->r = *pa;
        *pa = e - a;

    }
    else
    {           /* If RHS bigger ... */

        e->l = c->l;  /* Ptrs for demote */
        e->r = c->r;

        demote(pa, e);  /* Demote RH scrap */

        pa = &e->l;  /* Find home for LHS */
        while ((a + *pa)->s > c->s)
            pa = &(a + *pa)->r;

        c->l = *pa;  /* Insert LH scrap */
        c->r = 0;
        *pa = c - a;

    }

    y = pref;   /* Allocate at this addr */
    goto foot;  /* And handle rest below */


    /* Come here if no preference is given. Allocate 
       some soup using Better Fit.  (This is sure to 
       succeed, since I already checked the size of 
       the root.)  At this point:

       a = addr of the memory array
       c = the addr of the root node 

       size is as supplied by caller            */

  better:
    pa = &a->r; /* Addr of `ptr' to the root */

    while ((a + c->l)->s >= size || (a + c->r)->s >= size)
    {   /* While either son is big enough */

        if ((a + c->l)->s > (a + c->r)->s)  /* If left son is the larger */

            if ((a + c->r)->s >= size)  /* If right son is adequate */
                pa = &c->r;  /* Step down to the right */
            else  /* Otherwise ... */
                pa = &c->l;  /* Step down to the left */

        else
            /* If right son is larger ... */ if ((a + c->l)->s >= size)
            pa = &c->l;
        else
            pa = &c->r;

        c = a + *pa;  /* Step down (to left or right) */

    }

    /* Allocate from left edge of area. At this point:

       a = addr of the memory array
       c = addr of the winning node
       pa = addr of `ptr' to node

       size is as supplied by caller      */

  leftedge:
    y = c->p;   /* Allocate from left edge */
    c->p += size;  /* Adjust addr of remainder */
    goto eitheredge;

    /* Allocate from right edge of area. On arrival:

       a = addr of the memory array
       c = addr of the winning node
       pa = addr of `ptr' to winner
       z = end addr of winning node

       size is as supplied by caller      */

  rightedge:
    y = z - size;  /* Beginning addr for alloc */

    /* Adjust remaining size of area, and delete 
       or demote the winning node. At this point:

       a = addr of the memory array
       c = addr of the winning node
       pa = addr of `ptr' to node
       y = soup addr to allocate

       size is as supplied by caller      */

  eitheredge:
    c->s -= size;  /* Size of remaining scrap */

    if (c->s == 0)
    {           /* If nothing left at all */
        deletenode(pa, c);  /* Delete the winning node */
        c->l = a->l;  /* Recycle the memory node */
        a->l = c - a;
    }
    else        /* Otherwise ... */
        demote(pa, c);  /* Demote the winning node */


    /* All successful paths join here. Keep `almond'
       informed, and deliver the soup.  On arrival:

       y = soup addr to allocate
       size = size to allocate                 */

  foot:
    FreeMemCurrent -= size;  /* Maintain all the books */

#ifdef BGL
    if (FD_ISSET(BGL_MASK_OV, &BglMask))
        TBglBirth(y, size);
#endif /* BGL */


    return (y); /* Supply this soup addr */


    /* Come here if there is insufficient system memory
       available to obtain an enlarged node array. Emit 
       an unfriendly msg and abort execution.        */

  bust:
    FEError(-906, EXIT, WRITE,
        "Tierra MemAlloc error: insuf system memory avail");
    return 0;
}

/*  _____________________________________________________
   |                                                     | 
   |  Function MemDealloc -- Deallocate an area of soup  | 
   |_____________________________________________________|

   Call is:  MemDealloc (addr,size) 

   where:    addr and size are signed fullword integers

   and:      addr = soup addr of piece to be deallocated
   size = the size of the piece in instr slots

   Also:     FreeMemry      = addr of memory node array  :  May be changed
   MaxFreeBlocks  = size of memory node array  :  as side effect 
   FreeMemCurrent = sum of unoccupied space    :  of call

   SoupSize = size of soup in instr slots      :  Not changed  */


void MemDealloc(addr, size)
    I32s addr, size;
{

    Pmf a,                      /* Addr of memory node array */
        c,                      /* Addr of current memory node */
        e;                      /* Addr of the second neighbour */

    I32s Hp d,                  /* Addr of parental `ptr' to e */
        Hp pa,                  /* Addr of ptr to inserted node */
        pas,                    /* Size of node containing ptr */
        Hp lh,                  /* Left hook for root insertion */
        Hp rh,                  /* Right hook for root insertion */
        oc,                     /* Offset of node `c' in array */
        opa,                    /* Offset of `pa' in node array */
        x,                      /* Beg addr of (combined) piece */
        z;                      /* End addr of (combined) piece */

    MemFr scratch;              /* Scratch node */


#ifdef ERRORTIE

    if (addr < 0 || addr >= SoupSize)
    {   tsprintf((char *)(&(Fbuf[0])),
            "Tierra MemDealloc error: invalid soup addr %d", addr);
        FEError(-907, EXIT, WRITE, (char *)(&(Fbuf[0])));
    }
    if (size <= 0 || size > SoupSize)
    {   tsprintf((char *)(&(Fbuf[0])),
            "Tierra MemDealloc error: invalid size %d", size);
        FEError(-908, EXIT, WRITE, (char *)(&(Fbuf[0])));
    }
    if (addr + size > SoupSize)
        FEError(-909, EXIT, WRITE,
            "Tierra MemDealloc error: invalid soup args");

#endif /* ERRORTIE */

    a = FreeMemry;  /* Addr of node array */

    x = addr;   /* Given beginning addr */
    z = x + size;  /* Ending addr of area */

    pa = &a->r; /* Addr of `ptr' to root */
    pas = I32S_MAX;  /* Dummy indefinite size */

    c = a + *pa;  /* The addr of the root */

    /* Perform a passive binary search for the given piece 
       until I encounter a neighbour, or a node whose son 
       is no longer than the given piece.               */

    while (c->s > size)
    {           /* While among big nodes */

        if (c->p > z)  /* If node is to right */
            pa = &c->l;  /* Descend to the left */
        else if (c->p + c->s >= x)  /* If neighbour or overlap */
            goto highneigh;  /* Handle situation below */
        else
            pa = &c->r;  /* Descend to the right */

        pas = c->s;  /* Size of my new father */
        c = a + *pa;  /* Descend to this node */

    }


    /* Arrive here if I reach a point where I can insert
       a node describing the piece being deallocated before
       encountering a neighbour (or discovering invalid over-
       lap).  Start inserting a new node here (and continue 
       to watch for neighbours).  If it turns out there are 
       no neighbours, this will be the correct place for the
       node; and even if there are neighbour(s), it *may* be 
       the correct place. (At worst, I will have to promote 
       the node later.)  At this point:

       a = addr of the node array
       c = addr of the descendant node
       pa = addr of `ptr' to this node
       pas = size of the paternal node

       x = addr of piece being deallocated
       y = size of piece being deallocated
       z = endad of piece being deallocated         

       addr,size are as supplied by caller

       Use my scratch node for the time being (a new node 
       will not actually be needed if it turns out there 
       is a neighbour further down).                    */

    lh = &scratch.l;  /* Init hooks (scratch node) */
    rh = &scratch.r;

    while (c != a)  /* Until I fall from the tree */

        if (c->p > z)
        {       /* If this node is on right */
            *rh = c - a;  /* Attach to the right hook */
            rh = &c->l;  /* And descend to the left */
            c = a + c->l;
        }

        else if (c->p + c->s >= x)  /* If neighbour or overlap */
            goto lowneigh;  /* Handle situation below */

        else
        {       /* If node is to the left */
            *lh = c - a;  /* Attach to the left hook */
            lh = &c->r;  /* And descend to the right */
            c = a + c->r;
        }


    /* Arrive here if I complete the insertion without 
       encountering any neighbours (or discovering any 
       overlap). Clear both the hooks; and replace the
       scratch node by a proper one.  At this point:

       a = the addr of the node array
       pa = addr of `ptr' to scratch node

       x = addr of piece being deallocated
       y = size of piece being deallocated
       z = endad of piece being deallocated         

       addr,size are as supplied by caller

       See note in `memnode' on relocation of ptrs.  */

    *lh = *rh = 0;  /* Clear both the hooks */

    opa = pa - (I32s Hp) a;  /* Convert ptr to offset */

    oc = memnode();  /* Acquire new memory node */

    if (oc == 0)  /* If insufficient memory */
        goto bust;  /* Handle situation below */

    a = FreeMemry;  /* Addr of (new) node array */

    pa = (I32s Hp) a + opa;  /* Relocated parental ptr */
    c = a + oc; /* Addr of new memory node */

    c->l = scratch.l;  /* Copy ptrs from scratch */
    c->r = scratch.r;

    c->p = addr;  /* Set soup addr and size */
    c->s = size;

    *pa = c - a;  /* Attach new node to pa */
    goto foot;  /* Handle the rest below */


    /* Come here if I encounter a neighbour (or discover
       overlap) after I have started inserting the scratch
       node in the tree. Find the second neighbour, if any
       (and continue checking for overlap). If all is well,
       coalesce the 2 (or 3) pieces, using (and promoting) 
       the node that previously described the first neigh-
       bout (the higher one).  At this point:

       a = addr of the node array
       c = addr of possible neighbour

       pa = addr of `ptr' to scratch node
       pas = the size of the paternal node

       lh,rh = addr of left hook, right hook

       x = addr of piece being deallocated
       z = endad of piece being deallocated         

       addr,size are as supplied by caller          */

  lowneigh:
    *lh = c->l; /* Attach subtrees to hooks */
    *rh = c->r;

    if (c->p == z)
    {           /* If this is right neigh */

        if (*lh != 0)
        {       /* If left subtree exists */
            while ((a + *lh)->r != 0)  /* Find rightmost node */
                lh = &(a + *lh)->r;  /* in the left subtree */
            e = a + *lh;  /* This is the ultimate node */
            if (e->p + e->s > x)  /* Watch for invalid overlap */
                goto recover;
            if (e->p + e->s == x)
            {   /* If this is left neigh */
                x = e->p;  /* Expand node to left */
                *lh = e->l;  /* Remove left neighbour */
                e->l = a->l;  /* And recycle the node */
                a->l = e - a;
            }
        }

        z += c->s;  /* Expand node to the right */

    }

    else if (c->p + c->s == x)
    {           /* If this is left neigh */

        if (*rh != 0)
        {       /* If right subtree exists */
            while ((a + *rh)->l != 0)  /* Find leftmost node */
                rh = &(a + *rh)->l;  /* in right subtree */
            e = a + *rh;  /* This is the ultimate node */
            if (e->p < z)  /* Watch for invalid overlap */
                goto recover;
            if (e->p == z)
            {   /* If this is right neigh */
                z += e->s;  /* Expand node to right */
                *rh = e->r;  /* Remove right neighbour */
                e->l = a->l;  /* And recycle the node */
                a->l = e - a;
            }
        }

        x = c->p;  /* Expand node to the left */

    }

    else        /* Or if invalid overlap */
        goto recover;  /* Handle situation below */

    c->l = scratch.l;  /* Tree ptrs from scratch */
    c->r = scratch.r;

    goto weigh; /* Promote node if needed */


    /* Come here if I encounter a neighbour (or discover
       overlap) before making any alterations to the tree.
       Find the second neighbour, if any (and continue to 
       check for overlap).  If all goes well, coalesce the
       2 (or 3) pieces, using the node that previously de-
       scribed the first neighbour (the higher one).  At
       this pt:

       a = addr of the node array
       c = addr of possible neighbour

       pa = addr of `ptr' to scratch node
       pas = the size of the paternal node

       x = addr of piece being deallocated
       z = endad of piece being deallocated         

       addr,size are as supplied by caller          */

  highneigh:
    if (c->p == z)
    {           /* If this is right neigh */

        if (c->l != 0)
        {       /* If left subtree exists */
            d = &c->l;  /* Find rightmost node ... */
            while ((a + *d)->r != 0)  /* ... in left subtree */
                d = &(a + *d)->r;
            e = a + *d;  /* This is the ultimate node */
            if (e->p + e->s > x)  /* Watch for invalid overlap */
                goto overlap;
            if (e->p + e->s == x)
            {   /* If this is left neigh */
                x = e->p;  /* Expand node to left */
                *d = e->l;  /* Remove left neighbour */
                e->l = a->l;  /* And recycle the node */
                a->l = e - a;
            }
        }

        z += c->s;  /* Expand node to the right */

    }

    else if (c->p + c->s == x)
    {           /* If this is left neigh */

        if (c->r != 0)
        {       /* If right subtree exists */
            d = &c->r;  /* Find leftmost node ... */
            while ((a + *d)->l != 0)  /* ... in right subtree */
                d = &(a + *d)->l;
            e = a + *d;  /* This is the ultimate node */
            if (e->p < z)  /* Watch for invalid overlap */
                goto overlap;
            if (e->p == z)
            {   /* If this is right neigh */
                z += e->s;  /* Expand node to right */
                *d = e->r;  /* Remove right neighbour */
                e->l = a->l;  /* And recycle the node */
                a->l = e - a;
            }
        }

        x = c->p;  /* Expand node to the left */

    }

    else        /* Or if invalid overlap */
        goto overlap;  /* Handle situation below */


    /* Arrive here after combining the deallocated piece
       with one or two neighbours, to see if the combined
       node requires promotion.  At this point:

       a = addr of the node array
       c = addr of the combined node

       pa = addr of `ptr' to combined node
       pas = the size of the paternal node

       x = addr of piece being deallocated
       z = endad of piece being deallocated         

       addr,size are as supplied by caller          */

  weigh:
    c->p = x;   /* Combined addr and size */
    c->s = z - x;

    *pa = c - a;  /* Hang correctly in tree */

    if (c->s > pas)  /* If size exceeds father */
        promote(&a->r, c);  /* Promote combined node */


    /* All successful paths join here.  Keep `almond'
       informed, and return to my caller. On arrival:

       addr,size are as supplied by caller          */

  foot:
    FreeMemCurrent += size;  /* Maintain all the books */

#ifdef BGL
    if (FD_ISSET(BGL_MASK_OV, &BglMask))
        TBglDeath(addr, size);
#endif /* BGL */

    return;     /* And return exhausted */


    /* Come here if I discover invalid overlap after
       starting to insert a node describing the piece
       being deallocated. Promote the overlapping node
       (or the first neighbour, if that is higher) to
       the position occupied by the scratch node, and
       then demote it (again) if necessary. I do not 
       guarantee that the tree will have the same
       shape as before, but I do guarantee that 
       it will be valid.  On arrival:   

       a = the addr of the node array
       c = addr of 1st neighbour, or addr
       of overlapping node, whichever 
       is higher in the tree

       pa = addr of `ptr' to scratch node        */

  recover:
    c->l = scratch.l;  /* Tree ptrs from scratch */
    c->r = scratch.r;

    demote(pa, c);  /* Demote or reattach */


    /* Join here if I discover invalid overlap before
       making any alterations to the tree. Report the
       problem and terminate the entire program.   */

  overlap:
    FEError(-910, EXIT, WRITE,
        "Tierra MemDealloc error: corrupted soup addrs");

    /* Come here if a new memory node is required but there
       is insufficient system memory available.  Remove the 
       scratch node from the tree, report the problem verb-
       ally, and terminate the entire program. At this pt:

       pa = addr of `ptr' to scratch node              */

  bust:
    deletenode(pa, &scratch);  /* Delete the scratch node */

    FEError(-911, EXIT, WRITE,
        "Tierra MemDealloc error: insuf system memory avail");

}

/*  ______________________________________________________
   |                                                      | 
   |  Function deletenode -- Delete a node from the tree  |
   |______________________________________________________|

   Private function for MemAlloc() and MemDealloc()

   Call is:  deletenode (pa,victim)

   where:    pa = addr of parental `ptr' to victim
   victim = addr of the node to be deleted

   Also:     FreeMemry = addr of memory node array   

   Notes:    1.  On entry, the victim need not be
   connected to its paternal node.

   2.  The function deletes the node from
   the tree but it does not chain it
   into the recycle list. This is the
   responsibility of the caller.    */


static void deletenode(pa, victim)
    I32s Hp pa;
    Pmf victim;
{

    Pmf a;                      /* Addr of the memory node array */

    I32s lb, rb;                /* `Ptrs' for left and right branch */

    a = FreeMemry;  /* Addr of node array */
    lb = victim->l;  /* Index of the left son */
    rb = victim->r;  /* Index of the right son */

    while (lb != 0 && rb != 0)  /* While both subtrees exist */
        if ((a + lb)->s >= (a + rb)->s)
        {       /* If left node longer */
            *pa = lb;  /* Attach to my father */
            pa = &(a + lb)->r;  /* Father falls to right */
            lb = *pa;  /* Remaining left subtree */
        }
        else
        {   *pa = rb;
            pa = &(a + rb)->l;
            rb = *pa;
        }

    *pa = lb + rb;  /* Attach surviving subtree [sic] */
    return;

}

/*  ________________________________________________
   |                                                | 
   |  Function demote -- Demote a node in the tree  |
   |________________________________________________|

   Private function for MemAlloc() and MemDealloc()

   Call is:  demote (pa,victim)

   where:    pa = addr of parental `ptr' to victim
   victim = addr of the node to be demoted

   Also:     FreeMemry = addr of memory node array   

   Notes:    1.  On entry, the victim need not be
   connected to its paternal node.

   2.  If the victim is already at the
   correct level, it is simply con-
   nected to the given father.      */


static void demote(pa, victim)
    I32s Hp pa;
    Pmf victim;
{

    Pmf a;                      /* Addr of the memory node array */

    I32s lb, rb;                /* `Ptrs' for left and right branch */

    a = FreeMemry;  /* Addr of the node array */
    lb = victim->l;  /* Index of the left son */
    rb = victim->r;  /* Index of the right son */

    while ((a + lb)->s > victim->s || (a + rb)->s > victim->s)
        /* While among big nodes */
        if ((a + lb)->s >= (a + rb)->s)
        {       /* If left node is longer */
            *pa = lb;  /* Attach it to my father */
            pa = &(a + lb)->r;  /* Father falls to right */
            lb = *pa;  /* Remaining left subtree */
        }
        else
        {   *pa = rb;
            pa = &(a + rb)->l;
            rb = *pa;
        }

    *pa = victim - a;  /* Insert demoted node here */
    victim->l = lb;  /* Set ptrs to both new sons */
    victim->r = rb;
    return;

}

/*  __________________________________________________
   |                                                  | 
   |  Function promote -- Promote a node in the tree  |
   |__________________________________________________|

   Private function for MemAlloc() and MemDealloc()

   Call is:  promote (adam,riser)

   where:    adam = addr of `ptr' to root of tree
   riser = addr of node to be promoted
   (must be somewhere in tree)

   Also:     FreeMemry = addr of memory node array   

   Note:     On entry, the node to be promoted
   must be properly connected in the 
   tree, and must require promotion.    */


static void promote(pa, riser)
    I32s Hp pa;
    Pmf riser;
{

    Pmf a,                      /* Address of the memory node array */
        c;                      /* Addr of current node in old tree */

    I32s Hp lh,                 /* Left hook for root insertion */
        Hp rh,                  /* Right hook for root insertion */
        ls, rs;                 /* Indices for left,right subtree */


    a = FreeMemry;  /* Addr of the node array */
    c = a + *pa;  /* Addr of the tree root */

    /* Perform a passive binary search for the node to be
       promoted until I encounter a node which describes
       an area of soup that is no longer than that des-
       cribed by the node to be promoted.              */

    while (c->s > riser->s)
    {           /* While this size > riser */

        if (c->p < riser->p)  /* Perform a passive search */
            pa = &c->r;
        else
            pa = &c->l;

        c = a + *pa;  /* Addr of next node down */

    }

    /* Insert the rising node at the root of the remaining
       subtree.                                         */

    *pa = riser - a;  /* Attach riser to new pa */
    lh = &riser->l;
    ls = *lh;   /* Init hooks and save old */
    rh = &riser->r;
    rs = *rh;

    while (c != riser)  /* Until reach original riser */

        if (c->p < riser->p)
        {       /* If current node is to left */
            *lh = c - a;  /* Attach node to left hook */
            lh = &c->r;  /* Left hook falls to right */
            c = a + *lh;  /* Addr of next current node */
        }
        else
        {   *rh = c - a;
            rh = &c->l;
            c = a + *rh;
        }

    *lh = ls;
    *rh = rs;   /* Attach both the subtrees */
    return;

}

/*  ____________________________________________________
   |                                                    | 
   |  Function memnode -- Supply an unused memory node  | 
   |____________________________________________________|

   Private function for MemAlloc() and MemDealloc()

   Call is:  b = memnode ()

   where:    b = index of memory node supplied, or zero 
   if unable to supply a new memory node

   Also:     FreeMemry     = addr of memory node array  :  May be changed
   MaxFreeBlocks = size of memory node array  :  as side effect 

   Summary:  Supply a recycled node if available.  Else
   supply an untouched node if available. Else
   increase the size of the array and supply 
   the first untouched node from the new part
   -- or yield a dummy value of zero if the
   array already occupies I32S_MAX bytes or
   more, or if there is insufficient system
   memory available.

   Note:     Callers of this function must relocate any
   private pointers which address data in the 
   node array, in order to compensate for pos-
   sible reallocation of the array.  This re-
   quires peculiar care, owing to the way in 
   which some C compilers implement ptr arith-
   metic.  Consider for example the following 
   situation:

   a = addr of the old array
   b = addr of the new array
   c = addr of an (old) node

   It is tempting to try and relocate `c' by
   writing:

   c = c + (b - a);      [or c += b - a;]

   However, some compilers assume that `a' and
   `b' refer to nodes in the same instance of
   the node array.  They therefore `know' that
   the value of (b-a) must be a multiple of 16
   (the size of a memory node). As a result (it
   appears) they feel justified in clearing the
   four low-order bits, which yields the wrong
   answer unless (a mod 16) happens to equal
   (b mod 16). 

   To avoid this problem, one is tempted to
   write:

   c = b + (c - a);

   This may in fact work (I have not tried it);
   but in principle parentheses in C do not de-
   termine the order of evaluation, so there is
   no guarantee that it will behave any differ-
   ently from the first method.      

   The safest thing to do seems to be to convert 
   all ptrs to offsets before calling newnode(),
   and then convert them back to ptrs afterwards.
   Suppose for example that u is a ptr to a node,
   and v is a ptr to an object of type V *in* a
   node.  Then the following sequence allocates
   a new node w and relocates the old ptrs if
   necessary:

   ou = u - FreeMemry;     Integral offsets 
   ov = v - (V*)FreeMemry;

   ow = memnode();         Allocate new node

   if (ow == 0)            If insuf sys mem
   goto ...;             handle mess below 

   u = FreeMemry + ou;     Updated node ptr
   v = (V*)FreeMemry + ov; Internal ptr too
   w = FreeMemry + ow;     Addr of new node   */


static I32s memnode()
{

    Pmf a;                      /* Addr of memory node array */
    I32s b;                     /* Index of the winning node */

    a = FreeMemry;  /* Addr of the node array */
    b = a->l;   /* Magic for available nodes */

    if (b > 0)
    {           /* If recycled nodes exist */
        a->l = (a + b)->l;  /* Remove 1st node from list */
        return (b);  /* Well that was quite easy */
    }

    if (b < 0)
    {           /* If untouched nodes exist */
        a->l++; /* Reduce the untouched part */
        return (MaxFreeBlocks + b);  /* Tricky, but still easy */
    }

    /* Arrive here if the entire node array is in use.
       Double the size of the array and supply the 1st
       node in the (new) second half.               */

    b = MaxFreeBlocks;  /* Existing array size */

    if (b > I32S_MAX / sizeof(MemFr))  /* If expansion impossible */
        return (0);  /* Supply nothing whatever */

    a = (Pmf) threcalloc((void *) a,  /* Enlarge node array */
        (I32u) (2 * b * sizeof(MemFr)),  /* Desired array size */
        (I32u) (b * sizeof(MemFr)), 1907);  /* Old size of array */

    if (a == NULL)  /* If insuf system memory */
        return (0);  /* Supply nothing whatever */

    FreeMemry = a;  /* Reset the global variables */
    MaxFreeBlocks = 2 * b;

    a->l = b - (2 * b) + 1;  /* -ve index of untouched part */
    return (b); /* Supply 1st node in 2nd half */

}
