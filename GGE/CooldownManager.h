#ifndef __GGE_EVENTS_CDMANAGER__
# define __GGE_EVENTS_CDMANAGER__

# include "GCL/Preprocessor.h"
# include <iostream>
# include <map>
# include <chrono>
# include <functional>
# include <future>

namespace GGE
{
	namespace Events
	{
		struct CooldownsManager
		{
			using CallBack_Type = std::function<void(void)>;
			using CallBackMap_type = std::multimap < std::chrono::steady_clock::time_point, CallBack_Type >;

			CooldownsManager &	operator+=(const std::pair<typename CallBackMap_type::key_type, typename CallBackMap_type::mapped_type> && element)
			{
				this->_callbacks.emplace(element);
				return *this;
			}
			void				Check(void)
			{
				std::chrono::steady_clock::time_point current = std::chrono::steady_clock::now();

				auto max = this->_callbacks.lower_bound(current);
				for (auto it = this->_callbacks.begin(); it != max; ++it)
					it->second();
				this->_callbacks.erase(this->_callbacks.begin(), max);
				this->_last_check = current;
			}

			std::chrono::steady_clock::time_point	_last_check = std::chrono::steady_clock::now();
			CallBackMap_type						_callbacks;
		};
		struct ReconductibleCooldownsManager
		{
			using CallBackFunctionType = void(void);
			using CallBack_Type = std::function<CallBackFunctionType>;
			using EventType = struct
			{
				const std::chrono::duration<int>	_duration;
				const CallBack_Type					_cb;
			};
			using CallBackMap_type = std::multimap < std::chrono::steady_clock::time_point, EventType >;

			struct	AsyncCallBackManager // So, cannot use std::shared_future
			{
				using CB_future_TYPE = std::future < void >;
				using ContainerType = std::vector < CB_future_TYPE >;

				AsyncCallBackManager(){}
				AsyncCallBackManager(const AsyncCallBackManager &) = delete;
				AsyncCallBackManager(const AsyncCallBackManager &&) = delete;

				AsyncCallBackManager & operator+=(CallBack_Type && callBack)
				{
					_mutex.lock();
					if (this->_content.size() == 10)
						std::cerr << "[ALERT] : Attempting to add an event, but already 10 are queued. Aborting." << std::endl;
					else
						this->_content.emplace_back(std::move(std::async(std::launch::async, callBack)));
					_mutex.unlock();
					return *this;
				}
				AsyncCallBackManager & operator+=(CB_future_TYPE && callback)
				{
					_mutex.lock();
					if (this->_content.size() == 10)
						std::cerr << "[ALERT] : Attempting to add an event, but already 10 are queued. Aborting." << std::endl;
					else
						this->_content.emplace_back(std::move(callback));
					_mutex.unlock();
					return *this;
				}

				void	Start(void)
				{
					if (this->_isRunning.load() == false)
						this->_isRunning = true;
					this->_thread = std::move(std::thread(std::bind(&ReconductibleCooldownsManager::AsyncCallBackManager::Routine, this)));
				}
				void	Stop(void)
				{
					this->_isRunning = false;
				}
				bool	IsRunning(void) const
				{
					return this->_isRunning.is_lock_free();
				}
				void	ForceGet()
				{
					for (auto & elem : this->_content)
						elem.get();
					this->_content.clear();
				}

			protected:

				void		Routine()
				{
					while (this->_isRunning.load() == true)
					{
						_mutex.lock();
						DEBUG_INSTRUCTION(const size_t size_before_remove = this->_content.size();)
							ContainerType::iterator remove_token = std::remove_if(this->_content.begin(), this->_content.end(), [&](ContainerType::value_type & future) -> bool { return (future.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready); });
						ContainerType new_content;//(this->_content.begin(), remove_token);
						for (ContainerType::iterator it = this->_content.begin(); it != remove_token; ++it)
							new_content.emplace_back(std::move(*it));
						this->_content = std::move(new_content);
						std::cout
							<< "[+] : Event queue qty : " << _content.size()
							<< DEBUG_INSTRUCTION(", " << size_before_remove - this->_content.size() << " removed at last check." << ) std::endl;
						_mutex.unlock();
						std::this_thread::sleep_for(std::chrono::seconds(1));
					}
					std::cout << "[+] : [ReconductibleCooldownsManager::AsyncCallBackManager::Routine] : Stoped" << std::endl;
				}

				ContainerType		_content;
				std::atomic<bool>	_isRunning = false;
				std::thread			_thread;
				static std::mutex	_mutex;
			}		static _asyncCallBackManager;

			ReconductibleCooldownsManager()
			{
				_asyncCallBackManager.Start();
			}
			ReconductibleCooldownsManager(const ReconductibleCooldownsManager &) = delete;
			ReconductibleCooldownsManager(const ReconductibleCooldownsManager &&) = delete;

			ReconductibleCooldownsManager &	operator+=(const std::pair<typename CallBackMap_type::key_type, typename CallBackMap_type::mapped_type> && element)
			{
				this->_callbacks.emplace(element);
				return *this;
			}
			void							Check(void)
			{
				std::chrono::steady_clock::time_point		current = std::chrono::steady_clock::now();
				CallBackMap_type newContent;

				newContent.insert(this->_callbacks.upper_bound(current), this->_callbacks.end());
				auto max = this->_callbacks.lower_bound(current);
				for (auto it = this->_callbacks.begin(); it != max; ++it)
				{
					ReconductibleCooldownsManager::_asyncCallBackManager += std::async(std::launch::async, it->second._cb);
					// std::future<void> async_future = std::async(std::launch::async, it->second._cb);
					newContent.emplace(std::move(std::make_pair(it->first + it->second._duration, it->second)));
				}
				this->_callbacks = std::move(newContent);
				this->_last_check = current;
			}

			std::chrono::steady_clock::time_point	_last_check = std::chrono::steady_clock::now();
			CallBackMap_type						_callbacks;
		};
	}
 }
#endif // __GGE_EVENTS_CDMANAGER__

///
/// [TEST]
///


//int main()
//{
//	sf::RenderWindow	window(sf::VideoMode(180, 250), "My SFML POC");
//	SpriteSheet			spriteSheet{ "SPRITES/sheet1.png", std::move(std::make_pair(180, 250)), std::move(std::make_pair(5, 2)) };
//
//	//CooldownsManager	cooldownsManager;
//	//
//	//cooldownsManager += std::move(std::make_pair(std::chrono::steady_clock::now() + std::chrono::seconds(1), [](){ std::cout << '1' << std::endl;}));
//	//cooldownsManager += std::move(std::make_pair(std::chrono::steady_clock::now() + std::chrono::duration<int>(2), [](){ std::cout << '2' << std::endl;}));
//	//cooldownsManager += std::move(std::make_pair(std::chrono::steady_clock::now() + std::chrono::duration<int>(3), [](){ std::cout << '3' << std::endl;}));
//	//cooldownsManager += std::move(std::make_pair(std::chrono::steady_clock::now() + std::chrono::duration<int>(4), [](){ std::cout << '4' << std::endl;}));
//	//cooldownsManager += std::move(std::make_pair(std::chrono::steady_clock::now() + std::chrono::duration<int>(5), [](){ std::cout << '5' << std::endl;}));
//
//	ReconductibleCooldownsManager SpriteCooldownsManager;
//
//	SpriteCooldownsManager += std::move(std::make_pair(
//		std::chrono::steady_clock::now() + std::chrono::seconds(1),
//		ReconductibleCooldownsManager::EventType
//	{
//		std::chrono::seconds(3)
//		, std::bind([&](SpriteSheet & s){ for (auto & elem : s.GetContent()) elem.setColor(sf::Color::Green); }, std::ref(spriteSheet))
//	}
//	));
//	SpriteCooldownsManager += std::move(std::make_pair(
//		std::chrono::steady_clock::now() + std::chrono::duration<int>(2),
//		ReconductibleCooldownsManager::EventType
//	{
//		std::chrono::duration<int>(3)
//		, std::bind([&](SpriteSheet & s){ for (auto & elem : s.GetContent()) elem.setColor(sf::Color::Blue); }, std::ref(spriteSheet))
//	}
//	));
//	SpriteCooldownsManager += std::move(std::make_pair(
//		std::chrono::steady_clock::now() + std::chrono::duration<int>(3),
//		ReconductibleCooldownsManager::EventType
//	{
//		std::chrono::duration<int>(3)
//		, std::bind([&](SpriteSheet & s){ for (auto & elem : s.GetContent()) elem.setColor(sf::Color::Red); }, std::ref(spriteSheet))
//	}
//	));
//
//	while (window.isOpen())
//	{
//		sf::Event event;
//		while (window.pollEvent(event))
//		{
//			if (event.type == sf::Event::Closed)
//				window.close();
//		}
//		SpriteCooldownsManager.Check();
//
//		window.clear();
//		window.draw(*(spriteSheet.Get()));
//		window.display();
//
//		std::this_thread::sleep_for(std::chrono::milliseconds(120));
//	}
//
//	return 0;
//}

///
/// [/TEST]
///