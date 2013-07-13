#pragma once
#include <vector>
#include <map>
#include "tile.h"
#include "tileset.h"

namespace ISO
{

class map : public sf::Drawable, sf::Transformable
{
public:
	static const int tile_width = 128;
	static const int tile_height = 64;
	static const int tile_size = 128;

	map(void);
	~map(void);

	map(std::string fileName);

	map(unsigned int x, unsigned int y, unsigned int defaultHeight, tileset* defaultTileSet);

	bool setSize(unsigned int x, unsigned int y);
	sf::Vector2u getSize();

	bool setDefaultTileSet(tileset* set);
	tileset* getDefaultTileSet();

	bool setDefaultHeight(unsigned int height);
	unsigned int getDefaultHeight();

	tile* getMapTile(unsigned int x, unsigned int y, unsigned int zOrder = 0);

	tile* addTileToMap(unsigned int x, unsigned int y, unsigned int height, unsigned int tiletype = 0, tileset* tileSet = NULL, bool base = true, unsigned int baseTill = 0);

	void ISO::map::preDraw(const sf::Vector2f& camera, const sf::Vector2u& windowSize);

	bool loadFromFile(std::string fileName);

	bool saveToFile(std::string fileName);

private:

	unsigned int sizeX,sizeY,defaultZ;
	
	tileset* defaultSet;

	sf::VertexArray tilesToDraw;

	std::vector< std::vector< std::vector< ISO::tile > > > mapTiles;

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

	sf::Vector2i toIsometric(sf::Vector3f point) const;
};

} // end ISO namespace