//
// Created by Yifei Yang on 3/27/22.
//

#ifndef CS739_P3_BLOCK_STORE_UTIL_H
#define CS739_P3_BLOCK_STORE_UTIL_H

#include "string"

namespace block_store {

class Util {

public:
  static void initFile(const std::string &path);
  static void initDir(const std::string &path);
  static int getFilename(const int addr);
  static int writeSingleBlock(const std::string& filepath, int offset, const char* content, int st, int ed);
  static int64_t getTimestamp(const std::string &filePath);
};

}


#endif //CS739_P3_BLOCK_STORE_UTIL_H
