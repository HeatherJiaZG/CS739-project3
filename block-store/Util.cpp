//
// Created by Yifei Yang on 3/27/22.
//

#include "Util.h"
#include <iostream>
#include <fstream>

using namespace std;

namespace block_store {

void Util::initFile(const std::string &path) {
  ofstream outfile;
  outfile.open(path, ofstream::out | ofstream::trunc);
  outfile.close();
}

}
