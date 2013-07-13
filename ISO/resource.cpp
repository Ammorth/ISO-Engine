#include "resource.h"
#include <assert.h>
#include <fstream>

const std::string ISO::resource::rootDIR = "assets/";
std::map<std::string, ISO::resource::resourceInfo> ISO::resource::resourcePool;

ISO::resource::resource(void)
{
	internalIter = resourcePool.end();
}

ISO::resource::resource(const resource& r)
{
	internalIter = r.internalIter;
	if(internalIter != resourcePool.end())
	{
		internalIter->second.indexCount++;
	}
}

ISO::resource::~resource(void)
{
	unload();
}

ISO::resource& ISO::resource::operator=(const resource& other)
{
	if(this != &other)
	{
		unload();
		internalIter = other.internalIter;
		if(internalIter != resourcePool.end())
		{
			internalIter->second.indexCount++;
		}
	}
	return *this;
	
}

ISO::resource::resource(std::string file)
{
	load(file);
}

bool ISO::resource::load(std::string file)
{
	file = rootDIR + file;
	if(internalIter != resourcePool.end())
	{
		unload();
	}
	internalIter =  resourcePool.find(file);
	if( internalIter == resourcePool.end() )
	{
		// load file to buffer
		std::ifstream f(file, std::ios::binary | std::ios::beg);
		if(f && f.is_open())
		{
			resourceInfo info;
			f.seekg(0, f.end);
			info.size = f.tellg();
			f.seekg(0, f.beg);
			info.dataBuffer = new char[static_cast<unsigned int>(info.size) + 1];
			f.read(info.dataBuffer, info.size);
			info.dataBuffer[info.size] = '\0'; // null terminate;
			info.indexCount = 1; // we are number one

			internalIter = resourcePool.insert(std::pair<std::string, resourceInfo>(file, info)).first;
			return true;
		}
	}else
	{
		internalIter->second.indexCount++;
		return true;
	}
	return false;
}

bool ISO::resource::isLoaded()
{
	return internalIter != resourcePool.end();
}

const char * ISO::resource::getBuffer()
{
	if(internalIter != resourcePool.end())
		return internalIter->second.dataBuffer;
	return NULL;
}

unsigned long long ISO::resource::getBufferSize()
{
	if(internalIter != resourcePool.end())
		return internalIter->second.size;
	return 0;
}

bool ISO::resource::unload()
{
	if(internalIter != resourcePool.end())
	{
		internalIter->second.indexCount--;
		if(internalIter->second.indexCount == 0)
		{
			// time to delete
			delete internalIter->second.dataBuffer;
			resourcePool.erase(internalIter);
		}
		internalIter = resourcePool.end();
		return true;
	}
	return false;
}