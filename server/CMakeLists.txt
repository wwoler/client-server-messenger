cmake_minimum_required(VERSION 3.24 FATAL_ERROR)
project(server)

set(CMAKE_CXX_STANDARD 17)

set(JSON_BuildTests OFF CACHE INTERNAL "")

add_executable(server
        client.cpp
        client.h
        Connection.cpp
        Connection.h
        DataBase.cpp
        DataBase.h
        main.cpp
        Message.cpp
        Message.h
        Server.cpp
        Server.h
        Singleton.h
        User.cpp
        User.h)

add_subdirectory(external/json)

target_link_libraries(server nlohmann_json)
