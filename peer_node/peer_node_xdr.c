/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include "peer_node.h"

bool_t
xdr_file_packet (XDR *xdrs, file_packet *objp)
{
	register int32_t *buf;

	int i;
	 if (!xdr_int (xdrs, &objp->size))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->bytes, 2048,
		sizeof (char), (xdrproc_t) xdr_char))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_file_update (XDR *xdrs, file_update *objp)
{
	register int32_t *buf;

	int i;
	 if (!xdr_int (xdrs, &objp->mode))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->file_name, 256,
		sizeof (char), (xdrproc_t) xdr_char))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_download_1_argument (XDR *xdrs, download_1_argument *objp)
{
	 if (!xdr_string (xdrs, &objp->file_name, ~0))
		 return FALSE;
	 if (!xdr_int (xdrs, &objp->idx))
		 return FALSE;
	return TRUE;
}
