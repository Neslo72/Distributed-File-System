#include "peer_node.h"
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>


struct timeval TIMEOUT = { 1, 0 };

void
peer_node_prog_1(char *host)
{
	CLIENT *clnt;
	file_packet  *result_1;
	char *download_1_file_name;
	int download_1_idx;
	int  *result_2;
	int  *result_3;
	file_update  *result_4;
	int get_update_1_verno;
	int  *result_5;
	bool_t  *result_6;

#ifndef	DEBUG
	clnt = clnt_create (host, PEER_NODE_PROG, PEER_NODE_VERSION, "udp");
	if (clnt == NULL) {
		clnt_pcreateerror (host);
		exit (1);
	}
#endif	/* DEBUG */

	result_1 = download_1(download_1_file_name, download_1_idx, clnt);
	if (result_1 == (file_packet *) NULL) {
		clnt_perror (clnt, "call failed");
	}
	result_2 = update_list_1(clnt);
	if (result_2 == (int *) NULL) {
		clnt_perror (clnt, "call failed");
	}
	result_3 = get_version_1(clnt);
	if (result_3 == (int *) NULL) {
		clnt_perror (clnt, "call failed");
	}
	result_4 = get_update_1(get_update_1_verno, clnt);
	if (result_4 == (file_update *) NULL) {
		clnt_perror (clnt, "call failed");
	}
	result_5 = getload_1(clnt);
	if (result_5 == (int *) NULL) {
		clnt_perror (clnt, "call failed");
	}
	result_6 = ping_1(clnt);
	if (result_6 == (bool_t *) NULL) {
		clnt_perror (clnt, "call failed");
	}
#ifndef	DEBUG
	clnt_destroy (clnt);
#endif	 /* DEBUG */
}


int
main (int argc, char *argv[])
{
	char *host;

	if (argc < 2) {
		printf ("%s <server_ip> <server_port>\n", argv[0]);
		exit (1);
	}
	host = argv[1];
	int server_port = atoi(argv[2]);
	

	//Create RPC client
	CLIENT *clnt;

	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(server_port);
	
	if(inet_pton(AF_INET, host, &serveraddr.sin_addr) == -1) {
		perror("inet_pton");
		return 1;
	}

	int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sockfd == -1) {
		perror("sockfd");
		return 1;
	}

	clnt = clntudp_create(&serveraddr, PEER_NODE_PROG, PEER_NODE_VERSION, TIMEOUT, &sockfd);
	if (clnt == NULL) {
		clnt_pcreateerror (host);
		close(sockfd);
		return 1;
	}

	// DOWNLOAD AND PRINT FILE CONTENTS
	// int idx = 0; 
	// file_packet *contents;
	// int bytes_read = MAXSIZE;
	// while(bytes_read == MAXSIZE) {
	// 	if((contents = download_1("shr/rpc_linux_manual.txt", idx, clnt)) == NULL) {
	// 		printf("Connection to server failed\n");
	// 		clnt_destroy(clnt);
	// 		close(sockfd);
	// 		return 1;
	// 	}
	// 	bytes_read = contents->size;
	// 	printf("%s\n", contents->bytes);
	// 	idx++;
	// }
	// clnt_destroy(clnt);

	int *result;
	result = update_list_1(clnt);
	if(result == NULL || *result == -1) {
		printf("Error updating files in SHR directory\n");
		return 1;
	}
	printf("Server on version %d", *result);
	result = get_version_1(clnt);
	if(result == NULL || *result == -1) {
		printf("Error in version check\n");
		return 1;
	}
	printf(" =? %d\n", *result);
	return 0;
}
