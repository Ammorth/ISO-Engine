#pragma once
#include <string>
#include <map>

namespace ISO
{

/*
Needs:
	Load a file to binary buffer (I guess)
	support inheriting for specific classes (to use the buffer directly)
	If the file has been loaded once, don't load it again, point to it and increase the index
	Once a file is unloaded, unless specifically stated, hold onto it until some period of time
	If a file hasn't been used for awhile, release it from memory.

	this is strictly input only.

*/

class resource
{
public:
	resource(void);
	resource(const resource& r);
	resource(std::string file);

	bool load(std::string file);
	bool unload();

	bool isLoaded();

	const char * getBuffer();
	unsigned long long getBufferSize();

	resource& operator=(const resource& other);

	~resource(void);
private:
	const static std::string rootDIR;

	class resourceInfo
	{
	public:
		char* dataBuffer;
		unsigned long long size;
		unsigned int indexCount;
	};

	std::map<std::string, resourceInfo>:: iterator internalIter;

	static std::map<std::string, resourceInfo> resourcePool;
};

} // end ISO namespace