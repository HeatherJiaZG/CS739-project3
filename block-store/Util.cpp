//
// Created by Yifei Yang on 3/27/22.
//

#include "Util.h"
#include "Config.h"
#include <iostream>
#include <fstream>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <fmt/format.h>
#include "sstream"
#include <sys/types.h>
#include <filesystem>

using namespace std;

namespace block_store {

void Util::initFile(const std::string &path) {
  ofstream outfile;
  outfile.open(path, ofstream::out | ofstream::trunc);
  outfile.close();
}

int Util::getFilename(const int addr) {
    int quotient = addr / BLOCK_SIZE;
    return quotient;
}

int Util::writeSingleBlock(const std::string& filepath, int offset, const char* content, int st, int ed) {
    int fd;
    if ((fd = open(filepath.c_str(), O_WRONLY)) == -1) {
        Util::initFile(filepath);
        fd = open(filepath.c_str(), O_WRONLY);
    }

    auto nBytes =  pwrite(fd, content + st, ed - st, offset);
    close(fd);

    return 0;
}

int64_t Util::getTimestamp(const std::string &filePath) {
  if (std::filesystem::exists(filePath)) {
    auto ftime = std::filesystem::last_write_time(filePath);
    return std::chrono::duration_cast<std::chrono::nanoseconds>(ftime.time_since_epoch()).count();
  } else {
    return 0;
  }
}

}
