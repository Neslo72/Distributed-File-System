#include <rpc/rpc.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define SERVER_NODE_PROG 1000
#define SERVER_NODE_VERSION 1

int init_rpc_client(CLIENT** clnt, char* host, int port);

void *make_rpc_call(CLIENT* clnt, unsigned long func, void* arg_func, void* args, void* ret_type);

