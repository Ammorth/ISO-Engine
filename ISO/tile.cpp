#include "tile.h"


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
bool ISO::tile::setType(unsigned int top, unsigned int left, unsigned int bottom, unsigned int right)
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
	return tile_type == 0 || tile_type == 1 || tile_type == 2 || tile_type == 5 || tile_type == 8 || tile_type == 9 || tile_type == 12 || tile_type == 13 || tile_type == 14;
}
unsigned int ISO::tile::getLeft() const
{
	return tile_type == 0 || tile_type == 1 || tile_type == 3 || tile_type == 4 || tile_type == 8 || tile_type == 11 || tile_type == 12 || tile_type == 13 || tile_type == 15;
}
unsigned int ISO::tile::getBottom() const
{
	return tile_type == 0 || tile_type == 1 || tile_type == 2 || tile_type == 7 || tile_type == 10 || tile_type == 11 || tile_type == 12 || tile_type == 14 || tile_type == 15;
}
unsigned int ISO::tile::getRight() const
{
	return tile_type == 0 || tile_type == 1 || tile_type == 3 || tile_type == 6 || tile_type == 9 || tile_type == 10 || tile_type == 13 || tile_type == 14 || tile_type == 15;
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