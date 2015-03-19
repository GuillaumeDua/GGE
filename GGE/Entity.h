#ifndef __GGE_ENTITY__
# define __GGE_ENTITY__

# include <map>
# include <functional>
# include <queue>
# include <chrono>

# include "Sprite.h"
# include "GCL/Exception.h"
# include <SFML/Graphics.hpp>

struct IEntity
{
	virtual void	Draw(sf::RenderWindow & renderWindow) = 0;
	virtual bool	Behave(void)						  = 0;
};

template <typename EntityDescriptor> class Entity : public IEntity	//, Garbageable< Entity< EntityDescriptor > >
{
public:
	using Status	= typename EntityDescriptor::Status;
	using Behavior	= typename EntityDescriptor::Behavior;
	using Animation = typename EntityDescriptor::Animation;

	Entity(const std::pair<int, int> & pos)
		: _currentStatus(EntityDescriptor::Default)
		, _position(pos)
		, _size(EntityDescriptor::_size)
		, _behavior(EntityDescriptor::_behavior)
		, _animations(EntityDescriptor::_animation)
		, _rotation(.0f)
	{}
	virtual ~Entity(){}

	void								Draw(sf::RenderWindow & renderWindow)
	{
		sf::Sprite & sprite = *(this->_animations.at(this->_currentStatus).Get());
		sprite.setRotation(_rotation);
		renderWindow.draw(sprite);
	}
	bool								Behave(void)
	{
		return this->_behavior.at(this->_currentStatus)(*this);
	}

	inline const Status					GetCurrentStatus(void) const
	{
		return this->_currentStatus;
	}
	inline void							ForceCurrentStatus(const Status status)
	{
		this->_currentStatus = status;
	}

protected:
	/*Entity(const Entity<EntityDescriptor> &)		{ throw GCL::Exception("Not implemented"); }
	Entity(const Entity<EntityDescriptor> &&)		{ throw GCL::Exception("Not implemented"); }*/
	Entity(const Entity<EntityDescriptor> &)		= delete;
	Entity(const Entity<EntityDescriptor> &&)		= delete;
	Entity() = delete;

	//std::queue<Status>					_pendingStatus;	// [Todo]::[?]

	Status								_currentStatus;
	Behavior							_behavior;
	Animation							_animations;
	std::pair<int, int>					_position;
	std::pair<int, int>					_size;
	float								_rotation;
};

//
//	[Another file] : Sample of implementation
//

struct Sonic_EntityDescriptor
{
	friend Entity < Sonic_EntityDescriptor >;

	enum Status
	{
		Default
		, Standing
		, Walking
		, Running
		, Jumping
	};

	using Behavior	= std::map < Status, std::function<bool(Entity<Sonic_EntityDescriptor>&)> >;	// [Todo] : Multimap + not const [?]
	using Animation	= std::map < Status, GGE::SPRITE::Serie >;

	static const GGE::SPRITE::Sheet		gSpriteSheet;
	static const Behavior				_behavior;
	static const Animation				_animation;

	static const std::pair<int, int>	_size;
};

using Sonic = Entity < Sonic_EntityDescriptor > ;
//typedef Entity < Sonic_EntityDescriptor > Sonic;
//struct Sonic : public Entity < Sonic_EntityDescriptor >
//{
//	Sonic(std::pair<int, int> & pos)
//		: Entity < Sonic_EntityDescriptor >(pos)
//	{}
//};


#endif // __GGE_ENTITY__