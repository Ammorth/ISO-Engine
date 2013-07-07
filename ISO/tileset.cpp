#include "tileset.h"
#include "SFML\Graphics.hpp"


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
	if(valid)
	{
		textureSizeX = texture.getSize().x / 4;
		textureSizeY = texture.getSize().y / 4;
	}
	return valid;
}

sf::Rect<unsigned int> ISO::tileset::getTextureRect(unsigned int tileType)
{
	//TLBR
	//0000 = 0
	//0001 = 1
	//0010 = 2
	//0011 = 3
	//0100 = 4
	//0101 = 5
	//0110 = 6
	//0111 = 7
	//1000 = 8
	//1001 = 9
	//1010 = 10
	//1011 = 11
	//1100 = 12
	//1101 = 13
	//1110 = 14
	//1111 = 15
	sf::Rect<unsigned int> out;
	out.top = (textureSizeY) * (tileType / 4);
	out.left = (textureSizeX) * (tileType % 4);
	out.width = textureSizeX;
	out.height = textureSizeY;
	return out;	
}

bool ISO::tileset::isValid()
{
	return valid;
}

const sf::Texture* ISO::tileset::getTexture() const
{
	return &texture;
}