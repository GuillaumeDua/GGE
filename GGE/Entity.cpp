#include "Entity.h"

const GGE::SPRITE::Sheet Sonic_EntityDescriptor::gSpriteSheet(GGE::SPRITE::Sheet("C:/DEV/PROJECTS/GGE/GGE/SPRITES/Sonic_walking.png", std::move(std::pair<int, int>(516, 111)), std::move(std::pair<int, int>(6, 1))));

//
// [Sample of implementation]
//

static const Sonic_EntityDescriptor::Behavior _behavior =
{
	{
		Sonic_EntityDescriptor::Status::Walking,
		[&](Entity<Sonic_EntityDescriptor> & entity) mutable -> bool
		{
			std::cout << "Sonic_EntityDescriptor::_behavior called !" << std::endl;
			return true;
		}
	}
};
static const Sonic_EntityDescriptor::Animation _animation =
{
	{ Sonic_EntityDescriptor::Status::Walking, std::move(GGE::SPRITE::Serie(Sonic_EntityDescriptor::gSpriteSheet, 6, 0)) }
};
static const std::pair<int, int>				_size = std::make_pair(82, 111);

//
// [Sample of implementation]
//
