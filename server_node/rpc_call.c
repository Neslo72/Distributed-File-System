#include "rpc_call.h"


//////////// RPC CALLING FUNCTIONS ////////////

struct timeval CALL_TIMEOUT = { 1, 0 };

// Sets up an RPC client for the server to connect with other RPC servers
// IF USED, destroy client after use with clnt_destroy(clnt)
// Returns 1 on failure, 0 on success
int init_rpc_client(CLIENT** clnt, char* host, int port) 
{
    struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(port);
	
	if(inet_pton(AF_INET, host, &serveraddr.sin_addr) == -1) {
		perror("inet_pton");
		return 1;
	}

	int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sockfd == -1) {
		perror("sockfd");
		return 1;
	}

	*clnt = clntudp_create(&serveraddr, SERVER_NODE_PROG, SERVER_NODE_VERSION, CALL_TIMEOUT, &sockfd);
	if (*clnt == NULL) {
		clnt_pcreateerror (host);
		close(sockfd);
		return 1;
	}
	return 0;
}


// Make the actual RPC call to antoher RPC server.  Code functionality is basically
// what a function call would look like in ~_clnt.c with some interchangeable
// parts added to make the code reusable. func, arg_func, args, and ret_type MUST be setup
// as they are in the ~_clnt.c for separate calls to work.  The CLIENT must also be initialized
// Returns 1 on failure or the result of the RPC call on success
void *make_rpc_call(CLIENT* clnt, unsigned long func, void* arg_func, void* args, void* ret_type) 
{
	static bool_t result;
    memset((char *)&result, 0, sizeof(result));
    if (clnt_call (clnt, func, (xdrproc_t) arg_func, (caddr_t) args,
        (xdrproc_t) ret_type, (caddr_t) &result,
        CALL_TIMEOUT) != RPC_SUCCESS) {
        return (NULL);
    }
    return &result;
}