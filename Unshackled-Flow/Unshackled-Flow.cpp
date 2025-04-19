// Unshackled-Flow.cpp : Ce fichier contient la fonction 'main'. L'exécution du programme commence et se termine à cet endroit.
//

#include <iostream>
#ifdef _DEBUG
#include <windows.h> 
#endif // _DEBUG

#include "grid.hpp"

int main()
{
    grid* topleftcorner = new grid();
    grid::init(topleftcorner, 10, 15);
    grid::populate(topleftcorner, 10);
    grid* curcell = topleftcorner;
#ifdef _DEBUG
    //int x = 1;
    //int y = 1;
    //curcell = topleftcorner;
    //while (nullptr != curcell)
    //{
    //    grid* firstin = curcell;
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

    curcell = topleftcorner;
    while (nullptr != curcell)
    {
        grid* firstin = curcell;
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

    delete topleftcorner;
}

// Exécuter le programme : Ctrl+F5 ou menu Déboguer > Exécuter sans débogage
// Déboguer le programme : F5 ou menu Déboguer > Démarrer le débogage

// Astuces pour bien démarrer : 
//   1. Utilisez la fenêtre Explorateur de solutions pour ajouter des fichiers et les gérer.
//   2. Utilisez la fenêtre Team Explorer pour vous connecter au contrôle de code source.
//   3. Utilisez la fenêtre Sortie pour voir la sortie de la génération et d'autres messages.
//   4. Utilisez la fenêtre Liste d'erreurs pour voir les erreurs.
//   5. Accédez à Projet > Ajouter un nouvel élément pour créer des fichiers de code, ou à Projet > Ajouter un élément existant pour ajouter des fichiers de code existants au projet.
//   6. Pour rouvrir ce projet plus tard, accédez à Fichier > Ouvrir > Projet et sélectionnez le fichier .sln.
