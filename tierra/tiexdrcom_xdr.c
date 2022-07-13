#include "tiexdrcom.h"
#include "port_xdr.h"

bool_t
xdr_DynArrCtrl (XDR *xdrs, DynArrCtrl *objp)
{

	 if (!xdr_I32s (xdrs, &objp->dync_elsize))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->dync_elmused))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->dync_elmaloc))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->dync_indir))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_Mem (XDR *xdrs, Mem *objp)
{

	 if (!xdr_I32s (xdrs, &objp->p))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->s))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_Event (XDR *xdrs, Event *objp)
{

	 if (!xdr_I32s (xdrs, &objp->m))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->i))
		 return FALSE;
	return TRUE;
}
