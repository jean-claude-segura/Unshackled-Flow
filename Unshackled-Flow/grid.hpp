#ifdef _DEBUG
#include <iostream>
#endif // _DEBUG


class grid {
public:
	static void populaterec(grid* topleftcorner, int height, int width);
public:
	grid() {};
	~grid();
	grid* up = nullptr;
	grid* left = nullptr;
	grid* down = nullptr;
	grid* right = nullptr;
	unsigned int colour = 0;
};