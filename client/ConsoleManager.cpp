#include "ConsoleManager.h"


auto ConsoleAttributes::setConsoleColor(Color::Index colorIndex) -> void
{
    std::wcout << "\033[" << colorIndex << 'm';
}

auto ConsoleAttributes::SetConsoleCursor(Coord& coord) -> void
{
    std::wcout << "\033[" << coord.Y + 1 << ';' << coord.X + 1 << 'H';
}

auto ConsoleAttributes::currentConsoleWidth() -> unsigned
{
    unsigned width = 0;

#ifdef __linux__
    struct winsize win{};
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &win);
    width = win.ws_col;

#elif defined(_WIN32) || defined(_WIN64)
    CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	width = csbi.dwSize.X;

#endif
    return width;
}