//
// Created by Yifei Yang on 3/27/22.
//

#ifndef CS739_P3_BLOCK_STORE_CLIENTHANDLER_H
#define CS739_P3_BLOCK_STORE_CLIENTHANDLER_H

#include "gen-cpp/Client.h"

namespace block_store {

class ClientHandler: public ClientIf {

public:
  ClientHandler();

  void read(std::string& _return, const int64_t addr) override;
  int32_t write(const int64_t addr, const std::string& content) override;

};

}


#endif //CS739_P3_BLOCK_STORE_CLIENTHANDLER_H
