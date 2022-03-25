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