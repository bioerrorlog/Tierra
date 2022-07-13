#ifndef _DATPKT_H_RPCGEN
#define _DATPKT_H_RPCGEN

#ifndef _WIN32
#include <rpc/rpc.h>
#endif /* _WIN32 */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef DATPKT_H
#define DATPKT_H
#include "portable.h"
#ifdef _WIN32
#include "xdr.h"
#endif /* _WIN32 */

struct pkthdr {
	I32u pkth_msgid;
	I32u pkth_msglen;
	I32u pkth_offset;
	I32u pkth_len;
};
typedef struct pkthdr pkthdr;

struct datpkt {
	struct pkthdr dpkt_hdr;
	struct {
		u_int dpkt_data_len;
		xdrwrd *dpkt_data_val;
	} dpkt_data;
};
typedef struct datpkt datpkt;
#endif /* DATPKT_H */

/* the xdr functions */

#if defined(__STDC__) || defined(__cplusplus)
extern  bool_t xdr_pkthdr (XDR *, pkthdr*);
extern  bool_t xdr_datpkt (XDR *, datpkt*);

#else /* K&R C */
extern bool_t xdr_pkthdr ();
extern bool_t xdr_datpkt ();

#endif /* K&R C */

#ifdef __cplusplus
}
#endif

#endif /* !_DATPKT_H_RPCGEN */
