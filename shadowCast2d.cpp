// shadowCast2d.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#define _USE_MATH_DEFINES
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
	float fBlockWidth = 16.0f;

	olc::Sprite *sampleImg;
	olc::Decal *sampleImgDEC;

	olc::Sprite *wall;
	olc::Decal *wallDEC;

	olc::Sprite *grassBG;
	olc::Sprite *grassBGDark;
	olc::Decal *grassBGDEC;

	olc::Sprite *sprLightCast;
	olc::Sprite *buffLightRay;
	olc::Sprite *buffLightTex;

	olc::Sprite *bulbOff;
	olc::Decal *bulbOffDEC;
	olc::Sprite *bulbOn;
	olc::Decal *bulbOnDEC;

    
	// endpoints - <Xcordinate,Ycoordinate,angle>
 	vector <tuple<int,int,float>> endPoints;
	void startRay(float lightX, float lightY){

		endPoints.clear();
		// lightX and lightY are mouse positions which'll be starting posions of light rays
		float r_px = lightX;
		float r_py = lightY;
		float radius = 20 * fBlockWidth;

		for(auto edge1:vecEdges){
			// for each of the 2 end of each edge emerging 3 rays
			for(int i=0;i<2;i++){
				float r_dx = (i==0 ? edge1.sX : edge1.eX) - r_px;
				float r_dy = (i==0 ? edge1.sY : edge1.eY) - r_py;

				float baseAngle = atan2f(r_dy,r_dx),angle;

				for(int j=0;j<2;j++){
					if(j==0) angle = baseAngle - 0.0001f;
					if(j==2) angle = baseAngle;
					if(j==1) angle = baseAngle + 0.0001f;

					r_dx = radius * cosf(angle);
					r_dy = radius * sinf(angle);
					float minT1 = 50,T1,T2;
					bool validIntersect = false;
					for(auto iterEdge:vecEdges){
						
						float s_px = iterEdge.sX; float s_dx = iterEdge.eX-iterEdge.sX;
						float s_py = iterEdge.sY; float s_dy = iterEdge.eY-iterEdge.sY;
						if (fabs(s_dx - r_dx) > 0.0f && fabs(s_dy - r_dy) > 0.0f){
							// Solve for T2!
							T2 = (r_dx*(s_py-r_py) + r_dy*(r_px-s_px))/(s_dx*r_dy - s_dy*r_dx);

							// Plug the value of T2 to get T1
							T1 = (s_px+s_dx*T2-r_px)/r_dx;

							if(T1>0 && T2>0 && T2<1){
								if(T1 < minT1){
									minT1 = T1;
									validIntersect = true;
								}
							}
						}
					}
					if(validIntersect)
					endPoints.push_back( { r_px + r_dx*minT1  ,  r_py + r_dy*minT1, angle} );
				}

			}
		}
		// Sorting all points acc to angle , [&] means accessing all variables by reference
		sort(endPoints.begin(),endPoints.end(),  [&](const tuple<int, int, float> &t1, const tuple<int, int, float> &t2){
			return get<2>(t1) < get<2>(t2);
		});
	}

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

		sampleImg = new olc::Sprite("assets/light5.png");
		sampleImgDEC = new olc::Decal(sampleImg);

		wall = new olc::Sprite("assets/wall.png");
		wallDEC = new olc::Decal(wall);

		bulbOff = new olc::Sprite("assets/bulb-0.png");
		bulbOffDEC = new olc::Decal(bulbOff);

		bulbOn = new olc::Sprite("assets/bulb-1.png");
		bulbOnDEC = new olc::Decal(bulbOn);

		grassBGDark = new olc::Sprite("assets/grassBGDark.png");
		grassBG = new olc::Sprite("assets/grassBG.png");		
		grassBGDEC = new olc::Decal(grassBG);

		buffLightTex = new olc::Sprite(ScreenWidth(), ScreenHeight());
		buffLightRay = new olc::Sprite(ScreenWidth(), ScreenHeight());

		// Add a boundary to the world
		for (int x = 1; x < (nWorldWidth - 1); x++)
		{
			world[1 * nWorldWidth + x].exist = true;
			world[(nWorldHeight - 2) * nWorldWidth + x].exist = true;
		}

		for (int x = 1; x < (nWorldHeight - 1); x++)
		{
			world[x * nWorldWidth + 1].exist = true;
			world[x * nWorldWidth + (nWorldWidth - 2)].exist = true;
		}
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		
		// Code for finding out pixel color and alpha values from Pixel
		// for(int x=0;x<255;x++){
		// 	olc::Pixel p = grassBGDark->GetPixel(x,255);
		// 	unsigned int P = p.n;

		// 	unsigned int A = ( (((1<<8) - 1) << 24) & P ) >> 24;
		// 	unsigned int B = ( (((1<<8) - 1) << 16) & P ) >> 16;
		// 	unsigned int G = ( (((1<<8) - 1) <<  8) & P ) >>  8;
		// 	unsigned int R = ( (((1<<8) - 1) <<  0) & P ) >>  0;
		// 	// cout << R << ',' << G << ','  << B << ',' << A << '\n';
		// 	// cout << P << '\n';
		// }

		// Updating Mouse position
		float fSourceX = GetMouseX();
		float fSourceY = GetMouseY();

		// Checking if Mouse has been left clicked 
		// SO that to switch between making tile and light src
		if (GetMouse(1).bReleased) {
			// To get the index of the tile at which the mouse was released
			int i = ((int)fSourceY / (int)fBlockWidth) * nWorldWidth + ((int)fSourceX / (int)fBlockWidth);
			// Making the cell exist as there is a Opaque block here
			world[i].exist = !world[i].exist;
		}
		ConvertTileMapToPolyMap(0, 0, 40, 30, fBlockWidth, nWorldWidth); 
		
		bool draw = false;
		pair <int,int> curMouse;
		if(GetMouse(0).bHeld){
			draw = true;
			curMouse.first = GetMouseX();
			curMouse.second = GetMouseY();
			startRay(curMouse.first,curMouse.second);
		}
			

		// Drawing The selected blocks
		// First clearing all screen by making BG as Grass
		SetDrawTarget(nullptr);
		DrawSprite(0,0, grassBGDark);
		
		// Drawing Blocks form tilemap
		for(int x=0;x<nWorldWidth;x++)
			for (int y = 0; y < nWorldHeight; y++) {
				if(world[y*nWorldWidth + x].exist){
					olc::vf2d pos = { float(x*fBlockWidth) , float(y*fBlockWidth) };
					DrawDecal(pos, wallDEC);
				}
			}

		int nRaysCast = endPoints.size();
		// Removing similar points
		auto it = unique(endPoints.begin(),endPoints.end(),[&](tuple <int,int,float> &t1, tuple <int,int,float> &t2){
			return ( fabs(get<0>(t1)-get<0>(t2)) < 0.1f  && fabs(get<1>(t1) - get<0>(t2)) < 0.1f  );
		});
		endPoints.resize(distance(endPoints.begin(),it));
		int nRaysCast2 = endPoints.size();

		// DrawString(4, 4, "Rays Cast: " + to_string(nRaysCast) + " Rays Drawn: " + to_string(nRaysCast2));
		DrawString(16, 4, "Press Right-Click for Wall , Left Click for Light",olc::WHITE,1.5);

		olc::vf2d mouse = { float(fSourceX-16) , float(fSourceY-32) };
		if(draw)
			DrawDecal(mouse, bulbOnDEC);
		else
			DrawDecal(mouse, bulbOffDEC);

		if(draw){

			// Clear offscreen buffer for sprite
			SetDrawTarget(buffLightTex);
			Clear(olc::BLANK);

			// Draw "Radial Light" sprite to offscreen buffer, centered around 
			// source location (the mouse coordinates, buffer is 512x512)
			SetPixelMode(olc::Pixel::ALPHA);
			DrawSprite(fSourceX - sampleImg->width/2, fSourceY - sampleImg->height/2, sampleImg);
			SetPixelMode(olc::Pixel::NORMAL);

			// Clear offsecreen buffer for rays
			SetDrawTarget(buffLightRay);
			Clear(olc::BLANK);

			for(int i=0;i< endPoints.size()-1;i++){
				FillTriangle(fSourceX,fSourceY,
							get<0>(endPoints[i]),get<1>(endPoints[i]),
							get<0>(endPoints[i+1]),get<1>(endPoints[i+1]));
			}
			FillTriangle(fSourceX, fSourceY,
				get<0>(endPoints[0]), get<1>(endPoints[0]),
				get<0>(endPoints[endPoints.size()-1]), get<1>(endPoints[endPoints.size()-1]));

			SetDrawTarget(nullptr);
			for (int x = 0; x < ScreenWidth(); x++)
				for (int y = 0; y < ScreenHeight(); y++)
					if (buffLightRay->GetPixel(x, y).r > 0){
						// Getting the pixel value of the light and setting the alpha of grass background acc to it 
						olc::Pixel grad = buffLightTex->GetPixel(x, y);
						unsigned int P = grad.n;
						unsigned int alpha = ( (((1<<8) - 1) <<  0) & P ) >>  0;
						// Pumping up the very low values of alpha , so that bg is visible 
						// Default is 63
						if(alpha < 66)
							alpha = 65;

						olc::Pixel target = grassBGDark->GetPixel(x,y);
						P = target.n;
						unsigned int B = ( (((1<<8) - 1) << 16) & P ) >> 16;
						unsigned int G = ( (((1<<8) - 1) <<  8) & P ) >>  8;
						unsigned int R = ( (((1<<8) - 1) <<  0) & P ) >>  0;
						olc::Pixel final(R,G,B,alpha);
						Draw(x, y, final);
						
					}
						
						
		}

		return true;
	}
};


int main()
{
	ShadowCasting2D demo;
	if (demo.Construct(640, 480, 1, 1))
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
