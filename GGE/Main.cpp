#include <SFML/Graphics.hpp>
#include "GCL/Exception.h"
#include "GCL/Preprocessor.h"
#include "__Game.h"


int	main(int ac, char *av[])
{
	GGE::Game game;

	try
	{
		game.SetBackground("SPRITES/bg_blue.png");	// SPRITES/Sonic_spritesSheet.png

// [TEST]
		Sonic sonic(std::make_pair(200.f, 200.f));
		sonic.ForceCurrentStatus(Sonic::Status::Walking);
		game.GetRefEntityManager() += static_cast<IEntity*>(&sonic);

		// [Fun] : Funny event registering system test
		int		callIt = 0;
		Sonic *	sonicPtr = &sonic;
		game.GetEventRegisteringSytem().emplace(std::make_pair(sf::Event::MouseMoved, [&, sonicPtr](const sf::Event & event) mutable -> GGE::EventHandler::RegisteredCBReturn
		{
			sonicPtr->SetRotation(static_cast<float>(event.mouseMove.x));
			sonicPtr->SetPosition(std::make_pair(sonicPtr->GetPosition().first, static_cast<float>(event.mouseMove.y)));
			return GGE::EventHandler::RegisteredCBReturn::OK;
		}));
		game.GetEventRegisteringSytem().emplace(std::make_pair(sf::Event::MouseWheelMoved, [&, sonicPtr](const sf::Event & event) mutable -> GGE::EventHandler::RegisteredCBReturn
		{
			sf::Color color = sonicPtr->GetColor();

			if (event.mouseWheel.delta)
			{
				color.r += 10;
				color.g += 10;
				color.b += 10;
			}
			else
			{
				color.r -= 10;
				color.g -= 10;
				color.b -= 10;
			}
			sonicPtr->SetColor(color);

			return GGE::EventHandler::RegisteredCBReturn::OK;
		}));
		game.GetEventRegisteringSytem().emplace(std::make_pair(sf::Event::MouseLeft, [&](const sf::Event & event) mutable -> GGE::EventHandler::RegisteredCBReturn
		{
			std::cout << "Putain, mais te barre pas" << std::endl;
			return GGE::EventHandler::RegisteredCBReturn::OK;
		}));

		GGE::Game * gamePtr = &game;
		game.GetEventRegisteringSytem().emplace(std::make_pair(sf::Event::MouseButtonPressed, [&, gamePtr](const sf::Event & event) mutable -> GGE::EventHandler::RegisteredCBReturn
		{
			
			Sonic * sonic = new Sonic(std::make_pair(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y)));
			sonic->ForceCurrentStatus(Sonic::Status::Walking);
			game.GetRefEntityManager() += static_cast<IEntity*>(sonic);

			game.GetEventRegisteringSytem().emplace(std::make_pair(sf::Event::MouseMoved, [&, sonic](const sf::Event & event) mutable -> GGE::EventHandler::RegisteredCBReturn
			{
				sonic->SetRotation(static_cast<float>(event.mouseMove.x));
				return GGE::EventHandler::RegisteredCBReturn::OK;
			}));

			return GGE::EventHandler::RegisteredCBReturn::OK;
		}));

// [/TEST]

		game.Start();
	}
	catch (const GCL::Exception & ex)
	{
		std::cerr << "[ERROR] : GCL exception catch : [" << ex.what() << ']' << std::endl;
	}
	catch (const std::exception & ex)
	{
		std::cerr << "[ERROR] : STD exception catch : [" << ex.what() << ']' << std::endl;
	}
	catch (...)
	{
		std::cerr << "[FATAL_ERROR] : Unknown exception catch" << std::endl;
	}
	
	DEBUG_INSTRUCTION(system("pause");)
	return 0;
}
