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
#include "stdlib.h"

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace block_store;

void test1(ClientClient client);
void test2(ClientClient client);
void test3(ClientClient client);

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

  test3(client);

  return 0;
}

void test1(ClientClient client) {
    client.write(0, "hello world");
    std::string str;
    client.read(str, 0);
    std::cout << str << std::endl;
}

void test2(ClientClient client) {
    int rand_num = rand() % 100;
    std::cout << "num of rounds: " << rand_num << std::endl;
    std::string content = "hello world";
    std::cout << "write start" << std::endl;
    for (int i = 0; i < rand_num; i++) {
        client.write(0 + i * 4096, content);
        std::string str;
        client.read(str, 0 + i * 4096);
    }
}

void test3(ClientClient client) {
    // create a file
    client.write(0, "hello world");
    std::string str;
    client.read(str, 0);
    std::cout << str << "is hello world, read success!\n";

    // write across blocks
    std::string str1(4096*2, 'x');
    std::cout << str1.length() << std::endl;
    client.write(6, str1);
    std::string str2;
    client.read(str2, 0);
    std::cout << str2 << std::endl;
    std::string str3;
    client.read(str3, 4096);
    std::cout << str3 << std::endl;
    std::string str4;
    client.read(str4, 4096*2);
    std::cout << str4 << std::endl;
    std::cout << str4.length() << std::endl;
}

