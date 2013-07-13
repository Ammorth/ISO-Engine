#include "tileset.h"
#include "SFML\Graphics.hpp"
#include <bitset>


ISO::tileset::tileset(void)
{
	valid = false;
}


ISO::tileset::~tileset(void)
{
}

ISO::tileset::tileset(std::string fileName)
{
	valid =	loadFromFile(fileName);
}

bool ISO::tileset::loadFromFile(std::string fileName)
{
	valid = texture.loadFromFile(fileName);
	file = "";
	if(valid)
	{
		file = fileName;
		textureSizeX = texture.getSize().x / 4;
		textureSizeY = texture.getSize().y / 8;
	}
	return valid;
}

sf::Rect<unsigned int> ISO::tileset::getTextureRect(unsigned int tileType, unsigned int height)
{
	bool isDouble = height % 2 == 1;
	sf::Rect<unsigned int> out;
	out.top = (textureSizeY) * (tileType / 4);
	out.left = (textureSizeX) * (tileType % 4);
	out.width = textureSizeX;
	out.height = textureSizeY;
	if(!isDouble)
	{
		out.top += textureSizeY * 4;
	}
	return out;	
}

unsigned int ISO::tileset::pointHeightToType(bool top, bool left, bool bottom, bool right)
{
	std::bitset<4> bits;
	bits.set(0, right);
	bits.set(1, bottom);
	bits.set(2, left);
	bits.set(3, top);
	return bits.to_ulong();
}

bool ISO::tileset::isValid()
{
	return valid;
}

const sf::Texture* ISO::tileset::getTexture() const
{
	return &texture;
}

std::string ISO::tileset::getFileName()
{
	return file;
}