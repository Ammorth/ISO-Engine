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

unsigned int ISO::tileset::pointHeightToType(unsigned int top, unsigned int left, unsigned int bottom, unsigned int right)
{
	// its ugly, but it works
	if(top)
		if(left)
			if(bottom)
				if(right)
					return 0;	// tblr
				else // not right
					return 12;	// tbl_
			else // not bottom
				if(right)
					return 13;	// tl_r
				else // not right
					return 8;	// tl__
		else // not left
			if(bottom)
				if(right)
					return 14;	// tb_r
				else // not right
					return 2;	// tb__
			else // not bottom
				if(right)
					return 9;	// t__r
				else // not right
					return 5;	// t___
	else// not top
		if(left)
			if(bottom)
				if(right)
					return 15;	// _blr
				else // not right
					return 11;	// _bl_
			else // not bottom
				if(right)
					return 3;	// _l_r
				else // not right
					return 4;	// _l__
		else // not left
			if(bottom)
				if(right)
					return 10;	// _b_r
				else // not right
					return 7;	// _b__
			else // not bottom
				if(right)
					return 6;	// ___r
				else // not right
					return 1;	// ____
}

bool ISO::tileset::isValid()
{
	return valid;
}

const sf::Texture* ISO::tileset::getTexture() const
{
	return &texture;
}