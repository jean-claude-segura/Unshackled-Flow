
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
#include <map>
#include <tuple>
#include <array>

class RedGreenBlue {
private:
	Uint8 red;
	Uint8 green;
	Uint8 blue;
public:
	constexpr RedGreenBlue() : red(0), green(0), blue(0) {}
	constexpr RedGreenBlue(Uint8 _red, Uint8 _green, Uint8 _blue) : red(_red), green(_green), blue(_blue) {}
	Uint8 getRed() const { return red; }
	Uint8 getGreen() const { return green; }
	Uint8 getBlue() const { return blue; }
};

class GraphicBoard {
public:
	static constexpr std::array<RedGreenBlue, 16> consoleColours{
		RedGreenBlue{ 0, 0, 0 }, // 0 : BLACK
		RedGreenBlue{ 0,0,139 }, // 1 : DARK_BLUE 
		RedGreenBlue{ 0,139,0 }, // 2 : DARK_GREEN 
		RedGreenBlue{ 0,139,139 }, // 3 : DARK_CYAN
		RedGreenBlue{ 139,0,0 }, // 4 : DARK_RED 
		RedGreenBlue{ 139,0,139 }, // 5 : DARK_MAGENTA
		RedGreenBlue{ 139,139,0 }, // 6 : DARK_YELLOW  (rgb(139, 128, 0))
		RedGreenBlue{ 139,139,139 }, // 7 : DARK_WHITE
		RedGreenBlue{ 34,32,36 }, // 8 : BRIGHT_BLACK 
		RedGreenBlue{ 0,0,255 }, // 9 : BRIGHT_BLUE 
		RedGreenBlue{ 0,255,0 }, // A : BRIGHT_GREEN 
		RedGreenBlue{ 0,255,255 }, // B : BRIGHT_CYAN 
		RedGreenBlue{ 255,0,0 }, // C : BRIGHT_RED 
		RedGreenBlue{ 255,0,255 }, // D : BRIGHT_MAGENTA
		RedGreenBlue{ 255,255,0 }, // E : BRIGHT_YELLOW 
		RedGreenBlue{ 255,255,255 }, // F : WHITE
	};

	static constexpr std::array<RedGreenBlue, 12> arrColours{
		RedGreenBlue{ 0, 0, 0 }, // BLACK
		RedGreenBlue{ 139,0,139 }, // DARK_MAGENTA
		RedGreenBlue{ 0,139,0 }, // DARK_GREEN
		RedGreenBlue{ 139,0,0 }, // DARK_RED
		RedGreenBlue{ 255,255,255 }, // WHITE 
		RedGreenBlue{ 0,0,255 }, // BRIGHT_BLUE 
		RedGreenBlue{ 0,255,0 }, // BRIGHT_GREEN 
		RedGreenBlue{ 0,255,255 }, // BRIGHT_CYAN 
		RedGreenBlue{ 255,0,0 }, // BRIGHT_RED 
		RedGreenBlue{ 255,0,255 }, // BRIGHT_MAGENTA
		RedGreenBlue{ 255,255,0 }, // BRIGHT_YELLOW 
	};
private:
	void Init();
	void ThrowException(const int i);
	void Refresh();
	void DrawGrid();
	grid* GetCell(int x, int y);
	void FillFlow(int x, int y, int xprev, int yprev);
	void PutInFlow(int& x, int& y);
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
	int side;
	int gHeight, gWidth, clientHeight, clientWidth;
	double Ratio = 0.7;
};
