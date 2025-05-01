#ifdef _DEBUG
#include <iostream>
#endif // _DEBUG
#include <vector>
#include <random>
#include <map>


class Cell {
	friend class Grid;
private:
	Cell() {};
	uint8_t colour = 0;
	Cell* top = nullptr;
	Cell* left = nullptr;
	Cell* bottom = nullptr;
	Cell* right = nullptr;
	bool path = false;
	bool node = false;
public:
	~Cell();
	uint8_t GetColour() const { return colour; }
	bool IsPath() const { return path; }
	bool IsNode() const { return node; }
	bool IsFree() const { return !path && !node; }
	void SetColour(uint8_t _colour) { if (!node) colour = _colour; }
	void SetPath(uint8_t _colour) { if (!node) { path = true; colour = _colour; } }
	void UnsetPath() { if (!node) { path = false; colour = 0; } }

	Cell* GetTop(){ return top; }
	Cell* GetLeft() { return left; }
	Cell* GetBottom() { return bottom; }
	Cell* GetRight() { return right; }

	bool IsFinal(Cell* prevcell);
	bool IsAdjacent(Cell* prevcell);
	bool IsHorizontallyAdjacent(Cell* prevcell);
	bool IsVerticallyAdjacent(Cell* prevcell);
	bool IsTakeBack(Cell* prevcell);
	bool IsLink(Cell* prevcell);
public:
	Cell* operator++();
	Cell* operator--();
};

class Grid
{
	static void GetLongestPath(Cell* cell, std::vector<Cell*>& shortestpath);
	static void GetLongestPath(Cell* cell, std::vector<Cell*>& currenpath, std::vector<Cell*>& shortestpath);
	static void GetShortestPath(Cell* cell, std::vector<Cell*>& shortestpath);
	static void GetShortestPath(Cell* cell, std::vector<Cell*>& currenpath, std::vector<Cell*>& shortestpath);
	static void GetOrphanPath(Cell* cell, std::vector<Cell*>& Orphanpath);
	static void GetFullPathFromNode(Cell* cell, std::vector<Cell*>& fullPath);
	static void GetOrphanPath(Cell* prevcell, Cell* cell, std::vector<Cell*>& currenpath, std::vector<Cell*>& Orphanpath);
public:
	static void ClearPathFromNode(Cell* cell, std::vector<Cell*>& clearedPath);
	static void ClearRelevantPath(Cell* cell, std::vector<Cell*>& clearedPath);
public:
	void Populate(uint8_t colours) const;
	void Addrow(Cell* firstleftcell, int width);
	void Init(int width, int height);
	Grid(int x, int y, uint8_t colours);
	~Grid();

	Cell* startingPoint = nullptr;

	std::map<Cell*, std::pair<int, int>> mCellToCoordinates;
	std::unique_ptr<std::unique_ptr<Cell* []>[]> arrCoordinatesToCell;
};
