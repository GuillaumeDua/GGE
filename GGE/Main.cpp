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
			std::cout << ++callIt << std::endl;
			sonicPtr->SetRotation(static_cast<float>(event.mouseMove.x));
			sonicPtr->SetPosition(std::make_pair(sonicPtr->GetPosition().first, static_cast<float>(event.mouseMove.y)));
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
