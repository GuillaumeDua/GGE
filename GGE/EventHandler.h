#ifndef __GGE_EVENT_HANDLER__
# define __GGE_EVENT_HANDLER__

# include <SFML\Window\Event.hpp>
# include <iostream>
# include <map>
# include <functional>
# include <exception>
# include <stdexcept>

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

	namespace EventHandler
	{
		using GameType = Game;
		using CB = std::function<bool(const sf::Event & event, GameType & game)>;			// CB can be a callback, a closure, obj-binded function members with place-holders etc ...
		using MapType = std::map < const sf::Event::EventType, CB >;

		struct Debugger
		{
			static MapType & GetTypeToCB_Map(void)
			{
				std::cout << "[+] Debugger::GetTypeToCB_Map" << std::endl;
				return _eventTypeToCB_map;
			}

			static MapType	_eventTypeToCB_map;
		};
	}
}

#endif // __GGE_EVENT_HANDLER__