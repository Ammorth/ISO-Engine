#pragma once
#include "SFML\Graphics.hpp"
#include <map>
#include <queue>
#include <set>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <list>

namespace ISO
{

class Texture
{
	typedef std::list<std::string> LRU;
	typedef std::map<std::string, std::set<Texture*> > wait_group;
	typedef std::queue<std::string> work_list;
	typedef std::map<std::string, std::pair<unsigned int, sf::Texture*> > texture_map;
	typedef std::map<std::string, std::tuple<unsigned int, sf::Texture*, LRU::iterator> > cache_map;
	
public:

	// Default Constructor
	Texture(void);

	// Default Destructor
	// Will release textures but not keep them around.
	// If you want a texture to remain on the cache, you should use release(true)
	~Texture(void);

	// Load a texture by name, either synchronously or asynchronously
	// If synchronous is set, the calling thread will block until the texture has been loaded (possibly from disk).
	// If synchronous is not set, the calling thread will return immediately.
	// The function will return true if the texture is loaded and ready to use.
	// You can check the status of a texture by calling isLoaded()
	// If the texture was unable to be loaded (doesn't exist, no permission, ect) doesExist() will return false;
	bool get(std::string name, bool synchronous = true);

	// Release a global texture once you are done with it
	// If keepAround is true, the texture will perisit in GPU memory even if no one else is using it.
	// It will be removed if the GPU memory is exceeded.  Therefore using keepAround does not guarentee the texture will remain.
	void release(bool keepAround = false);

	// Returns true if the texture is ready to use
	// Returns false if the texture is not ready.
	bool isLoaded() const;

	// Returns true if the texture exists or if it hasn't been checked yet
	// Returns false if the texture could not be found on disk (bad name)
	// Since texture loading can be asynchronous, this may not return false on bad textures immediately after trying to get a texture.
	bool doesExist() const;

	// Gets a constant pointer to the global texture.
	// You should not modify the texture as it will also change for other instances of the texture.
	// If you want to edit the texture locally, get a copy of the texture as an image with getCopy()
	sf::Texture const* getPtr() const;

	// Get a mutable copy of the global texture as an image so you can make local changes to it.
	// You will need to delete this image yourself.
	// Warning, this requires a texture copy from the GPU to memory and is slow.
	sf::Image* getImageCopy() const;

	// Get a mutable copy of the global texture so you can make local changes to it.
	// You will need to delete this texture yourself.
	// Warning, this requires a texture copy on the GPU and could be slow.
	sf::Texture* getTextureCopy() const;

	// Specify how large you want the cache to be, in pixels.
	// This only includes unloaded textures.
	static void setCacheSize(unsigned int pixels);

	static unsigned int getCacheSize();

	static unsigned int getCurrentCacheSize();

	static void init();
	static void uninit();



private:
	static void loader_entry();

	static void addTextureToCache(sf::Texture* t, std::string name);

	wait_group::iterator waitingIterator;
	sf::Texture* tex;
	texture_map::iterator loadedIterator;
	bool valid;

	static std::thread loadThread;
	static std::mutex waitMutex;
	static std::mutex mapMutex;
	static std::condition_variable loadCondition;
	static bool keepThread;

	static unsigned int curCache;
	static unsigned int maxCache;
	static texture_map loadedTextures;
	static cache_map unloadedTextures;

	static work_list loadList;
	static wait_group waitingGroups;
	static LRU lruList;

	static const std::string Texture_DIR;
	static const std::string Texture_Type;

};

}