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

  int32_t sync(const int64_t addr, const std::string& content) override;
  void get_timestamps(std::map<std::string, int64_t> & _return, const std::vector<std::string> & primary_files) override;
  void sync_files(const std::map<std::string, std::string> & primary_files) override;

private:
  bool synced_ = false;

};

}


#endif //CS739_P3_BLOCK_STORE_PRIMARYBACKUPHANDLER_H
