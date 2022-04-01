//
// Created by Yifei Yang on 3/27/22.
//

#include "ClientHandler.h"
#include "Config.h"
#include "rwlock.h"
#include "Util.h"
#include <iostream>
#include <fstream>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <fmt/format.h>
#include "sstream"

using namespace std;

namespace block_store {

ClientHandler::ClientHandler() {
  // remove all contents in central storage to init
  Util::initFile(PRIMARY_CENTRAL_STORAGE.data());

  // connect to backup server
  toBackupSocket_ = std::make_shared<TSocket>(BACKUP_SERVER_HOSTNAME.data(), BACKUP_SERVER_PORT);
  toBackupTransport_ = std::make_shared<TBufferedTransport>(toBackupSocket_);
  toBackupProtocol_ = std::make_shared<TBinaryProtocol>(toBackupTransport_);
  toBackupClient_ = std::make_shared<PrimaryBackupClient>(toBackupProtocol_);
  try {
    toBackupTransport_->open();
  } catch (TException& tx) {
    std::cout << fmt::format("Fail to connect to backup server: {}", tx.what()) << std::endl;
    exit(1);
  }
}

void ClientHandler::read(std::string& _return, const int64_t addr) {
  int fd;
  off_t offset = addr;
  char readbuf[BLOCK_SIZE] = {};

  ReadLock r_lock(rwLock);

  string filename = std::to_string(Util::getFilename(addr));
  string filepath = PRIMARY_FILE_DIR.data() + filename + ".file";
  if ((fd = open(filepath.c_str(), O_RDONLY)) == -1) {
      // TODO: open err or return empty block?
      perror("open error");
      exit(1);
  }
  pread(fd, &readbuf, BLOCK_SIZE, 0);
  _return = std::string(readbuf);
  std::cout << _return << std::endl;
  close(fd);
  cout<<"read success\n"<<endl;
}

int32_t ClientHandler::write(const int64_t addr, const std::string& content) {

  WriteLock w_lock(rwLock);

  int content_len = content.length();
  int block_num = Util::getFilename(addr);
  int offset = addr - block_num * BLOCK_SIZE;

  //TODO 1) change lock granularity to single block; 2) writeSingleBlock can be paralleled

  // first write
  string filepath = PRIMARY_FILE_DIR.data() + std::to_string(block_num) + ".file";
  int write_len = content_len <= BLOCK_SIZE - offset? content_len : BLOCK_SIZE - offset;
  Util::writeSingleBlock(filepath, offset, content.c_str(), 0, write_len);

  // write to subsequent blocks
  while (content_len - write_len >= BLOCK_SIZE) {
      string filepath = PRIMARY_FILE_DIR.data() + std::to_string(++block_num) + ".file";
      Util::writeSingleBlock(filepath, 0, content.c_str(), write_len, write_len + BLOCK_SIZE);
      write_len += BLOCK_SIZE;
  }

  // process the remaining contents
  if (write_len < content_len) {
      string filepath = PRIMARY_FILE_DIR.data() + std::to_string(++block_num) + ".file";
      Util::writeSingleBlock(filepath, 0, content.c_str(), write_len, content_len);
  }

  // Sync to backup
  int32_t res = toBackupClient_->sync(addr, content);
  if (res == -1) {
    // backup out-of-date, abort
    std::cout << "write failed due to sync error." << std::endl;
    return -1;
  }

  std::cout << "write success\n" << std::endl;

  return 0;
}

}
