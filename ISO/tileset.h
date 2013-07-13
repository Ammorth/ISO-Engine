#pragma once
#include <string>
#include "SFML\Graphics.hpp"
#include <vector>
#include "resource.h"

namespace ISO
{

class tileset : public resource
{
public:
	tileset(void);
	~tileset(void);

	tileset(std::string fileName);

	bool load(std::string fileName);

	sf::Rect<unsigned int> getTextureRect(unsigned int tileType, unsigned int height);

	bool isValid();

	const sf::Texture* getTexture() const;

	static unsigned int pointHeightToType(bool top, bool left, bool bottom, bool right);

	std::string getFileName();

private:
	const static std::string tileDIR;
	const static std::string tileFileType;

	std::string fileN;
	bool valid;

	sf::Texture texture;
	unsigned int textureSizeX;
	unsigned int textureSizeY;

};

} // end ISO namespace