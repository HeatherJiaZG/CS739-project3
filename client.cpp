// This is built on gen-cpp/Client_server.skeleton.cpp

#include "client.h"
#include <iostream>
#include <fcntl.h>  
#include <unistd.h>
using namespace std;  

void read(std::string& _return, const int64_t addr) {
  int fd;
  off_t offset = addr;
  char readbuf[BLOCK_SIZE];

  if ((fd = open(CENTRAL_STORAGE, O_RDONLY)) == -1) {
		perror("open error");
		exit(1);
	}
  pread(fd, &readbuf, BLOCK_SIZE, offset);
  _return = readbuf;
  close(fd);
  cout<<"read success\n"<<endl;
}

int32_t write(const int64_t addr, const std::string& content) {
  int fd;
  off_t offset = addr;
  char writebuf[BLOCK_SIZE];

  if ((fd = open(CENTRAL_STORAGE, O_WRONLY)) == -1) { // TODO optimization: make fd a global var
		perror("open error");
		exit(1);
	}
  pwrite(fd, writebuf, BLOCK_SIZE, offset); 
  _return = writebuf;
  close(fd)
  // TODO: write to backup here
  cout<<"write success\n"<<endl;
}

int main(int argc, char **argv) {
  // if (argc != 2) {
  //   std::cout << "Usage: ./server <num of clients>\n";
  //   return -1;
  // }
  // int clients = atoi(argv[1]);
  
  ::std::shared_ptr<ClientHandler> handler(new ClientHandler());
  ::std::shared_ptr<TProcessor> processor(new ClientProcessor(handler));
  ::std::shared_ptr<TServerTransport> serverTransport(new TServerSocket(PRIMARY_SERVER_PORT));
  ::std::shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
  ::std::shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());
  // ::std::shared_ptr<ThreadManager> threadManager = ThreadManager::newSimpleThreadManager(clients); // TODO: how to ensure only 1 thread on primary server?

  TSimpleServer server(processor, serverTransport, transportFactory, protocolFactory);
  server.serve();
  return 0;
}

