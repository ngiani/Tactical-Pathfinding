
#include <cmath>
#include <string>
#include "map.h"



Tile::Tile(){

	ID = 0;
	size = 0;
	center = Vector2D(0, 0);

	influence = 0;
	
}

//Init tile
Tile::Tile(int id, float size, Vector2D center) : ID(id), size(size), center(center) {

	influence = 0;
}


Map::Map(const int size, const int mapWidth, const int mapHeigth) : tilesSize(size), mapWidth(mapWidth), mapHeigth(mapHeigth){}

int Map::getWidth()
{
	return mapWidth;
}

int Map::getHeigth()
{
	return mapHeigth;
}

//Convert position to a map tile
Tile* Map::quantize(Vector2D position){


	int tileX = position.x / tilesSize;
	int tileY = position.y / tilesSize;

	if (tileX <= 0)
		tileX = 0;
	if (tileY <= 0)
		tileY = 0;

	if (tileX > tiles.size())
		tileX = tiles.size();
	if (tileY > tiles[0].size())
		tileY = tiles[0].size();

	return &tiles[tileX][tileY];
}

//Convert map tile to position
Vector2D Map::localize(Tile* tile){

	return Vector2D(tile->center.x, tile->center.y);
}


void Map::generateTiles()
{
	int columnCount = mapWidth / tilesSize;
	int rowCount = mapHeigth / tilesSize;

	int count = 0;

	for (int i = 0; i < columnCount; i++) {
		tiles.push_back(std::vector<Tile>());
		for (int j = 0; j < rowCount; j++) {
			tiles[i].push_back(Tile(count, tilesSize, Vector2D((i*tilesSize) + tilesSize / 2, (j*tilesSize) + tilesSize / 2)));
			count++;
		}
	}
}

//Spread influence from point through all tiles in the map
void Map::spreadInfluence(Vector2D position, float influence)
{

	

	for (int i = 0; i < tiles.size(); i++)
		for (int j = 0; j < tiles[i].size(); j++) {
			
			//Calculate and normalize distance from point
			float distance = tiles[i][j].center.distance(position);

			float maxDistance = sqrt(powl(mapWidth,2.0) + powl(mapHeigth,2.0));

			float normalizedDistance = distance/maxDistance;

			//Add influence to tile with non-linear drop-off equation
			
			tiles[i][j].influence += influence/(pow(normalizedDistance + 1,10.0f));


		}


}

int Map::getTilesSize()
{
	return tilesSize;
}


