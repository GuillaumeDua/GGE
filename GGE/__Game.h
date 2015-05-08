#ifndef __GEE_GAME__
# define __GEE_GAME__

# include <SFML/Window\Window.hpp>
# include <SFML/Graphics/RenderWindow.hpp>
# include <iostream>
# include <atomic>
# include <functional>
# include <map>

# include "GCL/Exception.h"
# include "RENDERING/Types.h"
# include "EventHandler.h"
# include "EntityManager.h"
# include "Screen.h"

// Window::SetFramerateLimit => vertical sync ?
// screenshots => sf::Image Scren = App.Capture()
// [Todo] : std::atomic<bool> to lock while switching eventhandler + thread-safe

namespace GGE
{
	struct EmptyEventHolder {}; // Error -> Invalid event-holder
	// template <typename EventHolder = EmptyEventHolder> class Game
	class Game final	//	[Todo] : Replace final by a game-to-game inheritance (e.g final logic into a pure virtual [?])
	{
	public:
		Game()
			: _window(sf::VideoMode(800, 600, 32), "GEE Rendering")
		{
			this->Initialize();
		}
		Game(const size_t TicksPerSec)
			: _TicksPerSec(TicksPerSec)
			, _window(sf::VideoMode(800, 600, 32), "GEE Rendering")
		{
			this->Initialize();
		}
		Game(const Game &)	= delete;
		Game(const Game &&) = delete;
		~Game(){}

		void											Initialize(void)
		{
			// _window.setFramerateLimit
			// _window.getView
			if (!(_window.isOpen()))
				throw GCL::Exception("[Error] : Rendering window is not open");
		}

		// Run [-> I cld use my own Runnable class]
		bool											Start(void)
		{
			if (this->_IsRunning)
				return false;
			this->_IsRunning = true;

			std::cout << "[+] Start ... framerate set at : " << FPS << std::endl;
			bool ret(true);

			try
			{
				 ret = this->Loop();
			}
			catch (const GCL::Exception & ex)
			{
				std::cerr << "GCL exception catch : " << ex.what() << std::endl;
				return false;
			}
			catch (const std::exception & ex)
			{
				std::cerr << "STD exception catch : " << ex.what() << std::endl;
				return false;
			}
			catch (...)
			{
				std::cerr << "Unknown exception catch" << std::endl;
				return false;
			}
			this->_window.close();
			return ret;
		}
		bool											Stop(void)
		{
			this->_IsRunning = false;
		}

		// Rendering
		inline void										SetBackground(const Sprite & sprite)
		{
			this->_backgroundSprite = sprite;
		}
		inline void										SetBackground(const std::string & texture_path)
		{
			if (!(_bufBatckgroundTexture.loadFromFile(texture_path)))
				throw GCL::Exception("[Error] : GGE::Game::SetBackground : Cannot load texture from file : " + texture_path);
			this->_backgroundSprite.setTexture(_bufBatckgroundTexture);
		}
		inline void										SetBackground(const Texture & texture)
		{
			this->_backgroundSprite.setTexture(texture);
		}
		// Screen
		Game &											operator+=(Screen && screen)
		{
			throw GCL::Exception("[Error] : Not implemented");
		}
		// Events handling
		inline const EventHandler::MapType &			GetEventHandler_map(void)
		{
			return *(this->_EventTypeToCB);
		}
		inline EventHandler::RegistrableEventsMapType &	GetEventRegisteringSytem(void)
		{
			return _registeredEvents;
		}
		template <class T_EventHandler>
		inline void										SetEventHandler(void)
		{
			std::cout << "Switching event handler from [" << &(this->_EventTypeToCB) << "] to : [" << &(T_EventHandler::GetTypeToCB_Map()) << ']' << std::endl;
			this->_EventTypeToCB = &(T_EventHandler::GetTypeToCB_Map());
		}
		bool											HandleEvent(const sf::Event & event)
		{
			using GameEventRangeIt = std::pair < EventHandler::MapType::const_iterator, EventHandler::MapType::const_iterator >;
			using RegisteredEventRangeIt = std::pair < EventHandler::RegistrableEventsMapType::iterator, EventHandler::RegistrableEventsMapType::iterator >;
			using EventItQueue = std::queue < EventHandler::RegistrableEventsMapType::iterator >;

			// [Game events handling]
			GameEventRangeIt gameEventRangeIt = this->_EventTypeToCB->equal_range(event.type);
			for (EventHandler::MapType::const_iterator gameEventIt = gameEventRangeIt.first; gameEventIt != gameEventRangeIt.second; ++gameEventIt)
				gameEventIt->second(event, *this);

			// [Registered events handling]
			EventItQueue			toUnregisterEventsQueue;
			RegisteredEventRangeIt	registeredEventRangeIt = this->_registeredEvents.equal_range(event.type);
			for (EventHandler::RegistrableEventsMapType::iterator registeredEventsIt = registeredEventRangeIt.first; registeredEventsIt != registeredEventRangeIt.second; ++registeredEventsIt)
			{
				switch (registeredEventsIt->second(event))
				{
				case EventHandler::RegisteredCBReturn::FAILURE:
					throw std::runtime_error("Fatal error in registered event's callback execution");
					break;
				case EventHandler::RegisteredCBReturn::REQUIRE_UNREGISTERED:
					toUnregisterEventsQueue.push(registeredEventsIt);
					break;
				case EventHandler::RegisteredCBReturn::OK :
				default:
					break;
				}
			}

			// Unregistering
			while (toUnregisterEventsQueue.size() != 0)
			{
				this->_registeredEvents.erase(toUnregisterEventsQueue.back());
				toUnregisterEventsQueue.pop();
			}
			return true;
		}
		void											ManageEvents(void)
		{
			sf::Event event;
			while (this->_window.pollEvent(event))
				if (this->HandleEvent(event) == false) break;
		}
		// Entities 
		inline EntityManager &							GetRefEntityManager(void)
		{
			return this->_entityManager;
		}
		// [Todo] : protected
		void											ManageEntities(void)	// [Todo] : protected
		{
			if (this->_entityManager.TicksUp() && !(this->_entityManager.Behave()))
				throw GCL::Exception("[Error] : Game::ManageEntities -> IEntity::Behave call failed");
			this->_entityManager.Draw(this->_window);
		}
		// Loop
		bool											Update(void)
		{
			try
			{
				this->_window.clear();
				this->_window.draw(this->_backgroundSprite);
				this->ManageEvents();
				this->ManageEntities();
				this->_window.display();
			}
			catch (...)
			{
				return false;
			}

			return true;
		}
		bool											Loop(void)
		{
			sf::Clock	clock;
			sf::Time	diffTimeCt = sf::Time::Zero;

			while (this->_IsRunning)
			{
				sf::Time elaspedTime = clock.restart();
				diffTimeCt += elaspedTime;

				while (this->_IsRunning && diffTimeCt > TimePerFrame)
				{
					diffTimeCt -= TimePerFrame;
					if (this->Update() == false) return false;
				}
			}
			return true;
		}
		// Ticks :
		const float										FPS = 60.f;
		const sf::Time 									TimePerFrame = sf::seconds(1.f / FPS);

		static	const size_t							DEFAULT_TICKS_PER_SEC	= 50;
		static	const size_t							DEFAULT_FRAME_SKIP		= 10;
				const size_t							_TicksPerSec			= DEFAULT_TICKS_PER_SEC;
				const size_t							_TicksToSkip			= 1000 / _TicksPerSec;
				const size_t							_MaxFameSkip			= DEFAULT_FRAME_SKIP;
				std::atomic<bool>						_IsRunning				= false; // volatile

		// Rendering :
				// [Todo] : Use GGE::Screenhere
				RenderWindow							_window;
				Sprite									_backgroundSprite;
				Texture									_bufBatckgroundTexture; // To use as buffer. [Todo]=[To_test] -> SetSmooth

		// Collision engine :
				// [Todo] : CollisionEngine

		// Entities :
				EntityManager							_entityManager;

		// EventsHandler :
				EventHandler::MapType *					_EventTypeToCB = &(EventHandler::Debugger::GetTypeToCB_Map());
				EventHandler::RegistrableEventsMapType	_registeredEvents;
				// [Todo] : Add CooldownManager / time event handler
	};
}

#endif // __GEE_GAME__