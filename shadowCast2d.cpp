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


	vector<sEdge> vecEdges;
	void ConvertTileMapToPolyMap(int sx, int sy, int w, int h, float fBlockWidth, int pitch) {
		// Clearing PolyMap
		vecEdges.clear();
		for(int x=0;x<w;x++)
			for (int y = 0; y < h; y++) {
				for (int j = 0; j < 4; j++) {
					world[(y + sy) * pitch + (x + sx)].edge_exist[j] = false;
					world[(y + sy) * pitch + (x + sx)].edge_id[j] = 0;
				}
			}

		// The actual Algorithm
		for(int x=1 ; x < w - 1 ; x++)
			for (int y = 1; y < h - 1; y++) {
				// Settin some initial variables
				int i = (y + sy) * pitch + (x + sx); // Current cell
				int n = (y + sy-1) * pitch + (x + sx); // North Cell
				int s = (y + sy+1) * pitch + (x + sx); // South Cell
				int e = (y + sy) * pitch + (x + sx+1); // East Cell
				int w = (y + sy) * pitch + (x + sx-1); // West Cell

				// If current cell exist , updating edges
				if (world[i].exist) {

					// 1 . If the western neighbour DNE then giving it west edge 
					if (!world[w].exist) {
						// The western edge could also be the same as 
						// one of the one above it i.e WEST edge of the cell
						// north of it , so checking it
						if (world[n].edge_exist[WEST]) {
							// Now Northern neighbour has a western edge so, growing it
							vecEdges[world[n].edge_id[WEST]].eY += fBlockWidth;
							world[i].edge_id[WEST] = world[n].edge_id[WEST];
							world[i].edge_exist[WEST] = true;
						}
						else {
							// If Northern Neighbour is'nt there than creating a new edge
							sEdge edge;
							edge.sX = (sx + x) * fBlockWidth; edge.sY = (sy + y) * fBlockWidth;
							edge.eX = (sx + x) * fBlockWidth; edge.eY = (sy + y + 1) * fBlockWidth;

							int edge_id = vecEdges.size();
							vecEdges.push_back(edge);
							world[i].edge_id[WEST] = edge_id;
							world[i].edge_exist[WEST] = true;
						}
					}


					// 2 . If the Northern neighbour DNE then giving it Northern edge 
					if (!world[n].exist) {
						// The northern edge could also be the same as 
						// that of one on the left i.e EAST which is North edge of the cell
						// left of it , so checking it
						if (world[w].edge_exist[NORTH]) {
							// Now Western neighbour has a Northern edge so, growing it
							vecEdges[world[w].edge_id[NORTH]].eX += fBlockWidth;
							world[i].edge_id[NORTH] = world[w].edge_id[NORTH];
							world[i].edge_exist[NORTH] = true;
						}
						else {
							// If Eastern Neighbour is'nt there than creating a new edge
							sEdge edge;
							edge.sX = (sx + x) * fBlockWidth; edge.sY = (sy + y) * fBlockWidth;
							edge.eX = (sx + x + 1) * fBlockWidth; edge.eY = (sy + y) * fBlockWidth;

							int edge_id = vecEdges.size();
							vecEdges.push_back(edge);
							world[i].edge_id[NORTH] = edge_id;
							world[i].edge_exist[NORTH] = true;
						}
					}


					// 3 . If the Eastern neighbour DNE then giving it Eastern edge 
					if (!world[e].exist) {
						// The Eastern edge could also be the same as 
						// that of one on the top i.e North which is East edge of the cell
						// Above it , so checking it
						if (world[n].edge_exist[EAST]) {
							// Now Northern neighbour has a Eastern edge so, growing it
							vecEdges[world[n].edge_id[EAST]].eY += fBlockWidth;
							world[i].edge_id[EAST] = world[n].edge_id[EAST];
							world[i].edge_exist[EAST] = true;
						}
						else {
							// If Northern Neighbour is'nt there than creating a new edge
							sEdge edge;
							edge.sX = (sx + x + 1) * fBlockWidth; edge.sY = (sy + y) * fBlockWidth;
							edge.eX = (sx + x + 1) * fBlockWidth; edge.eY = (sy + y + 1) * fBlockWidth;

							int edge_id = vecEdges.size();
							vecEdges.push_back(edge);
							world[i].edge_id[EAST] = edge_id;
							world[i].edge_exist[EAST] = true;
						}
					}


					// 4 . If the Southern neighbour DNE then giving it Southern edge 
					if (!world[s].exist) {
						// The Southern edge could also be the same as 
						// that of one on the left i.e West which is Southern edge of the cell
						// Left of it , so checking it
						if (world[w].edge_exist[SOUTH]) {
							// Now Western neighbour has a Southern edge so, growing it
							vecEdges[world[w].edge_id[SOUTH]].eX += fBlockWidth;
							world[i].edge_id[SOUTH] = world[w].edge_id[SOUTH];
							world[i].edge_exist[SOUTH] = true;
						}
						else {
							// If Western Neighbour is'nt there than creating a new edge
							sEdge edge;
							edge.sX = (sx + x) * fBlockWidth; edge.sY = (sy + y + 1) * fBlockWidth;
							edge.eX = (sx + x + 1) * fBlockWidth; edge.eY = (sy + y + 1) * fBlockWidth;

							int edge_id = vecEdges.size();
							vecEdges.push_back(edge);
							world[i].edge_id[SOUTH] = edge_id;
							world[i].edge_exist[SOUTH] = true;
						}
					}
					// Updating Edges on all four sides of the cell completed
				}
			}
	}

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
			int i = ((int)fSourceY / (int)fBlockWidth) * nWorldWidth + ((int)fSourceX / (int)fBlockWidth);
			// Making the cell exist as there is a Opaque block here
			world[i].exist = !world[i].exist;
		}
		ConvertTileMapToPolyMap(0, 0, 40, 30, fBlockWidth, nWorldWidth); 
		
		// Drawing The selected blocks
		// First clearing all screen
		Clear(olc::DARK_BLUE);
		// Drawing Blocks form tilemap
		for(int x=0;x<nWorldWidth;x++)
			for (int y = 0; y < nWorldHeight; y++) {
				if(world[y*nWorldWidth + x].exist)
					FillRect(x*fBlockWidth , y*fBlockWidth , fBlockWidth,fBlockWidth , olc::BLUE);
			}

		// Drawing Edges from Polymap
		for(auto &e:vecEdges){
			DrawLine(e.sX,e.sY,e.eX,e.eY);
			FillCircle(e.sX, e.sY, 2, olc::RED);
			FillCircle(e.eX, e.eY, 2, olc::RED);
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
