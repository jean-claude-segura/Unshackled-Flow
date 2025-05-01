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

	static void GetLongestPath(grid* cell, std::vector<grid*>& shortestpath);
	static void GetLongestPath(grid* cell, std::vector<grid*>& currenpath, std::vector<grid*>& shortestpath);
	static void GetShortestPath(grid* cell, std::vector<grid*>& shortestpath);
	static void GetShortestPath(grid* cell, std::vector<grid*>& currenpath, std::vector<grid*>& shortestpath);
	static void GetOrphanPath(grid* cell, std::vector<grid*>& Orphanpath);
	static void GetFullPathFromNode(grid* cell, std::vector<grid*>& fullPath);
	static void GetOrphanPath(grid* prevcell, grid* cell, std::vector<grid*>& currenpath, std::vector<grid*>& Orphanpath);
public:
	static void ClearPathFromNode(grid* cell, std::vector<grid*>& clearedPath);
	static void ClearRelevantPath(grid* cell, std::vector<grid*>& clearedPath);
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
	uint8_t GetColour() const { return colour; }
	bool IsPath() const { return path; }
	bool IsNode() const { return node; }
	bool IsFree() const { return !path && !node; }
	void SetColour(uint8_t _colour) { if (!node) colour = _colour; }
	void SetPath(uint8_t _colour) { if (!node) { path = true; colour = _colour; } }
	void UnsetPath() { if (!node) { path = false; colour = 0; } }
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
