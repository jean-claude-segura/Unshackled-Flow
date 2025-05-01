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
	clientHeight = static_cast<int>(static_cast<double>(Height) * Ratio);
	clientWidth = static_cast<int>(static_cast<double>(Width) * Ratio);

	int h = clientHeight / (gHeight + 1);
	int w = clientWidth / (gWidth + 1);

#ifdef _DEBUG
	side = min(w, h) - 2;
#else
	side = std::min(w, h) - 2;
#endif
	if (side % 2 == 0)
		--side;

	curWidth = gWidth * side;
	curHeight = gHeight * side;
	xDec = (Width - curWidth) / 2;
	yDec = (Height - curHeight) / 2;

	Grille = new grid(gHeight, gWidth, 10);

	int xOrg = xDec;
	int yOrg = yDec;

	for (int y = 0; y <= gHeight; ++y)
	{
		SDL_RenderDrawLine(renderer, xDec, yOrg, curWidth + xDec, yOrg);
		SDL_RenderDrawLine(renderer, xDec, yOrg + 1, curWidth + xDec, yOrg + 1);
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

	arrCoordinatesToCell.release();
	arrCoordinatesToCell = std::make_unique <std::unique_ptr<grid * []>[]>(gWidth);
	for (int i = 0; i < gWidth; ++i) {
		arrCoordinatesToCell.get()[i] = std::make_unique<grid * []>(gHeight);
	}

	mCellToCoordinates.clear();
	while (nullptr != curcell)
	{
		auto firstin = curcell;
		x = 0;
		while (nullptr != curcell)
		{
			arrCoordinatesToCell[x][y] = curcell;
			mCellToCoordinates[curcell] = std::pair<int, int>(x, y);

			curcell = curcell->GetRight();
			++x;
		}
		curcell = firstin->GetBottom();
		++y;
	}

	Refresh();

#ifdef _DEBUG
	curcell = Grille;
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
			curcell = curcell->GetRight();
		}
		std::cout << std::endl;
		curcell = firstin->GetBottom();
	}
	std::cout << std::endl;
#endif // _DEBUG
}

void GraphicBoard::Refresh()
{
	/*
	auto curcell = Grille;

	int x = 0;
	int y = 0;

	for (x = 0; nullptr != curcell; ++x)
		curcell = curcell->GetRight();

	for (curcell = Grille, y = 0; nullptr != curcell; ++y)
		curcell = curcell->GetBottom();
	*/

	DrawGrid();
}

static void DrawCircleSub(SDL_Renderer* renderer, std::vector <std::pair<int, int>> & vIn)
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

static void DrawCircle(SDL_Renderer* renderer, int xOrg, int yOrg, double radius)
{
	std::vector <std::pair<int, int>> vTopLeft;
	std::vector <std::pair<int, int>> vBottomLeft;
	std::vector <std::pair<int, int>> vTopRight;
	std::vector <std::pair<int, int>> vBottomRight;
	for (double y = 1; y >= 0; y -= 0.01)
	{
		double alpha = asin(y);
		int xp = static_cast<int>(cos(alpha) * radius);
		int yp = static_cast<int>(y * radius);

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

static void FillCircle(SDL_Renderer* renderer, int xOrg, int yOrg, double radius)
{
	for (double y = 1; y >= 0; y -= 0.01)
	{
		double alpha = asin(y);
		int xp = static_cast<int>(cos(alpha) * radius);
		int yp = static_cast<int>(y * radius);

		SDL_RenderDrawLine(renderer, xOrg - xp, yOrg + yp, xOrg + xp, yOrg + yp);
		SDL_RenderDrawLine(renderer, xOrg - xp, yOrg - yp, xOrg + xp, yOrg - yp);
	}
}

void GraphicBoard::SetDrawColour(grid* curcell)
{
	// Cell is other colour node  :
	const auto& colour = arrColours[curcell->GetColour()];
	SDL_SetRenderDrawColor(renderer, colour.getRed(), colour.getGreen(), colour.getBlue(), 255);
}

void GraphicBoard::DrawGrid()
{
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

	SDL_RenderClear(renderer);

	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

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
			if (curcell->IsNode())
			{
				DrawNode(curcell);
			}
#ifdef _DEBUG
			else if (curcell->IsPath())
			{
				SetDrawColour(curcell);

				int xOrg = x * side + xDec + side / 2 + 1;
				int yOrg = y * side + yDec + side / 2 + 1;
				FillCircle(renderer, xOrg, yOrg, NODERADIUS / 2);
			}
#endif
			curcell = curcell->GetRight();
			++x;
		}
		curcell = firstin->GetBottom();
		++y;
	}

	/*
	for (const auto& m : mCellToCoordinates)
	{
		int x = m.second.first;
		int y = m.second.second;
		if (m.first != arrCoordinatesToCell[x][y])
			return;
	}
	*/

	SDL_RenderPresent(renderer);
}

grid* GraphicBoard::GetCell(int xscr, int yscr)
{
	grid* curcell = nullptr;

	if(xscr > xDec && xscr < (curWidth + xDec) && yscr > yDec && yscr < (curHeight + yDec))
	{
		int x = xscr - xDec;
		x /= side;

		int y = yscr - yDec;
		y /= side;

		curcell = arrCoordinatesToCell[x][y];
	}

	return curcell;
}

void GraphicBoard::DrawEmptyCell(grid* cell)
{
	const auto it = mCellToCoordinates.find(cell);
	const auto& cellCoord = it->second;
	
	int xscr = cellCoord.first * side + xDec + side / 2 + 1;
	int yscr = cellCoord.second * side + yDec + side / 2 + 1;

	int x = xscr - xDec;
	x /= side;
	xscr = x * side + xDec;

	int y = yscr - yDec;
	y /= side;
	yscr = y * side + yDec;


	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);

	SDL_Rect reccell{ xscr + 1, yscr + 1, side, side };
	SDL_RenderFillRect(renderer, &reccell);
	
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

	SDL_RenderDrawLine(renderer, xscr, yscr, xscr + side, yscr);
	SDL_RenderDrawLine(renderer, xscr, yscr + side, xscr + side, yscr + side);
	SDL_RenderDrawLine(renderer, xscr, yscr, xscr, yscr + side);
	SDL_RenderDrawLine(renderer, xscr + side, yscr, xscr + side, yscr + side);

	SDL_RenderDrawLine(renderer, xscr, yscr + 1 + side, xscr + side, yscr + 1 + side);
	SDL_RenderDrawLine(renderer, xscr, yscr + 1, xscr + side, yscr + 1);
	SDL_RenderDrawLine(renderer, xscr + 1 + side, yscr, xscr + 1 + side, yscr + side);
	SDL_RenderDrawLine(renderer, xscr + 1, yscr, xscr + 1, yscr + side);
}

void GraphicBoard::FillFlow(int x, int y, int xprev, int yprev)
{
	double radius = NODERADIUS / 2.;

	std::pair<int, int> curCoord;
	GetCellCenter(x, y, curCoord);
	std::pair<int, int> prevCoord;
	GetCellCenter(xprev, yprev, prevCoord);

	const auto prevcell = GetCell(xprev, yprev);
	const auto curcell = GetCell(x, y);

	if (prevcell == curcell)
	{

	}
	else if (curcell->IsAdjacent(prevcell) && prevcell->GetColour() != 0)
	{
		if (prevcell->IsNode())
		{
			// Restart :
			ClearPathFromNode(prevcell);
		}

		// Take back :
		if (curcell->IsTakeBack(prevcell))
		{
			// first we clear the previous cell :
			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
			if (curcell->IsVerticallyAdjacent(prevcell))
			{
				for (int ytemp = 0; ytemp < side; ++ytemp)
					FillCircle(renderer, curCoord.first, curCoord.second < prevCoord.second ? curCoord.second + ytemp : curCoord.second - ytemp, radius);
			}
			else //if (IsHorizontallyAdjacent(curcell, prevcell))
			{
				for (int xtemp = 0; xtemp < side; ++xtemp)
					FillCircle(renderer, curCoord.first < prevCoord.first ? curCoord.first + xtemp : curCoord.first - xtemp, curCoord.second, radius);
			}
			DrawEmptyCell(prevcell);

			// We redraw what should be redrawn :
			if (prevcell->IsPath())
			{
				SetDrawColour(curcell);
				FillCircle(renderer, curCoord.first, curCoord.second, curcell->IsPath() ? radius : NODERADIUS);
				prevcell->UnsetPath();
			}
			else
			{
				DrawNode(prevcell);
			}
		}
		else // Add flow :
		{
			if (curcell->IsNode())
			{
				if (curcell->GetColour() == prevcell->GetColour())
				{
					// Cell is same colour node : we close or restart :
					if (curcell->IsFinal(prevcell))
					{
						// Closure :
						if (curcell->IsVerticallyAdjacent(prevcell))
						{
							for (int ytemp = 0; ytemp < side; ++ytemp)
								FillCircle(renderer, curCoord.first, curCoord.second < prevCoord.second ? curCoord.second + ytemp : curCoord.second - ytemp, radius);
						}
						else //if (IsHorizontallyAdjacent(curcell, prevcell))
						{
							for (int xtemp = 0; xtemp < side; ++xtemp)
								FillCircle(renderer, curCoord.first < prevCoord.first ? curCoord.first + xtemp : curCoord.first - xtemp, curCoord.second, radius);
						}
					}
					else
					{
						// Restart :
						ClearPathFromNode(curcell);
					}
				}
				else
				{
					// Cell is other colour node  :
					SetDrawColour(curcell);
				}
			}
			if ((prevcell->IsNode() || prevcell->IsPath()) && (curcell->IsPath() || curcell->IsFree()))
			{
				// Cell is free or needs to be overwritten :
				if (!curcell->IsLink(prevcell) && curcell->IsPath())
				{
					// Cell needs to be overwritten :
					ClearRelevantPath(curcell);
				}
				SetDrawColour(prevcell);
				if (curcell->IsVerticallyAdjacent(prevcell))
				{
					for (int ytemp = 0; ytemp < side; ++ytemp)
						FillCircle(renderer, curCoord.first, curCoord.second < prevCoord.second ? curCoord.second + ytemp : curCoord.second - ytemp, radius);
				}
				else //if (IsHorizontallyAdjacent(curcell, prevcell))
				{
					for (int xtemp = 0; xtemp < side; ++xtemp)
						FillCircle(renderer, curCoord.first < prevCoord.first ? curCoord.first + xtemp : curCoord.first - xtemp, curCoord.second, radius);
				}
				curcell->SetPath(prevcell->GetColour());
			}
		}
	}
	SDL_RenderPresent(renderer);
}

bool GraphicBoard::GetCellCenter(const int xscr, const int yscr, std::pair<int, int> & coord)
{
	bool bRet = false;

	if (xscr > xDec && xscr < (curWidth + xDec) && yscr > yDec && yscr < (curHeight + yDec))
	{
		int x = xscr - xDec;
		x /= side;

		int y = yscr - yDec;
		y /= side;

		coord = std::make_pair<int, int>(x * side + xDec + side / 2 + 1, y * side + yDec + side / 2 + 1);

		bRet = true;
	}

	return bRet;
}

void GraphicBoard::DrawNode(grid* cell)
{
	const auto it = mCellToCoordinates.find(cell);
	const auto& cellCoord = it->second;
	
	int x = cellCoord.first * side + xDec + side / 2 + 1;
	int y = cellCoord.second * side + yDec + side / 2 + 1;

	SetDrawColour(cell);
	FillCircle(renderer, x, y, NODERADIUS);
}

void GraphicBoard::ClearPathFromNode(grid* cell)
{
	std::vector<grid*> fullPath;
	grid::ClearPathFromNode(cell, fullPath);
	for (const auto& curcell : fullPath)
	{
		DrawEmptyCell(curcell);
		if (curcell->IsNode())
			DrawNode(curcell);
	}
}

void GraphicBoard::ClearRelevantPath(grid* cell)
{
	std::vector<grid*> relevantpath;
	grid::ClearRelevantPath(cell, relevantpath);
	for (const auto& curcell : relevantpath)
	{
		DrawEmptyCell(curcell);
		if (curcell->IsNode())
			DrawNode(curcell);
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
					if(nullptr != curcell && (curcell->IsNode() || curcell->IsPath()))
					{
						/*if (curcell->IsNode())
						{
							ClearPath(curcell);
							SDL_RenderPresent(renderer);
						}*/
						auto prevcell = curcell;

						int xPrev = event.button.x;
						int yPrev = event.button.y;
						while (SDL_WaitEvent(&event) && (event.type != SDL_MOUSEBUTTONUP) &&
							(curcell = GetCell(event.button.x, event.button.y)) != nullptr)
						{
							if (prevcell != curcell)
							{
								FillFlow(event.button.x, event.button.y, xPrev, yPrev);
								xPrev = event.button.x;
								yPrev = event.button.y;
								prevcell = curcell;
							}
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

