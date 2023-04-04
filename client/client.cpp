#include "Chat.h"

int main()
{
    setlocale(LC_ALL, "");
    auto& chat =  Chat::get_instance();

    try
    {
        chat.run();
    } catch(std::exception const& ex) {
        std::cout << ex.what() << std::endl;
    }

    return 0;

}

