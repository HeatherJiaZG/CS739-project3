//
// Created by Yifei Yang on 3/27/22.
//

#ifndef CS739_P3_BLOCK_STORE_PRIMARYBACKUPHANDLER_H
#define CS739_P3_BLOCK_STORE_PRIMARYBACKUPHANDLER_H

#include "gen-cpp/PrimaryBackup.h"

namespace block_store {

class PrimaryBackupHandler: public PrimaryBackupIf {

public:
  PrimaryBackupHandler();

  int32_t heartbeat(const int32_t msg) override;
  int32_t sync(const std::vector<int64_t> & addr, const std::vector<std::string> & content) override;

};

}


#endif //CS739_P3_BLOCK_STORE_PRIMARYBACKUPHANDLER_H
