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
	uint8_t colour = 0;
public:
	grid(int height, int width, int colours);
	~grid();
	uint8_t GetColour() { return 0b01111111 & colour; }
	bool IsPath() { return (colour & 0b01111111) != 0; }
public:
	grid* top = nullptr;
	grid* left = nullptr;
	grid* bottom = nullptr;
	grid* right = nullptr;
	grid* operator++();
	grid* operator--();
};
