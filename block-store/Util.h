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
  static std::string getFilename(const int addr);

};

}


#endif //CS739_P3_BLOCK_STORE_UTIL_H
