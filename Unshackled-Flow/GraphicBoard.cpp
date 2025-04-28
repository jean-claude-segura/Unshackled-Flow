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

static void FillCircle(SDL_Renderer* renderer, int xOrg, int yOrg, double radius)
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

	arrCoordinatesToCell.release();
	arrCoordinatesToCell = std::make_unique <std::unique_ptr<grid*[]>[]>(gWidth);
	for (int i = 0; i < gWidth; ++i) {
		arrCoordinatesToCell.get()[i] = std::make_unique<grid*[]>(gHeight);
	}

	mCellToCoordinates.clear();
	while (nullptr != curcell)
	{
		auto firstin = curcell;
		x = 0;
		while (nullptr != curcell)
		{			
			if (0 != curcell->GetColour())
			{
				const auto& colour = arrColours[curcell->GetColour()];
				SDL_SetRenderDrawColor(renderer,
					colour.getRed(), // red
					colour.getGreen(), // green
					colour.getBlue(), // blue
					255); // Alpha

				double radius = side / 2. - 2.;
				int xOrg = x * side + xDec + side / 2 + 1;
				int yOrg = y * side + yDec + side / 2 + 1;
				if (curcell->IsPath())
					FillCircle(renderer, xOrg, yOrg, radius / 2);
				else
					FillCircle(renderer, xOrg, yOrg, radius);
			}
			else
			{
			}

			arrCoordinatesToCell[x][y] = curcell;
			mCellToCoordinates[curcell] = std::pair<int, int>(x, y);

			curcell = curcell->right;
			++x;
		}
		curcell = firstin->bottom;
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
	int curWidth = gWidth * side;
	int curHeight = gHeight * side;
	int xOrg = (Width - curWidth) / 2;
	int yOrg = (Height - curHeight) / 2;

	grid* curcell = nullptr;

	if(xscr > xOrg && xscr < (curWidth + xOrg) && yscr > yOrg && yscr < (curHeight + yOrg ))
	{
		int x = xscr - xOrg;
		x /= side;

		int y = yscr - yOrg;
		y /= side;

		curcell = arrCoordinatesToCell[x][y];
	}

	return curcell;
}

void GraphicBoard::DrawEmptyCell(int xscr, int yscr)
{
	int curWidth = gWidth * side;
	int curHeight = gHeight * side;
	int xOrg = (Width - curWidth) / 2;
	int yOrg = (Height - curHeight) / 2;

	int x = xscr - xOrg;
	x /= side;
	xscr = x * side + xOrg;

	int y = yscr - yOrg;
	y /= side;
	yscr = y * side + yOrg;


	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);

	SDL_Rect cell{ xscr + 1, yscr + 1, side, side };
	SDL_RenderFillRect(renderer, &cell);
	
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

bool GraphicBoard::IsFinal(grid * curcell, grid * prevcell)
{
	return
		(curcell->top == nullptr || curcell->top->GetColour() != curcell->GetColour() || curcell->top == prevcell) &&
		(curcell->left == nullptr || curcell->left->GetColour() != curcell->GetColour() || curcell->left == prevcell) &&
		(curcell->bottom == nullptr || curcell->bottom->GetColour() != curcell->GetColour() || curcell->bottom == prevcell) &&
		(curcell->right == nullptr || curcell->right->GetColour() != curcell->GetColour() || curcell->right == prevcell)
		;
}

bool GraphicBoard::IsAdjacent(grid* curcell, grid* prevcell)
{
	return (
		nullptr != curcell && nullptr != prevcell &&
		(
			curcell->top == prevcell ||
			curcell->left == prevcell ||
			curcell->bottom == prevcell ||
			curcell->right == prevcell
			)
		);
}

bool GraphicBoard::IsHorizontallyAdjacent(grid* curcell, grid* prevcell)
{
	return (
		nullptr != curcell && nullptr != prevcell &&
		(
			curcell->left == prevcell ||
			curcell->right == prevcell
			)
		);
}

bool GraphicBoard::IsVerticallyAdjacent(grid* curcell, grid* prevcell)
{
	return (
		nullptr != curcell && nullptr != prevcell &&
		(
			curcell->top == prevcell ||
			curcell->bottom == prevcell
			)
		);
}

bool GraphicBoard::IsTakeBack(grid* curcell, grid* prevcell)
{
	bool bRet = false;
	if(nullptr != prevcell && nullptr != curcell && prevcell->GetColour() == curcell->GetColour())
	{
		if(curcell->IsPath())
		{
			int links = 0;
			if (nullptr != curcell->top && curcell->top != prevcell && curcell->top->GetColour() == prevcell->GetColour())
				++links;
			if (nullptr != curcell->left && curcell->left != prevcell && curcell->left->GetColour() == prevcell->GetColour())
				++links;
			if (nullptr != curcell->bottom && curcell->bottom != prevcell && curcell->bottom->GetColour() == prevcell->GetColour())
				++links;
			if (nullptr != curcell->right && curcell->right != prevcell && curcell->right->GetColour() == prevcell->GetColour())
				++links;
			bRet = 0 != links;
			links = 0;
			if (nullptr != prevcell->top && prevcell->top != curcell && prevcell->top->GetColour() == curcell->GetColour())
				++links;
			if (nullptr != prevcell->left && prevcell->left != curcell && prevcell->left->GetColour() == curcell->GetColour())
				++links;
			if (nullptr != prevcell->bottom && prevcell->bottom != curcell && prevcell->bottom->GetColour() == curcell->GetColour())
				++links;
			if (nullptr != prevcell->right && prevcell->right != curcell && prevcell->right->GetColour() == curcell->GetColour())
				++links;
			bRet = bRet && (0 == links);
		}
		else
		{
			int links = 0;
			if (nullptr != prevcell->top && prevcell->top != curcell && prevcell->top->GetColour() == curcell->GetColour())
				++links;
			if (nullptr != prevcell->left && prevcell->left != curcell && prevcell->left->GetColour() == curcell->GetColour())
				++links;
			if (nullptr != prevcell->bottom && prevcell->bottom != curcell && prevcell->bottom->GetColour() == curcell->GetColour())
				++links;
			if (nullptr != prevcell->right && prevcell->right != curcell && prevcell->right->GetColour() == curcell->GetColour())
				++links;
			bRet = 0 == links;
		}
	}
	return bRet;
}

bool GraphicBoard::IsLink(grid* curcell, grid* prevcell)
{
	bool bRet = false;
	if (nullptr != prevcell && nullptr != curcell && prevcell->GetColour() == curcell->GetColour())
	{
		if (curcell->IsPath())
		{
			int links = 0;
			if (nullptr != curcell->top && curcell->top != prevcell && curcell->top->GetColour() == prevcell->GetColour())
				++links;
			if (nullptr != curcell->left && curcell->left != prevcell && curcell->left->GetColour() == prevcell->GetColour())
				++links;
			if (nullptr != curcell->bottom && curcell->bottom != prevcell && curcell->bottom->GetColour() == prevcell->GetColour())
				++links;
			if (nullptr != curcell->right && curcell->right != prevcell && curcell->right->GetColour() == prevcell->GetColour())
				++links;
			bRet = 0 != links;
		}
		else
		{
			int links = 0;
			if (nullptr != prevcell->top && prevcell->top != curcell && prevcell->top->GetColour() == curcell->GetColour())
				++links;
			if (nullptr != prevcell->left && prevcell->left != curcell && prevcell->left->GetColour() == curcell->GetColour())
				++links;
			if (nullptr != prevcell->bottom && prevcell->bottom != curcell && prevcell->bottom->GetColour() == curcell->GetColour())
				++links;
			if (nullptr != prevcell->right && prevcell->right != curcell && prevcell->right->GetColour() == curcell->GetColour())
				++links;
			bRet = 0 == links;
		}
	}
	return bRet;
}

void GraphicBoard::FillFlow(int x, int y, int xprev, int yprev)
{
	double radius = (side / 2. - 2.) / 2.;

	std::pair<int, int> curCoord;
	GetCellCenter(x, y, curCoord);
	std::pair<int, int> prevCoord;
	GetCellCenter(xprev, yprev, prevCoord);

	const auto prevcell = GetCell(xprev, yprev);
	const auto curcell = GetCell(x, y);

	if (prevcell == curcell)
	{

	}
	else if (IsAdjacent(curcell, prevcell) && prevcell->GetColour() != 0)
	{
		if (!prevcell->IsPath())
		{
			// Restart :
			ClearPath(prevcell);
		}

		// Take back :
		if (IsTakeBack(curcell, prevcell))
		{
			// first we clear the previous cell :
			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
			if (IsVerticallyAdjacent(curcell, prevcell))
			{
				for (int ytemp = 0; ytemp < side; ++ytemp)
					FillCircle(renderer, curCoord.first, curCoord.second < prevCoord.second ? curCoord.second + ytemp : curCoord.second - ytemp, radius);
			}
			else //if (IsHorizontallyAdjacent(curcell, prevcell))
			{
				for (int xtemp = 0; xtemp < side; ++xtemp)
					FillCircle(renderer, curCoord.first < prevCoord.first ? curCoord.first + xtemp : curCoord.first - xtemp, curCoord.second, radius);
			}
			DrawEmptyCell(prevCoord.first, prevCoord.second);

			// We redraw what should be redrawn :

			if (prevcell->IsPath())
			{
				const auto& colour = arrColours[curcell->GetColour()];
				SDL_SetRenderDrawColor(renderer, colour.getRed(), colour.getGreen(), colour.getBlue(), 255);
				FillCircle(renderer, curCoord.first, curCoord.second, curcell->IsPath() ? radius : (side / 2. - 2.));
				prevcell->SetColour(0);
			}
			else
			{
				const auto& colour = arrColours[prevcell->GetColour()];
				SDL_SetRenderDrawColor(renderer, colour.getRed(), colour.getGreen(), colour.getBlue(), 255);
				FillCircle(renderer, prevCoord.first, prevCoord.second, (side / 2. - 2.));
			}
		}
		else // Add flow :
		{
			if (
				!curcell->IsPath() && curcell->GetColour() == 0 ||
				curcell->IsPath()// && curcell->GetColour() != prevcell->GetColour()
				)
			{
				// Cell is free or needs to be overwritten :
				if (!IsLink(curcell, prevcell) && curcell->IsPath())
				{
					// Cell needs to be overwritten :
					//DrawEmptyCell(curCoord.first, curCoord.second);
					ClearPath(curcell);
				}
				const auto& colour = arrColours[prevcell->GetColour()];
				SDL_SetRenderDrawColor(renderer, colour.getRed(), colour.getGreen(), colour.getBlue(), 255);
				if (IsVerticallyAdjacent(curcell, prevcell))
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
			else if (!curcell->IsPath() && curcell->GetColour() == prevcell->GetColour())
			{
				// Cell is same colour node : we close or restart :
				if (IsFinal(curcell, prevcell))
				{
					// Closure :
					if (IsVerticallyAdjacent(curcell, prevcell))
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
					ClearPath(curcell);
				}
			}
			else
			{
				// Cell is other colour node  :
				const auto& colour = arrColours[curcell->GetColour()];
				SDL_SetRenderDrawColor(renderer, colour.getRed(), colour.getGreen(), colour.getBlue(), 255);
			}
		}
	}
}

bool GraphicBoard::GetCellCenter(const int xscr, const int yscr, std::pair<int, int> & coord)
{
	bool bRet = false;
	int curWidth = gWidth * side;
	int curHeight = gHeight * side;
	int xOrg = (Width - curWidth) / 2;
	int yOrg = (Height - curHeight) / 2;

	if (xscr > xOrg && xscr < (curWidth + xOrg) && yscr > yOrg && yscr < (curHeight + yOrg))
	{
		int x = xscr - xOrg;
		x /= side;

		int y = yscr - yOrg;
		y /= side;

		coord = std::make_pair<int, int>(x * side + xOrg + side / 2 + 1, y * side + yOrg + side / 2 + 1);

		bRet = true;
	}

	return bRet;
}

void GraphicBoard::ClearPath(grid* cell)
{
	int curWidth = gWidth * side;
	int curHeight = gHeight * side;
	int xOrg = (Width - curWidth) / 2;
	int yOrg = (Height - curHeight) / 2;
	const auto it = mCellToCoordinates.find(cell);
	const auto& cellCoord = it->second;
	std::pair<int, int> prevCoord = std::make_pair<int, int>(cellCoord.first * side + xOrg + side / 2 + 1, cellCoord.second * side + yOrg + side / 2 + 1);
	DrawEmptyCell(prevCoord.first, prevCoord.second);

	const auto colourIndex = cell->GetColour();
	if(colourIndex != 0)
	{
		if (cell->IsPath())
		{
			cell->SetColour(0);
		}
		else
		{
			const auto& colour = arrColours[cell->GetColour()];
			SDL_SetRenderDrawColor(renderer, colour.getRed(), colour.getGreen(), colour.getBlue(), 255);
			FillCircle(renderer, prevCoord.first, prevCoord.second, (side / 2. - 2.));
		}
		if (cell->top != nullptr && cell->top->GetColour() == colourIndex) ClearPath(cell->top);
		if (cell->left != nullptr && cell->left->GetColour() == colourIndex) ClearPath(cell->left);
		if (cell->bottom != nullptr && cell->bottom->GetColour() == colourIndex) ClearPath(cell->bottom);
		if (cell->right != nullptr && cell->right->GetColour() == colourIndex) ClearPath(cell->right);
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
						if (!curcell->IsPath())
						{
							ClearPath(curcell);
							SDL_RenderPresent(renderer);
						}
						auto prevcell = curcell;
						//double radius = (side / 2. - 2.) / 2.;
						//FillCircle(renderer, event.button.x, event.button.y, radius);
						//FillCross(event.button.x, event.button.y);
						//SDL_RenderPresent(renderer);

						int xPrev = event.button.x;
						int yPrev = event.button.y;
						while (SDL_WaitEvent(&event) && (event.type != SDL_MOUSEBUTTONUP) &&
							(curcell = GetCell(event.button.x, event.button.y)) != nullptr)
						{
							if (prevcell != curcell)
							{
								FillFlow(event.button.x, event.button.y, xPrev, yPrev);
								SDL_RenderPresent(renderer);
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

