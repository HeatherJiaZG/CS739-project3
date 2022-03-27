// This is built on gen-cpp/Client_server.skeleton.cpp

#include "config.h"
#include "gen-cpp/Client.h"

#include <iostream>
#include <fstream>

#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

class ClientHandler : virtual public ClientIf {
 public:
  ClientHandler() {
    // remove all contents in central storage to init
    ofstream outfile;
    outfile.open(CENTRAL_STORAGE, ofstream::out | ofstream::trunc);
    outfile.close();
  }

  void read(std::string& _return, const int64_t addr);
  int32_t write(const int64_t addr, const std::string& content);
};


