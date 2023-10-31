/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include <memory.h> /* for memset */
#include "server_node.h"

/* Default timeout can be changed using clnt_control() */
static struct timeval TIMEOUT = { 1, 0 };

file_info *
list_hosts_str_1(char *file_name,  CLIENT *clnt)
{
	static file_info clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call (clnt, list_hosts_str,
		(xdrproc_t) xdr_wrapstring, (caddr_t) &file_name,
		(xdrproc_t) xdr_file_info, (caddr_t) &clnt_res,
		TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return (&clnt_res);
}

file_info *
list_hosts_num_1(int fileno,  CLIENT *clnt)
{
	static file_info clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call (clnt, list_hosts_num,
		(xdrproc_t) xdr_int, (caddr_t) &fileno,
		(xdrproc_t) xdr_file_info, (caddr_t) &clnt_res,
		TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return (&clnt_res);
}

void *
check_client_update_1(char *ip, int port,  CLIENT *clnt)
{
	check_client_update_1_argument arg;
	static char clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	arg.ip = ip;
	arg.port = port;
	if (clnt_call (clnt, check_client_update, (xdrproc_t) xdr_check_client_update_1_argument, (caddr_t) &arg,
		(xdrproc_t) xdr_void, (caddr_t) &clnt_res,
		TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return ((void *)&clnt_res);
}

host_info *
find_host_1(char *file_name,  CLIENT *clnt)
{
	static host_info clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call (clnt, find_host,
		(xdrproc_t) xdr_wrapstring, (caddr_t) &file_name,
		(xdrproc_t) xdr_host_info, (caddr_t) &clnt_res,
		TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return (&clnt_res);
}

bool_t *
ping_1(CLIENT *clnt)
{
	static bool_t clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	 if (clnt_call (clnt, Ping, (xdrproc_t) xdr_void, (caddr_t) NULL,
		(xdrproc_t) xdr_bool, (caddr_t) &clnt_res,
		TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return (&clnt_res);
}