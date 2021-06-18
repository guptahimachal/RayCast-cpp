// shadowCast2d.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
using namespace std;

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

struct sEdge{
	float sX, sY;
	float eX, eY;
};
struct sCell {
	int edge_id[4];
	bool edge_exist[4];
	bool exist = false;
};

#define NORTH 0
#define SOUTH 1
#define EAST 2
#define WEST 3


// Override base class with your custom functionality
class ShadowCasting2D : public olc::PixelGameEngine
{
private:
	sCell* world;
	int nWorldWidth = 40;
	int nWorldHeight = 30;

public:
	ShadowCasting2D()
	{
		// Name your application
		sAppName = "ShadowCasting2D";
	}
public:
	bool OnUserCreate() override
	{
		// Called once at the start, so create things here
		world = new sCell[nWorldWidth * nWorldHeight];
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		float fBlockWidth = 16.0f;
		// Updating Mouse position
		float fSourceX = GetMouseX();
		float fSourceY = GetMouseY();

		// Checking if Mouse has been left clicked 
		// SO that to switch between making tile and light src
		if (GetMouse(0).bReleased) {
			// To get the index of the tile at which the mouse was released
			int i = ((int)fSourceX / (int)fBlockWidth) * nWorldWidth + ((int)fSourceY / (int)fBlockWidth);
			// Making the cell exist as there is a Opaque block here
			world[i].exist = !world[i].exist;
		}
		
		// Drawing The selected blocks
		// First clearing all screen
		Clear(olc::BLACK);
		// Drawing Blocks form tilemap
		for(int x=0;x<nWorldWidth;x++)
			for (int y = 0; y < nWorldHeight; y++) {
				if(world[x*nWorldWidth + y].exist)
					FillRect(x*fBlockWidth , y*fBlockWidth , fBlockWidth,fBlockWidth , olc::BLUE);
			}

		return true;
	}
};


int main()
{
	ShadowCasting2D demo;
	if (demo.Construct(640, 480, 2, 2))
		demo.Start();
	
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
