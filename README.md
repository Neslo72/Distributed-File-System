# CSCI5105-P3
This project was written by Thomas Reinitz (reini050) and Lucas Olsen (olse0280) for CSCI5105. It implements a distributed file system with peer nodes and a server coordinator. This README.md file is best viewed in a markdown compatible viewer, or on github through the following link: https://github.umn.edu/olse0280/CSCI5105_P3

### Pledge 
We, both Thomas and Lucas, pledge that no one sought out any on-line solutions, e.g. github, for portions of this project.

# To compile
Open a terminal and navigate to the project directory.  Then, execute the following command:
```
>:~/.../CSCI5105-P3$ make
```
This will create numerous exectuable files, notably ones in the following directories.  Instructions on how to use each executable will follow in the next segment of this readme:
```
server_node/
node_1/
node_2/
node_3/
```

# To Run
To start a peer server, navigate to a node directory (node_1, node_2, or node_3) and run the following command:
```
>:~/.../CSCI5105-P3/node_x$ ./peer_server <server_port> <latency (in ms)>
```
* `server_port` is the port for the peer node to run on  
* `latency` is a manufactured delay to be imposed on the peer server  


You can specify the port in which each node runs on, but the resulting port should be stored in the `serverlist.txt` file for the central server to work properly.  By default, node_1 should run on port 8001, node_2 on 8002, and node_3 on 8003.  If these ports are changed or new peer nodes are added, make sure to copy the changes to `serverlist.txt`.  The formatting of `serverlist.txt` has the first line equal to the number of servers contained within the file, then each following line is formatted as `[server_ip], [server_port]` to specify each individual server.

To start the coordinator server, navigate to the `server_node/` directory and use the following command:
```
>:~/.../CSCI5105-P3/server_node$ ./server_node_server <server_port>
```
* `server_port` specifies the port for the server to run on

To use the client app, navigate to a node directory and run the following command:
```
>:~/.../CSCI5105-P3/node_x/$ ./app <server_ip> <server_port> [option]
```
* `<server_ip>` specifies the ip of the coordinator server  
* `<server_port>` specifies the port of the coordinator server  
* `<command>` specifies the command to be used by the client app  


The three valid commands to be used are `list` `list_all` and `download`.  `list` requires and additional `[filename]` argument, `list_all` requires no extra arguments, and `download` requires a `[filename]` argument.  The client app is single use, as in it will only run one operation each time it is executed, so to run multiple operations, multiple calls to `./app` are required.

# Notable files
Each of the following files contain significant work done by each team member  
* `/peer_node/peer_node_server.c`  -- Peer RPC server functions  
* `/server_node/server_node_server.c` -- Coordinator RPC server functions  
* `/server_node/peer_node_svc.c`  -- Coordinator RPC server setup.  Contains pinger() child process  
* `/server_node/peer_node_client.c`  -- The app.  Client app functionality
* `/server_node/rpc_call.c` -- RPC calling functions needed by the coorindinator to call peers in the network.  

Both partners added significantly to each of the above files.  Lucas mainly worked on baseline functionality between the peer and central server while Thomas worked on fault tolerance specifics and client selection algorithms.

Additional changes were made to other RPC generated files to make the code function but they are not mentioned here for they do not contain significant work.  The compiled executables in each node directory are copies of the exectuables generated from the files `peer_node_server.c -> peer_server` and `server_node_client.c -> app`.


# Test Cases

*Each test case uses the provdied node directories and the existing contents within each node's /shr/ directories.  This content can be changed and more nodes can be added if desired*

## Coordinator Prinout

- Test 1: Start coordinator with no running peer nodes
```
./server_node_server 8000
Socket up and running on port 8000!
Client 127.0.0.1 : 8001 offline.
Client 127.0.0.1 : 8002 offline.
Client 127.0.0.1 : 8003 offline.
Client 127.0.0.1 : 8001 offline.
[Continually pings each of the clients]
```
- Test 2: Start coordinator with 1 already running peer node.  The server tracks 3 new files found in node_1's shr/ directory
```
./server_node_server 8000
Socket up and running on port 8000!
NEW FILE: file_in_all_nodes.txt
NEW FILE: rpc_linux_manual.txt
NEW FILE: testfile.txt
Tracked files:  file_in_all_nodes.txt,  rpc_linux_manual.txt,  testfile.txt, 
Client 127.0.0.1 : 8002 offline.
Client 127.0.0.1 : 8003 offline.
Tracked files:  file_in_all_nodes.txt,  rpc_linux_manual.txt,  testfile.txt, 
Client 127.0.0.1 : 8002 offline.
[Continually pings each of the clients]
```
- Test 3: Start coordinator with 2 already running peer nodes.  This time the files in node_1 and node_2's shr/ directories are tracked
```
./server_node_server 8000
Socket up and running on port 8000!
NEW FILE: file_in_all_nodes.txt
NEW FILE: rpc_linux_manual.txt
NEW FILE: testfile.txt
Tracked files:  file_in_all_nodes.txt,  rpc_linux_manual.txt,  testfile.txt, 
NEW FILE: node2.txt
Tracked files:  file_in_all_nodes.txt,  rpc_linux_manual.txt,  testfile.txt,  node2.txt, 
Client 127.0.0.1 : 8003 offline.
[Continually pings each of the clients]
```
- Test 4: Coordinator running, peer server disconect.  node_2's peer_server will disconect and its tracked files will be untracked
```
[coordinator running how test 3 ended]
Client 127.0.0.1 : 8003 offline.
Tracked files:  file_in_all_nodes.txt,  rpc_linux_manual.txt,  testfile.txt,  node2.txt, 
Tracked files:  file_in_all_nodes.txt,  rpc_linux_manual.txt,  testfile.txt,  node2.txt,  // node_2 still online here
Client 127.0.0.1 : 8003 offline.
Tracked files:  file_in_all_nodes.txt,  rpc_linux_manual.txt,  testfile.txt,  node2.txt, 
Client 127.0.0.1 : 8002 offline.  // node_2 goes down
File file_in_all_nodes.txt no longer tracked by host 127.0.0.1 : 8002
File node2.txt is no longer tracked by the server.
Client 127.0.0.1 : 8003 offline.
[Continually pings each of the clients]
```
- Test 5: Coordinator running, peer server join.  node_3's peer_server will join and its files will be tracked
```
[coordinator running with 1 peer connected]
Client 127.0.0.1 : 8003 offline.  // node 3 offline here
Tracked files:  file_in_all_nodes.txt,  rpc_linux_manual.txt,  testfile.txt, 
Client 127.0.0.1 : 8002 offline.
NEW FILE: node3.txt  // node 3 has come online
NEW FILE: epic.PNG
Tracked files:  file_in_all_nodes.txt,  rpc_linux_manual.txt,  testfile.txt,  node3.txt,  epic.PNG, 
Tracked files:  file_in_all_nodes.txt,  rpc_linux_manual.txt,  testfile.txt,  node3.txt,  epic.PNG, 
Client 127.0.0.1 : 8002 offline.
Tracked files:  file_in_all_nodes.txt,  rpc_linux_manual.txt,  testfile.txt,  node3.txt,  epic.PNG, 
Tracked files:  file_in_all_nodes.txt,  rpc_linux_manual.txt,  testfile.txt,  node3.txt,  epic.PNG,
[Continually pings each of the clients]
```

## App Printout

*The app exectuable can be run from anywhere that contains an shr/ directory.  It does not have to be in the same directory as an existing peer_server*

- Test 6: invalid arguments
```
./app nan
./app <server_ip> <server_port> [option]
```
- Test 7: Coordinator offline in list_all.  The same prinout is recieved if the coordinator is offline while using any of the other options.
```
./app 127.0.0.1 8000 list_all
Failed to make RPC call.  Central server offline
```
- Test 8: list_all with only node_1 online
```
./app 127.0.0.1 8000 list_all
File file_in_all_nodes.txt has hosts: 127.0.0.1 : 8001,
File rpc_linux_manual.txt has hosts: 127.0.0.1 : 8001,
File testfile.txt has hosts: 127.0.0.1 : 8001,
```
- Test 9: list_all with all 3 nodes online
```
./app 127.0.0.1 8000 list_all
File file_in_all_nodes.txt has hosts: 127.0.0.1 : 8001, 127.0.0.1 : 8003, 127.0.0.1 : 8002,
File rpc_linux_manual.txt has hosts: 127.0.0.1 : 8001,
File testfile.txt has hosts: 127.0.0.1 : 8001,
File node3.txt has hosts: 127.0.0.1 : 8003,
File epic.PNG has hosts: 127.0.0.1 : 8003,
File node2.txt has hosts: 127.0.0.1 : 8002,
```
- Test 10: list file_in_all_nodes.txt with all 3 nodes online
```
./app 127.0.0.1 8000 list file_in_all_nodes.txt
File file_in_all_nodes.txt has hosts: 127.0.0.1 : 8001, 127.0.0.1 : 8003, 127.0.0.1 : 8002,
```
- Test 11: list file_in_all_nodes.txt with only node_2 and node_3 online
```
./app 127.0.0.1 8000 list file_in_all_nodes.txt
File file_in_all_nodes.txt has hosts: 127.0.0.1 : 8002, 127.0.0.1 : 8003,
```
- Test 12: list a file not tracked by the server, node_2 is offline
```
./app 127.0.0.1 8000 list node2.txt
File not found on currently joined servers
```
- Test 13: Download file epic.PNG from server, node_3 is online.
```
./app 127.0.0.1 8000 download epic.PNG
requesting file epic.PNG
epic.PNG successfully downloaded to shr/epic.PNG
```
- Test 14: Download file file_in_all_nodes.txt from the server, all nodes online.
```
./app 127.0.0.1 8000 download file_in_all_nodes.txt
requesting file file_in_all_nodes.txt
file_in_all_nodes.txt successfully downloaded to shr/file_in_all_nodes.txt
```
- Test 15: Download file file_in_all_nodes.txt from the server, all nodes online, checksum failed.
```
./app 127.0.0.1 8000 download file_in_all_nodes.txt
requesting file file_in_all_nodes.txt
Download checksum failed. Expected A got B
```
- Test 16: Download a file not tracked by the server, node_1 is offline
```
./app 127.0.0.1 8000 download rpc_linux_manual.txt
Failed to find host for rpc_linux_manual.txt
```
## Peer Server Printout

- Test 17: Start peer server.  Example from node_2
```
./peer_server 8002 2
Socket up and running on port 8002!
NEW FILE: file_in_all_nodes.txt
NEW FILE: node2.txt
```
- Test 18: Peer server running, new file added to shr/ directory.  node_2 downloads epic.PNG
```
./peer_server 8002 2
Socket up and running on port 8002!
NEW FILE: file_in_all_nodes.txt
NEW FILE: node2.txt  // Delay exists before the next line since epic.PNG is not there on startup
NEW FILE: epic.PNG
```





