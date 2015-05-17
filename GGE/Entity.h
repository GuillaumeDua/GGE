#ifndef __GGE_ENTITY__
# define __GGE_ENTITY__

# include <map>
# include <functional>
# include <queue>
# include <chrono>
# include <math.h>

# include "Sprite.h"
# include "GCL/Exception.h"
# include <SFML/Graphics.hpp>
# include "IEntity.h"

static const double PI = 3.14159265;


template <typename EntityDescriptor> class Entity : public IEntity, public HitBox	//, Garbageable< Entity< EntityDescriptor > >
{
public:
	using ThisType = typename Entity < EntityDescriptor > ;

	using Status	= typename EntityDescriptor::Status;
	using Behavior	= typename EntityDescriptor::Behavior;
	using Animation = typename EntityDescriptor::Animation;

	struct MovementType
	{
		static inline const float Cos(const float angle) { return static_cast<float>(std::cos(angle * PI / 180.0)); }
		static inline const float Sin(const float angle) { return static_cast<float>(std::sin(angle * PI / 180.0)); }

		MovementType()
			: _isActive(false)
		{}
		MovementType(const PositionType & destination)
		{
			Set(destination);
		}
		MovementType & operator=(const PositionType & destination)
		{
			Set(destination);
			return *this;
		}

		inline const bool				IsActive(void) const
		{
			return this->_isActive;
		}
		void							Set(const PositionType & destination)
		{
			assert(destination.first > 0 && destination.second > 0);

			_destination		= destination;
			_modifiers.first	= Cos(_destination.first);
			_modifiers.second	= Sin(_destination.second);

			_isActive = true;
		}
		bool							Apply(PositionType & pos, const float speed)
		{
			assert(_isActive);

			//bool xP = (pos.first < _destination.first);
			//bool yP = (pos.second < _destination.second);
			//pos.first	+= (xP ? 1 : -1) * _modifiers.first * speed;
			//pos.second	+= (yP ? 1 : -1) * _modifiers.second * speed;
			//return (_isActive = (xP == (pos.first < _destination.first)));

			float delta = 1.0;

			float dx = _destination.first - pos.first;
			float dy = _destination.second - pos.second;
			float dist = std::sqrt(dx*dx + dy*dy);

			if (dist > speed * delta)
			{
				dx /= dist;
				dy /= dist;
				pos.first	+= dx * speed * delta;
				pos.second	+= dy * speed * delta;
				return true;
			}
			else
			{
				pos.first	= _destination.first;
				pos.second	= _destination.second;
				return false;
			}
		}

		std::pair<float, float>			_modifiers;		// x sin, y cos
		PositionType					_destination;
		bool							_isActive;
	};


	Entity(const std::pair<float, float> & pos)
		: _currentStatus(EntityDescriptor::Default)
		, HitBox(pos, EntityDescriptor::_size)
		, _behavior(EntityDescriptor::_behavior)
		, _animations(EntityDescriptor::_animation)
		, _rotation(.0f)
		, _speed(5.0)
	{}
	virtual ~Entity(){}

	// [IEntity]
	void								Draw(sf::RenderWindow & renderWindow)
	{
		//sf::Sprite & sprite = *(this->_animations.at(this->_currentStatus).Get());
		sf::Sprite & sprite = *(this->_animations.at(this->_currentStatus).GetCurrent());
		sprite.setPosition(_position.first, _position.second);
		sprite.setRotation(_rotation);
		sprite.setColor(_color);
		renderWindow.draw(sprite);
	}
	bool								Behave(void)
	{
		// [Move]
		if (this->_movement.IsActive())
			this->_movement.Apply(this->_position, this->_speed);
		// [IA]
		++(this->_animations.at(this->_currentStatus));
		return this->_behavior.at(this->_currentStatus)(*this);
	}
	// [Status]
	inline const Status					GetCurrentStatus(void) const
	{
		return this->_currentStatus;
	}
	inline void							ForceCurrentStatus(const Status status)
	{
		this->_currentStatus = status;
	}
	// [Basics]
	inline void							SetRotation(const float value)
	{
		this->_rotation = value;
	}
	inline void							SetColor(const sf::Color & value)
	{
		this->_color = value;
	}
	inline const sf::Color &			GetColor(void) const
	{
		return this->_color;
	}
	inline void							SetMovement(const PositionType & target)
	{
		this->_movement.Set(target);
	}

	inline const float &				GetRotation(void) const
	{
		return this->_rotation;
	}

protected:
	Entity(const ThisType &)		= delete;
	Entity(const ThisType &&)		= delete;
	Entity() = delete;

	//std::queue<Status>					_pendingStatus;	// [Todo]::[?]

	Status								_currentStatus;
	Behavior							_behavior;
	Animation							_animations;
	float								_speed;
	MovementType						_movement;
	float								_rotation;
	sf::Color							_color = sf::Color(100,100,100,255);
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

#endif // __GGE_ENTITY__