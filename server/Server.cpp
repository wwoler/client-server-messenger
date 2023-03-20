#include "Server.h"
#include <future>
#include <stdexcept>

TcpServer::TcpServer(int port,  DataBase* db):
    _server_port(port),
    _data_base(db),
    _server_socket(-1),
    _serverStatus(true) {}

TcpServer::~TcpServer()
{
    for(auto const& item: _connections)
    {
        item->stop();
    }
    close(_server_socket);
}

void TcpServer::start()
{
    _server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if ( _server_socket == -1)
    {
        close(_server_socket);
        throw 0;   
    }

    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));

    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(_server_port);
    serverAddr.sin_family = AF_INET;
    
    if (bind(_server_socket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1)
    {
        close(_server_socket);
        throw std::runtime_error("bind error");
    }

    if(listen(_server_socket, 10) == -1)
    {
        close(_server_socket);
        throw std::runtime_error("listen error");
    }
       
    std::cout << "Server is UP and waiting for connections" << std::endl;

    _connectionHandlerThread = std::thread(&TcpServer::connectionHandlerFunc, this);
}


void TcpServer::createConnection()
{
    sockaddr_in client;
    memset(&client, 0, sizeof(client));

    socklen_t sizeOfAddr = sizeof(client);
    int connection;

    if((connection = accept(_server_socket, (struct sockaddr*)&client, &sizeOfAddr)) == -1)
    {
        std::cout << "Connection Closed\a\n";
        return;
    }
    std::cout << "Client connected\n";

    auto* newConnection =
        new Connection(new Client(connection, client), _data_base);

    _connections.emplace_back(newConnection);

    newConnection->start();

}


void TcpServer::connectionHandlerFunc()
{
    while(_serverStatus)
    {
        createConnection();

        for(auto i = _connections.begin(); i != _connections.end(); )
        {
            if((*i)->getConnectionStatus() == Client::CONNECTION_STATUS::DOWN)
            {
                (*i)->stop();
                i = _connections.erase(i);
                continue;
            }
            ++i;
        }
    }
}    


void TcpServer::stop()
{
    _serverStatus = false;
    shutdown(_server_socket, 0);
    join();
    std::cout << "uwu\n";
}

void TcpServer::join()
{
    if(_connectionHandlerThread.joinable())
        _connectionHandlerThread.join();
}


