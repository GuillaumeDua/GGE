#ifndef __GGE_EVENT_HANDLER__
# define __GGE_EVENT_HANDLER__

# include <SFML\Window\Event.hpp>
# include <iostream>
# include <map>
# include <unordered_map>
# include <functional>
# include <exception>
# include <stdexcept>
# include <memory>

// # include "Game.h"

///////////////////////////////////////
//
// encapsulate std::function into an object that handle task's priority ?
// encapsulate std::function into an object that handle threads ?
// Use std::bind + placeholder
//
// [Todo] : std::multimap[EventType]
//			Plusieurs CB binded au meme event
//
///////////////////////////////////////

namespace GGE
{
	/*
	[Todo] : Keyboard event -> Do a stack of inputs
	[Todo] : Mouse event
	*/
	class Game;

	namespace UserEventsHandler
	{
		using GameType = Game;
		using GameEventCB = std::function<bool(const sf::Event & event, GameType & game)>;			// CB can be a callback, a closure, obj-binded function members with place-holders etc ...
		using MapType = std::unordered_multimap < const sf::Event::EventType, GameEventCB > ;

		struct Debugger
		{
			static const std::unique_ptr<MapType> GetTypeToCB_Map(void)
			{
				std::cout << "[+] Debugger::GetTypeToCB_Map" << std::endl;
				return std::make_unique<MapType>(_eventTypeToCB_map);
			}

		protected:
			static MapType	_eventTypeToCB_map;
		};

		enum RegisteredCBReturn
		{
			OK
			, FAILURE
			, REQUIRE_UNREGISTERED
		};
		using RegisteredCB = std::function<RegisteredCBReturn(const sf::Event & event)>;
		using RegistrableEventsMapType = std::unordered_multimap < const sf::Event::EventType, RegisteredCB > ;
	}
}

#endif // __GGE_EVENT_HANDLER__