//
// Created by Yifei Yang on 4/5/22.
//

#ifndef CS739_P3_BLOCK_STORE_STORESERVERHANDLER_H
#define CS739_P3_BLOCK_STORE_STORESERVERHANDLER_H

#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>
#include "gen-cpp/StoreServer.h"

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;

namespace block_store {

class StoreServerHandler: public StoreServerIf {

public:
  StoreServerHandler();

  // client -> primary RPC
  void read(std::string& _return, const int64_t addr) override;
  int32_t write(const int64_t addr, const std::string& content) override;

  // primary -> backup RPC
  int32_t sync(const int64_t addr, const std::string& content) override;
  void get_timestamps(std::map<std::string, int64_t> & _return, const std::vector<std::string> & primary_files) override;
  void sync_files(const std::map<std::string, std::string> & primary_files) override;

  // backup -> primary RPC
  void ping() override;

private:
  void syncFiles();
  bool tryConnectToPrimary();
  void connectToBackup();
  void pingPrimary();

  bool connectedToOther_ = false;
  bool isPrimary_ = false;
  bool isSynced_ = false;
  std::string otherIp_;

  // thrift client to connect to the other server
  std::shared_ptr<TTransport> toOtherSocket_;
  std::shared_ptr<TTransport> toOtherTransport_;
  std::shared_ptr<TProtocol> toOtherProtocol_;
  std::shared_ptr<StoreServerClient> toOtherClient_;
};

}


#endif //CS739_P3_BLOCK_STORE_STORESERVERHANDLER_H
