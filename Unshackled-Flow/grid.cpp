#include "grid.hpp"

void grid::populate(grid* topleftcorner, int colours)
{
	int cellscount = 0;
	for(grid* cur = topleftcorner; nullptr != cur; cur = ++ (*cur))
		++cellscount;
	std::vector<int> vAvailable;
	for (int i = 1; i <= colours; ++i, --cellscount)
		vAvailable.emplace_back(i);
	for (int i = 1; i <= colours; ++i, --cellscount)
		vAvailable.emplace_back(i);
	for(int i = 0; i < cellscount; ++i)
		vAvailable.emplace_back(0);

	static std::random_device r;
	static std::mt19937 e1(r());
	std::shuffle(vAvailable.begin(), vAvailable.end(), e1);

	for (grid* cur = topleftcorner; nullptr != cur;)
	{
		cur->colour = vAvailable.back();
		vAvailable.pop_back();
		cur = ++ (*cur);
	}
}

void grid::init(grid * topleftcorner, int height, int width)
{
	grid* firstcell = topleftcorner;
	for (int y = 0; y < height; ++y)
	{
		addrow(firstcell, width);
		if(y < height - 1)
		{
			firstcell->bottom = new grid();
			firstcell->bottom->top = firstcell;
			firstcell = firstcell->bottom;
		}
	}
}

void grid::addrow(grid* firstleftcell, int width)
{
	grid* curcell = firstleftcell;
	for (int x = 0; x < width - 1; ++x)
	{
		curcell->right = new grid();
		curcell->right->left = curcell;
		if (nullptr != curcell->top && nullptr != curcell->top->right)
		{
			curcell->right->top = curcell->top->right;
			curcell->top->right->bottom = curcell->right;
		}
		curcell = curcell->right;
	}
}

void grid::populate(int colours)
{
	int cellscount = 0;
	for (grid* cur = this; nullptr != cur; cur = ++ (*cur))
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

	for (grid* cur = this; nullptr != cur;)
	{
		cur->colour = vAvailable.back();
		cur->node = vAvailable.back() != 0;
		vAvailable.pop_back();
		cur = ++ (*cur);
	}
}

void grid::init(int height, int width)
{
	grid* firstcell = this;
	for (int y = 0; y < height; ++y)
	{
		addrow(firstcell, width);
		if (y < height - 1)
		{
			firstcell->bottom = new grid();
			firstcell->bottom->top = firstcell;
			firstcell = firstcell->bottom;
		}
	}
}

grid::grid(int height, int width, int colours)
{
	this->init(height, width);
	this->populate(colours);
}

grid::~grid()
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

grid* grid::operator++()
{
	if (nullptr != right)
		return right;
	else if (nullptr != this->left)
	{
		grid* cur = this->left;
		while (nullptr != cur->left)
			cur = cur->left;
		return cur->bottom;
	}
	else
		return nullptr;
}

grid* grid::operator--()
{
	if (nullptr != left)
		return left;
	else if (nullptr != this->right)
	{
		grid* cur = this->right;
		while (nullptr != cur->right)
			cur = cur->right;
		return cur->top;
	}
	else
		return nullptr;
}

bool grid::IsAdjacent(grid* prevcell)
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

bool grid::IsHorizontallyAdjacent(grid* prevcell)
{
	return (
		nullptr != prevcell &&
		(
			left == prevcell ||
			right == prevcell
			)
		);
}

bool grid::IsVerticallyAdjacent(grid* prevcell)
{
	return (
		nullptr != prevcell &&
		(
			top == prevcell ||
			bottom == prevcell
			)
		);
}

bool grid::IsTakeBack(grid* prevcell)
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

bool grid::IsLink(grid* prevcell)
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

bool grid::IsFinal(grid* prevcell)
{
	return prevcell != nullptr &&
		(top == nullptr || top->colour != colour || top == prevcell) &&
		(left == nullptr || left->colour != colour || left == prevcell) &&
		(bottom == nullptr || bottom->colour != colour || bottom == prevcell) &&
		(right == nullptr || right->colour != colour || right == prevcell)
		;
}

void grid::GetLongestPath(grid* cell, std::vector<grid*>& longestpath)
{
	std::vector<grid*> currenpath;
	GetLongestPath(cell, currenpath, longestpath);
}

void grid::GetLongestPath(grid* cell, std::vector<grid*>& currenpath, std::vector<grid*>& longestpath)
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

void grid::GetShortestPath(grid* cell, std::vector<grid*>& shortestpath)
{
	std::vector<grid*> currenpath;
	GetShortestPath(cell, currenpath, shortestpath);
}

void grid::GetShortestPath(grid* cell, std::vector<grid*>& currenpath, std::vector<grid*>& shortestpath)
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

void grid::GetOrphanPath(grid* cell, std::vector<grid*>& orphanPath)
{
	std::vector<grid*> currenpath;
	GetOrphanPath(nullptr, cell, currenpath, orphanPath);
}

void grid::GetFullPathFromNode(grid* cell, std::vector<grid*>& fullPath)
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

void grid::GetOrphanPath(grid* prevcell, grid* cell, std::vector<grid*>& currenpath, std::vector<grid*>& orphanPath)
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
