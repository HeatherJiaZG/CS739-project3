#ifndef BLOCK_STORE_CONFIG_H
#define BLOCK_STORE_CONFIG_H

#include <string>

std::string PRIMARY_SERVER_HOSTNAME = "localhost";
std::string BACKUP_SERVER_HOSTNAME = "localhost";
int PRIMARY_SERVER_PORT = 9090;
int BACKUP_SERVER_PORT = 9091;

std::string CENTRAL_STORAGE = "/tmp/central_storage"
size_t BLOCK_SIZE = 4096 // 4KB in bytes

#endif