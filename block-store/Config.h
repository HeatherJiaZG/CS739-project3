//
// Created by Yifei Yang on 3/27/22.
//

#ifndef CS739_P3_BLOCK_STORE_CONFIG_H
#define CS739_P3_BLOCK_STORE_CONFIG_H

#include <string>

namespace block_store {

constexpr static std::string_view PRIMARY_SERVER_HOSTNAME = "localhost";
constexpr static std::string_view BACKUP_SERVER_HOSTNAME = "localhost";
constexpr static int PRIMARY_SERVER_PORT = 9090;
constexpr static int BACKUP_SERVER_PORT = 9091;

constexpr static std::string_view PRIMARY_CENTRAL_STORAGE = "/tmp/primary_central_storage";
constexpr static std::string_view BACKUP_CENTRAL_STORAGE = "/tmp/backup_central_storage";

constexpr static std::string_view PRIMARY_FILE_DIR = "/tmp/primary/";
constexpr static std::string_view BACKUP_FILE_DIR = "/tmp/backup/";

constexpr static size_t BLOCK_SIZE = 4096; // 4KB in bytes

}

#endif //CS739_P3_BLOCK_STORE_CONFIG_H
