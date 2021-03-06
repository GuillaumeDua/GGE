#include <SFML/Graphics.hpp>

#include <gcl_cpp/exception.hpp>
#include <gcl_cpp/preprocessor.hpp>
#include <gcl_cpp/old/EventHandler.h>

#include "GameEngine.h"
#include "GameEditor.h"

//
#include "GamesImpl/Particles.h"
//

int	main(int ac, char *av[])
{
	{
		GameImpl::Particles::Run();

		system("pause");
		return 0;
	}

	//{
	//	GGE::Editor::Test();

	//	system("pause");
	//	return 0;
	//}

	try
	{
// [TEST]
		GGE::GameEngine game(GGE::GameEngine::Configuration::_Default);

		std::shared_ptr<Sonic> sonic = std::make_shared<Sonic>(std::move(std::make_pair(400.f, 400.f)));
		sonic->ForceCurrentStatus(Sonic::Status::Walking);

		std::shared_ptr<Sonic> sonicIA = std::make_shared<Sonic>(std::move(std::make_pair(200.f, 300.f)));
		sonicIA->ForceCurrentStatus(Sonic::Status::Walking);
		// [todo] : game.Entities() += sonicIA; // useless | to_fix

		sonicIA->on(GGE::CollisionEngine::Event::Collision) +=
		{
			[&sonicIA]() // HitBox
			{
				if (sonicIA->GetCurrentStatus() != Sonic::Status::Destroying)
				{
					std::cout << "[+] Collision detected" << std::endl;
					sonicIA->ForceCurrentStatus(Sonic::Status::Destroying);
				}
			}
		};

		game += std::make_shared<GGE::GameEngine::SceneType>(
			"SPRITES/bg_blue.png",
			std::initializer_list < std::shared_ptr<GGE::IEntity> >
		{
			sonic
			, sonicIA
		});

		// game.setActiveScene(0); // Automatic

// 1 : Controlable
		// [Fun] : Funny event registering system test
		int		callIt		= 0;

		game.GetEventRegisteringSytem().emplace(std::make_pair(sf::Event::MouseWheelMoved, [&, sonic](const sf::Event & event) mutable -> GGE::UserEventsHandler::RegisteredCBReturn
		{
			sf::Color color = sonic->GetSpriteModifier()._color;

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
			sonic->GetSpriteModifier()._color = color;

			return GGE::UserEventsHandler::RegisteredCBReturn::OK;
		}));
		game.GetEventRegisteringSytem().emplace(std::make_pair(sf::Event::MouseLeft, [&](const sf::Event & event) mutable -> GGE::UserEventsHandler::RegisteredCBReturn
		{
			// std::cout << "Putain, mais te barre pas" << std::endl;
			return GGE::UserEventsHandler::RegisteredCBReturn::OK;
		}));
		game.GetEventRegisteringSytem().emplace(std::make_pair(sf::Event::MouseButtonPressed, [&, sonic](const sf::Event & event) mutable -> GGE::UserEventsHandler::RegisteredCBReturn
		{
			sonic->SetMovement(std::make_pair(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y)));
			return GGE::UserEventsHandler::RegisteredCBReturn::OK;
		}));
		// game.GetCooldownManagerSystem() += GGE::Events::CooldownManager::Reconductible::EventType({ std::chrono::seconds(1), [](){ std::cout << "CD done !" << std::endl; return true; } });
// [/TEST]

		game.Start();
	}
	catch (const gcl::exception & ex)
	{
		std::cerr << "[ERROR] : gcl exception catch : [" << ex.what() << ']' << std::endl;
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
