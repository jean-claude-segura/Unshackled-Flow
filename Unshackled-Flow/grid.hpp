#ifdef _DEBUG
#include <iostream>
#include <vector>
#include <random>
#endif // _DEBUG


class grid {
public:
	static void populate(grid* topleftcorner, int colours);
	static void init(grid* topleftcorner, int height, int width);
	static void addrow(grid* topleftcorner, int width);
public:
	grid() {};
	~grid();
	grid* top = nullptr;
	grid* left = nullptr;
	grid* bottom = nullptr;
	grid* right = nullptr;
	unsigned int colour = 0;
	grid* operator++();
	grid* operator--();
};
