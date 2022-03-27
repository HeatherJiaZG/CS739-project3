project(cs739-p3-block-store)

find_package(Threads REQUIRED)

#-----------------------------------------------------------------------------------------------------------------------
# Generate .h .cpp source from .thrift
#-----------------------------------------------------------------------------------------------------------------------

add_custom_command(
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/service.thrift
        OUTPUT
        ${CMAKE_CURRENT_SOURCE_DIR}/gen-cpp/Client.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/gen-cpp/Client.h
        ${CMAKE_CURRENT_SOURCE_DIR}/gen-cpp/Client_server.skeleton.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/gen-cpp/PrimaryBackup.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/gen-cpp/PrimaryBackup.h
        ${CMAKE_CURRENT_SOURCE_DIR}/gen-cpp/PrimaryBackup_server.skeleton.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/gen-cpp/service_types.h
        COMMAND thrift_executable --gen cpp -r -o ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/service.thrift)


#-----------------------------------------------------------------------------------------------------------------------
# Generated .cpp source
#-----------------------------------------------------------------------------------------------------------------------

add_library(thrift_server_gen STATIC
        ${CMAKE_CURRENT_SOURCE_DIR}/gen-cpp/Client.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/gen-cpp/Client.h
        ${CMAKE_CURRENT_SOURCE_DIR}/gen-cpp/Client_server.skeleton.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/gen-cpp/PrimaryBackup.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/gen-cpp/PrimaryBackup.h
        ${CMAKE_CURRENT_SOURCE_DIR}/gen-cpp/PrimaryBackup_server.skeleton.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/gen-cpp/service_types.h)

target_link_libraries(thrift_server_gen PUBLIC thrift_static)
target_link_libraries(thrift_server_gen PUBLIC boost)

#-----------------------------------------------------------------------------------------------------------------------
# Thrift handlers
#-----------------------------------------------------------------------------------------------------------------------

add_library(client_handler STATIC
        ClientHandler.cpp ClientHandler.h
        Util.cpp Util.h
        Config.h
        rwlock.h)

target_link_libraries(client_handler PUBLIC thrift_server_gen)
target_link_libraries(client_handler PUBLIC fmt::fmt)
target_link_libraries(client_handler PUBLIC Threads::Threads)

add_library(primary_backup_handler STATIC
        PrimaryBackupHandler.cpp PrimaryBackupHandler.h
        Util.cpp Util.h
        Config.h
        rwlock.h)

target_link_libraries(primary_backup_handler PUBLIC thrift_server_gen)
target_link_libraries(primary_backup_handler PUBLIC fmt::fmt)
target_link_libraries(primary_backup_handler PUBLIC Threads::Threads)

#-----------------------------------------------------------------------------------------------------------------------
# Executables
#-----------------------------------------------------------------------------------------------------------------------

add_executable(primary_main
        PrimaryMain.cpp)

target_link_libraries(primary_main PUBLIC client_handler)

add_executable(backup_main
        BackupMain.cpp)

target_link_libraries(backup_main PUBLIC primary_backup_handler)

add_executable(client_main
        ClientMain.cpp
        Config.h)

target_link_libraries(client_main PUBLIC client_handler)
target_link_libraries(client_main PUBLIC Threads::Threads)
