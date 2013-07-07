#pragma once
#include "map.h"
#include <string>
#include <map>

namespace ISO
{

class world
{
public:
	world(void);
	~world(void);

	bool addMap(map which, std::string name);
	bool removeMap(std::string name);

private:
	std::map<std::string, map> worlds;
};

} // end ISO namespace

