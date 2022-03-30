//
// Created by Yifei Yang on 3/27/22.
//

#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>
#include <fmt/format.h>
#include "gen-cpp/Client.h"
#include "Config.h"
#include "iostream"

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace block_store;

void test1(ClientClient client);

int main() {
  std::shared_ptr<TTransport> socket(new TSocket(PRIMARY_SERVER_HOSTNAME.data(), PRIMARY_SERVER_PORT));
  std::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
  std::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
  ClientClient client(protocol);

  try {
    transport->open();
  } catch (TException& tx) {
    std::cout << fmt::format("Fail to connect to primary server: {}", tx.what()) << std::endl;
  }

  test1(client);

  return 0;
}

void test1(ClientClient client){
    client.write(0, "hello world");
    std::string str;
    client.read(str, 0);
    std::cout << str << std::endl;
}