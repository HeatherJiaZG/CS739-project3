cmake_minimum_required(VERSION 3.21)
project(cs739-p3)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_LIST_DIR}/cmake")
set(DEPS_PREFIX "_deps")

#-----------------------------------------------------------------------------------------------------------------------
# Set openssl path for mac os
#-----------------------------------------------------------------------------------------------------------------------

if (${APPLE})
  set(OPENSSL_ROOT_DIR /usr/local/opt/openssl)
  # This is a bug in CMake that causes it to prefer the system version over
  # the one in the specified ROOT folder.
  set(OPENSSL_CRYPTO_LIBRARY ${OPENSSL_ROOT_DIR}/lib/libcrypto.dylib CACHE FILEPATH "" FORCE)
  set(OPENSSL_SSL_LIBRARY ${OPENSSL_ROOT_DIR}/lib/libssl.dylib CACHE FILEPATH "" FORCE)
endif()

#-----------------------------------------------------------------------------------------------------------------------
# External libraries
#-----------------------------------------------------------------------------------------------------------------------

include(External_boost)
include(External_fmt)
include(External_thrift)

#-----------------------------------------------------------------------------------------------------------------------
# Modules
#-----------------------------------------------------------------------------------------------------------------------

add_subdirectory(block-store)