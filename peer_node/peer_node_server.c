#include "peer_node.h"
#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>

// Version tracker
#define MAXVERSION 128

static int verno = 0;
static file_update version[MAXVERSION];


// File tracker
#define MAXFILES 128

static int num_files = 0;
static char curr_files[MAXFILES][MAXNAME];

// Load index
static int loadIdx = 0;

// generate checksum character from data
unsigned char checksum(char *ptr, int size) {
    char * iterate; // first copy the pointer to not change the original
    unsigned char checksum = 0;

    for (iterate = ptr; size-- > 0; iterate++) {
        checksum ^= *iterate;
    }
    return checksum;
}


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


// Download a section of a file offset from the start by idx
// To be called repeatedly by the client to get an entire file
// Returns a file_packet with size -1 on error
file_packet *download_1_svc(char *file_name, int idx,  struct svc_req *rqstp)
{
	static file_packet contents;

	// setup for errors / issues
	contents.size = -1;
	contents.bytes[0] = 'E';

	char shr_file[MAXNAME + 4];
	strcpy(shr_file, "shr/");
	strcpy(&shr_file[4], file_name);

	// if at start of file, increment loadIdx
	if (idx == 0)
		loadIdx++;

	//open client file
	FILE *file = fopen(shr_file, "rb");
	if(file == NULL) {
		snprintf(contents.bytes, MAXSIZE, "fopen: %s\n", strerror(errno));
		loadIdx--;
		return &contents;
	}

	// seek to index offset
	if(fseek(file, idx * (MAXSIZE - 1), SEEK_SET) < 0) {
		snprintf(contents.bytes, MAXSIZE, "fseek: %s\n", strerror(errno));
		loadIdx--;
		return &contents;
	}

	// read file contents to buffer
	char buf[MAXSIZE];
	if((contents.size = fread(buf + 1, 1, MAXSIZE - 1, file)) < 0) {
		snprintf(contents.bytes, MAXSIZE, "fread: %s\n", strerror(errno));
		loadIdx--;
		return &contents;
	}

	// write checksum to last char of buffer
	char checksum_res = checksum(buf + 1, contents.size);
	buf[0] = checksum_res;
	contents.size++;

	// wait the latency amount (latency is in millisec) (have to cast to long otherwise overflow)
    struct timespec t, t2;
    t.tv_sec = latency / 100;
    t.tv_nsec = (( (long) latency * 1000000) % 1000000000);

	if (nanosleep(&t, &t2) < 0) {
		contents.size = -1;
		snprintf(contents.bytes, MAXSIZE, "usleep: %s\n", strerror(errno));
		loadIdx--;
		return &contents;
	}

	// if size is less than MAXSIZE, we are at the end so decrement loadIdx
	if (contents.size < MAXSIZE)
		loadIdx--;

	// close the file, copy the contents, and return
	if(fclose(file) != 0) {
		contents.size = -1;
		snprintf(contents.bytes, MAXSIZE, "fclose: %s\n", strerror(errno));
		return &contents;
	}
	memcpy(&contents.bytes, buf, MAXSIZE);
	return &contents;
}


// Called by the client program to prompt the server to search for changes
// Returns the version number on success, else -1 on failure
int *update_list_1_svc(struct svc_req *rqstp)
{
	static int result = -1;  //setup for error

	// open the shared directory
	DIR* shr = opendir("shr");
	if(shr == NULL) {
		perror("opendir");
		return &result;
	}

	// Can probably do stat(dir) and check the number of files
	// In the directory to optimize this function ...
	// Compare num stated files vs previously known files

	// Read entries
	errno = 0;
	struct dirent* entry;
	while(1) {
		entry = readdir(shr);
		if(entry == NULL) {
			break;
		}

		// Add new files to the version array
		// ACCOUNT FOR NEW FILES / DELETED FILES / CHANGES? ...
		// ALSO DOES NOT SUPPORT SUBDIRECTORIES RN
		if(entry->d_type == DT_REG) {
			if(is_tracked(&entry->d_type) == -1) {
				printf("NEW FILE: %s\n", entry->d_name);

				// Track the new file
				//curr_files[num_files] = strdup(&entry->d_type);
				strcpy(curr_files[num_files], &entry->d_type);
				num_files++;

				// Add to verison tracker
				file_update temp;
				temp.mode = NEW;
				strcpy(temp.file_name, entry->d_name);
				version[verno % MAXVERSION] = temp;
				verno++;
			}
			// If more time, somehow track new changes too?
		}
	}
	if(errno != 0) {
		perror("readdir");
		return &result;
	}

	// close the directory and exit
	if(closedir(shr) != 0) {
		perror("closedir");
		return &result;
	}
	result = verno;
	return &result;
}

// Return the current version number
// To be paired with get_update
int *get_version_1_svc(struct svc_req *rqstp)
{
	static int  result;
	result = verno;
	return &result;
}

// Return the file_update corresponding to the version number
// the file_update includes the name of the file and the type of change made to it
file_update *get_update_1_svc(int req_verno,  struct svc_req *rqstp)
{
	static file_update update;
	update = version[req_verno % MAXVERSION];
	return &update;
}

// CURRENTLY RETURNS LOAD SCORE ((load index + 1) * latency)
// MAY NEED CHANGE TO RETURN JUST LOAD INDEX
int *getload_1_svc(struct svc_req *rqstp)
{
	static int result;
	result = (loadIdx + 1) * latency;

	/*
	 * insert server code here
	 */

	return &result;
}

bool_t *ping_1_svc(struct svc_req *rqstp)
{
	static bool_t  result;
	result = 1;
	//printf("Pinged!\n");
	return &result;
}
