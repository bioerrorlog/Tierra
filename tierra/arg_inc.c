/* arg_inc.c 30-3-98                                                       */
/* this file is a "nice" way to make the arg binary easier to make         */
/* by Daniel Pirone March 1, 1992                                          */
/* Tierra Simulator V5.0: Copyright (c) 1991 - 1998                        */

/* Tom Ray & Virtual Life */
#include "license.h"
#include "genio.c"
#include "portable.c"
#include "thrdana.c"
#include "queues.c"
#if defined(MEM_CHECKER)||defined(_WIN32)
#include "xdr_mem.c"
#include "xdr.c"
#include "xdr_array.c"
#include "xdr_float.c"
#endif /* defined(MEM_CHECKER)||defined(_WIN32) */
