#pragma once
#include <string>
#include "SFML\Graphics.hpp"
#include <vector>

namespace ISO
{

class tileset
{
public:
	tileset(void);
	~tileset(void);

	tileset(std::string fileName);

	bool loadFromFile(std::string fileName);

	sf::Rect<unsigned int> getTextureRect(unsigned int tileType, unsigned int height);

	bool isValid();

	const sf::Texture* getTexture() const;

	static unsigned int pointHeightToType(unsigned int top, unsigned int left, unsigned int bottom, unsigned int right);

private:
	std::string file;
	bool valid;

	sf::Texture texture;
	unsigned int textureSizeX;
	unsigned int textureSizeY;
};

} // end ISO namespace