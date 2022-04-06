//
// Created by Yifei Yang on 3/27/22.
//

#ifndef CS739_P3_BLOCK_STORE_CONFIG_H
#define CS739_P3_BLOCK_STORE_CONFIG_H

#include <string>

namespace block_store {

constexpr static std::string_view SERVER_IP1 = "128.105.144.168";
constexpr static std::string_view SERVER_IP2 = "128.105.144.142";
constexpr static int SERVER_PORT = 9090;
constexpr static std::string_view STORE_DIR = "/tmp/block_store/";
constexpr static size_t BLOCK_SIZE = 4096; // 4KB in bytes
constexpr static int RECONNECT_GAP = 1000;

}

#endif //CS739_P3_BLOCK_STORE_CONFIG_H
