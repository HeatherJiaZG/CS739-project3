// This is built on gen-cpp/PrimaryBackup_server.skeleton.cpp

#include "PrimaryBackup.h"
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

class PrimaryBackupHandler : virtual public PrimaryBackupIf {
 public:
  PrimaryBackupHandler() {
    // Your initialization goes here
  }

  int32_t heartbeat(const int32_t msg) {
    // Your implementation goes here
    printf("heartbeat\n");
  }

  int32_t sync(const std::vector<int64_t> & addr, const std::vector<std::string> & content) {
    // Your implementation goes here
    printf("sync\n");
  }

};

int main(int argc, char **argv) {
  int port = 9090;
  ::std::shared_ptr<PrimaryBackupHandler> handler(new PrimaryBackupHandler());
  ::std::shared_ptr<TProcessor> processor(new PrimaryBackupProcessor(handler));
  ::std::shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
  ::std::shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
  ::std::shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());

  TSimpleServer server(processor, serverTransport, transportFactory, protocolFactory);
  server.serve();
  return 0;
}

