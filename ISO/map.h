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
	map(void);
	~map(void);

	map(unsigned int x, unsigned int y, unsigned int defaultHeight, tileset* defaultTileSet);

	bool setSize(unsigned int x, unsigned int y);
	sf::Vector2u getSize();

	bool setDefaultTileSet(tileset* set);
	tileset* getDefaultTileSet();

	bool setDefaultHeight(unsigned int height);
	unsigned int getDefaultHeight();

	tile* getMapTile(unsigned int x, unsigned int y);

	void ISO::map::preDraw(sf::Vector2f camera);

private:
	unsigned int size_x,size_y,default_z;
	
	tileset* defaultSet;

	sf::VertexArray tilesToDraw;

	std::vector< std::vector< ISO::tile > > mapTiles;

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

	sf::Vector2f toIsometric(sf::Vector2u point) const;
};

} // end ISO namespace