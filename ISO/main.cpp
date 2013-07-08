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
	sf::Vector2f* cameraPos;
	sf::Vector2u* winSize;

	job_preDrawMap(	ISO::map* whichMap,
					sf::Vector2f* cameraPosition,
					sf::Vector2u* windowSize ) 
					: map(whichMap), cameraPos(cameraPosition), winSize(windowSize) {}

private:
	void operator()()
	{
		map->preDraw(*cameraPos, *winSize);
	}
};

class job_updateCamera : public ISO::jobPool::job
{
public:
	std::vector<bool>* keys;
	sf::Vector2f* cameraPos;

	job_updateCamera( std::vector<bool>* keysPressed,
					sf::Vector2f* cameraPosition ) 
					: keys(keysPressed), cameraPos(cameraPosition) {}

private:
	void operator()()
	{
		if((*keys)[sf::Keyboard::Left])
			cameraPos->x -= 5;
		if((*keys)[sf::Keyboard::Right])
			cameraPos->x += 5;
		if((*keys)[sf::Keyboard::Up])
			cameraPos->y -= 5;
		if((*keys)[sf::Keyboard::Down])
			cameraPos->y += 5;
	}
};

class job_updateViews : public ISO::jobPool::job
{
public:
	sf::Vector2f* cameraPos;
	sf::Vector2u* winSize;
	sf::View* worldView;
	sf::View* uiView;

	job_updateViews(	sf::Vector2f* cameraPosition,
						sf::Vector2u* windowSize,
						sf::View* worldView,
						sf::View* uiView)
						: cameraPos(cameraPosition), winSize(windowSize), worldView(worldView), uiView(uiView) {}

private:
	void operator()()
	{
		worldView->setSize(static_cast<float>(winSize->x), static_cast<float>(winSize->y));
		uiView->setSize(static_cast<float>(winSize->x), static_cast<float>(winSize->y));

		worldView->setCenter(*cameraPos);
		uiView->setCenter(static_cast<float>(winSize->x) / 2.f, static_cast<float>(winSize->y) / 2.f);
	}
};

const sf::Uint64 MICROSECONDS_PER_SECOND = 1000000;

int main()
{
	std::vector<bool> keyState(256, false);
    sf::RenderWindow window(sf::VideoMode(800, 600), "ISO Engine");

	sf::Vector2u windowSize(800,600);
	sf::Vector2f cameraPos(0,0);
	sf::View worldView = window.getView();
	sf::View uiView = window.getView();

	sf::Clock gameClock;

	sf::Font consola;
	consola.loadFromFile("assets/fonts/consola.ttf");
	sf::Text infoText("", consola, 15);
	infoText.setColor(sf::Color::White);

	ISO::tileset grass("assets/textures/tiles/grass_new.png");
	ISO::map mymap(5,5,3,&grass);

	mymap.getMapTile(2,1)->setHeight(5);
	mymap.getMapTile(2,1)->setBaseTill(4);
	mymap.getMapTile(2,2)->setHeight(4);
	mymap.getMapTile(2,2)->setType(ISO::tileset::pointHeightToType(1,0,0,1));
	mymap.getMapTile(2,3)->setHeight(3);
	mymap.getMapTile(2,3)->setType(ISO::tileset::pointHeightToType(1,0,0,1));
	mymap.getMapTile(3,3)->setHeight(3);
	mymap.getMapTile(3,3)->setType(ISO::tileset::pointHeightToType(1,0,0,0));
	mymap.getMapTile(4,4)->setBaseTill(1);

	sf::Uint64 targetFPS = 60;
	sf::Uint64 targetMicrosecond = MICROSECONDS_PER_SECOND / static_cast<unsigned long int>(targetFPS);

	sf::Uint64 gameTime = 0;
	sf::Uint64 currentTime = gameClock.getElapsedTime().asMicroseconds();
	sf::Uint64 accumulator = 0;

	ISO::jobPool workPool;

	job_updateCamera camJob(&keyState, &cameraPos);

	job_preDrawMap mapJob(&mymap, &cameraPos, &windowSize);
	mapJob.addDependancy(&camJob);

	job_updateViews viewJob(&cameraPos, &windowSize, &worldView, &uiView);
	viewJob.addDependancy(&camJob);

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
			}else if(event.type == sf::Event::Resized)
			{
				windowSize.x = event.size.width;
				windowSize.y = event.size.height;
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

			workPool.addJobToPool(&camJob);
			workPool.addJobToPool(&mapJob);
			workPool.addJobToPool(&viewJob);
			
			workPool.waitForJobs();

			// end work
			gameTime += targetMicrosecond;
			accumulator -= targetMicrosecond;
		}

		const double interpolate = 1 - static_cast<double>(accumulator) / static_cast<double>(targetMicrosecond);

		// render with interpolation
		
		std::stringstream infoBuffer;

		double FPS = static_cast<double>(MICROSECONDS_PER_SECOND) / static_cast<double>(frameTime);
		double actualLoad = static_cast<double>(actualFrameTime) / static_cast<double>(targetMicrosecond) *100;
		
		infoBuffer << "FPS: " << std::fixed << std::setprecision(2) << std::setw(6) << FPS << "  Load: " << std::setw(6) << actualLoad;
		infoBuffer << "\nCAM: " << std::setw(9) << cameraPos.x << ", " << std::setw(9) << cameraPos.y;
		infoBuffer << "\nACC: " << std::setw(5) << accumulator << " " << interpolate << "  FRM: " << std::setw(5) << frameTime;

		infoText.setString(infoBuffer.str());

        window.clear();
		// draw world
		window.setView(worldView);
		window.draw(mymap);

		// draw UI
		window.setView(uiView);

		window.draw(infoText);
        window.display(); 
    }
    return 0;
}