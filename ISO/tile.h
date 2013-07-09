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

	tile(tileset* set, unsigned int type, unsigned int height, bool base = true, unsigned int baseTill = 0);

	bool setType(unsigned int type);
	bool setType(bool top, bool left, bool bottom, bool right);
	unsigned int getType() const;
	unsigned int getTop() const;
	unsigned int getLeft() const;
	unsigned int getBottom() const;
	unsigned int getRight() const;
	
	bool setHeight(unsigned int height);
	unsigned int getHeight() const;

	bool setTileSet(tileset* set);
	tileset* getTileSet() const;

	sf::Rect<unsigned int> getTextureRect() const;
	sf::Rect<unsigned int> getBaseTextureRect(unsigned int height) const;

	bool getDrawBase();
	void setDrawBase(bool draw);

	unsigned int getBaseTill();
	void setBaseTill(unsigned int);

private:
	tileset* tile_set;
	// top, left, bot, right
	unsigned int z_height;
	unsigned int tile_type;
	bool drawBase;
	unsigned int baseTil;
};

} // end ISO namespace