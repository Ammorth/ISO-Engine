#include "world.h"
#include <utility>


ISO::world::world(void)
{
}


ISO::world::~world(void)
{
}

bool ISO::world::addMap(map which, std::string name)
{
	return worlds.insert(std::make_pair(name, which)).second;
}

bool ISO::world::removeMap(std::string name)
{
	return worlds.erase(name) == 1;
}