#include "Texture.h"
#include "FileHandle.h"
#include <utility>

#include <assert.h>

std::thread ISO::Texture::loadThread;
std::mutex ISO::Texture::mapMutex;
std::mutex ISO::Texture::waitMutex;
std::condition_variable ISO::Texture::loadCondition;

const std::string ISO::Texture::Texture_DIR = "textures/";
const std::string ISO::Texture::Texture_Type = ".png";

unsigned int ISO::Texture::curCache = 0;
unsigned int ISO::Texture::maxCache = 10000; // approx 40 MB @ 4 bytes per pixel
ISO::Texture::texture_map ISO::Texture::loadedTextures;
ISO::Texture::cache_map ISO::Texture::unloadedTextures;

ISO::Texture::work_list ISO::Texture::loadList;
ISO::Texture::wait_group ISO::Texture::waitingGroups;

ISO::Texture::LRU ISO::Texture::lruList;

bool ISO::Texture::keepThread = true;

void ISO::Texture::init()
{
	static bool inited = false;
	if(!inited)
	{
		inited = true;
		loadThread = std::thread(&ISO::Texture::loader_entry);
	}
}

void ISO::Texture::uninit()
{
	if(loadThread.joinable())
	{
		keepThread = false;
		loadCondition.notify_all();
		loadThread.join();
	}
}

ISO::Texture::Texture(void) : tex(NULL), valid(false), loadedIterator(loadedTextures.end()), waitingIterator(waitingGroups.end())
{
}

ISO::Texture::~Texture(void)
{
}

bool ISO::Texture::get(std::string name, bool synchronous)
{
	release();
	mapMutex.lock();
	texture_map::iterator loc = loadedTextures.find(name);
	if(loc != loadedTextures.end())
	{
		// in loaded textures
		// increment ref count
		loc->second.first++;
		// get the texture pointer
		tex = loc->second.second;
		// get access to the position
		loadedIterator = loc;

		mapMutex.unlock();
		valid = true;
		return true;
	}
	cache_map::iterator loc2 = unloadedTextures.find(name);
	if(loc2 != unloadedTextures.end())
	{
		// in unloaded textures, move it to loaded
		// get the texture
		tex = std::get<1>(loc2->second);
		// decrease the cache usage
		curCache -= std::get<0>(loc2->second);
		// remove from the cache LRU list
		lruList.erase(std::get<2>(loc2->second));
		// remove from the cache
		unloadedTextures.erase(loc2);
		// add to the used textures, store with ref count 1 (we have it)
		loadedIterator = loadedTextures.insert(std::make_pair(name, std::make_pair(1, tex))).first;

		mapMutex.unlock();
		valid = true;
		return true;
	}
	mapMutex.unlock();
	if(synchronous)
	{
		// load the data ourselves
		FileHandle FH(Texture_DIR + name + Texture_Type);
		if(!FH.isValid())
		{
			return false;
		}
		sf::Texture* textur = new sf::Texture();
		textur->loadFromMemory(FH.getData(), FH.getSize());
		if(!textur)
		{
			return false;
		}
		// add to loaded textures
		mapMutex.lock();
		
		loadedIterator = loadedTextures.insert(std::make_pair(name, std::make_pair(1, textur))).first;
		mapMutex.unlock();
		valid = true;
		tex = textur;
		return true;
	}else
	{
		// schedule the worker and return
		waitMutex.lock();
		wait_group::iterator WIT = waitingGroups.find(name);
		if(WIT != waitingGroups.end())
		{
			// the texture is already on the work list
			// add us to the waiting group
			WIT->second.insert(this);
			
		}else
		{
			// no one is waiting yet, add the texture to the work list and us to a new waiting list
			loadList.push(name);
			// this will create the entry if it doesn't exist (which it doesn't)
			waitingGroups[name].insert(this);
			// and signal the loaded
			loadCondition.notify_one();
		}
		waitMutex.unlock();
	}
	return false;
}

void ISO::Texture::release(bool keepAround)
{
	// are we waiting for a texture?
	waitMutex.lock();
	if(waitingIterator != waitingGroups.end())
	{
		// remove us from the waiting group
		waitingIterator->second.erase(this);
		waitingIterator = waitingGroups.end();

		waitMutex.unlock();
		return;
	}
	waitMutex.unlock();

	// or do we have a texture?
	mapMutex.lock();
	if(loadedIterator != loadedTextures.end())
	{
		loadedIterator->second.first--; // deincrement counter
		if(loadedIterator->second.first == 0)
		{
			// no one else is using.
			if(keepAround)
			{
				addTextureToCache(loadedIterator->second.second, loadedIterator->first);
			}else
			{
				// don't want to cache it
				delete loadedIterator->second.second;
			}

			loadedTextures.erase(loadedIterator);
			loadedIterator = loadedTextures.end();
			tex = NULL;
			valid = false;
		}
		mapMutex.unlock();
		return;
	}
	mapMutex.unlock();

	// not holding a texture or waiting for one
}

sf::Texture const* ISO::Texture::getPtr() const
{
	return tex;
}

sf::Image* ISO::Texture::getImageCopy() const
{
	if(tex)
		return new sf::Image(tex->copyToImage());
	return NULL;
}

sf::Texture* ISO::Texture::getTextureCopy() const
{
	if(tex)
		return new sf::Texture(*tex);
	return NULL;
}

void ISO::Texture::setCacheSize(unsigned int bytes)
{
	while(curCache < bytes)
	{
		// if we are shrinking, remove stuff
		std::string removed = lruList.front();
		lruList.pop_front();
		cache_map::iterator IT = unloadedTextures.find(removed);
		assert(IT != unloadedTextures.end());

		curCache -= std::get<0>(IT->second); // decrement the cache size
		delete std::get<1>(IT->second); // delete the old texture;
		unloadedTextures.erase(IT); // remove from cache list
	}
	maxCache = bytes;
}

unsigned int ISO::Texture::getCacheSize()
{
	return maxCache;
}

unsigned int ISO::Texture::getCurrentCacheSize()
{
	return curCache;
}

void ISO::Texture::loader_entry()
{
	std::unique_lock<std::mutex> waitLock(waitMutex);
	while(keepThread)
	{
		if(loadList.empty())
		{
			loadCondition.wait(waitLock);
		}else
		{
			while(!loadList.empty())
			{
				// do work
				// get the firs job off the list
				std::string toGet = loadList.front();
				loadList.pop();
				// so people can give us work
				waitLock.unlock();

				FileHandle FH(Texture_DIR + toGet + Texture_Type);
				bool valid = FH.isValid();
				sf::Texture* tex = NULL;
				if(valid)
				{
					tex = new sf::Texture();
					tex->loadFromMemory(FH.getData(), FH.getSize());
				}

				// add the texture to the cache and update people waiting for us
				std::unique_lock<std::mutex> mapLock(mapMutex, std::defer_lock);
				// get both mutexes safely
				std::lock(mapLock, waitLock);
				
				// figure out who wants the texture
				wait_group::iterator IT = waitingGroups.find(toGet);
				if(IT != waitingGroups.end() && !IT->second.empty())
				{
					// at least one person is waiting
					// add the texture to loaded textures if valid
					texture_map::iterator mapIT;
					if(valid)
						mapIT = loadedTextures.insert(std::make_pair(toGet, std::make_pair(0, tex))).first;

					std::set<Texture*>::iterator waiter;
					for(waiter = IT->second.begin(); waiter != IT->second.end(); ++IT)
					{
						// give them the texture if valid
						(*waiter)->valid = valid;
						(*waiter)->tex = tex;
						// only increase ref count if its valid
						if(valid)
						{
							mapIT->second.first++;
						}
					}
					// get rid of waiting group
					waitingGroups.erase(IT);
				}else
				{
					// no one is waiting, add to cache if possible
					addTextureToCache(tex, toGet);
				}

				// unlock the map
				mapLock.unlock();
				// keep the the waitlock going around the loop
			}
			waitLock.unlock();
		}
	}
}

// assume we have the mapMutex before calling this
void ISO::Texture::addTextureToCache(sf::Texture* t, std::string name)
{
	// want to cache it, get size of the texture
	sf::Vector2u texSize = t->getSize();
	unsigned int pixelCount = texSize.x * texSize.y;
	// try add it to the cache
	if(pixelCount < maxCache)
	{
		// texture is small enough to add to cache
		curCache += pixelCount;
		while(curCache > maxCache)
		{
			// remove old textures until we have room
			// using a least recently used list
			std::string removed = lruList.front();
			lruList.pop_front();
			cache_map::iterator IT = unloadedTextures.find(removed);
			assert(IT != unloadedTextures.end());

			curCache -= std::get<0>(IT->second); // decrement the cache size
			delete std::get<1>(IT->second); // delete the old texture;
			unloadedTextures.erase(IT); // remove from cache list
		}

		// add the texture to the cache
		lruList.push_back(name);
		unloadedTextures.insert(std::make_pair(name, std::make_tuple(pixelCount, t, --lruList.end()) ));
	}else
	{
		// texture is too large to cache
		delete t;
	}
}

bool ISO::Texture::isLoaded() const
{
	return tex != NULL;
}

bool ISO::Texture::doesExist() const
{
	return valid;
}