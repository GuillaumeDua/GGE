#ifndef __GEE_GAME__
# define __GEE_GAME__

# include <SFML/Window\Window.hpp>
# include <SFML/Graphics/RenderWindow.hpp>
# include <iostream>
# include <atomic>
# include <functional>
# include <map>
# include <memory>
# include <future>
# include <chrono>

# include <GCL_CPP/Exception.h>
# include <GCL_CPP/Vector.h>
# include "RENDERING/Types.h"
# include "IEntity.h"
# include "Entity.h"
# include "Scene.h"
# include "EventHandler.h"
# include "CooldownManager.h"
# include "CollisionEngine.h"

// Window::SetFramerateLimit => vertical sync ?
// screenshots => sf::Image Scren = App.Capture()
// [Todo] : std::atomic<bool> to lock while switching eventhandler + thread-safe

namespace GGE
{
	class GameEngine final	//	[Todo] : Game object : Game logic wrapper around the engine for convinient end-term game
	{
	public:
		
		struct TicksSystem : GCL::Events::EventHandler<>
		{
			struct Event
			{
				static const GCL::Events::EventHandler<>::T_EventID FrameDrop;
			};

			struct Configuration
			{
				const float _FPS;
				const std::vector<GCL::Events::EventHandler<>::T_EventCallback> _onFrameDropEventCallbacks;
				static Configuration _Default;
			};

			TicksSystem() = delete;
			TicksSystem(const Configuration & c/* = Configuration::_default*/)
				: _FPS(c._FPS)
			{
				for (auto & cb : c._onFrameDropEventCallbacks)
					this->on(Event::FrameDrop) += GCL::Events::EventHandler<>::T_EventCallback(cb);
				this->Reset();
			}
			TicksSystem(const TicksSystem &) = default;
			TicksSystem & operator=(const TicksSystem &) = delete;
			~TicksSystem() = default;

			void			Reset()
			{
				_diffTimeCt = sf::Time::Zero;
			}
			void			Start()
			{
				if (_isRunning)
					throw std::runtime_error("TicksSystem::Start : already running");
				_isRunning = true;

				DEBUG_INSTRUCTION(
				assert(_tickCheckerThread.get() == 0x0);
				_tickCheckerThread = std::make_unique<std::thread>([this]()
				{
					while (_isRunning)
					{
						_fpsCounter = 0;
						std::this_thread::sleep_for(std::chrono::seconds(1));
						std::cout << "[+] TickSystem : FPS = " << _fpsCounter << " / " << _FPS << std::endl;
						if (_fpsCounter < _FPS)
							Notify(TicksSystem::Event::FrameDrop);
					}
				});
				);
				Reset();
			}
			void			Stop()
			{
				if (!_isRunning)
					throw std::runtime_error("TicksSystem::Start : not running");
				_isRunning = false;
				DEBUG_INSTRUCTION
				(
				_tickCheckerThread->join();
				_tickCheckerThread.reset();
				)
			}
			sf::Time		ExecuteForPendingTime(const std::function<void(void)> & callback)
			{
				sf::Time elaspedTime = _clock.restart();
				_diffTimeCt += elaspedTime;

				while (_diffTimeCt > _timePerFrame)
				{
					DEBUG_INSTRUCTION(++_fpsCounter;)
					_diffTimeCt -= _timePerFrame;
					callback();
				}
				return sf::Time(_timePerFrame - _diffTimeCt);
			}

			// Frame drop
			std::atomic<bool>				_isRunning = false;
			DEBUG_INSTRUCTION
			(
			size_t							_fpsCounter;
			std::unique_ptr<std::thread>	_tickCheckerThread;
			)
			// Ticks
			sf::Clock						_clock;
			sf::Time						_diffTimeCt;
			const float						_FPS;
			const sf::Time 					_timePerFrame = sf::seconds(1.f / _FPS);
			const size_t					_TicksToSkip = 1000 / static_cast<size_t>(_FPS);
		};

		struct Configuration
		{
			TicksSystem::Configuration _tickSystemConfiguration;
			struct WindowConfiguration
			{
				uint32_t _x;
				uint32_t _y;
				uint32_t _modeBit;
				std::string _name;
			}	_screenDim;

			static Configuration _Default;
		};

		explicit GameEngine(const Configuration & c/* = Configuration::_Default*/)
			: _tickSystem(c._tickSystemConfiguration)
			, _window(sf::VideoMode(c._screenDim._x, c._screenDim._y, c._screenDim._modeBit), c._screenDim._name)
		{
			if (!_window.isOpen())
				throw std::runtime_error("Fail to open window");
			this->Initialize();
		}
		GameEngine(void) = delete;
		GameEngine(const GameEngine &)	= delete;
		GameEngine(const GameEngine &&) = delete;
		~GameEngine(){}

		void												Initialize(void)
		{
            _currentSceneIt = _scenes.end();
			SceneType::BindWindow(_window);

			// Entities behavior
			this->_frameEventManager += {5, [this]() mutable -> bool
			{
				for (auto & elem : this->_entities)
					if (!elem->Behave())
						throw std::runtime_error("[Error] : Entity failed to behave correctly");
				return true;  
			}};
			// Entities collisions
			this->_frameEventManager += {5, [this]() mutable -> bool
			{
				this->_collisionEngine->Calculate();
				this->_collisionEngine->ApplyOnCollisionEvents();
				return true;
			}};
		}

		// Run [-> I cld use my own Runnable class]
		bool												Start(void)
		{
			if (this->_IsRunning)
			{
				std::cerr << "[Error]::[GGE::GameEngine::Start] : attempting to start a game that is already running" << std::endl;
				return false;
			}
			if (_currentSceneIt == _scenes.end())
			{
				std::cerr << "[Error]::[GGE::GameEngine::Start] : attempting to start a game that has no active scene set" << std::endl;
				return false;
			}

			this->_IsRunning = true;

			std::cout << "[+] Start ... framerate set at : " << _tickSystem._FPS << std::endl;

			try
			{
				 this->Loop();
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
			return true;
		}
		bool												Stop(void)
		{
			this->_IsRunning = false;
		}
		// Scenes
		using SceneType = GGE::Scene<IEntity>;
		GameEngine &										operator+=(const std::shared_ptr<SceneType> & scene)
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
			static sf::Time sleepTime;
			try
			{
				// Events
				sleepTime = _tickSystem.ExecuteForPendingTime([this]() mutable { this->ManageEvents(); });
				_tickSystem.TriggerPendingEvents();

				// Rendering
				(*_currentSceneIt)->Draw();

				std::this_thread::sleep_for(std::chrono::microseconds(sleepTime.asMicroseconds()));
			}
			catch (const std::exception & ex)
			{
				std::cerr << "[Error]::[GGE::GameEngine::Update] : std::exception catch : [" << ex.what() << ']' << std::endl;
			}
			catch (...)
			{
				std::cerr << "[Error]::[GGE::GameEngine::Update] : Unknown exception catch" << std::endl;
			}
		}
		void												Loop(void)
		{
			_tickSystem.Start();
			while (this->_IsRunning)
			{
				this->Update();
			}
			_tickSystem.Stop();
		}

		// Ticks :
		TicksSystem													_tickSystem;

		// Rendering :
		RenderWindow												_window;

		// Collision engine :
				std::unique_ptr<CollisionEngine::Interface>			_collisionEngine = std::make_unique<CollisionEngine::Implem::Linear<CollisionEngine::Algorythms::AABB>>();

		// Entities :
				// EntityManager										_entityManager;
				T_EntityVector										_entities;

		// EventsHandler :
				std::shared_ptr<UserEventsHandler::MapType>			_EventTypeToCB = UserEventsHandler::Debugger::GetTypeToCB_Map();
				UserEventsHandler::RegistrableEventsMapType			_userEventsManager;
				Events::CooldownManager::Reconductible				_cooldownManager;
				Events::CooldownManager::ByTicks					_frameEventManager;

		// Screens
				std::vector<std::shared_ptr<SceneType>>				_scenes;
				std::vector<std::shared_ptr<SceneType>>::iterator	_currentSceneIt = _scenes.end();
	};
}

#endif // __GEE_GAME__