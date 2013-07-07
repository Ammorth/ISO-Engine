#include <SFML/Graphics.hpp>
#include <chrono>
#include <thread>
#include <sstream>
#include <iomanip>
#include <string>
#include "map.h"
#include "tileset.h"
#include <stdlib.h>
#include <Windows.h>
#include "jobPool.h"

class job_preDrawMap : public ISO::jobPool::job
{
public:
	ISO::map* map;
	sf::Vector2f* cam;

	job_preDrawMap(ISO::map* whichMap = NULL, sf::Vector2f* camera = NULL) : map(whichMap), cam(camera) {}

private:
	void operator()()
	{
		map->preDraw(*cam);
	}
};

class job_updateCamera : public ISO::jobPool::job
{
public:
	std::vector<bool>* keys;
	sf::Vector2f* cam;

	job_updateCamera(sf::Vector2f* camera = NULL, std::vector<bool>* keyStates = NULL) : cam(camera), keys(keyStates) {}
private:
	void operator()()
	{
		if((*keys)[sf::Keyboard::Left])
		{
			cam->x += 5;
		}
		if((*keys)[sf::Keyboard::Right])
		{
			cam->x -= 5;
		}
		if((*keys)[sf::Keyboard::Up])
		{
			cam->y += 5;
		}
		if((*keys)[sf::Keyboard::Down])
		{
			cam->y -= 5;
		}
	}
};

const sf::Uint64 MICROSECONDS_PER_SECOND = 1000000;

int main()
{
	std::vector<bool> keyState(256, false);
    sf::RenderWindow window(sf::VideoMode(800, 600), "XCB Error Test");

	sf::Clock gameClock;

	sf::Font consola;
	consola.loadFromFile("assets/fonts/consola.ttf");
	sf::Text infoText("", consola, 15);
	infoText.setColor(sf::Color::White);

	ISO::tileset grass("assets/textures/tiles/grass.png");
	ISO::map mymap(10,10,2,&grass);

	sf::Uint64 targetFPS = 60;
	sf::Uint64 targetMicrosecond = MICROSECONDS_PER_SECOND / static_cast<unsigned long int>(targetFPS);

	sf::Uint64 gameTime = 0;
	sf::Uint64 currentTime = gameClock.getElapsedTime().asMicroseconds();
	sf::Uint64 accumulator = 0;

	sf::Vector2f camera;

	ISO::jobPool workPool;

	job_updateCamera camJob(&camera, &keyState);

	job_preDrawMap mapJob(&mymap, &camera);
	mapJob.addDependancy(&camJob);

	const sf::Uint64 maxFrameTime = MICROSECONDS_PER_SECOND;
	        
	// main game loop
    while (window.isOpen())
    {
		// handle events before next loop
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }else if(event.type == sf::Event::KeyPressed)
			{
				if(event.key.code < 256 && event.key.code >= 0)
				{
					keyState[event.key.code] = true;
				}
			}else if(event.type = sf::Event::KeyReleased)
			{
				if(event.key.code < 256 && event.key.code >= 0)
				{
					keyState[event.key.code] = false;
				}
			}
        }

		// start next loop

		sf::Uint64 newTime = gameClock.getElapsedTime().asMicroseconds();
		sf::Uint64 frameTime = newTime - currentTime;
		sf::Uint64 actualFrameTime = frameTime;

		if(frameTime < targetMicrosecond)
		{
			unsigned long long delay = targetMicrosecond - frameTime;
			const unsigned long long error = 2000;
			if(delay > error * 2)
			{
				std::this_thread::sleep_for(std::chrono::microseconds(delay-error));
			}
			do
			{
				newTime = gameClock.getElapsedTime().asMicroseconds();
				frameTime = newTime - currentTime;
			}while(frameTime < targetMicrosecond);
		}

		if(frameTime > maxFrameTime)
		{
			frameTime = maxFrameTime;
		}
		currentTime = newTime;

		accumulator += frameTime;

		while( accumulator >= targetMicrosecond)
		{
			// update game	

			camJob.init();
			mapJob.init();

			workPool.addJobToPool(&camJob);
			workPool.addJobToPool(&mapJob);
			
			workPool.waitForJobs();

			// end work
			gameTime += targetMicrosecond;
			accumulator -= targetMicrosecond;
		}

		const double interpolate = static_cast<double>(accumulator) / static_cast<double>(targetMicrosecond);

		// render with interpolation
		/*
		if(renderClock.getElapsedTime().asMicroseconds() < targetMicroseconds)
		{
			unsigned long long delay = targetMicroseconds - renderClock.getElapsedTime().asMicroseconds();
			const unsigned long long error = 2000;
			if(delay > error * 2)
			{
				std::this_thread::sleep_for(std::chrono::microseconds(delay-error));
			}
			while(renderClock.getElapsedTime().asMicroseconds() < targetMicroseconds);
		}
		double FPS = MICROSECONDS_PER_SECOND / static_cast<double>(renderClock.restart().asMicroseconds());
		std::stringstream infoBuffer;
		
		infoBuffer << "FPS: " << std::fixed << std::setprecision(2) << std::setw(7) << FPS << "  Load: " << std::setw(6) << workAmount;
		*/
		std::stringstream infoBuffer;

		double FPS = static_cast<double>(MICROSECONDS_PER_SECOND) / static_cast<double>(frameTime);
		double actualLoad = static_cast<double>(actualFrameTime) / static_cast<double>(targetMicrosecond) *100;
		
		infoBuffer << "FPS: " << std::setw(7) << FPS << "  Load: " << std::setw(6) << actualLoad;

		infoText.setString(infoBuffer.str());

        window.clear();
		window.draw(mymap);
		window.draw(infoText);
        window.display(); 
    }
    return 0;
}