#pragma once
#include "tileset.h"
#include "SFML\Graphics.hpp"

namespace ISO
{

class tile
{
public:
	tile(void);
	~tile(void);

	tile(tileset* set, unsigned int type, unsigned int height);

	bool setType(unsigned int type);
	unsigned int getType() const;

	bool setHeight(unsigned int height);
	unsigned int getHeight() const;

	bool setTileSet(tileset* set);
	tileset* getTileSet() const;

	sf::Rect<unsigned int> getTextureRect() const;

private:
	tileset* tile_set;
	// top, left, bot, right
	unsigned int z_height;
	unsigned int tile_type;
};

} // end ISO namespace