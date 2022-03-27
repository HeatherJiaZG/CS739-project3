//
// Created by Yifei Yang on 3/27/22.
//

#ifndef CS739_P3_BLOCK_STORE_CLIENTHANDLER_H
#define CS739_P3_BLOCK_STORE_CLIENTHANDLER_H

#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>
#include "gen-cpp/Client.h"
#include "gen-cpp/PrimaryBackup.h"

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;

namespace block_store {

class ClientHandler: public ClientIf {

public:
  ClientHandler();

  void read(std::string& _return, const int64_t addr) override;
  int32_t write(const int64_t addr, const std::string& content) override;

private:
  // thrift client to connect to backup server
  std::shared_ptr<TTransport> toBackupSocket_;
  std::shared_ptr<TTransport> toBackupTransport_;
  std::shared_ptr<TProtocol> toBackupProtocol_;
  std::shared_ptr<PrimaryBackupClient> toBackupClient_;

};

}


#endif //CS739_P3_BLOCK_STORE_CLIENTHANDLER_H
