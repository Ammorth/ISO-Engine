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

void ISO::map::preDraw(sf::Rect<int> camera)
{
	// figure out which tiles will be drawn
	// x = 0, y = 0 is at the top
	// x goes down and right,
	// y goes down and left
	// top left = 0,0;
	int tile_size = 128;
	int tile_width = 128;
	int tile_height = 64;
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
	tilesToDraw.clear();
	int x = 0;
	int y = 0;
	// each "tile line"

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
			for(unsigned int height = 0; height <= mapTiles[x][y].getHeight(); ++height)
			{
				int x_draw = camera.left + x * (tile_width / 2) - y * (tile_width / 2);
				int y_draw = camera.top + x * (tile_height / 2) + y * (tile_height / 2) - tile_height/2 * static_cast<int>(height);

				if(x_draw + tile_size < 0 || x_draw > camera.width || y_draw + tile_size < 0 || y_draw > camera.height)
				{
					continue;
				}

				sf::Vertex v1;
				sf::Vertex v2;
				sf::Vertex v3;
				sf::Vertex v4;
				v1.position = sf::Vector2f( static_cast<float>(x_draw)				, static_cast<float>(y_draw));
				v2.position = sf::Vector2f( static_cast<float>(x_draw + tile_width)	, static_cast<float>(y_draw));
				v3.position = sf::Vector2f( static_cast<float>(x_draw + tile_width)	, static_cast<float>(y_draw + tile_width));
				v4.position = sf::Vector2f( static_cast<float>(x_draw)				, static_cast<float>(y_draw + tile_width));
				sf::Rect<unsigned int> textureRect = mapTiles[x][y].getTextureRect();
				v1.texCoords = sf::Vector2f(float(textureRect.left)						, float(textureRect.top));
				v2.texCoords = sf::Vector2f(float(textureRect.left + textureRect.width)	, float(textureRect.top));
				v3.texCoords = sf::Vector2f(float(textureRect.left + textureRect.width)	, float(textureRect.top + textureRect.height));
				v4.texCoords = sf::Vector2f(float(textureRect.left)						, float(textureRect.top + textureRect.height));

				tilesToDraw.append(v1);
				tilesToDraw.append(v2);
				tilesToDraw.append(v3);
				tilesToDraw.append(v4);
	
			}
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