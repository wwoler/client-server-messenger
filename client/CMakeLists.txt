cmake_minimum_required(VERSION 3.24)
project(clientt)

set(CMAKE_CXX_STANDARD 17)

include_directories(.)

add_executable(clientt
        Chat.cpp
        Chat.h
        client.cpp
        ConsoleManager.cpp
        ConsoleManager.h
        Message.cpp
        Message.h
        Singleton.h
        User.cpp
        User.h)
