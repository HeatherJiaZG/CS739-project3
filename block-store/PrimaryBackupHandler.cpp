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

  cout<<"backup write success\n"<<endl;

  return 0;
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
