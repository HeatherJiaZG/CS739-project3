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

  if ((fd = open(PRIMARY_CENTRAL_STORAGE.data(), O_RDONLY)) == -1) {
    perror("open error");
    exit(1);
  }
  pread(fd, &readbuf, BLOCK_SIZE, offset);
  _return = std::string(readbuf);
  close(fd);
  cout<<"read success\n"<<endl;
}

int32_t ClientHandler::write(const int64_t addr, const std::string& content) {
  int fd;
  off_t offset = addr;
  char writebuf[BLOCK_SIZE];

  WriteLock w_lock(rwLock);

  if ((fd = open(PRIMARY_CENTRAL_STORAGE.data(), O_WRONLY)) == -1) { // TODO optimization: make fd a global var
    perror("open error");
    exit(1);
  }
  auto nBytes = pwrite(fd, writebuf, BLOCK_SIZE, offset);
  close(fd);

  // Sync to backup
  int32_t res = toBackupClient_->sync(addr, content);
  if (res == -1) {
    // backup out-of-date, sync entire storage
    std::ifstream f(PRIMARY_CENTRAL_STORAGE);
    std::stringstream ss;
    ss << f.rdbuf();
    toBackupClient_->sync_entire(ss.str());
  }

  cout<<"write success\n"<<endl;

  return (int32_t) nBytes;
}

}
