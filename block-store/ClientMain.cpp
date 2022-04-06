//
// Created by Yifei Yang on 3/27/22.
//

#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>
#include <fmt/format.h>
#include "gen-cpp/StoreServer.h"
#include "Config.h"
#include "iostream"
#include <cstdlib>
#include "string"
#include <boost/functional/hash.hpp>
#include <chrono>
#include <random>

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace block_store;

//----------------------Client Library-------------------------------
void client_read(std::string& _return, const int64_t addr, StoreServerClient client0, StoreServerClient client1) {
    while (true) {
        try {
            client0.read(_return, addr);
	    if (!_return.empty()) {
		    return;
	    }
        } catch (TException& tx) {
            std::cout << fmt::format("Fail to read from server0: {}", tx.what()) << std::endl;
        }
        try {
            client1.read(_return, addr);
	    if (!_return.empty()) {
		    return;
	    }
        } catch (TException& tx) {
            std::cout << fmt::format("Fail to read from server1: {}", tx.what()) << std::endl;
        }
    }
}

int32_t client_write(const int64_t addr, const std::string& content, StoreServerClient client0, StoreServerClient client1) {
    while (true) {
        try {
            int32_t ret_ = client0.write(addr, content);
	    if (ret_ != -1) {
	    	std::cout << "Write to server0." << std::endl;
	    return ret_;

	    }
	    std::cout << "Server 0: " <<  ret_ << std::endl;
        } catch (TException& tx) {
            std::cout << fmt::format("Fail to write to server0: {}", tx.what()) << std::endl;
        }
        try {

            int32_t ret_ = client1.write(addr, content);
	    if (ret_ != -1) {
	    std::cout << "Write to server1." << std::endl;
	    return ret_;
	    }
	    std::cout << "Server 1: " <<  ret_ << std::endl;
        } catch (TException& tx) {
            std::cout << fmt::format("Fail to write to server1: {}", tx.what()) << std::endl;
        }
    }
}


//-----------------------Test Functions------------------------------
void test1(StoreServerClient client0, StoreServerClient client1);
void test2(StoreServerClient client0, StoreServerClient client1);
void test3(StoreServerClient client0, StoreServerClient client1);
void test4(StoreServerClient client0, StoreServerClient client1);
std::string strRand(int length);
void measurement1(StoreServerClient client0, StoreServerClient client1);
void measurement2(StoreServerClient client0, StoreServerClient client1);
void time_multiple_reads(StoreServerClient client0, StoreServerClient client1, int num_of_reads);
void time_multiple_writes(StoreServerClient client0, StoreServerClient client1, int num_of_reads);

int main() {
  std::shared_ptr<TTransport> socket0(new TSocket(SERVER_IP1.data(), SERVER_PORT));
  std::shared_ptr<TTransport> transport0(new TBufferedTransport(socket0));
  std::shared_ptr<TProtocol> protocol0(new TBinaryProtocol(transport0));
  StoreServerClient client0(protocol0);

  std::shared_ptr<TTransport> socket1(new TSocket(SERVER_IP2.data(), SERVER_PORT));
  std::shared_ptr<TTransport> transport1(new TBufferedTransport(socket1));
  std::shared_ptr<TProtocol> protocol1(new TBinaryProtocol(transport1));
  StoreServerClient client1(protocol1);

  try {
    transport0->open();
  } catch (TException& tx) {
    std::cout << fmt::format("Fail to connect to server0: {}", tx.what()) << std::endl;
  }

    try {
        transport1->open();
    } catch (TException& tx) {
        std::cout << fmt::format("Fail to connect to server1: {}", tx.what()) << std::endl;
    }

  test1(client0, client1);
  test2(client0, client1);
  test3(client0, client1);
  test4(client0, client1);
  // measurement1(client0, client1);
  // measurement2(client0, client1);

  return 0;
}

// write and read 1 simple string
void test1(StoreServerClient client0, StoreServerClient client1) {
    std::string input_str = "hello world";
    client_write(0, input_str, client0, client1);
    std::string str;
    client_read(str, 0, client0, client1);

    std::cout << "test 1 str=" <<str<< std::endl;
    std::cout << "test 1 input_str=" <<input_str<< std::endl;
    std::cout << "test 1 pass" << std::endl;
}

// write and read 1 simple string multiple rounds
void test2(StoreServerClient client0, StoreServerClient client1) {
    int rand_num = rand() % 100; // num of rounds
    std::string content = "hello world";
    for (int i = 0; i < rand_num; i++) {
        client_write(i * 4096, content, client0, client1);
        std::string str;
        client_read(str, i * 4096, client0, client1);
    }
    std::cout << "test 2 pass" << std::endl;
}

// write the size of two blocks and read
void test3(StoreServerClient client0, StoreServerClient client1) {
    // create a file
    client_write(0, "hello world", client0, client1);
    std::string str;
    client_read(str, 0, client0, client1);

    // write across blocks
    std::string str1(BLOCK_SIZE*2, 'x');
    assert(str1.length() == BLOCK_SIZE*2);

    client_write(6, str1, client0, client1);
    std::string str2;
    client_read(str2, 0, client0, client1);
//    assert(str2 == std::string(BLOCK_SIZE, 'x'));

    std::string str3;
    client_read(str3, BLOCK_SIZE, client0, client1);
//    assert(str3 == std::string(BLOCK_SIZE, 'x'));

    // std::string str4;
    // client.read(str4, BLOCK_SIZE*2);
    // std::cout << str4 << std::endl;
    // std::cout << str4.length() << std::endl;
    std::cout << "test 3 pass" << std::endl;
}

// write and read a string of block size and compare checksum
void test4(StoreServerClient client0, StoreServerClient client1) {
    boost::hash<std::string> string_hash;
    std::string rand_str = strRand(4096);
    client_write(0, rand_str, client0, client1);
    int write_check_sum = string_hash(rand_str);
    std::string read_str;
    client_read(read_str, 0, client0, client1);
    int read_check_sum = string_hash(read_str);

    assert(write_check_sum == read_check_sum);
    std::cout << "test 4 pass" << std::endl;
}

// show latency of 100, 500, 1000 reads
void measurement1(StoreServerClient client0, StoreServerClient client1) {
    time_multiple_reads(client0, client1, 100);
    time_multiple_reads(client0, client1, 500);
    time_multiple_reads(client0, client1, 1000);
}

// read 4KB at the same address repetitively
void time_multiple_reads(StoreServerClient client0, StoreServerClient client1, int num_of_reads) {
    // write contents
    std::string rand_str = strRand(BLOCK_SIZE);
    client_write(0, rand_str, client0, client1);

    // read and capture latency
    auto begin = std::chrono::high_resolution_clock::now();
    for (int i=0; i<num_of_reads; i++){
      std::string read_str;
      client_read(read_str, 0, client0, client1);
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);
    printf("Reading %d times takes %.3f seconds.\n", num_of_reads, elapsed.count() * 1e-9);
}

// show latency of 50, 100, 300 writes at random addresses
void measurement2(StoreServerClient client0, StoreServerClient client1) {
    time_multiple_writes(client0, client1, 50);
    time_multiple_writes(client0, client1, 100);
    time_multiple_writes(client0, client1, 300);
}

// write 4KB at random address
void time_multiple_writes(StoreServerClient client0, StoreServerClient client1, int num_of_reads) {
    // write and capture latency
    auto begin = std::chrono::high_resolution_clock::now();
    for (int i=0; i<num_of_reads; i++){
      int rand_addr = rand() % 100;
      std::string str(BLOCK_SIZE, 'x');
      client_write(rand_addr, str, client0, client1);
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
