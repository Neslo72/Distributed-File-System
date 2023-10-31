
const MAXNAME = 256;
const MAXHOST = 10;

struct file_update {
   int mode;
   char file_name[MAXNAME];
};
typedef struct file_update file_update;


struct host_info {
   char ip[16];
   int port;
};
typedef struct host_info host_info;


struct file_info {
   char name[MAXNAME];
   host_info hosts[MAXHOST];
};
typedef struct file_info file_info;


program SERVER_NODE_PROG {
 version SERVER_NODE_VERSION
 {
   file_info list_hosts_str(string file_name) = 1;
   file_info list_hosts_num(int fileno) = 2;
   void check_client_update(string ip, int port) = 3;
   host_info find_host(string file_name) = 4;
   bool Ping() = 99;
 } = 1;
} = 1000;
