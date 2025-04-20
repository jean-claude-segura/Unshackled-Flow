#ifdef _DEBUG
#include <iostream>
#endif // _DEBUG
#include <vector>
#include <random>


class grid {
private:
	static void populate(grid* topleftcorner, int colours);
	static void init(grid* topleftcorner, int height, int width);
	static void addrow(grid* topleftcorner, int width);
private:
	grid() {};
	void populate(int colours);
	void init(int height, int width);
public:
	grid(int height, int width, int colours);
	~grid();
	grid* top = nullptr;
	grid* left = nullptr;
	grid* bottom = nullptr;
	grid* right = nullptr;
	unsigned int colour = 0;
	grid* operator++();
	grid* operator--();
};
