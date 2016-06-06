#include "Particles.h"

namespace GameImpl
{
	namespace Particles
	{
		// crash on : std::move(std::pair<int, int>(5, -->[0]<-- )));
		const GGE::SPRITE::Sheet ParticleTrait::gSpriteSheet_walking("C:/DEV/PROJECTS/GGE/GGE/GamesImpl/SPRITES/Particle.bmp", std::move(std::pair<int, int>(50, 10)), std::move(std::pair<int, int>(5, 1)));

		const ParticleTrait::Behavior ParticleTrait::_behavior =
		{
			{
				ParticleTrait::Status::Moving,
				[&](GGE::Entity<ParticleTrait> & entity) mutable -> bool
				{
					entity.Position().first += randomGen.Get();
					entity.Position().second += randomGen.Get();

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