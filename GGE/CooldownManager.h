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
			using CallBack_Type = std::function < void(void) > ;
			using CallBackMap_type = std::multimap < std::chrono::steady_clock::time_point, CallBack_Type > ;

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
			using CallBackFunctionType = bool(void);
			using CallBack_Type = std::function < CallBackFunctionType > ;
			using EventType = struct
			{
				const std::chrono::duration<int>	_interval;
				const CallBack_Type					_cb;
			};
			using CallBackMap_type = std::multimap < std::chrono::steady_clock::time_point, EventType > ;

			ReconductibleCooldownsManager() = default;
			ReconductibleCooldownsManager(const ReconductibleCooldownsManager &) = delete;
			ReconductibleCooldownsManager(const ReconductibleCooldownsManager &&) = delete;

			ReconductibleCooldownsManager &			operator+=(const EventType && element)
			{
				this->_callbacks.insert(std::make_pair(std::chrono::steady_clock::now() + element._interval, element));
				return *this;
			}
			void									Check(void)
			{
				std::chrono::steady_clock::time_point		current = std::chrono::steady_clock::now();
				CallBackMap_type newContent;

				newContent.insert(this->_callbacks.upper_bound(current), this->_callbacks.end());
				auto max = this->_callbacks.lower_bound(current);
				for (auto it = this->_callbacks.begin(); it != max; ++it)
				{
					if (it->second._cb())
						newContent.emplace(std::move(std::make_pair(it->first + it->second._interval, it->second)));
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