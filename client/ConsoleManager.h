#pragma once
#include <iostream>
#include <cstdlib>

#ifdef __linux__

#include <sys/ioctl.h>
#include <unistd.h>
#define cls system("clear");

#elif defined(_WIN32) || defined(_WIN64)
#include <Windows.h>
	#include <io.h>
	#include <fcntl.h>
	#define cls system("cls");
#endif


namespace ConsoleAttributes
{
    struct Coord
    {
        float X, Y;
    };

    struct Color
    {
        enum  Index
        {
            RED		= 31,
            GREEN	= 32,
            YELLOW	= 33,
            BLUE	= 34,
            MAGENTA = 35,
            CYAN	= 36,
            DEFAULT	= 39,
        };

    };

    auto setConsoleColor(Color::Index colorIndex) -> void;

    auto SetConsoleCursor(Coord& coord)			  -> void;

    auto currentConsoleWidth()					  -> unsigned;

};