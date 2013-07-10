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
	for(unsigned int i = 0; i < x; ++i)
	{
		mapTiles[i].resize(y);
		for(unsigned int j = 0; j < y; ++j)
		{
			mapTiles[i][j].push_back(tile(defaultTileSet, 0, defaultHeight));
		}
	}
	sizeX = x;
	sizeY = y;
	defaultZ = defaultHeight;
	defaultSet = defaultTileSet;
}

bool ISO::map::setSize(unsigned int newX, unsigned int newY)
{
	// TODO: write resize command
	if(sizeX >= newX)
	{
		// shrinking x
		// since nothing is in the heap (that we need to care about), safe to just use resize;
		mapTiles.resize(newX);
		sizeX = newX;
	}else
	{
		// expanding x
		mapTiles.resize(newX);
		// and then set up the new tiles
		for(unsigned int x = sizeX; x < newX; ++x)
		{
			// ignore newY for this
			mapTiles[x].resize(sizeY);
			for(unsigned int y = 0; y < sizeY; ++y)
			{
				mapTiles[x][y].push_back(tile(defaultSet, 0, defaultZ));
			}
		}
		sizeX = newX;
	}

	if(sizeY >= newY)
	{
		// shrinking y
		// since nothing is in the heap (that we need to care about), safe to just use resize;
		for(unsigned int x = 0; x < sizeX; ++x)
		{
			mapTiles[x].resize(newY);
		}
		sizeY = newY;
	}else
	{
		// expanding y
		for(unsigned int x = 0; x < sizeX; ++x)
		{
			mapTiles[x].resize(newY);
			for(unsigned int y = sizeY; y < newY; ++y)
			{
				mapTiles[x][y].push_back(tile(defaultSet, 0, defaultZ));
			}
		}
		sizeY = newY;
	}
	return true;
}
sf::Vector2u ISO::map::getSize()
{
	return sf::Vector2u(sizeX, sizeY);
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
	defaultZ = height;
	return true;
}
unsigned int ISO::map::getDefaultHeight()
{
	return defaultZ;
}

ISO::tile* ISO::map::getMapTile(unsigned int x, unsigned int y, unsigned int zOrder)
{
	return &mapTiles[x][y][zOrder];
}

ISO::tile* ISO::map::addTileToMap(unsigned int x, unsigned int y, unsigned int height, unsigned int tiletype, tileset* tileSet, bool base, unsigned int baseTill)
{
	tile newTile(tileSet, tiletype, height, base, baseTill);
	if(!tileSet)
	{
		newTile.setTileSet(defaultSet);
	}
	// figure out where it goes
	std::vector<tile>::iterator I;
	for(I = mapTiles[x][y].begin(); I != mapTiles[x][y].end(); ++I)
	{
		if(I->getHeight() > height)
		{
			// insert here
			I = mapTiles[x][y].insert(I, newTile);
			return &(*I);
		}
	}
	// otherwise insert at end
	mapTiles[x][y].push_back(newTile);
	return &mapTiles[x][y].back();
}

void ISO::map::preDraw(const sf::Vector2f& camera, const sf::Vector2u& windowSize)
{
	// figure out which tiles will be drawn

	// build the camera bounds
	sf::Rect<float> cameraBounds;
	cameraBounds.width = static_cast<float>(windowSize.x);
	cameraBounds.height = static_cast<float>(windowSize.y);
	cameraBounds.left = camera.x - (cameraBounds.width / 2);
	cameraBounds.top = camera.y - (cameraBounds.height / 2);

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

	for(unsigned int i = 0; i < sizeX + sizeX; ++i)
	{
		
		y = i;
		x = 0;
		while(y >= int(sizeY))
		{
			y--;
			x++;
		}
		while(y >= 0 && x < int(sizeX))
		{
			int x_draw = x * (tile_width / 2) - y * (tile_width / 2);
			int y_draw = x * (tile_height / 2) + y * (tile_height / 2);
			for(unsigned int z = 0; z < mapTiles[x][y].size(); ++z)
			{
				unsigned int height = mapTiles[x][y][z].getHeight();
				unsigned int drawHeight = mapTiles[x][y][z].getBaseTill();
				int y_temp = y_draw - drawHeight * tile_height/2;
				// draw base tiles first
				if(mapTiles[x][y][z].getDrawBase())
				{
					while(drawHeight < height)
					{
						// check bounds
						if(	x_draw + tile_size < cameraBounds.left ||
							x_draw > cameraBounds.left + cameraBounds.width ||
							y_temp + tile_size < cameraBounds.top ||
							y_temp > cameraBounds.top + cameraBounds.height )
						{
							y_temp -= tile_height/2;
							drawHeight++;
							continue;
						}
						// draw base height;
						sf::Vertex v1;
						sf::Vertex v2;
						sf::Vertex v3;
						sf::Vertex v4;
						v1.position = sf::Vector2f( static_cast<float>(x_draw)				, static_cast<float>(y_temp));
						v2.position = sf::Vector2f( static_cast<float>(x_draw + tile_width)	, static_cast<float>(y_temp));
						v3.position = sf::Vector2f( static_cast<float>(x_draw + tile_width)	, static_cast<float>(y_temp + tile_width));
						v4.position = sf::Vector2f( static_cast<float>(x_draw)				, static_cast<float>(y_temp + tile_width));
						sf::Rect<unsigned int> textureRect = mapTiles[x][y][z].getBaseTextureRect(drawHeight);
						v1.texCoords = sf::Vector2f(float(textureRect.left)						, float(textureRect.top));
						v2.texCoords = sf::Vector2f(float(textureRect.left + textureRect.width)	, float(textureRect.top));
						v3.texCoords = sf::Vector2f(float(textureRect.left + textureRect.width)	, float(textureRect.top + textureRect.height));
						v4.texCoords = sf::Vector2f(float(textureRect.left)						, float(textureRect.top + textureRect.height));

						tilesToDraw.append(v1);
						tilesToDraw.append(v2);
						tilesToDraw.append(v3);
						tilesToDraw.append(v4);

						y_temp -= tile_height/2;
						drawHeight++;
					}
				}else
				{
					while(drawHeight < height)
					{
						y_temp -= tile_height/2;
						drawHeight++;
					}
				}
				// draw final tile;
				
				// check bounds
				if(	x_draw + tile_size < cameraBounds.left ||
					x_draw > cameraBounds.left + cameraBounds.width ||
					y_temp + tile_size < cameraBounds.top ||
					y_temp > cameraBounds.top + cameraBounds.height )
				{
					continue;
				}

				sf::Vertex v1;
				sf::Vertex v2;
				sf::Vertex v3;
				sf::Vertex v4;
				v1.position = sf::Vector2f( static_cast<float>(x_draw)				, static_cast<float>(y_temp));
				v2.position = sf::Vector2f( static_cast<float>(x_draw + tile_width)	, static_cast<float>(y_temp));
				v3.position = sf::Vector2f( static_cast<float>(x_draw + tile_width)	, static_cast<float>(y_temp + tile_width));
				v4.position = sf::Vector2f( static_cast<float>(x_draw)				, static_cast<float>(y_temp + tile_width));
				sf::Rect<unsigned int> textureRect = mapTiles[x][y][z].getTextureRect();
				v1.texCoords = sf::Vector2f(float(textureRect.left)						, float(textureRect.top));
				v2.texCoords = sf::Vector2f(float(textureRect.left + textureRect.width)	, float(textureRect.top));
				v3.texCoords = sf::Vector2f(float(textureRect.left + textureRect.width)	, float(textureRect.top + textureRect.height));
				v4.texCoords = sf::Vector2f(float(textureRect.left)						, float(textureRect.top + textureRect.height));

				tilesToDraw.append(v1);
				tilesToDraw.append(v2);
				tilesToDraw.append(v3);
				tilesToDraw.append(v4);
	
			} // end z
			y--;
			x++;
		} // end while
	} // end for line
}

void ISO::map::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.texture = defaultSet->getTexture();

	target.draw(tilesToDraw, states);
}

sf::Vector2i ISO::map::toIsometric(sf::Vector3f point) const
{
	sf::Vector2i iso;
	iso.x = static_cast<int>( floorf( 0.5f + point.x * (static_cast<float>(tile_width) / 2.f) - point.y * (static_cast<float>(tile_width) / 2.f) ) );
	iso.y = static_cast<int>( floorf( 0.5f + point.x * (static_cast<float>(tile_height) / 2.f) + point.y * (static_cast<float>(tile_height) / 2.f) - point.z * (static_cast<float>(tile_height) / 2.f ) ) );
	return iso;
}