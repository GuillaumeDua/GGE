#include <SFML/Graphics.hpp>
#include "GCL/Exception.h"
#include "GCL/Preprocessor.h"
# include "__Game.h"

int	main(int ac, char *av[])
{
	GGE::Game game;

	try
	{
		// SPRITES/Sonic_spritesSheet.png

		game.SetBackground("SPRITES/bg_blue.png");

		// [TEST]
		Sonic sonic(std::make_pair(200, 200));
		sonic.ForceCurrentStatus(Sonic::Status::Walking);
		game.GetRefEntityManager() += static_cast<IEntity*>(&sonic);
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
