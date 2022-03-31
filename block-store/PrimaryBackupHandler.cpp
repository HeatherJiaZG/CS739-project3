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

PrimaryBackupHandler::PrimaryBackupHandler()
{
  // remove all contents in central storage to init
  // Util::initFile(BACKUP_CENTRAL_STORAGE.data());
}

int32_t PrimaryBackupHandler::heartbeat(const int32_t msg) {
  return 0;
}

int32_t PrimaryBackupHandler::sync(const int64_t addr, const std::string& content) {
  // if backup store is not synced to primary
//  if (!synced_) {
//    return -1;
//  }

  int fd;
  char writebuf[BLOCK_SIZE] = {};
  string filename = BACKUP_FILE_DIR.data() + Util::getFilename(addr) + ".file";
  std::cout << "write to " << filename << std::endl;

  if ((fd = open(filename.c_str(), O_WRONLY)) == -1) {
      Util::initFile(filename);
      fd = open(filename.c_str(), O_WRONLY);
  }

  strcat(writebuf, content.c_str());
  int len = strlen(writebuf);
  // here we do not consider the offset is 10 and the buf size is 4k, which means we need to write 2 files
  auto nBytes = pwrite(fd, writebuf, len, 0);
  close(fd);

  cout<<"backup write success\n"<<endl;

  return (int32_t) nBytes;
}

int32_t PrimaryBackupHandler::sync_entire(const std::string& content) {
//  // re-init central storage
//  Util::initFile(BACKUP_CENTRAL_STORAGE.data());
//
//  // write entire content to file
//  WriteLock w_lock(rwLock);
//
//  ofstream file;
//  file.open(BACKUP_CENTRAL_STORAGE.data());
//  file << content;
//  file.close();
//
//  cout<<"backup write-entire success\n"<<endl;
//
//  return (int32_t) content.size();
return 0;
}

}
