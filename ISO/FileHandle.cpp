#include "FileHandle.h"

ISO::FileHandle::FileHandle() : data(NULL), fileName(""), fileDir(""), size(0)
{
}

ISO::FileHandle::FileHandle(std::string name) : data(NULL), fileName(""), fileDir(""), size(0)
{
	loadFile(name);
}

ISO::FileHandle::~FileHandle()
{
	unloadFile();
}

bool ISO::FileHandle::loadFile(std::string path)
{
	replaceAll(path, "../", "");
		
	// load data from director
	for(directories::iterator DI = dirList.begin(); DI != dirList.end(); ++DI)
	{
		// try load file from directory
		std::string dir = DI->first;
		std::ifstream f(dir+path, std::ios::binary | std::ios::beg);
		if(f && f.is_open())
		{
			f.seekg(0, f.end);
			size = static_cast<unsigned int>(f.tellg());
			f.seekg(0, f.beg);
			data = new char[size + 1];
			f.read(data, size);
			data[size] = '\0'; // null terminate;

			f.close();
			fileName = path;
			fileDir = dir;
			return true;
		}
	}
	// couldn't find file
	return false;
}

void ISO::FileHandle::unloadFile()
{
	if(data)
	{
		delete[] data;
	}
	fileName = "";
	fileDir = "";
	size = 0;
	data = NULL;
}

bool ISO::FileHandle::isValid()
{
	return data != NULL && size != 0;
}

std::string ISO::FileHandle::getName()
{
	return fileName;
}

std::string ISO::FileHandle::getDIR()
{
	return fileDir;
}

char const* ISO::FileHandle::getData()
{
	return data;
}

unsigned int ISO::FileHandle::getSize()
{
	return size;
}

void ISO::FileHandle::addDirectory(std::string dir, unsigned int priority, bool writeDir)
{
	if(writeDir)
		writeDirectory = dir;

	directories::iterator I = dirList.begin();
	while(I != dirList.end())
	{
		if(I->second < priority)
		{
			// insert here
			dirList.insert(I, make_pair(dir, priority));
			return;
		}
	}
	dirList.insert(I, make_pair(dir, priority));
}

	

bool ISO::FileHandle::writeFile(std::string fileName, char* data, unsigned int size)
{
	replaceAll(fileName, "../", "");
	if(writeDirectory != "")
	{
		std::ofstream f(writeDirectory+fileName, std::ios::binary | std::ios::trunc);
		if(f && f.is_open())
		{
				
			f.write(data, size);
			f.close();
			return !f.fail();
		}
	}
	return false;
}


void ISO::FileHandle::replaceAll(std::string& str, const std::string& from, const std::string& to)
{
	if(from.empty())
		return;
	size_t start_pos = 0;
	while((start_pos = str.find(from, start_pos)) != std::string::npos) {
		str.replace(start_pos, from.length(), to);
		start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
	}
}
	

ISO::FileHandle::directories ISO::FileHandle::dirList;
std::string ISO::FileHandle::writeDirectory = "";
