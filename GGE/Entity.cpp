#include "Entity.h"

//
// [Sample of implementation]
//

//const std::vector<const GGE::SPRITE::Sheet> Sonic_EntityTrait::SheetsPack =
//{
//		{ GGE::SPRITE::Sheet("C:/DEV/PROJECTS/GGE/GGE/SPRITES/Sonic_walking.png", std::move(std::pair<int, int>(516, 111)), std::move(std::pair<int, int>(6, 1))) }
//	,	{ GGE::SPRITE::Sheet("C:/DEV/PROJECTS/GGE/GGE/SPRITES/explosion.png", std::move(std::pair<int, int>(320, 320)), std::move(std::pair<int, int>(5, 5)))}
//};

const GGE::SPRITE::Sheet Sonic_EntityTrait::gSpriteSheet_walking("C:/DEV/PROJECTS/GGE/GGE/SPRITES/Sonic_walking.png", std::move(std::pair<int, int>(516, 111)), std::move(std::pair<int, int>(6, 1)));
const GGE::SPRITE::Sheet Sonic_EntityTrait::gSpriteSheet_Destroying("C:/DEV/PROJECTS/GGE/GGE/SPRITES/explosion.png", std::move(std::pair<int, int>(320, 320)), std::move(std::pair<int, int>(5, 5)));

const Sonic_EntityTrait::Behavior Sonic_EntityTrait::_behavior =
{
	{
		Sonic_EntityTrait::Status::Walking,
		[&](GGE::Entity<Sonic_EntityTrait> & entity) mutable -> bool
		{
			// std::cout << "Sonic_EntityTrait::_behavior called !" << std::endl;
			return true;
		}
	},
	{
		Sonic_EntityTrait::Status::Destroying,
		[&](GGE::Entity<Sonic_EntityTrait> & entity) mutable -> bool
		{
			if (entity.GetAnimation().at(entity.GetCurrentStatus()).IsOver())
				entity.ForceCurrentStatus(Sonic_EntityTrait::Status::Walking);
				//delete &entity;

			return true;
		}
	}
};
const Sonic_EntityTrait::Animation Sonic_EntityTrait::_animation =
{
		{ Sonic_EntityTrait::Status::Walking, std::move(GGE::SPRITE::Serie(Sonic_EntityTrait::gSpriteSheet_walking, 6, 0)) }
		, { Sonic_EntityTrait::Status::Destroying, std::move(GGE::SPRITE::Serie(Sonic_EntityTrait::gSpriteSheet_Destroying, 25, 0)) }
};
const std::pair<int, int>				Sonic_EntityTrait::_size = std::make_pair(82, 111);

//
// [Sample of implementation]
//
