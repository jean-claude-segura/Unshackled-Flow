#include "GraphicBoard.hpp"

void GraphicBoard::ThrowException(const int i)
{
	throw i;
}

GraphicBoard::GraphicBoard()
{
	exitEvent.type = SDL_QUIT;
}

GraphicBoard::~GraphicBoard()
{
	// Renderer :
	if (renderer != NULL)
		SDL_DestroyRenderer(renderer);

	// Windows :
	if (window != NULL)
		SDL_DestroyWindow(window);

	IMG_Quit();
	SDL_Quit();
}
void GraphicBoard::Init()
{
	SDL_SetMainReady();

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		std::cout << stderr << "could not initialize sdl2 : " << SDL_GetError() << std::endl;
		ThrowException(1);
	}

	if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2")) {
		std::cout << stderr << "could not initialize render scale quality : " << SDL_GetError() << std::endl;
		ThrowException(1);
	}

	SDL_GetCurrentDisplayMode(0, &DisplayMode);

	const auto nRenderDrivers = SDL_GetNumRenderDrivers();

	window = SDL_CreateWindow(
		"Unshackled Flow",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		DisplayMode.w, DisplayMode.h,
		//SDL_WINDOW_FULLSCREEN | SDL_WINDOW_VULKAN
		//SDL_WINDOW_FULLSCREEN | SDL_WINDOW_OPENGL
		//SDL_WINDOW_FULLSCREEN | SDL_WINDOW_VULKAN
		SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN
	);
	if (window == NULL) {
		std::cout << stderr << "could not create window: " << SDL_GetError() << std::endl;
		ThrowException(1);
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderer == NULL)
	{
		std::cout << stderr << "could not create renderer: " << SDL_GetError() << std::endl;
		ThrowException(1);
	}

	Width = 3840;
	Height = 2160;
	SDL_RenderSetLogicalSize(renderer, Width, Height);

	/*screenSurface = SDL_GetWindowSurface(window);
	SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0xFF, 0xFF, 0xFF));*/
	//SDL_UpdateWindowSurface(window);

	gHeight = 10;
	gWidth = 15;
	clientHeight = Height * Ratio;;
	clientWidth = Width * Ratio;
	int h = clientHeight / (gHeight + 1);
	int w = clientWidth / (gWidth + 1);

#ifdef _DEBUG
	side = min(w, h) - 2;
#else
	side = std::min(w, h) - 2;
#endif
	if (side % 2 == 0)
		--side;

	Grille = new grid(gHeight, gWidth, 10);
#ifdef _DEBUG
	auto curcell = Grille;
	while (nullptr != curcell)
	{
		auto firstin = curcell;
		while (nullptr != curcell)
		{
			std::cout << "[";
			HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
			if (0 != curcell->GetColour())
			{
				PCONSOLE_SCREEN_BUFFER_INFO consoleSBF = new CONSOLE_SCREEN_BUFFER_INFO;
				GetConsoleScreenBufferInfo(hConsole, consoleSBF);
				SetConsoleTextAttribute(hConsole, curcell->GetColour());
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
	Refresh();
}

void GraphicBoard::Refresh()
{
	/*
	auto curcell = Grille;

	int x = 0;
	int y = 0;

	for (x = 0; nullptr != curcell; ++x)
		curcell = curcell->right;

	for (curcell = Grille, y = 0; nullptr != curcell; ++y)
		curcell = curcell->bottom;
	*/

	DrawGrid();
}

void DrawCircleSub(SDL_Renderer* renderer, std::vector <std::pair<int, int>> & vIn)
{
	std::vector <std::pair<int, int>>::iterator it = vIn.begin();
	while (it != vIn.end())
	{
		const std::vector <std::pair<int, int>>::iterator itprev = it;
		++it;
		if (it != vIn.end())
			SDL_RenderDrawLine(renderer, itprev->first, itprev->second, it->first, it->second);
	}
}

void DrawCircle(SDL_Renderer* renderer, int xOrg, int yOrg, double radius)
{
	std::vector <std::pair<int, int>> vTopLeft;
	std::vector <std::pair<int, int>> vBottomLeft;
	std::vector <std::pair<int, int>> vTopRight;
	std::vector <std::pair<int, int>> vBottomRight;
	for (double y = 1; y >= 0; y -= 0.01)
	{
		double alpha = asin(y);
		double xp = cos(alpha) * radius;
		double yp = y * radius;

		/**/
		SDL_RenderDrawPoint(renderer, xOrg - xp, yOrg + yp);
		SDL_RenderDrawPoint(renderer, xOrg + xp, yOrg + yp);
		SDL_RenderDrawPoint(renderer, xOrg - xp, yOrg - yp);
		SDL_RenderDrawPoint(renderer, xOrg + xp, yOrg - yp);
		/**/
		vTopLeft.emplace_back(std::pair(xOrg - xp, yOrg + yp));
		vBottomLeft.emplace_back(std::pair(xOrg - xp, yOrg - yp));
		vTopRight.emplace_back(std::pair(xOrg + xp, yOrg + yp));
		vBottomRight.emplace_back(std::pair(xOrg + xp, yOrg - yp));
		/**/
	}

	DrawCircleSub(renderer, vTopLeft);
	DrawCircleSub(renderer, vBottomLeft);
	DrawCircleSub(renderer, vTopRight);
	DrawCircleSub(renderer, vBottomRight);
}

void FillCircle(SDL_Renderer* renderer, int xOrg, int yOrg, double radius)
{
	for (double y = 1; y >= 0; y -= 0.01)
	{
		double alpha = asin(y);
		double xp = cos(alpha) * radius;
		double yp = y * radius;

		SDL_RenderDrawLine(renderer, xOrg - xp, yOrg + yp, xOrg + xp, yOrg + yp);
		SDL_RenderDrawLine(renderer, xOrg - xp, yOrg - yp, xOrg + xp, yOrg - yp);
	}
}

void GraphicBoard::DrawGrid()
{
	SDL_SetRenderDrawColor(renderer,
		0, // red
		0, // green
		0, // blue
		255); // Alpha

	SDL_RenderClear(renderer);

	SDL_SetRenderDrawColor(renderer,
		255, // red
		255, // green
		255, // blue
		255); // Alpha

	int curWidth = gWidth * side;
	int curHeight = gHeight * side;
	int xDec = (Width - curWidth) / 2;
	int yDec = (Height - curHeight) / 2;

	int xOrg = xDec;
	int yOrg = yDec;

	for (int y = 0; y <= gHeight; ++y)
	{
		SDL_RenderDrawLine(renderer, xDec, yOrg, curWidth + xDec, yOrg);
		SDL_RenderDrawLine(renderer, xDec, yOrg+1, curWidth + xDec, yOrg+1);
		yOrg += side;
	}

	for (int x = 0; x <= gWidth; ++x)
	{
		SDL_RenderDrawLine(renderer, xOrg, yDec, xOrg, curHeight + yDec);
		SDL_RenderDrawLine(renderer, xOrg + 1, yDec, xOrg + 1, curHeight + yDec);
		xOrg += side;
	}

	int x = 0;
	int y = 0;
	auto curcell = Grille;

	while (nullptr != curcell)
	{
		auto firstin = curcell;
		x = 0;
		while (nullptr != curcell)
		{			
			if (0 != curcell->GetColour())
			{
				const auto colour = arrColours[curcell->GetColour()];
				SDL_SetRenderDrawColor(renderer,
					colour.getRed(), // red
					colour.getGreen(), // green
					colour.getBlue(), // blue
					255); // Alpha

				double radius = side / 2. - 2.;
				int xOrg = x * side + xDec + side / 2 + 1;
				int yOrg = y * side + yDec + side / 2 + 1;
				FillCircle(renderer, xOrg, yOrg, radius);
			}
			else
			{
			}
			curcell = curcell->right;
			++x;
		}
		curcell = firstin->bottom;
		++y;
	}

	SDL_RenderPresent(renderer);
}

grid* GraphicBoard::GetCell(int xscr, int yscr)
{
	int curWidth = gWidth * side;
	int curHeight = gHeight * side;
	int xDec = (Width - curWidth) / 2;
	int yDec = (Height - curHeight) / 2;

	int xOrg = xDec;
	int yOrg = yDec;

	if(xscr > xOrg && xscr < (curWidth + xOrg) && yscr > yOrg && yscr < (curHeight + yOrg ))
	{
		int x = xscr - xOrg;
		x /= side;

		int y = yscr - yOrg;
		y /= side;

		auto curcell = Grille;

		int _x = 0, _y = 0;

		while (nullptr != curcell && x != _x)
		{
			curcell = curcell->right;
			++_x;
		}

		if (x == _x)
		{
			while (nullptr != curcell && y != _y)
			{
				curcell = curcell->bottom;
				++_y;
			}
		}

		return x == _x && y == _y ? curcell : nullptr;
	}

	return nullptr;
}

void GraphicBoard::FillFlow(int x, int y, int xprev, int yprev)
{
	double radius = (side / 2. - 2.) / 2.;

	int xcross = x;
	int ycross = y;
	PutInFlow(xcross, ycross);
	int xprevcross = xprev;
	int yprevcross = yprev;
	PutInFlow(xprevcross, yprevcross);
	if (xcross == xprevcross && ycross == yprevcross)
	{

	}
	else if (xcross == xprevcross)
	{
		for(int ytemp = 0; ytemp < side; ++ytemp)
			FillCircle(renderer, xcross, ycross < yprevcross ? ycross + ytemp : ycross - ytemp, radius);
	}
	else if (ycross == yprevcross)
	{
		for (int xtemp = 0; xtemp < side; ++xtemp)
			FillCircle(renderer, xcross < xprevcross ? xcross + xtemp : xcross - xtemp, ycross, radius);
	}

	//FillCircle(renderer, xcross, ycross, radius);
}

void GraphicBoard::PutInFlow(int& xscr, int& yscr)
{
	int curWidth = gWidth * side;
	int curHeight = gHeight * side;
	int xDec = (Width - curWidth) / 2;
	int yDec = (Height - curHeight) / 2;

	int xOrg = xDec;
	int yOrg = yDec;

	if (xscr > xOrg && xscr < (curWidth + xOrg) && yscr > yOrg && yscr < (curHeight + yOrg))
	{
		int x = xscr - xOrg;
		x /= side;
		xscr = x * side + xOrg + side / 2 + 1;

		int y = yscr - yOrg;
		y /= side;
		yscr = y * side + yOrg + side / 2 + 1;
	}
}

void GraphicBoard::Loop()
{
	Init();

	SDL_Event event;
	bool done = false;
	while (!done && SDL_WaitEvent(&event)) // SDL_PollEvent(&event) et adieu l'autonomie...
	{
		switch (event.type)
		{
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym)
			{
			case SDLK_ESCAPE:
				SDL_PushEvent(&exitEvent);
				break;
			default:
				break;
			}
			break;
		case SDL_MOUSEBUTTONDOWN:
			switch (event.button.button)
			{
			case SDL_BUTTON_LEFT:
				if ((event.type == SDL_MOUSEBUTTONDOWN))
				{
					auto curcell = GetCell(event.button.x, event.button.y);
					if(nullptr != curcell && 0 != curcell->GetColour())
					{
						auto prevcolour = curcell->GetColour();
						const auto colour = arrColours[curcell->GetColour()];
						SDL_SetRenderDrawColor(renderer,
							colour.getRed(), // red
							colour.getGreen(), // green
							colour.getBlue(), // blue
							255); // Alpha
						//double radius = (side / 2. - 2.) / 2.;
						//FillCircle(renderer, event.button.x, event.button.y, radius);
						//FillCross(event.button.x, event.button.y);
						//SDL_RenderPresent(renderer);

						int xPrev = event.button.x;
						int yPrev = event.button.y;

						while (SDL_WaitEvent(&event) && (event.type != SDL_MOUSEBUTTONUP) &&
							(curcell = GetCell(event.button.x, event.button.y)) != nullptr )
						{
							//FillCircle(renderer, event.button.x, event.button.y, radius);
							FillFlow(event.button.x, event.button.y, xPrev, yPrev);
							SDL_RenderPresent(renderer);
							xPrev = event.button.x;
							yPrev = event.button.y;
						}
					}
				}
				break;
			case SDL_BUTTON_RIGHT:
				if ((event.type == SDL_MOUSEBUTTONDOWN))
				{
					while (SDL_WaitEvent(&event) && (event.type != SDL_MOUSEBUTTONUP))
					{
					}
					Refresh();
				}
				break;
			default:
				break;
			}
			break;
			// Evénement déclenché par une fermeture de la fenêtre:
		case SDL_QUIT:
			done = true;
			break;
		default:
			break;
		}
	}
}

