#include "server_node.h"
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <limits.h>


// File and host tracker
#define MAXFILES 128

static int num_files = 0;
static char curr_files[MAXFILES][MAXNAME];
static host_info file_hosts[MAXFILES][MAXHOST];


/////////// HELPER FUNCTIONS ////////////

// Helper function
// Check if the RPC server has an input file tracked
// Returns index of file in curr_files if the file is currently tracked, -1 if not
int is_tracked(char* file_name)
{
	for(int i = 0; i < num_files; i++) {
		int idx = 0;
		while(idx < MAXNAME) {
			if(file_name[idx] != curr_files[i][idx]) {
				break;
			}
			else if(file_name[idx] == '\0' && curr_files[i][idx] == '\0') {
				return i;  // return index of filename in curr_files
			}
			idx++;
		}
	}
	return -1;
}


// Untrack a host from a file
// Also untracks the file if the only host is to be removed
// Returns -1 if no hosts removed, 0 if a host is removed, 1 if file removed
int untrack_file(int fileno, host_info host)
{
	if(fileno == -1) {  // File is not tracked anyways...
		return -1;
	}

	// Get some indicies of things we need
	int del_idx = -1;
	int last_idx = 0;
	for(int i = 0; last_idx < MAXHOST; i++) {
		if(file_hosts[fileno][i].port == host.port
			&& strcmp(file_hosts[fileno][i].ip, host.ip) == 0) {
			del_idx = i;
		}
		if(file_hosts[fileno][i].port == -1) {
			last_idx = i - 1;
			break;
		}
	}

	// Untracking logic
	if(del_idx == -1 || last_idx == -1) {  // File not tracked by host
		return -1;
	}
	else if(del_idx == last_idx) {  // Delete a the last host in the list
		file_hosts[fileno][del_idx].port = -1;
		if(del_idx == 0) {  // Remove the file (no more hosts)
			// Swap the node to delete with the last existing node
			memcpy(file_hosts[fileno], file_hosts[num_files - 1], sizeof(host_info) * MAXHOST);
			strcpy(curr_files[fileno], curr_files[num_files - 1]);
			curr_files[num_files][0] = '\0';  // "Delete" last file
			num_files--;
			return 1;
		}
		return 0;
	}
	else {  // Remove a host in the middle (swap last and host to delete)
		host_info temp = file_hosts[fileno][last_idx];
		file_hosts[fileno][del_idx] = temp;
		file_hosts[fileno][last_idx].port = -1;
		num_files--;
	}
	return 0;
}


//////////// SERVER FUNCTIONS ////////////

// List hosts based on an input file name
// Returns a file_info with name '\0' on error, else the requested file's info
file_info *list_hosts_str_1_svc(char *file_name,  struct svc_req *rqstp)
{
	static file_info info;
	int fileno;
	info.name[0] = '\0';
	if((fileno = is_tracked(file_name)) == -1) {
		return &info;
	}
	strcpy(info.name, curr_files[fileno]);
	memcpy(info.hosts, file_hosts[fileno], sizeof(host_info) * MAXHOST);
	return &info;
}


// List hosts based on a 'fileno' index
// Meant to be called repeatedly by the client until an 'error' is returned
// Returns file with name '\0' on error, else return the requested file's info
file_info *list_hosts_num_1_svc(int fileno,  struct svc_req *rqstp)
{
	static file_info info;
	info.name[0] = '\0';
	if(!(fileno < num_files)) {
		return &info;
	}
	strcpy(info.name, curr_files[fileno]);
	memcpy(info.hosts, file_hosts[fileno], sizeof(host_info) * MAXHOST);
	return &info;
}


// Check for client updates, then 'track' the changes
// Does not return, meant to be called on a time interval by the server...
void *check_client_update_1_svc(char *ip, int port,  struct svc_req *rqstp)
{
	static char * result;  // Do not need to init, void return func

	// Make a test for pinging rn
	CLIENT *clnt;
	if(init_rpc_client(&clnt, ip, port) != 0) {
		printf("Error initializing RPC client\n");
		return (void *) &result;
	}

	int verno;
    // update client list and return version, or untrack all client files if failure
	void* res = make_rpc_call(clnt, update_list, xdr_void, NULL, xdr_int); 
	if(res == NULL) {
        printf("Client %s : %d offline.\n", ip, port);
        host_info host;
        strcpy(host.ip, ip);
		host.port = port;

		// Untrack files from the disconnected server
		int i = 0;
		while(i < num_files) {
			char tmp[MAXNAME];
			strcpy(tmp, curr_files[i]);  // Copy name incase of deletion for printout
			int res = untrack_file(i, host);
			if(res == 0) {
				printf("File %s no longer tracked by host %s : %d\n", tmp, host.ip, host.port);
			}
			else if(res == 1) {
				printf("File %s is no longer tracked by the server.\n", tmp);
				i--;  // File deleted, decrement the loop var
			}
			i++;
		}
		return (void *) &result;
	}
	verno = *((int*) res);  // ugly :)))  Gets the peer's version number

	// TODO: CHANGE i TO TRACKED CLIENT VERSION NO
	// Or leave as is, tracking version would make this more efficient
	for(int i = 0; i < verno; i++) {
		res = make_rpc_call(clnt, get_update, xdr_int, &i, xdr_file_update);
		file_update *upd = ((file_update*) res);

		if(upd->mode == NEW)  // Track a new file (or existing file)
		{
			// New file, the host is for sure gonna get tracked
			host_info host;
			strcpy(host.ip, ip);
			host.port = port;

			int fileno = is_tracked(upd->file_name);
			if(fileno == -1) {  // File new to server
				// Track new file
				strcpy(curr_files[num_files], upd->file_name);
				fileno = num_files;
				num_files++;

				printf("NEW FILE: %s\n", upd->file_name);

				// Invalidate any leftover data in the tracked servers
				for(int i = 0; i < MAXHOST; i++) {
					file_hosts[fileno][i].port = -1;
				}
			}

			// Add server to tracked servers list for corresponding file
			for(int i = 0; i < MAXHOST; i++) {
				if(file_hosts[fileno][i].port == port
					&& strcmp(file_hosts[fileno][i].ip, ip) == 0) {
					//printf("Host is already tracking %s\n", upd->file_name);
					break;
				}
				else if(file_hosts[fileno][i].port == -1) {
					file_hosts[fileno][i] = host;
					//printf("File %s tracked on %s : %d\n", upd->file_name, ip, port);
					break;
				}
			}
		}

		else if(upd->mode == DEL)  // Untrack a host (or even a file)
		{
			host_info host;
			strcpy(host.ip, ip);
			host.port = port;

			int fileno = is_tracked(upd->file_name);
			int res = untrack_file(fileno, host);
			if(res == -1) {
				printf("File %s was not tracked and cannot be untracked\n", upd->file_name);
			}
			else if(res == 0) {
				printf("File %s no longer tracked by host %s : %d\n", upd->file_name, host.ip, host.port);
			}
			else if(res == 1) {
				printf("File %s is no longer tracked by the server.  All hosts removed\n", upd->file_name);
			}
		}
		// Possibly add for UPD
	}

	printf("Tracked files: ");
	for(int i = 0; i < num_files; i++) {
		printf(" %s, ", curr_files[i]);
	}
	printf("\n");
	return (void *) &result;
}


// Find the most efficient connection for a client to connect to
// Returns a host with port = -1 on error, else proper host info
host_info *find_host_1_svc(char *file_name,  struct svc_req *rqstp)
{

	static host_info min_info;
	int fileno;
	int minLoadScore = INT_MAX;
	min_info.port = -1;
	if((fileno = is_tracked(file_name)) == -1) {
		return &min_info;
	}

	// Determine the best host for a client to download from
	host_info info;
	int i = 0;
	for (int i = 0; i < MAXHOST; i++) {
		info = file_hosts[fileno][i];

		// if info doesn't exist, exit
		if (info.port == -1)
			break;

		// init RPC client to get load
		CLIENT *clnt;
		if(init_rpc_client(&clnt, info.ip, info.port) != 0) {
			printf("Error initializing RPC client\n");
			return &min_info;
		}

		int loadScore;
		void* res = make_rpc_call(clnt, getLoad, xdr_void, NULL, xdr_int);
		if(res == NULL) {
			printf("Failed RPC call\n");
			return &min_info;
		}
		loadScore = *((int*) res);

		// update minimum load
		if (loadScore < minLoadScore) {
			min_info = info;
			minLoadScore = loadScore;
		}

	}

	return &min_info;
}


// Ping
bool_t *ping_1_svc(struct svc_req *rqstp)
{
	static bool_t  result;
	result = 1;
	// printf("Pinged!\n");
	return &result;
}
