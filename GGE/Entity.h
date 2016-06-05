#ifndef __GGE_ENTITY__
# define __GGE_ENTITY__

# include <map>
# include <functional>
# include <queue>
# include <chrono>
# include <math.h>
# include <set>

# include <SFML/Graphics.hpp>

# include <GCL_CPP/Exception.h>
# include <GCL_CPP/Maths.h>
# include <GCL_CPP/Vector.h>

# include "IEntity.h"
# include "Sprite.h"

# ifdef DEBUG_ENTITY_HITBOX
#  define DEBUG_ENTITY_HITBOX_INSTR(instr) instr
# else
#  define DEBUG_ENTITY_HITBOX_INSTR(instr)
# endif

namespace GGE
{
	template <typename T_EntityTrait>
	class Entity
		: public IEntity
		, public HitBox
		//, Garbageable< Entity< EntityDescriptor > >
	{
	public:
		using Trait = T_EntityTrait;
		using ThisType = typename Entity < T_EntityTrait >;

		using Status = typename T_EntityTrait::Status;
		using Behavior = typename T_EntityTrait::Behavior;
		using Animation = typename T_EntityTrait::Animation;

		//enum class DirectionType : char
		//{
		//	NORTH = 0x01
		//	, EAST = 0x02
		//	, WEST = 0x04
		//	, SOUTH = 0x08
		//	, NORTH_EAST = NORTH + EAST	// 0x03
		//	, NORTH_WEST = NORTH + WEST	// 0x05
		//	, SOUTH_EAST = SOUTH + EAST // 0x10
		//	, SOUTH_WEST = SOUTH + WEST // 0x12
		//};
		struct MovementType
		{
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
				_destination.first = destination.first < 0 ? 0 : destination.first;
				_destination.second = destination.second < 0 ? 0 : destination.second;

				_destination = destination;
				_modifiers.first = GCL::Maths::Cos(_destination.first);
				_modifiers.second = GCL::Maths::Sin(_destination.second);

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
					pos.first += dx * speed * delta;
					pos.second += dy * speed * delta;
					return true;
				}
				else
				{
					pos.first = _destination.first;
					pos.second = _destination.second;
					return false;
				}
			}

			std::pair<float, float>			_modifiers;		// x sin, y cos
			PositionType					_destination;
			bool							_isActive;
		};
		struct SpriteModifiers
		{
			explicit SpriteModifiers()
			{}

			void	Apply(sf::Sprite & sprite)
			{
				sprite.setRotation(_rotation);
				sprite.setColor(_color);
				sprite.setScale(_scale.first, _scale.second);
			}

			sf::Color								_color = sf::Color(100, 100, 100, 255);
			std::pair<float, float>					_scale = { 1.0f, 1.0f };
			float									_rotation = .0f;
		};

		Entity(const std::pair<float, float> & pos)
			: _currentStatus(T_EntityTrait::Default)
			, HitBox(pos, T_EntityTrait::_size)
			, _behavior(T_EntityTrait::_behavior)
			, _animations(T_EntityTrait::_animation)
			, _speed(15.0f)
		{}
		virtual ~Entity() {}

		// [IEntity]
		void									Draw(sf::RenderWindow & renderWindow) override
		{
			sf::Sprite & sprite = *(this->_animations.at(this->_currentStatus).GetCurrent());
			sprite.setPosition(_position.first + (_size.first / 2), _position.second + (_size.second / 2));
			_spriteModifier.Apply(sprite);
			renderWindow.draw(sprite);

			DEBUG_INSTRUCTION(DEBUG_ENTITY_HITBOX_INSTR(
				_debugBox.setSize(sf::Vector2f(static_cast<float>(_size.first), static_cast<float>(_size.second)));
			_debugBox.setOutlineColor(sf::Color::Red);
			_debugBox.setFillColor(sf::Color::Transparent);
			_debugBox.setOutlineThickness(5);
			_debugBox.setPosition(_position.first, _position.second);
			));
			renderWindow.draw(_debugBox);
		}
		bool									Behave(void) override
		{
			// [Move]
			if (this->_movement.IsActive())
				this->_movement.Apply(this->_position, this->_speed);
			// [IA]
			++(this->_animations.at(this->_currentStatus));
			return this->_behavior.at(this->_currentStatus)(*this);
		}
		// [Status]
		inline const Status						GetCurrentStatus(void) const
		{
			return this->_currentStatus;
		}
		inline void								ForceCurrentStatus(const Status status)
		{
			this->_currentStatus = status;
		}
		// [Basics]
		inline void								SetRotation(const float value)
		{
			this->_rotation = value;
		}
		inline void								SetMovement(const PositionType & target)
		{
			PositionType applicatedDestination = target;

			// X-move
			if (target.first < _position.first)							// non-inclusive left
				_spriteModifier._scale = { -1.0f, 1.0f };
			else if (target.first > _position.first + _size.first)		// non-inclusive right
			{
				_spriteModifier._scale = { 1.0f, 1.0f };
				applicatedDestination.first = target.first - _size.first;
			}
			else														// inclusive x-move
				applicatedDestination.first = _position.first;

			// Y-move
			if (target.second < _position.second)						// non-inclusive top
				;
			else if (target.second > _position.second + _size.second)	// non-inclusive bot
				applicatedDestination.second = target.second - _size.second;
			else														// inclusive y-move
				applicatedDestination.second = _position.second;

			this->_movement.Set(applicatedDestination);
		}
		inline void								SetAbsoluteMovement(const PositionType & target)	// Absolute movement from top-left corner
		{
			if (static_cast<float>(target.first) < _position.first)	// Moving to the left
				_spriteModifier._scale = { -1.0f, 1.0f };
			else if (target.first > _position.first + _size.first)	// Moving to the right
				_spriteModifier._scale = { 1.0f, 1.0f };
			this->_movement.Set(applicatedDestination);
		}
		inline const float &					GetRotation(void) const
		{
			return this->_rotation;
		}
		inline SpriteModifiers &				GetSpriteModifier(void)
		{
			return _spriteModifier;
		}
		inline const Animation &				GetAnimation(void) const
		{
			return this->_animations;
		}

	protected:
		Entity(const ThisType &) = delete;
		Entity(const ThisType &&) = delete;
		Entity() = delete;

		//std::queue<Status>					_pendingStatus;	// [Todo]::[?]
		Status									_currentStatus;
		Behavior								_behavior;
		Animation								_animations;

		float									_speed;
		MovementType							_movement;

		SpriteModifiers							_spriteModifier;

		DEBUG_INSTRUCTION(sf::RectangleShape	_debugBox;);
	};
}
//
//	[Todo] : Move to test
//	[Another file] : Sample of implementation
//

struct Sonic_EntityTrait
{
	friend GGE::Entity < Sonic_EntityTrait >;

	enum Status
	{
		Default
		, Standing
		, Walking
		, Running
		, Jumping
		, Destroying
	};

	using Behavior	= std::map < Status, std::function<bool(GGE::Entity<Sonic_EntityTrait>&)> >;	// [Todo] : Multimap + not const [?]
	using Animation	= std::map < Status, GGE::SPRITE::Serie >;

	// [Todo] : Fix GGE::SPRITE::Sheet's move constructor
	/*static const std::vector<const GGE::SPRITE::Sheet>	SheetsPack;*/

	static const GGE::SPRITE::Sheet			gSpriteSheet_walking;
	static const GGE::SPRITE::Sheet			gSpriteSheet_Destroying;

	static const Behavior					_behavior;
	static const Animation					_animation;

	static const std::pair<int, int>		_size;
};

using Sonic = GGE::Entity < Sonic_EntityTrait > ;

#endif // __GGE_ENTITY__