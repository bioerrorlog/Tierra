#ifndef _TIEXDRCOM_H_RPCGEN
#define _TIEXDRCOM_H_RPCGEN

#ifndef _WIN32
#include <rpc/rpc.h>
#endif /* _WIN32 */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef TIEXDRCOM_H
#define TIEXDRCOM_H
#include "portable.h"
#ifdef _WIN32
#include "xdr.h"
#endif /* _WIN32 */

struct DynArrCtrl {
	I32s dync_elsize;
	I32s dync_elmused;
	I32s dync_elmaloc;
	I32s dync_indir;
};
typedef struct DynArrCtrl DynArrCtrl;

struct Mem {
	I32s p;
	I32s s;
};
typedef struct Mem Mem;

struct Event {
	I32s m;
	I32s i;
};
typedef struct Event Event;
#endif /* TIEXDRCOM_H */

/* the xdr functions */

#if defined(__STDC__) || defined(__cplusplus) || defined(_WIN32)
extern  bool_t xdr_DynArrCtrl (XDR *, DynArrCtrl*);
extern  bool_t xdr_Mem (XDR *, Mem*);
extern  bool_t xdr_Event (XDR *, Event*);

#else /* K&R C */
extern bool_t xdr_DynArrCtrl ();
extern bool_t xdr_Mem ();
extern bool_t xdr_Event ();

#endif /* K&R C */

#ifdef __cplusplus
}
#endif

#endif /* !_TIEXDRCOM_H_RPCGEN */
