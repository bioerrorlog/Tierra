#include "mesg.h"
#include "port_xdr.h"

bool_t
xdr_TagsType (XDR *xdrs, TagsType *objp)
{

	 if (!xdr_enum (xdrs, (enum_t *) objp))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_NetAddr (XDR *xdrs, NetAddr *objp)
{

	 if (!xdr_N32u (xdrs, &objp->node))
		 return FALSE;
	 if (!xdr_N16u (xdrs, &objp->portnb))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_BglTrackFlag (XDR *xdrs, BglTrackFlag *objp)
{

	 if (!xdr_N32u (xdrs, &objp->node))
		 return FALSE;
	 if (!xdr_N16u (xdrs, &objp->portnb))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->session))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->param, 8,
		sizeof (I8s), (xdrproc_t) xdr_I8s))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_ComAddr (XDR *xdrs, ComAddr *objp)
{

	 if (!xdr_NetAddr (xdrs, &objp->address))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->ranID))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_TPingData (XDR *xdrs, TPingData *objp)
{

	 if (!xdr_I32s (xdrs, &objp->FecundityAvg))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->Speed))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->NumCells))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->AgeAvg))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->SoupSize))
		 return FALSE;
	 if (!xdr_I32u (xdrs, &objp->TransitTime))
		 return FALSE;
	 if (!xdr_I32u (xdrs, &objp->Fresh))
		 return FALSE;
	 if (!xdr_I32u (xdrs, &objp->Time))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->InstExec))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->InstExecConnect))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->OS))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_TPrivPingData (XDR *xdrs, TPrivPingData *objp)
{

	 if (!xdr_I32s (xdrs, &objp->subnet))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->apoctoggle))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->outcapupdtog))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->remoutbndcap))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->avgsize))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_AddrBody (XDR *xdrs, AddrBody *objp)
{

	 if (!xdr_array (xdrs, (char **)&objp->name.name_val, (u_int *) &objp->name.name_len, 80,
		sizeof (I8s), (xdrproc_t) xdr_I8s))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_EmigrateBody (XDR *xdrs, EmigrateBody *objp)
{

	 if (!xdr_NetAddr (xdrs, &objp->dest))
		 return FALSE;
	 if (!xdr_BglTrackFlag (xdrs, &objp->BglTrack))
		 return FALSE;
	 if (!xdr_Mem (xdrs, &objp->mg))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->ranID))
		 return FALSE;
#ifdef EXEINSTSIZTIMSLC
	 if (!xdr_I32s (xdrs, &objp->PExeSegSiz))
		 return FALSE;
#endif /* EXEINSTSIZTIMSLC */
	 if (!xdr_I32u (xdrs, &objp->genome_siz))
		 return FALSE;
	 if (!xdr_array (xdrs, (char **)&objp->genome.genome_val, (u_int *) &objp->genome.genome_len, ~0,
		sizeof (xdrwrd), (xdrproc_t) xdr_xdrwrd))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_TPingReqBody (XDR *xdrs, TPingReqBody *objp)
{

	 if (!xdr_ComAddr (xdrs, &objp->source))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_TPingRepBody (XDR *xdrs, TPingRepBody *objp)
{

	 if (!xdr_ComAddr (xdrs, &objp->dest))
		 return FALSE;
	 if (!xdr_ComAddr (xdrs, &objp->source))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_Data (XDR *xdrs, Data *objp)
{

	 if (!xdr_TagsType (xdrs, &objp->tag))
		 return FALSE;
	switch (objp->tag) {
	case NULLTAG:
		break;
	case SENDADDRTAG:
		 if (!xdr_AddrBody (xdrs, &objp->Data_u.Addr))
			 return FALSE;
		break;
	case EMIGRATETAG:
		 if (!xdr_EmigrateBody (xdrs, &objp->Data_u.Creature))
			 return FALSE;
		break;
	case TPINGREQTAG:
		 if (!xdr_TPingReqBody (xdrs, &objp->Data_u.TPingReq))
			 return FALSE;
		break;
	case TPINGREPTAG:
		 if (!xdr_TPingRepBody (xdrs, &objp->Data_u.TPingRep))
			 return FALSE;
		break;
	default:
		break;
	}
	return TRUE;
}

bool_t
xdr_TMessage (XDR *xdrs, TMessage *objp)
{

	 if (!xdr_Data (xdrs, &objp->info))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->dstmap))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->srcmap))
		 return FALSE;
	 if (!xdr_TPingData (xdrs, &objp->PingData))
		 return FALSE;
	 if (!xdr_TPrivPingData (xdrs, &objp->PrivPingData))
		 return FALSE;
	return TRUE;
}
