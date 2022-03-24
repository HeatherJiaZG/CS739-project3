#/bin/bash

sudo apt update && sudo apt install thrift-compiler
thrift --gen cpp service.thrift
