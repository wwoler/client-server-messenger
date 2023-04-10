#pragma once

#include "Connection.h"
#include <cstdint>
#include <list>

class TcpServer final
{
public:
    friend class Connection;
    TcpServer(int port, DataBase* db);
    ~TcpServer();

private:
    DataBase*                                               _data_base;
    bool                                                    _serverStatus;
    int                                                     _server_socket;
    int                                                     _server_port;
    std::thread                                             _connectionHandlerThread;
    std::list<std::unique_ptr<Connection> >                 _connections;

    void createConnection();
    void connectionHandlerFunc();
    void join();

public:
    void start();
    void stop();
};
