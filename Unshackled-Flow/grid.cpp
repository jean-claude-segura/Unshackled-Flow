#include "grid.hpp"

void grid::populaterec(grid * topleftcorner, int height, int width)
{
	grid* curcell = topleftcorner;
	grid* firstcell = curcell;
	// Première ligne :
	for (int x = 0; x < width - 1; ++x)
	{
		curcell->right = new grid();
		curcell->right->left = curcell;
		curcell = curcell->right;
	}
	for (int y = 0; y < height - 1; ++y)
	{		
		curcell = firstcell;
		while (nullptr != curcell)
		{
			curcell->down = new grid();
			curcell->down->up = curcell;
			curcell->down->right = new grid();
			curcell->down->right->up = curcell->right;
			curcell->down->right->left = curcell;
			curcell = curcell->right;
		}
		firstcell = firstcell->down;
	}
}

grid::~grid()
{
	//if (nullptr != up) delete up;
	//if (nullptr != left) delete left;
	if (nullptr != down) delete down;
	if (nullptr != right) delete right;
#ifdef _DEBUG
	static unsigned int cell = 0;
	std::cout << "cellule(s) détruite(s) : " << ++cell << std::endl;
#endif // _DEBUG
}
