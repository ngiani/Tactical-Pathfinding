#pragma once
#include <vector>
#include "dstructures.h"



struct Tile
{
	
	Vector2D center;
	int size;
	float influence;


	public:	
		Tile();
		Tile(int id, float size, Vector2D center);

		int ID;
};

//Influence map
class Map {

	const int tilesSize;
	const int mapWidth;
	const int mapHeigth;
	

	public:
		Map(const int, const int, const int);

		int getWidth();
		int getHeigth();
		Tile* quantize(Vector2D);
		Vector2D localize(Tile*);
		void generateTiles();
		void spreadInfluence(Vector2D,float influence);
		int getTilesSize();
		std::vector<std::vector<Tile>> tiles;
		
};



