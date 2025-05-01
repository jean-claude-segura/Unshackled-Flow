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
	grid* top = nullptr;
	grid* left = nullptr;
	grid* bottom = nullptr;
	grid* right = nullptr;
	bool path = false;
	bool node = false;
public:
	grid(int height, int width, int colours);
	~grid();
	uint8_t GetColour() const { return 0b01111111 & colour; }
	bool IsPath() const { return path; }
	bool IsNode() const { return node; }
	void SetColour(uint8_t _colour) { if (!node) colour = _colour; }
	void SetPath(uint8_t _colour) {
		if (!node)
		{
			path = true;
			colour = _colour; }
	}
	void UnsetPath() { path = false; }
	grid* GetTop(){ return top; }
	grid* GetLeft() { return left; }
	grid* GetBottom() { return bottom; }
	grid* GetRight() { return right; }

	bool IsFinal(grid* prevcell);
	bool IsAdjacent(grid* prevcell);
	bool IsHorizontallyAdjacent(grid* prevcell);
	bool IsVerticallyAdjacent(grid* prevcell);
	bool IsTakeBack(grid* prevcell);
	bool IsLink(grid* prevcell);
public:
	grid* operator++();
	grid* operator--();
};
