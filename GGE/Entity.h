#ifndef __GGE_ENTITY__
# define __GGE_ENTITY__

# include <map>
# include <functional>
# include <queue>

# include "Sprite.h"
# include "GCL/Exception.h"
# include <SFML/Graphics.hpp>

struct IEntity
{
	virtual void	Draw(sf::RenderWindow & renderWindow) = 0;
	virtual bool	Behave(void)						  = 0;
};

template <typename EntityDescriptor> class Entity : public IEntity
{
public:
	using Behavior = std::map < EntityDescriptor::Status, std::function<bool(Entity<EntityDescriptor::Status>&)> >;
	using Animation = std::map < EntityDescriptor::Status, GGE::Sprite::Serie >;

	Entity(	const std::pair<int, int> & pos
		,	const std::pair<int, int> & size
		)
		: _currentStatus(EntityDescriptor::Status::Default)
		, _position(pos)
		, _size(size)
		, _behavior(EntityDescriptor::_behavior)
		, _animations(EntityDescriptor::_animation)
	{}

	void									Draw(sf::RenderWindow & renderWindow)
	{
		renderWindow.draw(*(this->_animations.at(this->_currentStatus).Get()));
	}
	bool									Behave(void)
	{
		return this->_behavior.at(this->_currentStatus)(*this);
	}
	inline const EntityDescriptor::Status	GetCurrentStatus(void) const
	{
		return this->_currentStatus;
	}

protected:
	Entity(const Entity<EntityDescriptor::Status> &)	{ throw GCL::Exception("Not implemented"); }
	Entity(const Entity<EntityDescriptor::Status> &&)	{ throw GCL::Exception("Not implemented"); }
	Entity(){}
	virtual ~Entity();	

	std::queue<EntityDescriptor::Status>	_pendingStatus;

	EntityDescriptor::Status				_currentStatus;
	const Behavior	&						_behavior;
	const Animation	&						_animations;
	std::pair<int, int>						_position;
	std::pair<int, int>						_size;
};


struct Sonic_EntityDescriptor
{
	enum Status
	{
		Default
		, Standing
		, Walking
		, Running
		, Jumping
	};

	using Behavior = std::map < Status, std::function<bool(Entity<Status>&)> >;
	using Animation = std::map < Status, GGE::Sprite::Serie >;

	const Behavior _behavior = 
	{
	};

	const Animation _animation =
	{
	};
};


struct Sonic : public Entity < Sonic_EntityDescriptor >
{
	
};

#endif // __GGE_ENTITY__