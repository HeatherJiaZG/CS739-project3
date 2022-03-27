//
// Created by Yifei Yang on 3/27/22.
//

#include <thrift/server/TThreadedServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TTransportUtils.h>
#include "ClientHandler.h"
#include "Config.h"
#include "iostream"

using namespace apache::thrift::server;
using namespace apache::thrift::transport;
using namespace block_store;

int main(int argc, char **argv) {
  // if (argc != 2) {
  //   std::cout << "Usage: ./PrimaryMain <num of clients>\n";
  //   return -1;
  // }
  // int clients = atoi(argv[1]);

  TThreadedServer server(
          std::make_shared<ClientProcessor>(std::make_shared<ClientHandler>()),
          std::make_shared<TServerSocket>(PRIMARY_SERVER_PORT), //port
          std::make_shared<TBufferedTransportFactory>(),
          std::make_shared<TBinaryProtocolFactory>());
  std::cout << "Primary server started..." << std::endl;
  server.serve();

  return 0;
}

