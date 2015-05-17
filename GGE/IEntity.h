#ifndef __IENTITY__
# define __IENTITY__

# include <map>
# include <SFML/Graphics/RenderWindow.hpp>

struct IEntity
{
	virtual void							Draw(sf::RenderWindow & renderWindow) = 0;
	virtual bool							Behave(void) = 0;
};

//
// [Todo] : OnCollisionPolicy
//
struct HitBox
{
	using PositionType = std::pair < float, float >;
	using SizeType = std::pair < int, int >;

	explicit HitBox(const PositionType & pos, const SizeType & size)
		: _position(pos)
		, _size(size)
	{}
	HitBox() = delete;
	HitBox(const HitBox & hb)
		: _position(hb._position)
		, _size(hb._size)
	{}
	HitBox(const HitBox && hb) = delete;
	~HitBox() = default;

	HitBox &	operator=(const HitBox & hb)
	{
		this->_position = hb.GetPosition();
		this->_size = hb.GetSize();
	}

	inline const PositionType &			GetPosition(void) const
	{
		return this->_position;
	}
	inline const SizeType &				GetSize(void) const
	{
		return this->_size;
	}
	inline void							SetPosition(const PositionType & value)
	{
		this->_position = value;
	}
	inline void							SetSize(const SizeType & value)
	{
		this->_size = value;
	}

protected:
	PositionType						_position;
	SizeType							_size;
};

#endif // __IENTITY__