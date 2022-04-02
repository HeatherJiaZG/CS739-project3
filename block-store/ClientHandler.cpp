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
#include <filesystem>
#include "map"

using namespace std;

namespace block_store {

ClientHandler::ClientHandler() {
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
  cout<<"read success"<<endl;
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
    // backup out-of-date, sync files to backup
    sync_files();
    std::cout << "sync files to backup succeed" << std::endl;
    return -1;
  }

  std::cout << "write success" << std::endl;

  return 0;
}

void ClientHandler::sync_files() {
  // collect file names
  std::vector<std::string> fileNames;
  for (const auto &entry: std::filesystem::directory_iterator(PRIMARY_FILE_DIR)) {
    fileNames.emplace_back(entry.path().filename());
  }

  // get timestamps on primary
  std::map<std::string, int64_t> primary_timestamps;
  for (const auto &fileName: fileNames) {
    auto filePath = PRIMARY_FILE_DIR.data() + fileName;
    auto timestamp = Util::getTimestamp(filePath);
    primary_timestamps.emplace(filePath, timestamp);
  }

  // get timestamp on backup
  std::map<std::string, int64_t> backup_timestamps;
  toBackupClient_->get_timestamps(backup_timestamps, fileNames);

  // check timestamps, and collect files needed to be synced
  std::map<std::string, std::string> files_to_sync;
  for (const auto &primary_timestamps_it: primary_timestamps) {
    auto fileName = primary_timestamps_it.first;
    auto primary_timestamp = primary_timestamps_it.second;
    auto backup_timestamp = backup_timestamps.find(fileName)->second;

    if (primary_timestamp > backup_timestamp) {
      // read file
      auto filePath = PRIMARY_FILE_DIR.data() + fileName;
      std::ifstream fs(filePath);
      std::stringstream ss;
      ss << fs.rdbuf();
      files_to_sync.emplace(fileName, ss.str());
    }
  }

  // sync files
  toBackupClient_->sync_files(files_to_sync);
}

}
