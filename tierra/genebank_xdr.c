#include "genebank.h"
#include "port_xdr.h"

bool_t
xdr_head_t (XDR *xdrs, head_t *objp)
{

	 if (!xdr_I32s (xdrs, &objp->g_off))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->size))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->n))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->n_alloc))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->n_thread))
		 return FALSE;
	 if (!xdr_double (xdrs, &objp->hdsucsiznslrat))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->hdsvsiznsl))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->hdsvsucsiznsl))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_GBGenome (XDR *xdrs, GBGenome *objp)
{

	 if (!xdr_array (xdrs, (char **)&objp->array.array_val, (u_int *) &objp->array.array_len, ~0,
		sizeof (xdrwrd), (xdrproc_t) xdr_xdrwrd))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_GBGenBits (XDR *xdrs, GBGenBits *objp)
{

	 if (!xdr_array (xdrs, (char **)&objp->array.array_val, (u_int *) &objp->array.array_len, ~0,
		sizeof (xdrwrd), (xdrproc_t) xdr_xdrwrd))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_Metabolism (XDR *xdrs, Metabolism *objp)
{

	 if (!xdr_I32s (xdrs, &objp->inst))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->instP))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->flags))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->mov_daught))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->BreedTrue))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_indx_t (XDR *xdrs, indx_t *objp)
{

	 if (!xdr_vector (xdrs, (char *)objp->gen, 4,
		sizeof (I8s), (xdrproc_t) xdr_I8s))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->pgen, 4,
		sizeof (I8s), (xdrproc_t) xdr_I8s))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->psize))
		 return FALSE;
	 if (!xdr_Mem (xdrs, &objp->mg))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->hash))
		 return FALSE;
	 if (!xdr_I32u (xdrs, &objp->bits))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->originC))
		 return FALSE;
	 if (!xdr_Event (xdrs, &objp->originI))
		 return FALSE;
	 if (!xdr_Event (xdrs, &objp->mppT))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->mpp))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->mpi))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->MaxPop))
		 return FALSE;
	 if (!xdr_Metabolism (xdrs, &objp->d1))
		 return FALSE;
	 if (!xdr_Metabolism (xdrs, &objp->d2))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->pt))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->max_cpus))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->g_off))
		 return FALSE;
#ifdef NET
	 if (!xdr_double (xdrs, &objp->ixsucgennslrat))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->ixsvgennsl))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->ixsvsucgennsl))
		 return FALSE;
#endif /* NET */
	return TRUE;
}

bool_t
xdr_GBindx_t (XDR *xdrs, GBindx_t *objp)
{

	 if (!xdr_array (xdrs, (char **)&objp->array.array_val, (u_int *) &objp->array.array_len, ~0,
		sizeof (indx_t), (xdrproc_t) xdr_indx_t))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_BaseThrdAnaDat (XDR *xdrs, BaseThrdAnaDat *objp)
{

	 if (!xdr_I32s (xdrs, &objp->btad_extrnterm))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->btad_termcause))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->btad_termthrdid))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->btad_termInstP))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->btad_FilInstrThrd_ct))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->btad_CalLvlIntByID_ct))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->btad_callvlintcnt))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_FilThrdAnaDat (XDR *xdrs, FilThrdAnaDat *objp)
{

	 if (!xdr_DynArrCtrl (xdrs, &objp->ftad_thdatara_ctrl))
		 return FALSE;
	 if (!xdr_DynArrCtrl (xdrs, &objp->ftad_cpysegbti_ctrl))
		 return FALSE;
	 if (!xdr_DynArrCtrl (xdrs, &objp->ftad_codsegbti_ctrl))
		 return FALSE;
	 if (!xdr_DynArrCtrl (xdrs, &objp->ftad_event_ctrl))
		 return FALSE;
	 if (!xdr_DynArrCtrl (xdrs, &objp->ftad_soupupdevt_ctrl))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->ftad_rtcalvlint))
		 return FALSE;
	 if (!xdr_BaseThrdAnaDat (xdrs, &objp->ftad_base))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_BaseCallLvlIntrv (XDR *xdrs, BaseCallLvlIntrv *objp)
{

	 if (!xdr_I32s (xdrs, &objp->clb_id))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->clb_parentid))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->clb_celcallvl))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->clb_thdcallvl))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->clb_entinp))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->clb_enthipofsttype))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->clb_enthipofst))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->clb_extinp))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->clb_exthipofsttype))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->clb_exthipofst))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->clb_prom))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->clb_subintrvcnt))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_BasThreadDat (XDR *xdrs, BasThreadDat *objp)
{

	 if (!xdr_I32s (xdrs, &objp->tdt_dght1bytcpd))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->tdt_dght1instex))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->tdt_dght2bytcpd))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->tdt_dght2instex))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->tdt_instex))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->tdt_fstinstex))
		 return FALSE;
#ifdef HAVE_TDT_FST_LST_OFSTTYP
	 if (!xdr_I32s (xdrs, &objp->tdt_fstofsttyp))
		 return FALSE;
#endif /* HAVE_TDT_FST_LST_OFSTTYP */
	 if (!xdr_I32s (xdrs, &objp->tdt_lstinstex))
		 return FALSE;
#ifdef HAVE_TDT_FST_LST_OFSTTYP
	 if (!xdr_I32s (xdrs, &objp->tdt_lstofsttyp))
		 return FALSE;
#endif /* HAVE_TDT_FST_LST_OFSTTYP */
	 if (!xdr_I32s (xdrs, &objp->tdt_locrepl))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->tdt_nlocrepl))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->tdt_parent))
		 return FALSE;
	 if (!xdr_I32u (xdrs, &objp->tdt_bits))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->tdt_cpu))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->tdt_termcause))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->tdt_fstInstP))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->tdt_lstInstP))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_IdxThreadDat (XDR *xdrs, IdxThreadDat *objp)
{

	 if (!xdr_I32s (xdrs, &objp->xtdt_threadid))
		 return FALSE;
	 if (!xdr_BasThreadDat (xdrs, &objp->xtdt_thrdat))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_ThrdAnaEvent (XDR *xdrs, ThrdAnaEvent *objp)
{

	 if (!xdr_I8u (xdrs, &objp->taev_eventcode))
		 return FALSE;
	 if (!xdr_I8u (xdrs, &objp->taev_ipoffsettype))
		 return FALSE;
	 if (!xdr_I32u (xdrs, &objp->taev_ipoffset))
		 return FALSE;
	 if (!xdr_I32u (xdrs, &objp->taev_thrd))
		 return FALSE;
	 if (!xdr_I8u (xdrs, &objp->taev_dstoffsettype))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->taev_dstoffset))
		 return FALSE;
	 if (!xdr_N32u (xdrs, &objp->taev_i32a))
		 return FALSE;
#ifdef USE_PORT
	 if (!xdr_N16u (xdrs, &objp->taev_port))
		 return FALSE;
#endif /* USE_PORT */
	 if (!xdr_I8u (xdrs, &objp->taev_nodetype))
		 return FALSE;
	 if (!xdr_I8u (xdrs, &objp->taev_genetrn))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->taev_instP))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_SoupUpdEvent (XDR *xdrs, SoupUpdEvent *objp)
{

	 if (!xdr_I8u (xdrs, &objp->supd_eventcode))
		 return FALSE;
	 if (!xdr_I8u (xdrs, &objp->supd_ipoffsettype))
		 return FALSE;
	 if (!xdr_I8u (xdrs, &objp->supd_extrk))
		 return FALSE;
	 if (!xdr_I32u (xdrs, &objp->supd_ipoffset))
		 return FALSE;
	 if (!xdr_I32u (xdrs, &objp->supd_thrd))
		 return FALSE;
	 if (!xdr_I8u (xdrs, &objp->supd_dstoffsettype))
		 return FALSE;
	 if (!xdr_I8u (xdrs, &objp->supd_dtra))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->supd_dstoffset))
		 return FALSE;
	 if (!xdr_I8u (xdrs, &objp->supd_srcoffsettype))
		 return FALSE;
	 if (!xdr_I8u (xdrs, &objp->supd_stra))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->supd_srcoffset))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->supd_instP))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_BasGenThrdDat (XDR *xdrs, BasGenThrdDat *objp)
{

	 if (!xdr_I32s (xdrs, &objp->gdt_opcnt))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->gdt_dght1opcnt))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->gdt_dght2opcnt))
		 return FALSE;
	 if (!xdr_I8s (xdrs, &objp->gdt_gbits))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_FilGenThrdDat (XDR *xdrs, FilGenThrdDat *objp)
{

	 if (!xdr_I32s (xdrs, &objp->xgdt_bytoffst))
		 return FALSE;
	 if (!xdr_DynArrCtrl (xdrs, &objp->xgdt_thrd_ara_ctrl))
		 return FALSE;
	 if (!xdr_BasGenThrdDat (xdrs, &objp->xgdt_gbdt))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_BasInstrThrd (XDR *xdrs, BasInstrThrd *objp)
{

	 if (!xdr_I32s (xdrs, &objp->ith_nonprom))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->ith_prom))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_FilInstrThrd (XDR *xdrs, FilInstrThrd *objp)
{

	 if (!xdr_I32s (xdrs, &objp->fith_threadid))
		 return FALSE;
	 if (!xdr_BasInstrThrd (xdrs, &objp->fith_base))
		 return FALSE;
	 if (!xdr_DynArrCtrl (xdrs, &objp->fith_calvlint_ctrl))
		 return FALSE;
	return TRUE;
}
