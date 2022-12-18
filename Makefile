PORT=4009
HOSTNAME=localhost
FILE=files/arquivo_teste.txt
CLIENT_BUFFER_SIZE=1024
SERVER_BUFFER_SIZE=1024

default: help


help:
	@echo "File sender"
	@echo
	@echo "Target rules:"
	@echo "    run_server_uni      	- Compiles and run unidirectional server"
	@echo "    run_server_rr       	- Compiles and run request response server"
	@echo "    buid_server_uni     	- Compiles unidirectional server"
	@echo "    build_server_rr      	- Compiles request response server"
	@echo "    run_client_uni      	- Compiles and run unidirectional client"
	@echo "    run_client_rr       	- Compiles and run request response client"
	@echo "    buid_client_uni     	- Compiles unidirectional client"
	@echo "    build_client_rr      	- Compiles request response client"
	@echo "    help     			- Prints a help message with target rules"

run_server_uni: build_server_uni
	./bin/server_uni_exec $(PORT) $(SERVER_BUFFER_SIZE)

build_server_uni: server.o server_uni.o
	gcc -o bin/server_uni_exec bin/server.o bin/server_uni.o

server_uni.o: src/server_uni.c
	gcc -o bin/server_uni.o src/server_uni.c -c -Wall -lpthread

run_server_rr: build_server_rr
	./bin/server_rr_exec $(PORT) $(SERVER_BUFFER_SIZE)

build_server_rr: server.o server_rr.o
	gcc -o bin/server_rr_exec bin/server.o bin/server_rr.o

server_rr.o: src/server_rr.c
	gcc -o bin/server_rr.o src/server_rr.c -c -Wall -lpthread

server.o: src/server.c src/server.h
	gcc -o bin/server.o src/server.c -c -Wall -lpthread

run_client_rr: build_client_rr
	./bin/client_rr_exec $(HOSTNAME) $(PORT) $(FILE) $(CLIENT_BUFFER_SIZE)

build_client_rr: client_rr.o client.o
	gcc -o bin/client_rr_exec bin/client_rr.o bin/client.o -Wall -lpthread

run_client_uni: build_client_uni
	./bin/client_uni_exec $(HOSTNAME) $(PORT) $(FILE) $(CLIENT_BUFFER_SIZE)

build_client_uni: client_uni.o client.o
	gcc -o bin/client_uni_exec bin/client_uni.o bin/client.o -Wall -lpthread

client_rr.o: src/client_rr.c
	gcc -o bin/client_rr.o src/client_rr.c -c -Wall -lpthread

client_uni.o: src/client_uni.c
	gcc -o bin/client_uni.o src/client_uni.c -c -Wall -lpthread

client.o: src/client.c src/client.h
	gcc -o bin/client.o src/client.c -c -Wall -lpthread

clean:
	@rm ./bin/*