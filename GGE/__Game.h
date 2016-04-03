#ifndef __GEE_GAME__
# define __GEE_GAME__

# include <SFML/Window\Window.hpp>
# include <SFML/Graphics/RenderWindow.hpp>
# include <iostream>
# include <atomic>
# include <functional>
# include <map>
# include <memory>

# include <GCL_CPP/Exception.h>
# include <GCL_CPP/Vector.h>
# include "RENDERING/Types.h"
# include "IEntity.h"
# include "Entity.h"
# include "Scene.h"
# include "EventHandler.h"
# include "CooldownManager.h"
# include "ICollisionEngine.h"

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
		// Game() = delete;
		Game(const size_t TicksPerSec = 60)
			: _ticksSystem(TicksPerSec)
			, _window(sf::VideoMode(800, 600, 32), "GEE : Instance rendering")
		{
			this->Initialize();
		}
		Game(const Game &)	= delete;
		Game(const Game &&) = delete;
		~Game(){}

		void												Initialize(void)
		{
			// _window.setFramerateLimit
			// _window.getView
			if (!(_window.isOpen()))
				throw GCL::Exception("[Error] : Rendering window is not open");
			SceneType::BindWindow(&_window);

			this->_frameEventManager += {5, [this]() mutable -> bool
			{
				this->_collisionEngine->Calculate();
				this->_collisionEngine->ApplyOnCollisionEvents();
				return true;
			}};
			// [Todo]::[Refactoring] : Split entities sprite refresh and behave logics
			this->_frameEventManager += {5, [this]() mutable -> bool
			{ 
				for (auto & elem : this->_entities)
					if (!elem->Behave())
						throw std::runtime_error("[Error] : Entity failed to behave correctly");
				return true;  
			}};
			/*this->_frameEventManager += {1, [this]() mutable -> bool
			{
				for (auto & elem : this->_entities)
					elem->Draw(this->_window);
				return true;
			}};*/
		}

		// Run [-> I cld use my own Runnable class]
		bool												Start(void)
		{
			if (this->_IsRunning)
			{
				std::cerr << "[Error]::[GGE::Game::Start] : attempting to start a game that is already running" << std::endl;
				return false;
			}
			if (_currentSceneIt == _scenes.end())
			{
				std::cerr << "[Error]::[GGE::Game::Start] : attempting to start a game that has no active scene set" << std::endl;
				return false;
			}

			this->_IsRunning = true;

			std::cout << "[+] Start ... framerate set at : " << _ticksSystem.FPS << std::endl;
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
		bool												Stop(void)
		{
			this->_IsRunning = false;
		}
		// Scenes
		using SceneType = GGE::Scene<IEntity>;
		Game &												operator+=(const std::shared_ptr<SceneType> & scene)
		{
			this->_scenes.push_back(scene);

			if (_scenes.size() == 1)
				this->setActiveScene(0);

			return *this;
		}
		inline std::shared_ptr<SceneType>					operator[](const size_t it)// throw (std::out_of_range)
		{
			return this->_scenes.at(it);
		}
		inline std::vector<std::shared_ptr<SceneType>> &	Scenes(void)
		{
			return _scenes;
		}
		void												setActiveScene(const size_t index)
		{
			this->_currentSceneIt = _scenes.begin();
			std::advance(_currentSceneIt, index);

			// Load scene entities into game engine
			_entities = _currentSceneIt->get()->GetContent();

			// Load scene entities into collision engine
			this->_collisionEngine->Unload();
			for (auto & entity : _entities)
				*(this->_collisionEngine) += std::dynamic_pointer_cast<HitBox>(entity);
		}
		// Events handling
		inline const UserEventsHandler::MapType &			GetEventHandler_map(void)
		{
			return *(this->_EventTypeToCB);
		}
		inline UserEventsHandler::RegistrableEventsMapType& GetEventRegisteringSytem(void)
		{
			return _userEventsManager;
		}
		inline  Events::CooldownManager::Reconductible &	GetCooldownManagerSystem(void)
		{
			return this->_cooldownManager;
		}
		template <class T_EventHandler>
		inline void											SetEventHandler(void)
		{
			std::cout << "Switching event handler from [" << &(this->_EventTypeToCB) << "] to : [" << &(T_EventHandler::GetTypeToCB_Map()) << ']' << std::endl;
			this->_EventTypeToCB = &(T_EventHandler::GetTypeToCB_Map());
		}
		bool												HandleEvent(const sf::Event & event)
		{
			using GameEventRangeIt = std::pair < UserEventsHandler::MapType::const_iterator, UserEventsHandler::MapType::const_iterator >;
			using RegisteredEventRangeIt = std::pair < UserEventsHandler::RegistrableEventsMapType::iterator, UserEventsHandler::RegistrableEventsMapType::iterator >;
			using EventItQueue = std::queue < UserEventsHandler::RegistrableEventsMapType::iterator >;

			// [Game events handling]
			GameEventRangeIt gameEventRangeIt = this->_EventTypeToCB->equal_range(event.type);
			for (UserEventsHandler::MapType::const_iterator gameEventIt = gameEventRangeIt.first; gameEventIt != gameEventRangeIt.second; ++gameEventIt)
				gameEventIt->second(event, *this);

			// [Registered events handling]
			EventItQueue			toUnregisterEventsQueue;
			RegisteredEventRangeIt	registeredEventRangeIt = this->_userEventsManager.equal_range(event.type);
			for (UserEventsHandler::RegistrableEventsMapType::iterator registeredEventsIt = registeredEventRangeIt.first; registeredEventsIt != registeredEventRangeIt.second; ++registeredEventsIt)
			{
				switch (registeredEventsIt->second(event))
				{
				case UserEventsHandler::RegisteredCBReturn::FAILURE:
					throw std::runtime_error("Fatal error in registered event's callback execution");
					break;
				case UserEventsHandler::RegisteredCBReturn::REQUIRE_UNREGISTERED:
					toUnregisterEventsQueue.push(registeredEventsIt);
					break;
				case UserEventsHandler::RegisteredCBReturn::OK :
				default:
					break;
				}
			}
			// Unregistering
			while (toUnregisterEventsQueue.size() != 0)
			{
				this->_userEventsManager.erase(toUnregisterEventsQueue.back());
				toUnregisterEventsQueue.pop();
			}
			return true;
		}
		void												ManageEvents(void)
		{
			// [Cooldown events handling]
			this->_cooldownManager.Check();

			// [Ui events]
			sf::Event event;
			while (this->_window.pollEvent(event))
				if (this->HandleEvent(event) == false)
					throw std::runtime_error("[UnexpectedException]::[ManageEvents] : Failure while handling an event");

			// [Frame events]
			_frameEventManager.Check();
		}
		// Entities 
		using T_EntityVector = GCL::Vector<std::shared_ptr<IEntity>>;
		inline T_EntityVector &								Entities(void)
		{
			return _entities;
		}

		// Runnable :
		std::atomic<bool>									_IsRunning = false; // volatile

	protected:
		// Loop
		void												Update(void)
		{
			try
			{
				this->ManageEvents();
				(*_currentSceneIt)->Draw();
			}
			catch (const std::exception & ex)
			{
				std::cerr << "[Error]::[GGE::Game::Update] : std::exception catch : [" << ex.what() << ']' << std::endl;
			}
			catch (...)
			{
				std::cerr << "[Error]::[GGE::Game::Update] : Unknown exception catch" << std::endl;
			}
		}
		bool												Loop(void)
		{
			sf::Clock	clock;
			sf::Time	diffTimeCt = sf::Time::Zero;

			while (this->_IsRunning)
			{
				sf::Time elaspedTime = clock.restart();
				diffTimeCt += elaspedTime;

				while (this->_IsRunning && diffTimeCt > _ticksSystem.TimePerFrame)
				{
					diffTimeCt -= _ticksSystem.TimePerFrame;
					this->Update();
				}
			}
			return true;
		}

		// Ticks :
		// [Todo] : Refactoring
		struct TicksSystem
		{
			TicksSystem(const size_t TicksPerSec)
				: _TicksPerSec(TicksPerSec)
			{}
			TicksSystem(const TicksSystem &) = default;
			TicksSystem & operator=(const TicksSystem &) = delete;
			~TicksSystem() = default;

			const float												FPS = 60.f;
			const sf::Time 											TimePerFrame = sf::seconds(1.f / FPS);

			static	const size_t									DEFAULT_TICKS_PER_SEC	= 50;
			static	const size_t									DEFAULT_FRAME_SKIP		= 5;
					const size_t									_TicksPerSec			= DEFAULT_TICKS_PER_SEC;
					const size_t									_TicksToSkip			= 1000 / _TicksPerSec;
					const size_t									_MaxFameSkip			= DEFAULT_FRAME_SKIP;
					
		}									_ticksSystem;

		// Rendering :
				// [Todo] : Use GGE::Screenhere
				RenderWindow										_window;
				Sprite												_backgroundSprite;
				Texture												_bufBatckgroundTexture; // To use as buffer. [Todo]=[To_test] -> SetSmooth

		// Collision engine :
				// [Todo] : CollisionEngine
				std::unique_ptr<CollisionEngine::Interface>			_collisionEngine = new CollisionEngine::Implem::Linear<CollisionEngine::Algorythms::AABB>();

		// Entities :
				// EntityManager										_entityManager;
				T_EntityVector										_entities;

		// EventsHandler :
				std::unique_ptr<UserEventsHandler::MapType>			_EventTypeToCB = UserEventsHandler::Debugger::GetTypeToCB_Map();
				UserEventsHandler::RegistrableEventsMapType			_userEventsManager;
				Events::CooldownManager::Reconductible				_cooldownManager;
				Events::CooldownManager::ByTicks					_frameEventManager;

		// Screens
				std::vector<std::shared_ptr<SceneType>>				_scenes;
				std::vector<std::shared_ptr<SceneType>>::iterator	_currentSceneIt = _scenes.end();
	};
}

#endif // __GEE_GAME__