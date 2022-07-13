#include "datpkt.h"
#include "port_xdr.h"

bool_t
xdr_pkthdr (XDR *xdrs, pkthdr *objp)
{

	 if (!xdr_I32u (xdrs, &objp->pkth_msgid))
		 return FALSE;
	 if (!xdr_I32u (xdrs, &objp->pkth_msglen))
		 return FALSE;
	 if (!xdr_I32u (xdrs, &objp->pkth_offset))
		 return FALSE;
	 if (!xdr_I32u (xdrs, &objp->pkth_len))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_datpkt (XDR *xdrs, datpkt *objp)
{

	 if (!xdr_pkthdr (xdrs, &objp->dpkt_hdr))
		 return FALSE;
	 if (!xdr_array (xdrs, (char **)&objp->dpkt_data.dpkt_data_val, (u_int *) &objp->dpkt_data.dpkt_data_len, ~0,
		sizeof (xdrwrd), (xdrproc_t) xdr_xdrwrd))
		 return FALSE;
	return TRUE;
}
