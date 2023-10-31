
## Compile all and copy things to separate node directories
all : compile copy

compile : 
	cd peer_node && $(MAKE)
	cd server_node && $(MAKE)

copy : 
	$(shell cp peer_node/peer_node_server node_1/peer_server)
	$(shell cp server_node/server_node_client node_1/app)
	$(shell cp peer_node/peer_node_server node_2/peer_server)
	$(shell cp server_node/server_node_client node_2/app)
	$(shell cp peer_node/peer_node_server node_3/peer_server)
	$(shell cp server_node/server_node_client node_3/app)

## Clean all
clean : 
	cd peer_node && $(MAKE) clean
	cd server_node && $(MAKE) clean
	$(shell rm -f node_1/peer_server)
	$(shell rm -f node_1/app)
	$(shell rm -f node_2/peer_server)
	$(shell rm -f node_2/app)
	$(shell rm -f node_3/peer_server)
	$(shell rm -f node_3/app)