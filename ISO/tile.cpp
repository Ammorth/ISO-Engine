#include "tile.h"

#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))

ISO::tile::tile(void)
	: tile_set(NULL), tile_type(0), z_height(0), drawBase(true), baseTil(0)
{
}


ISO::tile::~tile(void)
{
}

ISO::tile::tile(tileset* set, unsigned int type, unsigned int height, bool drawBase, unsigned int baseTill)
	: tile_set(NULL), tile_type(0), z_height(height), drawBase(drawBase), baseTil(baseTill)
{
	this->setTileSet(set);
	this->setType(type);
}


bool ISO::tile::setType(unsigned int type)
{
	if(type < 16)
	{
		tile_type = type;
		return true;
	}else
	{
		return false;
	}
}
bool ISO::tile::setType(bool top, bool left, bool bottom, bool right)
{
	tile_type = ISO::tileset::pointHeightToType(top, left, bottom, right);
	return true;
}
unsigned int ISO::tile::getType() const
{
	return tile_type;
}
unsigned int ISO::tile::getTop() const
{
	return CHECK_BIT(tile_type, 3);
}
unsigned int ISO::tile::getLeft() const
{
	return CHECK_BIT(tile_type, 2);
}
unsigned int ISO::tile::getBottom() const
{
	return CHECK_BIT(tile_type, 1);
}
unsigned int ISO::tile::getRight() const
{
	return CHECK_BIT(tile_type, 0);
}

bool ISO::tile::setHeight(unsigned int height)
{
	z_height = height;
	return true;
}
unsigned int ISO::tile::getHeight() const
{
	return z_height;
}

bool ISO::tile::setTileSet(tileset* set)
{
	if(set && set->isValid())
	{
		tile_set = set;
		return true;
	}else
	{
		return false;
	}
}
ISO::tileset* ISO::tile::getTileSet() const
{
	return tile_set;
}


sf::Rect<unsigned int> ISO::tile::getTextureRect() const
{
	if(tile_set)
	{
		return tile_set->getTextureRect(tile_type, z_height);
	}else
	{
		return sf::Rect<unsigned int>();
	}
}

sf::Rect<unsigned int> ISO::tile::getBaseTextureRect(unsigned int height) const
{
	if(tile_set)
	{
		return tile_set->getTextureRect(15, height);
	}else
	{
		return sf::Rect<unsigned int>();
	}
}

bool ISO::tile::getDrawBase()
{
	return drawBase;
}
void ISO::tile::setDrawBase(bool draw)
{
	drawBase = draw;
}

unsigned int ISO::tile::getBaseTill()
{
	return baseTil;
}
void ISO::tile::setBaseTill(unsigned int base)
{
	if(z_height >= base)
	{
		baseTil = base;
	}else
	{
		baseTil = z_height;
	}
}