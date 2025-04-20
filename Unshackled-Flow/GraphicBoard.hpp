
#ifdef _DEBUG
#include <windows.h> 
#endif // _DEBUG

#define SDL_MAIN_HANDLED
//#include <SDL2/SDL.h>
#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include "grid.hpp"
#include "math.h"

class GraphicBoard {
private:
	void Init();
	void ThrowException(const int i);
	void Refresh();
	void DrawGrid(int gHeight, int gWidth);
public:
	GraphicBoard();
	~GraphicBoard();
	void Loop();
private:
	SDL_DisplayMode DisplayMode;
	SDL_Window* window;
	SDL_Renderer* renderer;
	int Height, Width;
	grid* Grille;
	SDL_Event exitEvent;
};