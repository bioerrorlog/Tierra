#include "bgl_dat_xdr.h"
#include "port_xdr.h"
#include "tierra.h"
#include "bgl_define.h"

bool_t
xdr_BglConnectSt (XDR *xdrs, BglConnectSt *objp)
{

	 if (!xdr_vector (xdrs, (char *)objp->host, 64,
		sizeof (I8s), (xdrproc_t) xdr_I8s))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->dotted, 64,
		sizeof (I8s), (xdrproc_t) xdr_I8s))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->ver, BGL_MAX_VERSION_LEN,
		sizeof (I8s), (xdrproc_t) xdr_I8s))
		 return FALSE;
	 if (!xdr_I8s (xdrs, &objp->mode))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->name, BGL_MAX_USERNAME_LEN,
		sizeof (I8s), (xdrproc_t) xdr_I8s))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->passwd, BGL_MAX_PASSWD_LEN,
		sizeof (I8s), (xdrproc_t) xdr_I8s))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_BglQuitSt (XDR *xdrs, BglQuitSt *objp)
{

	 if (!xdr_I32s (xdrs, &objp->state))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_BglMessageSt (XDR *xdrs, BglMessageSt *objp)
{

	 if (!xdr_vector (xdrs, (char *)objp->message, BGL_MAX_MESSAGE_LEN,
		sizeof (I8s), (xdrproc_t) xdr_I8s))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_BglMCStateSt (XDR *xdrs, BglMCStateSt *objp)
{

	 if (!xdr_I8s (xdrs, &objp->MC_step))
		 return FALSE;
	 if (!xdr_I8s (xdrs, &objp->Slice))
		 return FALSE;
	 if (!xdr_I8s (xdrs, &objp->Cpu))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_BglStatsSt (XDR *xdrs, BglStatsSt *objp)
{

	 if (!xdr_I32s (xdrs, &objp->GeneBnker))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->InstExe_m))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->InstExe_i))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->NumCells))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->NumGenotypes))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->NumSizes))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->ExtrG_gen, BGL_EXTR_G_LEN,
		sizeof (I8s), (xdrproc_t) xdr_I8s))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_BglPlanSt (XDR *xdrs, BglPlanSt *objp)
{

	 if (!xdr_I32s (xdrs, &objp->GeneBnker))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->InstExe_m))
		 return FALSE;
	 if (!xdr_double (xdrs, &objp->Generations))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->tp))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->NumCells))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->NumGenotypes))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->NumSizes))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->Speed))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->RateMut))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->RateMovMut))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->RateFlaw))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->AverageSize))
		 return FALSE;
	 if (!xdr_double (xdrs, &objp->AvgPop))
		 return FALSE;
	 if (!xdr_double (xdrs, &objp->FecundityAvg))
		 return FALSE;
	 if (!xdr_double (xdrs, &objp->RepInstEff))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->FEBirthNum))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->FEBirthLocal))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->FEBirthInject))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->FEBirthEject))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->FEDeathNum))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->FEFecundYesSum))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->FEFecunZeroSum))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->FEFecunEjecSum))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->MaxPop))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->MaxGenPop_size))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->MaxGenPop_label, 4,
		sizeof (I8s), (xdrproc_t) xdr_I8s))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->MaxMemry))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->MaxGenMem_size))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->MaxGenMem_label, 4,
		sizeof (I8s), (xdrproc_t) xdr_I8s))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->NumGenDG))
		 return FALSE;
	 if (!xdr_I8s (xdrs, &objp->NetFlg))
		 return FALSE;
	 if (!xdr_I32u (xdrs, &objp->NumNodes))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->FESImmigraNum))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->FERImmigraNum))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->FEEjectToSelf))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->FESPingReqNum))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->FESPingRepNum))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->FESSendAddNum))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->FESByeNum))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->FERPingReqNum))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->FERPingRepNum))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->FERSendAddNum))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->FERByeNum))
		 return FALSE;
	 if (!xdr_I8s (xdrs, &objp->MemProFlg))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->DynMemUse))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->TotNumAloc))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_BglHistClearSt (XDR *xdrs, BglHistClearSt *objp)
{

	 if (!xdr_I8s (xdrs, &objp->flg))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_BglSizHistSt (XDR *xdrs, BglSizHistSt *objp)
{

	 if (!xdr_I32s (xdrs, &objp->index))
		 return FALSE;
	 if (!xdr_I32u (xdrs, &objp->size))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->num))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->count))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_BglGenHistSt (XDR *xdrs, BglGenHistSt *objp)
{

	 if (!xdr_I32s (xdrs, &objp->index))
		 return FALSE;
	 if (!xdr_I32u (xdrs, &objp->size))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->lbl, 4,
		sizeof (I8s), (xdrproc_t) xdr_I8s))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->count))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_BglMemHistSt (XDR *xdrs, BglMemHistSt *objp)
{

	 if (!xdr_I32s (xdrs, &objp->index))
		 return FALSE;
	 if (!xdr_I32u (xdrs, &objp->size))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->num))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->count))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_BglEfficSHistSt (XDR *xdrs, BglEfficSHistSt *objp)
{

	 if (!xdr_I32s (xdrs, &objp->index))
		 return FALSE;
	 if (!xdr_I32u (xdrs, &objp->size))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->num))
		 return FALSE;
	 if (!xdr_double (xdrs, &objp->dblcount))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_BglEfficGHistSt (XDR *xdrs, BglEfficGHistSt *objp)
{

	 if (!xdr_I32s (xdrs, &objp->index))
		 return FALSE;
	 if (!xdr_I32u (xdrs, &objp->size))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->lbl, 4,
		sizeof (I8s), (xdrproc_t) xdr_I8s))
		 return FALSE;
	 if (!xdr_double (xdrs, &objp->dblcount))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_BglSizQueryOutputSt (XDR *xdrs, BglSizQueryOutputSt *objp)
{

	 if (!xdr_I32s (xdrs, &objp->count))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->lbl, 4,
		sizeof (I8s), (xdrproc_t) xdr_I8s))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->moves))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->flags))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->bits))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->csS))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->WatchBits, BGL_MAX_WATCH_BITS,
		sizeof (I8s), (xdrproc_t) xdr_I8s))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_BglVarSt (XDR *xdrs, BglVarSt *objp)
{

	 if (!xdr_I32s (xdrs, &objp->index))
		 return FALSE;
	 if (!xdr_I8s (xdrs, &objp->type))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->name, BGL_MAX_VAR_LEN,
		sizeof (I8s), (xdrproc_t) xdr_I8s))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->value, BGL_MAX_VAR_LEN,
		sizeof (I8s), (xdrproc_t) xdr_I8s))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_BglInjectGeneSt (XDR *xdrs, BglInjectGeneSt *objp)
{

	 if (!xdr_vector (xdrs, (char *)objp->gene, 8,
		sizeof (I8s), (xdrproc_t) xdr_I8s))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->num))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->addr))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_BglEventSt (XDR *xdrs, BglEventSt *objp)
{

	 if (!xdr_I32s (xdrs, &objp->m))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->i))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_BrkPointSt (XDR *xdrs, BrkPointSt *objp)
{

	 if (!xdr_I8s (xdrs, &objp->type))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->gene, BGL_EXTR_G_LEN,
		sizeof (I8s), (xdrproc_t) xdr_I8s))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->parasite, BGL_EXTR_G_LEN,
		sizeof (I8s), (xdrproc_t) xdr_I8s))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->offset))
		 return FALSE;
	 if (!xdr_BglEventSt (xdrs, &objp->time))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_BglTPingSt (XDR *xdrs, BglTPingSt *objp)
{

	 if (!xdr_I8s (xdrs, &objp->mode))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->id))
		 return FALSE;
	 if (!xdr_N32u (xdrs, &objp->node))
		 return FALSE;
	 if (!xdr_N16u (xdrs, &objp->portnb))
		 return FALSE;
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
xdr_BglOvFrameSt (XDR *xdrs, BglOvFrameSt *objp)
{

	 if (!xdr_I32s (xdrs, &objp->InstNum))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->SoupSize))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->NumCells))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->NumSizes))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->NumGenotypes))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_BglOvGInstSt (XDR *xdrs, BglOvGInstSt *objp)
{

	 if (!xdr_I32u (xdrs, &objp->op))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->mn, BGL_OV_G_INST_LEN,
		sizeof (I8s), (xdrproc_t) xdr_I8s))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_BglOvMesSt (XDR *xdrs, BglOvMesSt *objp)
{

	 if (!xdr_I8s (xdrs, &objp->event))
		 return FALSE;
	 if (!xdr_I32u (xdrs, &objp->start))
		 return FALSE;
	 if (!xdr_I32u (xdrs, &objp->length))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_BglReplySt (XDR *xdrs, BglReplySt *objp)
{

	 if (!xdr_I32s (xdrs, &objp->ack))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_BglReplyMesSt (XDR *xdrs, BglReplyMesSt *objp)
{

	 if (!xdr_vector (xdrs, (char *)objp->mes, BGL_MAX_MESSAGE_LEN,
		sizeof (I8s), (xdrproc_t) xdr_I8s))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_BglMigrationConnectSt (XDR *xdrs, BglMigrationConnectSt *objp)
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
xdr_BglMigrationDataSt (XDR *xdrs, BglMigrationDataSt *objp)
{

	 if (!xdr_N32u (xdrs, &objp->node))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->localparameter, 8,
		sizeof (I8s), (xdrproc_t) xdr_I8s))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->storelparam))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->session))
		 return FALSE;
	 if (!xdr_I8s (xdrs, &objp->information))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->ranID))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->tierraTime))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->beagleTime))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->fecundity))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_SaveCodeSt (XDR *xdrs, SaveCodeSt *objp)
{

	 if (!xdr_vector (xdrs, (char *)objp->filename, BGL_MAX_LETTERS,
		sizeof (I8s), (xdrproc_t) xdr_I8s))
		 return FALSE;
	 if (!xdr_I8s (xdrs, &objp->mode))
		 return FALSE;
	 if (!xdr_I8s (xdrs, &objp->curorig))
		 return FALSE;
	 if (!xdr_I8s (xdrs, &objp->tarpt))
		 return FALSE;
	 if (!xdr_I8s (xdrs, &objp->detailrpt))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_BglOvGOutputSt (XDR *xdrs, BglOvGOutputSt *objp)
{

	 if (!xdr_vector (xdrs, (char *)objp->comment, BGL_OV_G_COMMENT_LEN,
		sizeof (I8s), (xdrproc_t) xdr_I8s))
		 return FALSE;
	 if (!xdr_I8s (xdrs, &objp->type))
		 return FALSE;
	 if (!xdr_I32u (xdrs, &objp->start))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->label, 4,
		sizeof (I8s), (xdrproc_t) xdr_I8s))
		 return FALSE;
	 if (!xdr_I8s (xdrs, &objp->difforig))
		 return FALSE;
	 if (!xdr_I32u (xdrs, &objp->genome_siz))
		 return FALSE;
	 if (!xdr_array (xdrs, (char **)&objp->genome.genome_val, (u_int *) &objp->genome.genome_len, ~0,
		sizeof (xdrwrd), (xdrproc_t) xdr_xdrwrd))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_BglMCSpySt (XDR *xdrs, BglMCSpySt *objp)
{

	 if (!xdr_I32s (xdrs, &objp->Ploidy))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->MinCellSize))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->Stack_size))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->Numreg))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->cp_md_p))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->cp_md_s))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->cp_q_this_a))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->cp_q_this_i))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->cp_mm_s))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->lbl, 4,
		sizeof (I8s), (xdrproc_t) xdr_I8s))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->soup))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->id, BGL_MICRO_SPY_ID_LEN_P_1,
		sizeof (I8s), (xdrproc_t) xdr_I8s))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->cp_mm_p))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->cp_c_ib))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->cp_c_c_st, BGL_MICRO_SPY_ST_N,
		sizeof (I32s), (xdrproc_t) xdr_I32s))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->cp_c_c_re, BGL_MICRO_SPY_RE_N,
		sizeof (I32s), (xdrproc_t) xdr_I32s))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->cp_c_c_sp))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->cp_c_c_ip))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->cp_c_c_threadid))
		 return FALSE;
	 if (!xdr_I8s (xdrs, &objp->off_char))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->off_int))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->cp_c_c_fl_E))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->d_data, BGL_MICRO_SPY_DATA_LEN,
		sizeof (I8s), (xdrproc_t) xdr_I8s))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->cp_c_c_ex))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->cp_c_c_so))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->cp_c_c_de))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->cp_c_ac))
		 return FALSE;
	 if (!xdr_I32s (xdrs, &objp->cp_c_n))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->regcol0, BGL_MICRO_SPY_NUMSPECREG_P_1,
		sizeof (I8s), (xdrproc_t) xdr_I8s))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->regcol1, BGL_MICRO_SPY_NUMSPECREG_P_1,
		sizeof (I8s), (xdrproc_t) xdr_I8s))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->regcol2, BGL_MICRO_SPY_NUMSPECREG_P_1,
		sizeof (I8s), (xdrproc_t) xdr_I8s))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->regcol3, BGL_MICRO_SPY_NUMSPECREG_P_1,
		sizeof (I8s), (xdrproc_t) xdr_I8s))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->regcol4, BGL_MICRO_SPY_NUMSPECREG_P_1,
		sizeof (I8s), (xdrproc_t) xdr_I8s))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->regcol5, BGL_MICRO_SPY_NUMSPECREG_P_1,
		sizeof (I8s), (xdrproc_t) xdr_I8s))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->hostbuf1, 30,
		sizeof (I8s), (xdrproc_t) xdr_I8s))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->hostbuf2, 30,
		sizeof (I8s), (xdrproc_t) xdr_I8s))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->hostbuf3, 30,
		sizeof (I8s), (xdrproc_t) xdr_I8s))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->hostbuf4, 30,
		sizeof (I8s), (xdrproc_t) xdr_I8s))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->hostbuf5, 30,
		sizeof (I8s), (xdrproc_t) xdr_I8s))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->hostbuf6, 30,
		sizeof (I8s), (xdrproc_t) xdr_I8s))
		 return FALSE;
	return TRUE;
}
