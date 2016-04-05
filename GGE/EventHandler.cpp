#include "EventHandler.h"
#include "RENDERING\Types.h"

# include "Game.h"

///////////////////////////////////////
//
// Sample of implementation
//
///////////////////////////////////////

using namespace GGE;

UserEventsHandler::MapType	UserEventsHandler::Debugger::_eventTypeToCB_map =
{
	{
		sf::Event::Closed,
		[&](const sf::Event & event, UserEventsHandler::GameType & game) -> bool { std::cout << "Window closed" << std::endl; game._IsRunning = false; return true; }
	},
	{
		sf::Event::Resized,
		[&](const sf::Event & event, UserEventsHandler::GameType & game) -> bool { std::cout << "Resized : " << event.size.width << 'x' << event.size.height << std::endl; return true; }
	},
	{
		sf::Event::LostFocus,
		[&](const sf::Event & event, UserEventsHandler::GameType & game) -> bool { std::cout << "Focus lost" << std::endl; return true; }
	},
	{
		sf::Event::GainedFocus,
		[&](const sf::Event & event, UserEventsHandler::GameType & game) -> bool { std::cout << "Focus gained" << std::endl; return true; }
	},
	{
		sf::Event::KeyPressed,
		[&](const sf::Event & event, UserEventsHandler::GameType & game) -> bool { std::cout << "KeyPress : " << event.key.code << std::endl; return true; }
	},
	{
		sf::Event::KeyReleased,
		[&](const sf::Event & event, UserEventsHandler::GameType & game) -> bool { std::cout << "KeyReleased : " << event.key.code << std::endl; return true; }
	},
	{
		sf::Event::TextEntered,
		[&](const sf::Event & event, UserEventsHandler::GameType & game) -> bool { std::cout << "Text entered : " << event.text.unicode << " [" << static_cast<char>(event.text.unicode) << ']' << std::endl; return true; }
	},
	{
		sf::Event::MouseWheelMoved,
		[&](const sf::Event & event, UserEventsHandler::GameType & game) -> bool { std::cout << "Wheel movement : " << event.mouseWheel.delta << " on : " << event.mouseWheel.x << 'x' << event.mouseWheel.y << std::endl; return true; }
	},
	{
		sf::Event::MouseButtonPressed,
		[&](const sf::Event & event, UserEventsHandler::GameType & game) -> bool { std::cout << "MouseButtonPressed : " << event.mouseButton.button << " : " << event.mouseButton.x << 'x' << event.mouseButton.y << std::endl; return true; }
	},
	{
		sf::Event::MouseMoved,
		[&](const sf::Event & event, UserEventsHandler::GameType & game) -> bool { /*std::cout << "MouveMoved : " << event.mouseMove.x << 'x' << event.mouseMove.y << std::endl;*/ return true; }
	},
	{
		sf::Event::MouseEntered,
		[&](const sf::Event & event, UserEventsHandler::GameType & game) -> bool { std::cout << "MouseEntered" << std::endl; return true; }
	},
	{
		sf::Event::MouseLeft,
		[&](const sf::Event & event, UserEventsHandler::GameType & game) -> bool { std::cout << "MouseLeft" << std::endl; return true; }
	}
};