#include "tile.h"


ISO::tile::tile(void)
{
	z_height = 0;
	tile_type = 0;
	tile_set = NULL;
}


ISO::tile::~tile(void)
{
}

ISO::tile::tile(tileset* set, unsigned int type, unsigned int height)
{
	tile_set = NULL;
	this->setTileSet(set);
	type = 0;
	this->setType(type);
	z_height = height;
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
unsigned int ISO::tile::getType() const
{
	return tile_type;
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
		return tile_set->getTextureRect(tile_type);
	}else
	{
		return sf::Rect<unsigned int>();
	}
}