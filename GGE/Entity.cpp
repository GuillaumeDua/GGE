#include "Entity.h"

//
// [Sample of implementation]
//

//const std::vector<const GGE::SPRITE::Sheet> Sonic_EntityDescriptor::SheetsPack =
//{
//		{ GGE::SPRITE::Sheet("C:/DEV/PROJECTS/GGE/GGE/SPRITES/Sonic_walking.png", std::move(std::pair<int, int>(516, 111)), std::move(std::pair<int, int>(6, 1))) }
//	,	{ GGE::SPRITE::Sheet("C:/DEV/PROJECTS/GGE/GGE/SPRITES/explosion.png", std::move(std::pair<int, int>(320, 320)), std::move(std::pair<int, int>(5, 5)))}
//};

const GGE::SPRITE::Sheet Sonic_EntityDescriptor::gSpriteSheet_walking("C:/DEV/PROJECTS/GGE/GGE/SPRITES/Sonic_walking.png", std::move(std::pair<int, int>(516, 111)), std::move(std::pair<int, int>(6, 1)));
const GGE::SPRITE::Sheet Sonic_EntityDescriptor::gSpriteSheet_Destroying("C:/DEV/PROJECTS/GGE/GGE/SPRITES/explosion.png", std::move(std::pair<int, int>(320, 320)), std::move(std::pair<int, int>(5, 5)));

const Sonic_EntityDescriptor::Behavior Sonic_EntityDescriptor::_behavior =
{
	{
		Sonic_EntityDescriptor::Status::Walking,
		[&](Entity<Sonic_EntityDescriptor> & entity) mutable -> bool
		{
			// std::cout << "Sonic_EntityDescriptor::_behavior called !" << std::endl;
			return true;
		}
	},
	{
		Sonic_EntityDescriptor::Status::Destroying,
		[&](Entity<Sonic_EntityDescriptor> & entity) mutable -> bool
		{
			if (entity.GetAnimation().at(entity.GetCurrentStatus()).IsOver())
				entity.ForceCurrentStatus(Sonic_EntityDescriptor::Status::Walking);
				//delete &entity;

			// std::cout << "Sonic_EntityDescriptor::_behavior called !" << std::endl;
			return true;
		}
	}
};
const Sonic_EntityDescriptor::Animation Sonic_EntityDescriptor::_animation =
{
		{ Sonic_EntityDescriptor::Status::Walking, std::move(GGE::SPRITE::Serie(Sonic_EntityDescriptor::gSpriteSheet_walking, 6, 0)) }
		, { Sonic_EntityDescriptor::Status::Destroying, std::move(GGE::SPRITE::Serie(Sonic_EntityDescriptor::gSpriteSheet_Destroying, 25, 0)) }
};
const std::pair<int, int>				Sonic_EntityDescriptor::_size = std::make_pair(82, 111);

//
// [Sample of implementation]
//
