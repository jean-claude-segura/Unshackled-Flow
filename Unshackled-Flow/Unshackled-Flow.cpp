#include "GraphicBoard.hpp"
#include <windows.h>

int main(int argc, char* argv[])
{
	try
	{
		// https://stackoverflow.com/questions/31162367/significance-of-ios-basesync-with-stdiofalse-cin-tienull
		std::ios_base::sync_with_stdio(false);
		std::cin.tie(NULL);
#ifndef _DEBUG
		ShowWindow(GetConsoleWindow(), SW_HIDE);
#else
		ShowWindow(GetConsoleWindow(), SW_SHOW);
#endif
		GraphicBoard plateau;
		plateau.Loop();
	}
	catch (...)
	{
	}

#ifndef _DEBUG
	ShowWindow(GetConsoleWindow(), SW_SHOW);
#endif
	return 0;
}
