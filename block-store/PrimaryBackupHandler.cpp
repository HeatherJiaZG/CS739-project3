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

using namespace std;

namespace block_store {

PrimaryBackupHandler::PrimaryBackupHandler():
  synced_(false) {
  // remove all contents in central storage to init
  Util::initFile(BACKUP_CENTRAL_STORAGE.data());
}

int32_t PrimaryBackupHandler::heartbeat(const int32_t msg) {
  return 0;
}

int32_t PrimaryBackupHandler::sync(const int64_t addr, const std::string& content) {
  // if backup store is not synced to primary
  if (!synced_) {
    return -1;
  }

  int fd;
  off_t offset = addr;
  char writebuf[BLOCK_SIZE];

  WriteLock w_lock(rwLock);

  if ((fd = open(BACKUP_CENTRAL_STORAGE.data(), O_WRONLY)) == -1) { // TODO optimization: make fd a global var
    perror("open error");
    exit(1);
  }
  auto nBytes = pwrite(fd, writebuf, BLOCK_SIZE, offset);
  close(fd);

  cout<<"backup write success\n"<<endl;

  return (int32_t) nBytes;
}

int32_t PrimaryBackupHandler::sync_entire(const std::string& content) {
  // re-init central storage
  Util::initFile(BACKUP_CENTRAL_STORAGE.data());

  // write entire content to file
  WriteLock w_lock(rwLock);

  ofstream file;
  file.open(BACKUP_CENTRAL_STORAGE);
  file << content;
  file.close();

  cout<<"backup write-entire success\n"<<endl;

  return (int32_t) content.size();
}

}
