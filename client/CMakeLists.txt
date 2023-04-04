cmake_minimum_required(VERSION 3.24)
project(client)

set(CMAKE_CXX_STANDARD 17)

add_executable(client
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
