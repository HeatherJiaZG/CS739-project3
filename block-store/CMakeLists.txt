project(cs739-p3-block-store)

find_package(Threads REQUIRED)

#-----------------------------------------------------------------------------------------------------------------------
# Generate .h .cpp source from .thrift
#-----------------------------------------------------------------------------------------------------------------------

add_custom_command(
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/service.thrift
        OUTPUT
        ${CMAKE_CURRENT_SOURCE_DIR}/gen-cpp/StoreServer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/gen-cpp/StoreServer.h
        ${CMAKE_CURRENT_SOURCE_DIR}/gen-cpp/StoreServer_server.skeleton.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/gen-cpp/service_types.h
        COMMAND thrift_executable --gen cpp -r -o ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/service.thrift)


#-----------------------------------------------------------------------------------------------------------------------
# Generated .cpp source
#-----------------------------------------------------------------------------------------------------------------------

add_library(thrift_server_gen STATIC
        ${CMAKE_CURRENT_SOURCE_DIR}/gen-cpp/StoreServer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/gen-cpp/StoreServer.h
        ${CMAKE_CURRENT_SOURCE_DIR}/gen-cpp/StoreServer_server.skeleton.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/gen-cpp/service_types.h)

target_link_libraries(thrift_server_gen PUBLIC thrift_static)
target_link_libraries(thrift_server_gen PUBLIC boost)

#-----------------------------------------------------------------------------------------------------------------------
# Thrift handlers
#-----------------------------------------------------------------------------------------------------------------------

add_library(store_server_handler STATIC
        StoreServerHandler.cpp StoreServerHandler.h
        Util.cpp Util.h
        Config.h
        rwlock.h)

target_link_libraries(store_server_handler PUBLIC thrift_server_gen)
target_link_libraries(store_server_handler PUBLIC fmt::fmt)
target_link_libraries(store_server_handler PUBLIC Threads::Threads)

#-----------------------------------------------------------------------------------------------------------------------
# Executables
#-----------------------------------------------------------------------------------------------------------------------

add_executable(server_main
        ServerMain.cpp)

target_link_libraries(server_main PUBLIC store_server_handler)

add_executable(client_main
        ClientMain.cpp)

target_link_libraries(client_main PUBLIC store_server_handler)
