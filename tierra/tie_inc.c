/* tie_inc.c */
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
