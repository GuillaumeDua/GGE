#include "BrownienParticles.h"

namespace GameImpl
{
	namespace BrownienParticles
	{
		const GGE::SPRITE::Sheet ParticleTrait::gSpriteSheet_walking("C:/DEV/PROJECTS/GGE/GGE/GamesImpl/SPRITES/Particle_alpha.bmp", std::move(std::pair<int, int>(50, 10)), std::move(std::pair<int, int>(5, 1)));

		const ParticleTrait::Behavior ParticleTrait::_behavior =
		{
			{
				ParticleTrait::Status::Moving,
				[&](GGE::Entity<ParticleTrait> & entity) mutable -> bool
				{
					// std::cout << "ParticleTrait::_behavior called !" << std::endl;
					return true;
				}
			}
		};
		const ParticleTrait::Animation ParticleTrait::_animation =
		{
				{ ParticleTrait::Status::Moving, std::move(GGE::SPRITE::Serie(ParticleTrait::gSpriteSheet_walking, 5, 0)) }
		};
		const std::pair<int, int>				ParticleTrait::_size = std::make_pair(10, 10);
	}
}