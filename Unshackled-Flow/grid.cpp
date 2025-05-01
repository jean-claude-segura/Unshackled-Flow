#include "grid.hpp"

Cell::~Cell()
{
	//if (nullptr != top) delete top;
	//if (nullptr != left) delete left;
	if (nullptr != right) 
	{
		delete right;
		right = nullptr;
	}
	if (nullptr != bottom)
	{
		delete bottom;
		bottom = nullptr;
	}
#ifdef _DEBUG
	//static unsigned int cell = 0;
	//std::cout << "delete count : " << ++cell << std::endl;
#endif // _DEBUG
}

Cell* Cell::operator++()
{
	if (nullptr != right)
		return right;
	else if (nullptr != this->left)
	{
		Cell* cur = this->left;
		while (nullptr != cur->left)
			cur = cur->left;
		return cur->bottom;
	}
	else
		return nullptr;
}

Cell* Cell::operator--()
{
	if (nullptr != left)
		return left;
	else if (nullptr != this->right)
	{
		Cell* cur = this->right;
		while (nullptr != cur->right)
			cur = cur->right;
		return cur->top;
	}
	else
		return nullptr;
}

bool Cell::IsAdjacent(Cell* prevcell)
{
	return (
		nullptr != prevcell &&
		(
			top == prevcell ||
			left == prevcell ||
			bottom == prevcell ||
			right == prevcell
			)
		);
}

bool Cell::IsHorizontallyAdjacent(Cell* prevcell)
{
	return (
		nullptr != prevcell &&
		(
			left == prevcell ||
			right == prevcell
			)
		);
}

bool Cell::IsVerticallyAdjacent(Cell* prevcell)
{
	return (
		nullptr != prevcell &&
		(
			top == prevcell ||
			bottom == prevcell
			)
		);
}

bool Cell::IsTakeBack(Cell* prevcell)
{
	bool bRet = false;
	if (nullptr != prevcell && prevcell->colour == colour)
	{
		if (path)
		{
			int links = 0;
			if (nullptr != top && top != prevcell && top->colour == prevcell->colour)
				++links;
			if (nullptr != left && left != prevcell && left->colour == prevcell->colour)
				++links;
			if (nullptr != bottom && bottom != prevcell && bottom->colour == prevcell->colour)
				++links;
			if (nullptr != right && right != prevcell && right->colour == prevcell->colour)
				++links;
			bRet = 0 != links;
			links = 0;
			if (nullptr != prevcell->top && prevcell->top != this && prevcell->top->colour == colour)
				++links;
			if (nullptr != prevcell->left && prevcell->left != this && prevcell->left->colour == colour)
				++links;
			if (nullptr != prevcell->bottom && prevcell->bottom != this && prevcell->bottom->colour == colour)
				++links;
			if (nullptr != prevcell->right && prevcell->right != this && prevcell->right->colour == colour)
				++links;
			bRet = bRet && (0 == links);
		}
		else
		{
			int links = 0;
			if (nullptr != prevcell->top && prevcell->top != this && prevcell->top->colour == colour)
				++links;
			if (nullptr != prevcell->left && prevcell->left != this && prevcell->left->colour == colour)
				++links;
			if (nullptr != prevcell->bottom && prevcell->bottom != this && prevcell->bottom->colour == colour)
				++links;
			if (nullptr != prevcell->right && prevcell->right != this && prevcell->right->colour == colour)
				++links;
			bRet = 0 == links;
		}
	}
	return bRet;
}

bool Cell::IsLink(Cell* prevcell)
{
	bool bRet = false;
	if (nullptr != prevcell && prevcell->colour == colour)
	{
		if (path)
		{
			int links = 0;
			if (nullptr != top && top != prevcell && top->colour == prevcell->colour)
				++links;
			if (nullptr != left && left != prevcell && left->colour == prevcell->colour)
				++links;
			if (nullptr != bottom && bottom != prevcell && bottom->colour == prevcell->colour)
				++links;
			if (nullptr != right && right != prevcell && right->colour == prevcell->colour)
				++links;
			bRet = 0 != links;
		}
		else
		{
			int links = 0;
			if (nullptr != prevcell->top && prevcell->top != this && prevcell->top->colour == colour)
				++links;
			if (nullptr != prevcell->left && prevcell->left != this && prevcell->left->colour == colour)
				++links;
			if (nullptr != prevcell->bottom && prevcell->bottom != this && prevcell->bottom->colour == colour)
				++links;
			if (nullptr != prevcell->right && prevcell->right != this && prevcell->right->colour == colour)
				++links;
			bRet = 0 == links;
		}
	}
	return bRet;
}

bool Cell::IsFinal(Cell* prevcell)
{
	return prevcell != nullptr &&
		(top == nullptr || top->colour != colour || top == prevcell) &&
		(left == nullptr || left->colour != colour || left == prevcell) &&
		(bottom == nullptr || bottom->colour != colour || bottom == prevcell) &&
		(right == nullptr || right->colour != colour || right == prevcell)
		;
}

void Grid::GetLongestPath(Cell* cell, std::vector<Cell*>& longestpath)
{
	std::vector<Cell*> currenpath;
	GetLongestPath(cell, currenpath, longestpath);
}

void Grid::GetLongestPath(Cell* cell, std::vector<Cell*>& currenpath, std::vector<Cell*>& longestpath)
{
	if (currenpath.end() != std::find(currenpath.begin(), currenpath.end(), cell))
		return;
	currenpath.emplace_back(cell);
	if (cell->node)
	{
		if (!currenpath.empty() && (longestpath.size() < currenpath.size() || longestpath.empty()))
			longestpath = currenpath;
	}
	else if (cell->path)
	{
		if (cell->top != nullptr && cell->top->colour == cell->colour) GetLongestPath(cell->top, currenpath, longestpath);
		if (cell->left != nullptr && cell->left->colour == cell->colour) GetLongestPath(cell->left, currenpath, longestpath);
		if (cell->bottom != nullptr && cell->bottom->colour == cell->colour) GetLongestPath(cell->bottom, currenpath, longestpath);
		if (cell->right != nullptr && cell->right->colour == cell->colour) GetLongestPath(cell->right, currenpath, longestpath);

	}
	if (!currenpath.empty())
		currenpath.pop_back();
}

void Grid::GetShortestPath(Cell* cell, std::vector<Cell*>& shortestpath)
{
	std::vector<Cell*> currenpath;
	GetShortestPath(cell, currenpath, shortestpath);
}

void Grid::GetShortestPath(Cell* cell, std::vector<Cell*>& currenpath, std::vector<Cell*>& shortestpath)
{
	if (currenpath.end() != std::find(currenpath.begin(), currenpath.end(), cell))
		return;
	currenpath.emplace_back(cell);
	if (cell->node)
	{
		if (!currenpath.empty() && (shortestpath.size() > currenpath.size() || shortestpath.empty()))
			shortestpath = currenpath;
	}
	else if(cell->path)
	{
		if (cell->top != nullptr && cell->top->colour == cell->colour) GetShortestPath(cell->top, currenpath, shortestpath);
		if (cell->left != nullptr && cell->left->colour == cell->colour) GetShortestPath(cell->left, currenpath, shortestpath);
		if (cell->bottom != nullptr && cell->bottom->colour == cell->colour) GetShortestPath(cell->bottom, currenpath, shortestpath);
		if (cell->right != nullptr && cell->right->colour == cell->colour) GetShortestPath(cell->right, currenpath, shortestpath);

	}
	if (!currenpath.empty())
		currenpath.pop_back();
}

void Grid::GetOrphanPath(Cell* cell, std::vector<Cell*>& orphanPath)
{
	std::vector<Cell*> currenpath;
	GetOrphanPath(nullptr, cell, currenpath, orphanPath);
}

void Grid::GetFullPathFromNode(Cell* cell, std::vector<Cell*>& fullPath)
{
	if (fullPath.end() != std::find(fullPath.begin(), fullPath.end(), cell))
		return;

	if (cell->path || cell->node)
	{
		fullPath.emplace_back(cell);

		if (cell->top != nullptr && cell->top->colour == cell->colour) GetFullPathFromNode(cell->top, fullPath);
		if (cell->left != nullptr && cell->left->colour == cell->colour) GetFullPathFromNode(cell->left, fullPath);
		if (cell->bottom != nullptr && cell->bottom->colour == cell->colour) GetFullPathFromNode(cell->bottom, fullPath);
		if (cell->right != nullptr && cell->right->colour == cell->colour) GetFullPathFromNode(cell->right, fullPath);
	}
}

void Grid::GetOrphanPath(Cell* prevcell, Cell* cell, std::vector<Cell*>& currenpath, std::vector<Cell*>& orphanPath)
{
	if (currenpath.end() != std::find(currenpath.begin(), currenpath.end(), cell))
		return;
	if (cell->path)
	{
		currenpath.emplace_back(cell);

		if (cell->IsFinal(prevcell))
		{
			orphanPath = currenpath;
		}
		else
		{
			if (cell->top != nullptr && cell->top->colour == cell->colour) GetOrphanPath(cell, cell->top, currenpath, orphanPath);
			if (cell->left != nullptr && cell->left->colour == cell->colour) GetOrphanPath(cell, cell->left, currenpath, orphanPath);
			if (cell->bottom != nullptr && cell->bottom->colour == cell->colour) GetOrphanPath(cell, cell->bottom, currenpath, orphanPath);
			if (cell->right != nullptr && cell->right->colour == cell->colour) GetOrphanPath(cell, cell->right, currenpath, orphanPath);
		}

		if (!currenpath.empty())
			currenpath.pop_back();
	}
}

void Grid::ClearPathFromNode(Cell* cell, std::vector<Cell*>& clearedPath)
{
	GetFullPathFromNode(cell, clearedPath);
	for (const auto& curcell : clearedPath)
	{
		if (curcell->IsPath())
			curcell->UnsetPath();
	}
}

void Grid::ClearRelevantPath(Cell* cell, std::vector<Cell*>& clearedPath)
{
	GetOrphanPath(cell, clearedPath);
	if (clearedPath.empty())
		GetShortestPath(cell, clearedPath);
	for (const auto& curcell : clearedPath)
	{
		if (curcell->IsPath())
			curcell->UnsetPath();
	}
}

void Grid::Populate(uint8_t colours) const
{
	int cellscount = 0;
	for (Cell* cur = this->startingPoint; nullptr != cur; cur = ++ (*cur))
		++cellscount;
	std::vector<int> vAvailable;
	for (int i = 1; i <= colours; ++i, --cellscount)
		vAvailable.emplace_back(i);
	for (int i = 1; i <= colours; ++i, --cellscount)
		vAvailable.emplace_back(i);
	for (int i = 0; i < cellscount; ++i)
		vAvailable.emplace_back(0);

	static std::random_device r;
	static std::mt19937 e1(r());
	std::shuffle(vAvailable.begin(), vAvailable.end(), e1);

	for (Cell* cur = this->startingPoint; nullptr != cur;)
	{
		cur->colour = vAvailable.back();
		cur->node = vAvailable.back() != 0;
		vAvailable.pop_back();
		cur = ++ (*cur);
	}
}

void Grid::Addrow(Cell* firstleftcell, int width)
{
	Cell* curcell = firstleftcell;
	for (int x = 0; x < width - 1; ++x)
	{
		curcell->right = new Cell();
		curcell->right->left = curcell;
		if (nullptr != curcell->top && nullptr != curcell->top->right)
		{
			curcell->right->top = curcell->top->right;
			curcell->top->right->bottom = curcell->right;
		}
		curcell = curcell->right;
	}
}

void Grid::Init(int width, int height)
{
	this->startingPoint = new Cell();
	Cell* firstcell = this->startingPoint;
	for (int y = 0; y < height; ++y)
	{
		Addrow(firstcell, width);
		if (y < height - 1)
		{
			firstcell->bottom = new Cell();
			firstcell->bottom->top = firstcell;
			firstcell = firstcell->bottom;
		}
	}
}

Grid::Grid(int width, int height, uint8_t colours) : startingPoint(nullptr)
{
	Init(width, height);
	Populate(colours);

	int x = 0;
	int y = 0;
	auto curcell = startingPoint;

	arrCoordinatesToCell.release();
	arrCoordinatesToCell = std::make_unique <std::unique_ptr<Cell * []>[]>(width);
	for (int i = 0; i < width; ++i) {
		arrCoordinatesToCell.get()[i] = std::make_unique<Cell * []>(height);
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
}

Grid::~Grid()
{
	if (nullptr != startingPoint)
		delete startingPoint;
}
