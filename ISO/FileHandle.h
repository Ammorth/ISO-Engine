#pragma once
#include <string>
#include <list>
#include <fstream>

namespace ISO
{

class FileHandle
{
	typedef std::list<std::pair<std::string, unsigned int> > directories;
public:
	FileHandle();

	FileHandle(std::string name);

	~FileHandle();

	bool loadFile(std::string path);

	void unloadFile();

	bool isValid();

	std::string getName();

	std::string getDIR();

	char const* getData();

	unsigned int getSize();

private:
	std::string fileName;
	std::string fileDir;
	char* data;
	unsigned int size;

public:
	static void addDirectory(std::string dir, unsigned int priority, bool writeDir = false);

	

	static bool writeFile(std::string fileName, char* data, unsigned int size);

private:

	static void replaceAll(std::string& str, const std::string& from, const std::string& to);
	
	static directories dirList;
	static std::string writeDirectory;
};

}