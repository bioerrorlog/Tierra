#define xdr_xdrwrd       xdr_N32u
#ifdef AMIGA
bool_t __regargs xdr_I8s P_((XDR * xdrs, I8s * objp));
bool_t __regargs xdr_I8u P_((XDR * xdrs, I8u * objp));
bool_t __regargs xdr_I16s P_((XDR * xdrs, I16s * objp));
bool_t __regargs xdr_I16u P_((XDR * xdrs, I16u * objp));
bool_t __regargs xdr_I32s P_((XDR * xdrs, I32s * objp));
bool_t __regargs xdr_I32u P_((XDR * xdrs, I32u * objp));
bool_t __regargs xdr_N32u P_((XDR * xdrs, N32u * objp));
bool_t __regargs xdr_N16u P_((XDR * xdrs, N16u * objp));
#else /* AMIGA */
extern bool_t xdr_I8s P_((XDR * xdrs, I8s * objp));
extern bool_t xdr_I8u P_((XDR * xdrs, I8u * objp));
extern bool_t xdr_I16s P_((XDR * xdrs, I16s * objp));
extern bool_t xdr_I16u P_((XDR * xdrs, I16u * objp));
extern bool_t xdr_I32s P_((XDR * xdrs, I32s * objp));
extern bool_t xdr_I32u P_((XDR * xdrs, I32u * objp));
extern bool_t xdr_N32u P_((XDR * xdrs, N32u * objp));
extern bool_t xdr_N16u P_((XDR * xdrs, N16u * objp));
#endif /* AMIGA */
