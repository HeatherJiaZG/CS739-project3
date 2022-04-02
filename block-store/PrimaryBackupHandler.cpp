//
// Created by Yifei Yang on 3/27/22.
//

#include "PrimaryBackupHandler.h"
#include "rwlock.h"
#include "Config.h"
#include "Util.h"
#include <iostream>
#include "fstream"
#include <fcntl.h>
#include <string.h>

using namespace std;

namespace block_store {

PrimaryBackupHandler::PrimaryBackupHandler() {
  Util::initDir(BACKUP_FILE_DIR.data());
}

int32_t PrimaryBackupHandler::sync(const int64_t addr, const std::string& content) {
  // if backup store is not synced to primary
  if (!synced_) {
    return -1;
  }

  int content_len = content.length();
  int block_num = Util::getFilename(addr);
  int offset = addr - block_num * BLOCK_SIZE;

  //TODO: writeSingleBlock can be paralleled

  // first write
  string filepath = BACKUP_FILE_DIR.data() + std::to_string(block_num) + ".file";
  int write_len = content_len <= BLOCK_SIZE - offset? content_len : BLOCK_SIZE - offset;
  Util::writeSingleBlock(filepath, offset, content.c_str(), 0, write_len);

  // write to subsequent blocks
  while (content_len - write_len >= BLOCK_SIZE) {
      string filepath = BACKUP_FILE_DIR.data() + std::to_string(++block_num) + ".file";
      Util::writeSingleBlock(filepath, 0, content.c_str(), write_len, write_len + BLOCK_SIZE);
      write_len += BLOCK_SIZE;
  }

  // process the remaining contents
  if (write_len < content_len) {
      string filepath = BACKUP_FILE_DIR.data() + std::to_string(++block_num) + ".file";
      Util::writeSingleBlock(filepath, 0, content.c_str(), write_len, content_len);
  }

  cout<<"backup write success"<<endl;

  return 0;
}

void PrimaryBackupHandler::get_timestamps(std::map<std::string, int64_t> & _return,
                                          const std::vector<std::string> & primary_files) {
  for (const auto &fileName: primary_files) {
    auto filePath = BACKUP_FILE_DIR.data() + fileName;
    auto timestamp = Util::getTimestamp(filePath);
    _return.emplace(fileName, timestamp);
  }
}

void PrimaryBackupHandler::sync_files(const std::map<std::string, std::string> & primary_files) {
  for (const auto &it: primary_files) {
    auto fileName = it.first;
    auto filePath = BACKUP_FILE_DIR.data() + fileName;
    auto content = it.second;

    ofstream fs;
    fs.open(filePath);
    fs << content;
    fs.close();
  }

  synced_ = true;
}

}
