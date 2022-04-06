//
// Created by Yifei Yang on 3/27/22.
//

#include "Util.h"
#include "Config.h"
#include <iostream>
#include <fstream>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <filesystem>

using namespace std;

namespace block_store {

void Util::initFile(const std::string &path) {
  ofstream outfile;
  outfile.open(path, ofstream::out | ofstream::trunc);
  outfile.close();
}

void Util::initDir(const std::string &path) {
    string command;
    command = "mkdir -p " + path;
    system(command.c_str());
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

std::string Util::execCmd(const char *cmd) {
  char buffer[128];
  string result;
  FILE* pipe = popen(cmd, "r");
  while (fgets(buffer, sizeof buffer, pipe) != nullptr) {
    result += buffer;
  }
  pclose(pipe);
  return result;
}

std::string Util::getLocalIp() {
  return execCmd("curl -s ifconfig.me");
}

}
