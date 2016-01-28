#include <SFML/Graphics.hpp>
#include "GCL/Exception.h"
#include "GCL/Preprocessor.h"
#include "__Game.h"

#include "GCL/Notification.h"


int	main(int ac, char *av[])
{
	//GCL::Notification::Test::Process();

	//DEBUG_INSTRUCTION(system("pause");)
	//return 0;


	GGE::Game game{ 60 };

	try
	{
// [TEST]

		Sonic sonic = std::move(std::make_pair(0.f, 0.f));
		sonic.ForceCurrentStatus(Sonic::Status::Walking);
		game.Entities() += static_cast<IEntity*>(&sonic);

		Sonic sonicIA({ 400.f, 400.f });
		sonicIA.ForceCurrentStatus(Sonic::Status::Walking);
		game.Entities() += static_cast<IEntity*>(&sonicIA);

		sonicIA.CollisionsEvents() +=
		{
			[&sonicIA](const HitBox * hb)
			{
				std::cout << "[+] Collision detected" << std::endl;
				sonicIA.ForceCurrentStatus(Sonic::Status::Destroying);
				// sonicIA.DoesRequierUnregisterFromCollisionEngine();
			}
		};

		game += new GGE::Game::SceneType(
			"SPRITES/bg_blue.png",
			{
					static_cast<IEntity*>(&sonic)
				,	static_cast<IEntity*>(&sonicIA)
			}
		);

		game.setActiveScene(0);

// 1 : Controlable
		// [Fun] : Funny event registering system test
		int		callIt		= 0;
		Sonic *	sonicPtr	= &sonic;

		game.GetEventRegisteringSytem().emplace(std::make_pair(sf::Event::MouseWheelMoved, [&, sonicPtr](const sf::Event & event) mutable -> GGE::UserEventsHandler::RegisteredCBReturn
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

			return GGE::UserEventsHandler::RegisteredCBReturn::OK;
		}));
		game.GetEventRegisteringSytem().emplace(std::make_pair(sf::Event::MouseLeft, [&](const sf::Event & event) mutable -> GGE::UserEventsHandler::RegisteredCBReturn
		{
			// std::cout << "Putain, mais te barre pas" << std::endl;
			return GGE::UserEventsHandler::RegisteredCBReturn::OK;
		}));
		game.GetEventRegisteringSytem().emplace(std::make_pair(sf::Event::MouseButtonPressed, [&, sonicPtr](const sf::Event & event) mutable -> GGE::UserEventsHandler::RegisteredCBReturn
		{
			sonicPtr->SetMovement(std::make_pair(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y)));
			return GGE::UserEventsHandler::RegisteredCBReturn::OK;
		}));
		game.GetCooldownManagerSystem() += GGE::Events::CooldownManager::Reconductible::EventType({ std::chrono::seconds(1), [](){ std::cout << "CD done !" << std::endl; return true; } });
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
