//
// Created by Yifei Yang on 3/27/22.
//

#include "ClientHandler.h"
#include "Config.h"
#include "rwlock.h"
#include <iostream>
#include <fstream>
#include <fcntl.h>
#include <unistd.h>

using namespace std;

namespace block_store {

ClientHandler::ClientHandler() {
  // remove all contents in central storage to init
  ofstream outfile;
  outfile.open(CENTRAL_STORAGE, ofstream::out | ofstream::trunc);
  outfile.close();
}

void ClientHandler::read(std::string& _return, const int64_t addr) {
  int fd;
  off_t offset = addr;
  char readbuf[BLOCK_SIZE];

  ReadLock r_lock(rwLock);

  if ((fd = open(CENTRAL_STORAGE.data(), O_RDONLY)) == -1) {
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

  if ((fd = open(CENTRAL_STORAGE.data(), O_WRONLY)) == -1) { // TODO optimization: make fd a global var
    perror("open error");
    exit(1);
  }
  auto nBytes = pwrite(fd, writebuf, BLOCK_SIZE, offset);
  close(fd);
  // TODO: write to backup here
  cout<<"write success\n"<<endl;

  return (int32_t) nBytes;
}

}
