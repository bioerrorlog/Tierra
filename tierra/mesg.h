#ifndef _MESG_H_RPCGEN
#define _MESG_H_RPCGEN

#ifndef _WIN32
#include <rpc/rpc.h>
#endif /* _WIN32 */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef MESG_H
#define MESG_H
#include "portable.h"
#include "tiexdrcom.h"

enum TagsType {
	NULLTAG = 0,
	TPINGREPTAG = 1,
	TPINGREQTAG = 2,
	SENDADDRTAG = 3,
	EMIGRATETAG = 4,
	BYETAG = 5,
};
typedef enum TagsType TagsType;

struct NetAddr {
	N32u node;
	N16u portnb;
};
typedef struct NetAddr NetAddr;

struct BglTrackFlag {
	N32u node;
	N16u portnb;
	I32s session;
	I8s param[8];
};
typedef struct BglTrackFlag BglTrackFlag;

struct ComAddr {
	struct NetAddr address;
	I32s ranID;
};
typedef struct ComAddr ComAddr;

struct TPingData {
	I32s FecundityAvg;
	I32s Speed;
	I32s NumCells;
	I32s AgeAvg;
	I32s SoupSize;
	I32u TransitTime;
	I32u Fresh;
	I32u Time;
	I32s InstExec;
	I32s InstExecConnect;
	I32s OS;
};
typedef struct TPingData TPingData;

struct TPrivPingData {
	I32s subnet;
	I32s apoctoggle;
	I32s outcapupdtog;
	I32s remoutbndcap;
	I32s avgsize;
};
typedef struct TPrivPingData TPrivPingData;

struct AddrBody {
	struct {
		u_int name_len;
		I8s *name_val;
	} name;
};
typedef struct AddrBody AddrBody;

struct EmigrateBody {
	struct NetAddr dest;
	struct BglTrackFlag BglTrack;
	Mem mg;
	I32s ranID;
#ifdef EXEINSTSIZTIMSLC
	I32s PExeSegSiz;
#endif /* EXEINSTSIZTIMSLC */
	I32u genome_siz;
	struct {
		u_int genome_len;
		xdrwrd *genome_val;
	} genome;
};
typedef struct EmigrateBody EmigrateBody;

struct TPingReqBody {
	struct ComAddr source;
};
typedef struct TPingReqBody TPingReqBody;

struct TPingRepBody {
	struct ComAddr dest;
	struct ComAddr source;
};
typedef struct TPingRepBody TPingRepBody;

struct Data {
	TagsType tag;
	union {
		struct AddrBody Addr;
		struct EmigrateBody Creature;
		struct TPingReqBody TPingReq;
		struct TPingRepBody TPingRep;
	} Data_u;
};
typedef struct Data Data;

struct TMessage {
	Data info;
	I32s dstmap;
	I32s srcmap;
	struct TPingData PingData;
	struct TPrivPingData PrivPingData;
};
typedef struct TMessage TMessage;
#endif /* MESG_H */

/* the xdr functions */

#if defined(__STDC__) || defined(__cplusplus)
extern  bool_t xdr_TagsType (XDR *, TagsType*);
extern  bool_t xdr_NetAddr (XDR *, NetAddr*);
extern  bool_t xdr_BglTrackFlag (XDR *, BglTrackFlag*);
extern  bool_t xdr_ComAddr (XDR *, ComAddr*);
extern  bool_t xdr_TPingData (XDR *, TPingData*);
extern  bool_t xdr_TPrivPingData (XDR *, TPrivPingData*);
extern  bool_t xdr_AddrBody (XDR *, AddrBody*);
extern  bool_t xdr_EmigrateBody (XDR *, EmigrateBody*);
extern  bool_t xdr_TPingReqBody (XDR *, TPingReqBody*);
extern  bool_t xdr_TPingRepBody (XDR *, TPingRepBody*);
extern  bool_t xdr_Data (XDR *, Data*);
extern  bool_t xdr_TMessage (XDR *, TMessage*);

#else /* K&R C */
extern bool_t xdr_TagsType ();
extern bool_t xdr_NetAddr ();
extern bool_t xdr_BglTrackFlag ();
extern bool_t xdr_ComAddr ();
extern bool_t xdr_TPingData ();
extern bool_t xdr_TPrivPingData ();
extern bool_t xdr_AddrBody ();
extern bool_t xdr_EmigrateBody ();
extern bool_t xdr_TPingReqBody ();
extern bool_t xdr_TPingRepBody ();
extern bool_t xdr_Data ();
extern bool_t xdr_TMessage ();

#endif /* K&R C */

#ifdef __cplusplus
}
#endif

#endif /* !_MESG_H_RPCGEN */
