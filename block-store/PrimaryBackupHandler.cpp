//
// Created by Yifei Yang on 3/27/22.
//

#include "PrimaryBackupHandler.h"

namespace block_store {

PrimaryBackupHandler::PrimaryBackupHandler() {
  // Initialization
}

int32_t PrimaryBackupHandler::heartbeat(const int32_t msg) {
  // TODO
  return 0;
}

int32_t PrimaryBackupHandler::sync(const std::vector<int64_t> & addr, const std::vector<std::string> & content) {
  // TODO
  return 0;
}

}
