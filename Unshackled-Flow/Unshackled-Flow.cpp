// Unshackled-Flow.cpp : Ce fichier contient la fonction 'main'. L'exécution du programme commence et se termine à cet endroit.
//

#include <iostream>
#ifdef _DEBUG
#include <windows.h> 
#endif // _DEBUG

#include "grid.hpp"

int main()
{
    //grid* topleftcorner = new grid();
    //grid::init(topleftcorner, 10, 15);
    //grid::populate(topleftcorner, 10);
    const auto topleftcorner = std::make_unique<const grid>(10, 15, 10);
    //grid* topleftcorner = new grid(10, 15, 10);
#ifdef _DEBUG
    auto curcell = topleftcorner.get();

    //int x = 1;
    //int y = 1;
    //while (nullptr != curcell)
    //{
    //    auto firstin = curcell;
    //    std::cout << y << std::endl;
    //    x = 1;
    //    while (nullptr != curcell)
    //    {
    //        std::cout << x << " ";
    //        ++x;
    //        curcell = curcell->right;
    //    }
    //    std::cout << std::endl;
    //    curcell = firstin->bottom;
    //    ++y;
    //}
    //std::cout << std::endl;

    //curcell = topleftcorner.get();

    while (nullptr != curcell)
    {
        auto firstin = curcell;
        while (nullptr != curcell)
        {
            std::cout << "[";
            HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
            if (0 != curcell->colour)
            {
                PCONSOLE_SCREEN_BUFFER_INFO consoleSBF = new CONSOLE_SCREEN_BUFFER_INFO;
                GetConsoleScreenBufferInfo(hConsole, consoleSBF);
                SetConsoleTextAttribute(hConsole, curcell->colour);
                std::cout << char(1);
                SetConsoleTextAttribute(hConsole, consoleSBF->wAttributes);
            }
            else
            {
                std::cout << " ";
            }
            std::cout << "]";
            curcell = curcell->right;
        }
        std::cout << std::endl;
        curcell = firstin->bottom;
    }
    std::cout << std::endl;
#endif // _DEBUG

    //delete topleftcorner;
}
