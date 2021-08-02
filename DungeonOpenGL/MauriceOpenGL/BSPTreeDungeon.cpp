#include "BSPTreeDungeon.h"
#include <iostream>
#include <random>

namespace
{
	std::random_device rd;
	std::mt19937 mt(rd());

	bool randomBool()
	{
		std::uniform_int_distribution<> dist(0, 1);
		if (dist(mt) == 0)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	int randomInt(int min, int max)
	{
		std::uniform_int_distribution<> dist(0, max - min);
		return dist(mt) + min;
	}
}

using namespace std;

void BSPTreeDungeon::Setup()
{
	for (int x = 0; x < 30; x++)
	{
		for (int y = 0; y < 30; y++)
		{
			if (x == 0 || x == 29 || y == 0 || y == 29)
			{
				tiles[x][y] = Wall;
			}
			else
			{
				tiles[x][y] = Floor;
			}
		}
	}

	Rooms.clear();

	Box* root = new Box(0, 0, 30, 30);
	if (CanSplit(root))
	{
		cout << "Splitting\n";
	}

	IsARoom(root);
	cout << "Amount of rooms: " << Rooms.size() << endl;
	int roomNumber = randomInt(0, Rooms.size() - 1);

	// To make sure it's not in front of a door
	// TODO: Add door tile
	int bedX = randomInt(Rooms[roomNumber]->x + 2, Rooms[roomNumber]->x + Rooms[roomNumber]->width - 2);
	int bedY = randomInt(Rooms[roomNumber]->y + 2, Rooms[roomNumber]->y + Rooms[roomNumber]->height - 2);
	tiles[bedX][bedY] = Bed;

	// TODO: make actual rooms, examples: sleeping room with a bed, dining room with table, storage room with those barrels
	// Create a list of options for rooms and loop through the rooms, to make sure not all are bedrooms for now check when chosing a room if same type of room is not equal to number of rooms

	for (int y = 0; y < 30; y++)
	{
		for (int x = 0; x < 30; x++)
		{
			if (tiles[x][y] == Wall)
			{
				cout << "+";
			}
			else if (tiles[x][y] == Bed)
			{
				cout << "<";
			}
			else
			{
				cout << ".";
			}
		}

		// Add an end to the line otherwise the layout of the dungeon is wrong
		cout << endl;
	}

}

// If the box doesn't have any children then it means it's a room
void BSPTreeDungeon::IsARoom(Box* rect)
{
	if (rect->left != NULL)
	{
		IsARoom(rect->left);
		IsARoom(rect->right);
	}
	else
	{
		Rooms.push_back(rect);
	}
}

bool BSPTreeDungeon::CanSplit(Box* rect)
{
	// Not to be able to split in two rooms that are at least the MinSize
	if (rect->width > MinSize * 2 || rect->height > MinSize * 2)
	{
		bool splitH;
		bool tooBig = false;
		int splitIndex;
		int door;

		// if the width is double the height - split in vertical
		if (rect->width > rect->height * 2)
		{
			splitH = false;
			tooBig = true;
		}
		else if (rect->height > rect->width * 2)
		{
			splitH = true;
			tooBig = true;
		}

		// if not one size is at least double the size of the other
		if (!tooBig)
		{
			cout << "not overly big on one side\n";
			// see if it's possible to split in width or height, when both are possible - pick one at random
			if (rect->width < MinSize * 2)
			{
				cout << "split in width impossible too small\n";
				splitH = true;
			}
			else if (rect->height < MinSize * 2)
			{
				cout << "split in height impossible too small\n";
				splitH = false;
			}
			else
			{
				splitH = randomBool();
			}
		}

		//****************Actual splitting*************************************

		// splitting horizontal
		if (splitH)
		{
			cout << "split horizontal\n";
			// get a random spot but not on the edge of the wall because that would overlap
			splitIndex = randomInt(rect->y+1, rect->y + rect->height);
			// when the random number gets too low - make it MinSize
			if (splitIndex < (rect->y + MinSize))
			{
				cout << "changing to MinSize\n";
				splitIndex = rect->y + MinSize;
			}
			else if ((rect->height - splitIndex) < MinSize)
			{
				cout << "too big -- changing to MinSize\n";
				splitIndex = rect->height - MinSize;
			}

			// get a random spot to leave room to walk through, not on the edges of the wall because that wouldn't be a passage
			door = randomInt(rect->x + 1, rect->x + rect->width - 1);
			for (int x = rect->x; x < rect->x + rect->width; x++)
			{
				if (x != door)
				{
					tiles[x][splitIndex] = Wall;
				}
			}

			// create the new boxes to be split further if possible
			rect->left = new Box(rect->x, rect->y, rect->width, splitIndex);
			rect->right = new Box(rect->x, splitIndex, rect->width, rect->y + rect->height - splitIndex);

			cout << "left x: " << rect->left->x << " y: " << rect->left->y << " width: " << rect->left->width << " height: " << rect->left->height << endl;
			cout << "right x: " << rect->right->x << " y: " << rect->right->y << " width: " << rect->right->width << " height: " << rect->right->height << endl;

			if (CanSplit(rect->left))
			{
				cout << "another split\n";
			}
			if (CanSplit(rect->right))
			{
				cout << "another split\n";
			}
		}
		// splitting vertical
		else
		{
			cout << "split vertical\n";
			// get a random spot but not on the edge of the wall because that would overlap
			splitIndex = randomInt(rect->x + 1, rect->x + rect->width);
			// when the random number gets too low - make it MinSize
			//TODO:Fix this
			if (splitIndex < (rect->x + MinSize))
			{
				cout << "changing to MinSize\n";
				splitIndex = rect->x + MinSize;
			}
			else if ((rect->width - splitIndex) < MinSize)
			{
				cout << "too big -- changing to MinSize\n";
				splitIndex = rect->x + (rect->width - MinSize);
			}

			// get a random spot to leave room to walk through, not on the edges of the wall because that wouldn't be a passage
			door = randomInt(rect->y + 1, rect->y + rect->height - 1);
			for (int y = rect->y; y < rect->y + rect->height; y++)
			{
				if (y != door)
				{
					tiles[splitIndex][y] = Wall;
				}
			}

			// create the new boxes to be split further if possible
			rect->left = new Box(rect->x, rect->y, splitIndex - rect->x, rect->height);
			rect->right = new Box(splitIndex, rect->y, rect->x + rect->width - splitIndex, rect->height);

			cout << "left x: " << rect->left->x << " y: " << rect->left->y << " width: " << rect->left->width << " height: " << rect->left->height << endl;
			cout << "right x: " << rect->right->x << " y: " << rect->right->y << " width: " << rect->right->width << " height: " << rect->right->height << endl;

			if (CanSplit(rect->left))
			{
				cout << "another split\n";
			}
			if (CanSplit(rect->right))
			{
				cout << "another split\n";
			}
		}
	}
	else
	{
		cout << "can't be split because one side will be below minsize\n";
		return false;
	}

	return true;
}