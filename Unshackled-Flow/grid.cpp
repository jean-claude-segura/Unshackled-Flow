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
