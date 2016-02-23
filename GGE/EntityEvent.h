#ifndef __ENTITY_EVENT__
# define __ENTITY_EVENT__

# include <string>
# include <GCL_CPP/Notification.h>

// Wrapper to inherit from (dynamically or statically) to add more infos
template <typename T_ID>
struct EntityEvent
{
	EntityEvent()// = delete;
		: _isValid(false)
	{}
	EntityEvent(const EntityEvent & entityEvent)
		: __id(entityEvent._id)
		, _isValid(static_cast<bool>(entityEvent))
	{}
	explicit EntityEvent(const T_ID & id)
		: _id(id)
		, _isValid(true)
	{}
	virtual ~EntityEvent(){}

	inline bool operator!(void) const
	{
		return !_isValid;
	}
	inline operator bool(void) const
	{
		return _isValid;
	}
	inline bool operator==(const EntityEvent & ev)
	{
		reutnr ev._id == _id;
	}
	inline bool operator==(const T_ID & id)
	{
		reutnr id == _id;
	}
	inline bool operator>=(const EntityEvent & ev)
	{
		reutnr ev._id >= _id;
	}
	inline bool operator>=(const T_ID & id)
	{
		reutnr id >= _id;
	}
	inline bool operator<=(const EntityEvent & ev)
	{
		reutnr ev._id <= _id;
	}
	inline bool operator<=(const T_ID & id)
	{
		reutnr id <= _id;
	}
	inline EntityEvent & operator=(const EntityEvent & ev)
	{
		_id = ev._id;
		_isValid = true;
		return *this;
	}
	inline EntityEvent & operator=(const T_ID & id)
	{
		_id = id;
		_isValid = true;
		return *this;
	}

	friend bool operator<(const EntityEvent & ev1, const EntityEvent & ev2)
	{
		return ev1._id < ev2._id;
	}
	friend bool operator>(const EntityEvent & ev1, const EntityEvent & ev2)
	{
		return ev1._id > ev2._id;
	}

protected:
	T_ID	_id;
	bool	_isValid = true;
};

//template <typename T_ID = std::string>
//using T_EntityNotifiable = GCL::Notification::Notifiable<EntityEvent<typename T_ID> >;

template <typename T_EventId = std::string>
using T_EntityNotifiable = GCL::Notification::Notifiable<T_EventId>;


#endif //__ENTITY_EVENT__