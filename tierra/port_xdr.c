#include "portable.h"

#ifdef DECVAX
#include "multinet_root:[multinet.include.sys]types.h"
#else /* DECVAX */
#include <sys/types.h>
#endif /* DECVAX */

#include <string.h>

#ifdef DECVAX
#include "multinet_root:[multinet.include.netinet]in.h"
#else /* DECVAX */
#if defined(unix)||defined(AMIGA)
#include <netinet/in.h>
#endif /* defined(unix)||defined(AMIGA) */
#endif /* DECVAX */

#ifdef DECVAX
#include "multinet_root:[multinet.include.sys]socket.h"
#else /* DECVAX */
#ifdef _WIN32
#include <winsock.h>
#else /* _WIN32 */
#include <sys/socket.h>
#endif /* _WIN32 */
#ifdef __MWERKS__
#include <machine/endian.h>
#endif /* __MWERKS__ */
#endif /* DECVAX */

#if defined(unix)||defined(AMIGA)
#include <rpc/types.h>
#include <rpc/xdr.h>
#else /* defined(unix)||defined(AMIGA) */
#include "xdr.h"
#endif /* defined(unix)||defined(AMIGA) */

#include "port_xdr.h"

#ifdef AMIGA

/*
 * xdr_I8s - XDR encode/decode function for I8s type
 */
bool_t __regargs xdr_I8s(xdrs, objp)
#else /* AMIGA */
bool_t xdr_I8s(xdrs, objp)
#endif                          /* AMIGA */
    XDR *xdrs;
    I8s *objp;
{   return (xdr_char(xdrs, (char *)objp));
}

/*
 * xdr_I8u - XDR encode/decode function for I8u type
 */
#ifdef AMIGA
bool_t __regargs xdr_I8u(xdrs, objp)
#else /* AMIGA */
bool_t xdr_I8u(xdrs, objp)
#endif                          /* AMIGA */
    XDR *xdrs;
    I8u *objp;
{   return (xdr_u_char(xdrs, objp));
}

/*
 * xdr_I16s - XDR encode/decode function for I16s type
 */
#ifdef AMIGA
bool_t __regargs xdr_I16s(xdrs, objp)
#else /* AMIGA */
bool_t xdr_I16s(xdrs, objp)
#endif                          /* AMIGA */
    XDR *xdrs;
    I16s *objp;
{   return (xdr_short(xdrs, objp));
}

/*
 * xdr_I16u - XDR encode/decode function for I16u type
 */
#ifdef AMIGA
bool_t __regargs xdr_I16u(xdrs, objp)
#else /* AMIGA */
bool_t xdr_I16u(xdrs, objp)
#endif                          /* AMIGA */
    XDR *xdrs;
    I16u *objp;
{   return (xdr_u_short(xdrs, objp));
}

/*
 * xdr_I32s - XDR encode/decode function for I32s type
 */
#ifdef AMIGA
bool_t __regargs xdr_I32s(xdrs, objp)
#else /* AMIGA */
bool_t xdr_I32s(xdrs, objp)
#endif                          /* AMIGA */
    XDR *xdrs;
    I32s *objp;
{   return xdr_int(xdrs, (int *)objp);
}

/*
 * xdr_I32u - XDR encode/decode function for I32u type
 */
#ifdef AMIGA
bool_t __regargs xdr_I32u(xdrs, objp)
#else /* AMIGA */
bool_t xdr_I32u(xdrs, objp)
#endif                          /* AMIGA */
    XDR *xdrs;
    I32u *objp;
{   return (xdr_u_int(xdrs, (unsigned int *)objp));
}

/*
 * xdr_N32u - XDR encode/decode function for N32u type
 */
#ifdef AMIGA
bool_t __regargs xdr_N32u(xdrs, objp)
#else /* AMIGA */
bool_t xdr_N32u(xdrs, objp)
#endif                          /* AMIGA */
    XDR *xdrs;
    N32u *objp;
{   bool_t rtnval;

    *objp = ntohl(*objp);
    rtnval = xdr_I32u(xdrs, (I32u *)objp);
    *objp = htonl(*objp);
    return (rtnval);
}

/*
 * xdr_N16u - XDR encode/decode function for N16u type
 */
#ifdef AMIGA
bool_t __regargs xdr_N16u(xdrs, objp)
#else /* AMIGA */
bool_t xdr_N16u(xdrs, objp)
#endif                          /* AMIGA */
    XDR *xdrs;
    N16u *objp;
{   bool_t rtnval;

    *objp = ntohs(*objp);
    rtnval = xdr_I16u(xdrs, objp);
    *objp = htons(*objp);
    return (rtnval);
}
