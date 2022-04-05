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
#include "string"
#include <boost/functional/hash.hpp>
#include <chrono>
#include <random>

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace block_store;

void test1(ClientClient client);
void test2(ClientClient client);
void test3(ClientClient client);
void test4(ClientClient client);
std::string strRand(int length);
void measurement1(ClientClient client);
void measurement2(ClientClient client);
void time_multiple_reads(ClientClient client, int num_of_reads);
void time_multiple_writes(ClientClient client, int num_of_reads);

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
  test2(client);
  test3(client);
  test4(client);
  measurement1(client);
  measurement2(client);

  return 0;
}

// write and read 1 simple string
void test1(ClientClient client) { 
    std::string input_str = "hello world";
    client.write(0, input_str);
    std::string str;
    client.read(str, 0);

    std::cout << "test 1 str=" <<str<< std::endl;
    std::cout << "test 1 input_str=" <<input_str<< std::endl;
    std::cout << "test 1 pass" << std::endl;
}

// write and read 1 simple string multiple rounds
void test2(ClientClient client) { 
    int rand_num = rand() % 100; // num of rounds
    std::string content = "hello world";
    for (int i = 0; i < rand_num; i++) {
        client.write(i * 4096, content);
        std::string str;
        client.read(str, i * 4096);
    }
    std::cout << "test 2 pass" << std::endl;
}

// write the size of two blocks and read
void test3(ClientClient client) { 
    // create a file
    client.write(0, "hello world");
    std::string str;
    client.read(str, 0);

    // write across blocks
    std::string str1(BLOCK_SIZE*2, 'x');
    assert(str1.length() == BLOCK_SIZE*2);

    client.write(6, str1);
    std::string str2;
    client.read(str2, 0);
//    assert(str2 == std::string(BLOCK_SIZE, 'x'));

    std::string str3;
    client.read(str3, BLOCK_SIZE);
//    assert(str3 == std::string(BLOCK_SIZE, 'x'));

    // std::string str4;
    // client.read(str4, BLOCK_SIZE*2);
    // std::cout << str4 << std::endl;
    // std::cout << str4.length() << std::endl;
    std::cout << "test 3 pass" << std::endl;
}

// write and read a string of block size and compare checksum
void test4(ClientClient client) {
    boost::hash<std::string> string_hash;
    std::string rand_str = strRand(4096);
    client.write(0, rand_str);
    int write_check_sum = string_hash(rand_str);
    std::string read_str;
    client.read(read_str, 0);
    int read_check_sum = string_hash(read_str);

    assert(write_check_sum == read_check_sum);
    std::cout << "test 4 pass" << std::endl;
}

// show latency of 100, 500, 1000 reads
void measurement1(ClientClient client) {
    time_multiple_reads(client, 100);
    time_multiple_reads(client, 500);
    time_multiple_reads(client, 1000);
}

// read 4KB at the same address repetitively
void time_multiple_reads(ClientClient client, int num_of_reads) {
    // write contents
    std::string rand_str = strRand(BLOCK_SIZE);
    client.write(0, rand_str);

    // read and capture latency
    auto begin = std::chrono::high_resolution_clock::now();
    for (int i=0; i<num_of_reads; i++){
      std::string read_str;
      client.read(read_str, 0);
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);
    printf("Reading %d times takes %.3f seconds.\n", num_of_reads, elapsed.count() * 1e-9);
}

// show latency of 50, 100, 300 writes at random addresses
void measurement2(ClientClient client) {
    time_multiple_writes(client, 50);
    time_multiple_writes(client, 100);
    time_multiple_writes(client, 300);
}

// write 4KB at random address
void time_multiple_writes(ClientClient client, int num_of_reads) {
    // write and capture latency
    auto begin = std::chrono::high_resolution_clock::now();
    for (int i=0; i<num_of_reads; i++){
      int rand_addr = rand() % 100;
      std::string str(BLOCK_SIZE, 'x');
      client.write(rand_addr, str);
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);
    printf("Writing %d times takes %.3f seconds.\n", num_of_reads, elapsed.count() * 1e-9);
}

std::string strRand(int length) {
    char tmp;
    std::string buffer;

    std::random_device rd;
    std::default_random_engine random(rd());

    for (int i = 0; i < length; i++) {

        tmp = random() % 36;
        if (tmp < 10) {
            tmp += '0';
        } else {
            tmp -= 10;
            tmp += 'A';
        }
        buffer += tmp;
    }
    return buffer;
}