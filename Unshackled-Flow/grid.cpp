#include "grid.hpp"

void grid::populate(grid * topleftcorner, int height, int width)
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
	if (nullptr != right) delete right;
	if (nullptr != bottom) delete bottom;
#ifdef _DEBUG
	static unsigned int cell = 0;
	std::cout << "cellule(s) détruite(s) : " << ++cell << std::endl;
#endif // _DEBUG
}

grid* grid::operator++()
{
	if (nullptr != left)
		return left;
	else if (nullptr != this->right)
	{
		grid* cur = this;
		while (nullptr != cur->right)
			cur = cur->right;
		return cur->bottom;
	}
	else
		return nullptr;
}

grid* grid::operator--()
{
	if (nullptr != right)
		return right;
	else if (nullptr != this->left)
	{
		grid* cur = this;
		while (nullptr != cur->left)
			cur = cur->left;
		return cur->top;
	}
	else
		return nullptr;
}
