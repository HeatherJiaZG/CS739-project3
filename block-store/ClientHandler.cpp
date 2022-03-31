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
  char readbuf[BLOCK_SIZE];

  ReadLock r_lock(rwLock);

  string filename = Util::getFilename(addr);
  string filepath = "/tmp/primary/" + filename + ".file";
  if ((fd = open(filepath.c_str(), O_RDONLY)) == -1) {
      perror("open error");
      exit(1);
  }
  cout << readbuf << endl;
  pread(fd, &readbuf, BLOCK_SIZE, 0);
  _return = std::string(readbuf);
  cout << _return << endl;
  close(fd);
  cout<<"read success\n"<<endl;
}

int32_t ClientHandler::write(const int64_t addr, const std::string& content) {
  int fd;
  off_t offset = addr;
  char writebuf[BLOCK_SIZE] = {};

  WriteLock w_lock(rwLock);

  string filename = Util::getFilename(addr);
  string filepath = "/tmp/primary/" + filename + ".file";

  if ((fd = open(filepath.c_str(), O_WRONLY)) == -1) {
      Util::initFile(filepath);
      fd = open(filepath.c_str(), O_WRONLY);
  }

  strcat(writebuf, content.c_str());
  int len = strlen(writebuf);
  // here we do not consider the offset is 10 and the buf size is 4k, which means we need to write 2 files
  auto nBytes = pwrite(fd, writebuf, len, 0);
  close(fd);

//  // Sync to backup
//  int32_t res = toBackupClient_->sync(addr, content);
//  if (res == -1) {
//    // backup out-of-date, sync entire storage
//    std::ifstream f(PRIMARY_CENTRAL_STORAGE);
//    std::stringstream ss;
//    ss << f.rdbuf();
//    toBackupClient_->sync_entire(ss.str());
//  }

  cout<<"write success\n"<<endl;

  return (int32_t) nBytes;
}

}
