#pragma once
#include <vector>

class Box
{
public:
	Box()
	{

	}
	~Box()
	{

	}
	Box(int _x, int _y, int _width, int _height)
	{
		x = _x;
		y = _y;
		width = _width;
		height = _height;
	}

	int x, y, width, height;
	Box* left;
	Box* right;
};

class BSPTreeDungeon
{
public:
	BSPTreeDungeon()
	{

	}
	~BSPTreeDungeon()
	{

	}

	int MinSize = 10;
	int rows = 30;
	int columns = 30;

	int counter = 0;

	enum TileType { Wall, Floor, Bed };
	TileType tiles[30][30];

	std::vector<Box*> Rooms;

	void Setup();

	void IsARoom(Box* rect);

	bool CanSplit(Box* rect);
};
