#ifndef __GGE_EVENTS_CDMANAGER__
# define __GGE_EVENTS_CDMANAGER__

# include <GCL_CPP/Preprocessor.h>
# include <iostream>
# include <map>
# include <chrono>
# include <functional>
# include <future>
# include <type_traits>

namespace GGE
{
	namespace Events
	{
		namespace CooldownManager
		{
			template <typename T_Key_Type, typename T_CB_Type>
			struct Interface
			{
				using T_CallBackMap_type = std::multimap<typename T_Key_Type, typename std::function<T_CB_Type> >;
				using T_KeyType = typename T_Key_Type;
				using T_CallBackType = typename T_CB_Type;

				explicit Interface() = default;
				Interface(const Interface &) = delete;
				Interface(const Interface &&) = delete;
				virtual ~Interface() = default;

				virtual Interface &	operator+=(const std::pair<typename T_CallBackMap_type::key_type, typename T_CallBackMap_type::mapped_type> && element) = 0;
				virtual void		Check(void) = 0;

			protected:
				typename T_CallBackMap_type	_callbacks;
			};

			struct SingleTime : public Interface < std::chrono::steady_clock::time_point, void(void) >
			{
				SingleTime &	operator+=(const std::pair<typename T_CallBackMap_type::key_type, typename T_CallBackMap_type::mapped_type> && element)
				{
					this->_callbacks.emplace(element);
					return *this;
				}
				void			Check(void)
				{
					std::chrono::steady_clock::time_point current = std::chrono::steady_clock::now();

					auto max = this->_callbacks.lower_bound(current);
					for (auto it = this->_callbacks.begin(); it != max; ++it)
						it->second();
					this->_callbacks.erase(this->_callbacks.begin(), max);
					this->_last_check = current;
				}

			protected:
				std::chrono::steady_clock::time_point	_last_check = std::chrono::steady_clock::now();
			};
			struct ByTicks : public Interface< size_t, bool(void) >
			{
				ByTicks &		operator+=(const std::pair<typename T_CallBackMap_type::key_type, typename T_CallBackMap_type::mapped_type> && element)
				{
					_callbacks.insert(element);
					return *this;
				}
				void			Check(void)
				{
					_ticks = (_ticks == std::numeric_limits<size_t>::max() ? 1 : _ticks + 1);
					for (auto & elem : _callbacks)
					{
						if (_ticks % elem.first == 0)
							elem.second();
					}
				}

			protected:
				size_t			_ticks = 0;
			};
			struct Reconductible
			{
				using CallBack_Type = std::function < bool(void) > ;
				using EventType = struct
				{
					const std::chrono::duration<int>	_interval;
					const CallBack_Type					_cb;
				};
				using CallBackMap_type = std::multimap < std::chrono::steady_clock::time_point, EventType > ;

				Reconductible() = default;
				Reconductible(const Reconductible &) = delete;
				Reconductible(const Reconductible &&) = delete;

				Reconductible &							operator+=(const EventType && element)
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

			protected:
				std::chrono::steady_clock::time_point	_last_check = std::chrono::steady_clock::now();
				CallBackMap_type						_callbacks;
			};
		}
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