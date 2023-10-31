
struct file_packet {
   int size;
   char bytes[2048];
};
typedef struct file_packet file_packet;


struct file_update {
    int mode;
    char file_name[256];
};
typedef struct file_update file_update;


program PEER_NODE_PROG {
 version PEER_NODE_VERSION
 {
   file_packet download(string file_name, int idx) = 1;
   int update_list() = 2;
   int get_version() = 3; 
   file_update get_update(int verno) = 4;  
   int getLoad() = 5;
   bool Ping() = 99;
 } = 1;
} = 1000;

/*
*
* file_packet get_list() = 3;
* bring back get_list or keep as get_version and get_update?
* 
*/

