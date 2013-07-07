#include "map.h"
#include <utility>
#include <math.h>
#include <vector>
#include <windows.h>
#include <sstream>


ISO::map::map(void)
{
}


ISO::map::~map(void)
{
}

ISO::map::map(unsigned int x, unsigned int y, unsigned int defaultHeight, tileset* defaultTileSet)
{
	mapTiles.resize(x);
	for(unsigned i = 0; i < x; ++i)
	{
		mapTiles[i].resize(y, tile(defaultTileSet, 0, defaultHeight));
	}
	size_x = x;
	size_y = y;
	default_z = defaultHeight;
	defaultSet = defaultTileSet;
}

bool ISO::map::setSize(unsigned int x, unsigned int y)
{
	// TODO: write resize command
	return false;
}
sf::Vector2u ISO::map::getSize()
{
	return sf::Vector2u(size_x, size_y);
}

bool ISO::map::setDefaultTileSet(tileset* set)
{
	if(set && set->isValid())
	{
		defaultSet = set;
		return true;
	}
	return false;
}
ISO::tileset* ISO::map::getDefaultTileSet()
{
	return defaultSet;
}

bool ISO::map::setDefaultHeight(unsigned int height)
{
	default_z = height;
	return true;
}
unsigned int ISO::map::getDefaultHeight()
{
	return default_z;
}

ISO::tile* ISO::map::getMapTile(unsigned int x, unsigned int y)
{
	return &mapTiles[x][y];
}

void ISO::map::preDraw(sf::Vector2f camera)
{
	// figure out which tiles will be drawn
	// x = 0, y = 0 is at the top
	// x goes down and right,
	// y goes down and left
	// top left = 0,0;
	unsigned int tile_width = 128;
	unsigned int tile_height = 64;
	/*
	sf::Vector2f topLeft = toIsometric(sf::Vector2u(cam.left, cam.top));
	sf::Vector2f topRight = toIsometric(sf::Vector2u(cam.left + cam.width, cam.top));
	sf::Vector2f botLeft = toIsometric(sf::Vector2u(cam.left, cam.top + cam.height));
	sf::Vector2f botRight = toIsometric(sf::Vector2u(cam.left + cam.width, cam.top + cam.height));

	// not going to be very efficient for now...
	int minX = int(floor(topLeft.x));
	int maxX = int(ceil(botRight.x));
	int minY = int(floor(topRight.y));
	int maxY = int(ceil(botLeft.y));

	std::vector<const tile*> toDraw;

	for(unsigned int x = minX; x <= maxX; ++x)
	{
		if(x < 0)
		{
			continue;
		}
		for(unsigned int y = minY; y <= maxY; ++y)
		{
			if(y < 0)
			{
				continue;
			}
			toDraw.push_back(&mapTiles[x][y]);
		}
	}

	//so now we want to loop through all the tiles and add them to the vertexlist*/
	tilesToDraw.setPrimitiveType(sf::Quads);
	tilesToDraw.resize(size_x * size_y * 4);
	int x = 0;
	int y = 0;
	// each "tile line"
	//OutputDebugString(L"Draw Start!\n");
	int index = 0;
	for(unsigned int i = 0; i < size_x + size_y; ++i)
	{
		
		y = i;
		x = 0;
		while(y >= int(size_y))
		{
			y--;
			x++;
		}
		while(y >= 0 && x < int(size_x))
		{
			//std::wstringstream buffer;
			int x_draw = x * (tile_width / 2) - y * (tile_width / 2);
			int y_draw = x * (tile_height / 2) + y * (tile_height / 2) + tile_height/2 * mapTiles[x][y].getHeight() ;
			//buffer << "X= "<<x<<"  Y= "<<y<<"  x_draw= "<<x_draw<<"  y_draw= "<<y_draw<<"\n";
			//OutputDebugString(buffer.str().c_str());
			/*
			if(x+size_x*y != drawCell)
			{
				y--;
				x++;
				continue;
			}
			tilesToDraw[0].position = sf::Vector2f(cam.x + x_draw				, cam.y + y_draw);
			tilesToDraw[1].position = sf::Vector2f(cam.x + x_draw + tile_width	, cam.y + y_draw);
			tilesToDraw[2].position = sf::Vector2f(cam.x + x_draw + tile_width	, cam.y + y_draw + tile_width);
			tilesToDraw[3].position = sf::Vector2f(cam.x + x_draw				, cam.y + y_draw + tile_width);
			sf::Rect<unsigned int> textureRect = mapTiles[x][y].getTextureRect();
			tilesToDraw[0].texCoords = sf::Vector2f(float(textureRect.left)						, float(textureRect.top));
			tilesToDraw[1].texCoords = sf::Vector2f(float(textureRect.left + textureRect.width)	, float(textureRect.top));
			tilesToDraw[2].texCoords = sf::Vector2f(float(textureRect.left + textureRect.width)	, float(textureRect.top + textureRect.height));
			tilesToDraw[3].texCoords = sf::Vector2f(float(textureRect.left)						, float(textureRect.top + textureRect.height));
			*/
			
			tilesToDraw[index*4+0].position = sf::Vector2f(camera.x + x_draw				, camera.y + y_draw);
			tilesToDraw[index*4+1].position = sf::Vector2f(camera.x + x_draw + tile_width	, camera.y + y_draw);
			tilesToDraw[index*4+2].position = sf::Vector2f(camera.x + x_draw + tile_width	, camera.y + y_draw + tile_width);
			tilesToDraw[index*4+3].position = sf::Vector2f(camera.x + x_draw				, camera.y + y_draw + tile_width);
			sf::Rect<unsigned int> textureRect = mapTiles[x][y].getTextureRect();
			tilesToDraw[index*4+0].texCoords = sf::Vector2f(float(textureRect.left)						, float(textureRect.top));
			tilesToDraw[index*4+1].texCoords = sf::Vector2f(float(textureRect.left + textureRect.width)	, float(textureRect.top));
			tilesToDraw[index*4+2].texCoords = sf::Vector2f(float(textureRect.left + textureRect.width)	, float(textureRect.top + textureRect.height));
			tilesToDraw[index*4+3].texCoords = sf::Vector2f(float(textureRect.left)						, float(textureRect.top + textureRect.height));
			index++;
			y--;
			x++;
		}
	}

	//OutputDebugString(L"Draw End!\n");
}

void ISO::map::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	

	states.texture = defaultSet->getTexture();

	target.draw(tilesToDraw, states);
}

sf::Vector2f ISO::map::toIsometric(sf::Vector2u point) const
{
	sf::Vector2f iso;
	// tile width = 128;
	// tile height = 64;
	iso.x = float(point.y) / 64.f + float(point.x) / 128.f - (float(size_x) * 64.f);
	iso.y = float(point.y) / 64.f - float(point.x) / 128.f - (float(size_y) * 32.f);
	return iso;
}