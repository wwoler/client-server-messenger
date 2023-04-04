#include <iostream>
#include "Server.h"

auto main() ->int
{
    setlocale(LC_ALL, "");
    auto& db = DataBase::get_instance();
    TcpServer serv(55555, &db);

    try
    {
        serv.start();
    }
    catch (std::exception const& ex)
    {
        std::cout << ex.what() << std::endl;
    }

    std::string stopWord;
    std::cout << "Enter 'stop' to stop the server\n";
    while(stopWord != "stop") std::cin >> stopWord;

    serv.stop();

    return 0;

}