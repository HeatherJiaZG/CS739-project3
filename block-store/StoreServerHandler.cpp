//
// Created by Yifei Yang on 4/5/22.
//

#include "StoreServerHandler.h"
#include "Config.h"
#include "rwlock.h"
#include "Util.h"
#include <iostream>
#include <fstream>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <fmt/format.h>
#include "sstream"
#include <filesystem>
#include "map"
#include "thread"

using namespace std;

namespace block_store {

StoreServerHandler::StoreServerHandler() {
  Util::initDir(STORE_DIR.data());

  // get other ip
  auto localIp = Util::getLocalIp();
  if (localIp == SERVER_IP1) {
    otherIp_ = SERVER_IP2;
  } else {
    otherIp_ = SERVER_IP1;
  }

  connectedToOther_ = tryConnectToPrimary();

  if (connectedToOther_) {
    // this means connected to primary, then we keep pinging primary
    isPrimary_ = false;
    std::thread t(&StoreServerHandler::pingPrimary, this);
    t.detach();
  } else {
    // this means primary is myself, then we keep connecting backup
    isPrimary_ = true;
    std::thread t(&StoreServerHandler::connectToBackup, this);
    t.detach();
  }
}

void StoreServerHandler::read(std::string& _return, const int64_t addr) {
  if (!isPrimary_) {
    return;
  }

  int fd;
  int block_num = Util::getFilename(addr);
  int offset = addr - block_num * BLOCK_SIZE;
  int read_len = BLOCK_SIZE - offset;
  int second_read_len = BLOCK_SIZE-read_len; // if need a second read
  char readbuf1[BLOCK_SIZE] = {};
  char readbuf2[BLOCK_SIZE] = {};

  ReadLock r_lock(rwLock);

  // first read
  string filepath = STORE_DIR.data() + std::to_string(block_num) + ".file";
  if ((fd = open(filepath.c_str(), O_RDONLY)) == -1) {
    // TODO: open err or return empty block?
    perror("open error");
    exit(1);
  }
  pread(fd, &readbuf1, read_len, offset);
  close(fd);

  if(offset != 0) { // when need a second read
    filepath = STORE_DIR.data() + std::to_string(++block_num) + ".file";
    if ((fd = open(filepath.c_str(), O_RDONLY)) == -1) {
      perror("open error");
      exit(1);
    }
    pread(fd, &readbuf2, second_read_len, 0);
    close(fd);
  }

  _return = std::string(readbuf1, read_len) + std::string(readbuf2, second_read_len);
  cout<<"read success"<<endl;
}

int32_t StoreServerHandler::write(const int64_t addr, const std::string& content) {
  if (!isPrimary_) {
    return -1;
  }

  WriteLock w_lock(rwLock);

  int content_len = content.length();
  int block_num = Util::getFilename(addr);
  int offset = addr - block_num * BLOCK_SIZE;

  //TODO 1) change lock granularity to single block; 2) writeSingleBlock can be paralleled

  // first write
  string filepath = STORE_DIR.data() + std::to_string(block_num) + ".file";
  int write_len = content_len <= BLOCK_SIZE - offset? content_len : BLOCK_SIZE - offset;
  Util::writeSingleBlock(filepath, offset, content.c_str(), 0, write_len);

  // write to subsequent blocks
  while (content_len - write_len >= BLOCK_SIZE) {
    filepath = STORE_DIR.data() + std::to_string(++block_num) + ".file";
    Util::writeSingleBlock(filepath, 0, content.c_str(), write_len, write_len + BLOCK_SIZE);
    write_len += BLOCK_SIZE;
  }

  // process the remaining contents
  if (write_len < content_len) {
    filepath = STORE_DIR.data() + std::to_string(++block_num) + ".file";
    Util::writeSingleBlock(filepath, 0, content.c_str(), write_len, content_len);
  }

  // Sync to backup if connected
  if (connectedToOther_) {
    try {
      int32_t res = toOtherClient_->sync(addr, content);
      if (res == -1) {
        // backup out-of-date, sync files to backup
        syncFiles();
        std::cout << "sync files to backup succeed" << std::endl;
      }
    } catch (TException& tx) {
      std::cout << fmt::format("Fail to connect to backup server when sync: {}", tx.what()) << std::endl;
      connectedToOther_ = false;
      std::thread t(&StoreServerHandler::connectToBackup, this);
      t.detach();
    }
  }

  std::cout << "write success" << std::endl;

  return 0;
}

int32_t StoreServerHandler::sync(const int64_t addr, const std::string& content) {
  // if backup store is not synced to primary
  if (!isSynced_) {
    return -1;
  }

  int content_len = content.length();
  int block_num = Util::getFilename(addr);
  int offset = addr - block_num * BLOCK_SIZE;

  //TODO: writeSingleBlock can be paralleled

  // first write
  string filepath = STORE_DIR.data() + std::to_string(block_num) + ".file";
  int write_len = content_len <= BLOCK_SIZE - offset? content_len : BLOCK_SIZE - offset;
  Util::writeSingleBlock(filepath, offset, content.c_str(), 0, write_len);

  // write to subsequent blocks
  while (content_len - write_len >= BLOCK_SIZE) {
    filepath = STORE_DIR.data() + std::to_string(++block_num) + ".file";
    Util::writeSingleBlock(filepath, 0, content.c_str(), write_len, write_len + BLOCK_SIZE);
    write_len += BLOCK_SIZE;
  }

  // process the remaining contents
  if (write_len < content_len) {
    filepath = STORE_DIR.data() + std::to_string(++block_num) + ".file";
    Util::writeSingleBlock(filepath, 0, content.c_str(), write_len, content_len);
  }

  cout<<"backup write success"<<endl;

  return 0;
}

void StoreServerHandler::get_timestamps(std::map<std::string, int64_t> & _return,
                                          const std::vector<std::string> & primary_files) {
  for (const auto &fileName: primary_files) {
    auto filePath = STORE_DIR.data() + fileName;
    auto timestamp = Util::getTimestamp(filePath);
    _return.emplace(fileName, timestamp);
  }
}

void StoreServerHandler::sync_files(const std::map<std::string, std::string> & primary_files) {
  for (const auto &it: primary_files) {
    auto fileName = it.first;
    auto filePath = STORE_DIR.data() + fileName;
    auto content = it.second;

    ofstream fs;
    fs.open(filePath);
    fs << content;
    fs.close();
  }

  isSynced_ = true;
}

void StoreServerHandler::ping() {
  // noop, it's just a heartbeat from backup to make sure I (primary) am alive
}

void StoreServerHandler::pingPrimary() {
  while (true) {
    try {
      toOtherClient_->ping();
    } catch (TException& tx) {
      connectedToOther_ = false;
      isPrimary_ = true;
      isSynced_ = true;
      std::thread t(&StoreServerHandler::connectToBackup, this);
      t.detach();
      return;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(RECONNECT_GAP));
  }
}

void StoreServerHandler::syncFiles() {
  // collect file names
  std::vector<std::string> fileNames;
  for (const auto &entry: std::filesystem::directory_iterator(STORE_DIR)) {
    fileNames.emplace_back(entry.path().filename());
  }

  // get timestamps on primary
  std::map<std::string, int64_t> primary_timestamps;
  for (const auto &fileName: fileNames) {
    auto filePath = STORE_DIR.data() + fileName;
    auto timestamp = Util::getTimestamp(filePath);
    primary_timestamps.emplace(fileName, timestamp);
  }

  // get timestamp on backup
  std::map<std::string, int64_t> backup_timestamps;
  toOtherClient_->get_timestamps(backup_timestamps, fileNames);

  // check timestamps, and collect files needed to be synced
  std::map<std::string, std::string> files_to_sync;
  for (const auto &primary_timestamps_it: primary_timestamps) {
    auto fileName = primary_timestamps_it.first;
    auto primary_timestamp = primary_timestamps_it.second;
    auto backup_timestamp = backup_timestamps.find(fileName)->second;

    if (primary_timestamp > backup_timestamp) {
      // read file
      auto filePath = STORE_DIR.data() + fileName;
      std::ifstream fs(filePath);
      std::stringstream ss;
      ss << fs.rdbuf();
      files_to_sync.emplace(fileName, ss.str());
    }
  }

  // sync files
  toOtherClient_->sync_files(files_to_sync);
}

bool StoreServerHandler::tryConnectToPrimary() {
  toOtherSocket_ = std::make_shared<TSocket>(otherIp_.data(), SERVER_PORT);
  toOtherTransport_ = std::make_shared<TBufferedTransport>(toOtherSocket_);
  toOtherProtocol_ = std::make_shared<TBinaryProtocol>(toOtherTransport_);
  toOtherClient_ = std::make_shared<StoreServerClient>(toOtherProtocol_);
  try {
    toOtherTransport_->open();
    return true;
  } catch (TException& tx) {
    return false;
  }
}

void StoreServerHandler::connectToBackup() {
  WriteLock connect_lock(connectLock);

  if (connectedToOther_) {
    return;
  }

  while (true) {
    toOtherSocket_ = std::make_shared<TSocket>(otherIp_.data(), SERVER_PORT);
    toOtherTransport_ = std::make_shared<TBufferedTransport>(toOtherSocket_);
    toOtherProtocol_ = std::make_shared<TBinaryProtocol>(toOtherTransport_);
    toOtherClient_ = std::make_shared<StoreServerClient>(toOtherProtocol_);
    try {
      toOtherTransport_->open();
      connectedToOther_ = true;
      syncFiles();
      return;
    } catch (TException& tx) {
      connectedToOther_ = false;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(RECONNECT_GAP));
  }
}

}
