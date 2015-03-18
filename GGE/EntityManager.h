#ifndef __ENTITY_MANAGER__
# define __ENTITY_MANAGER__

# include "Entity.h"

// [Todo] : Singleton
struct EntityManager
{
	static const size_t	RefreshRate = 15;

	explicit EntityManager(){}
	~EntityManager(){}
	EntityManager(const EntityManager &) = delete;
	EntityManager(const EntityManager &&) = delete;
	EntityManager & operator=(const EntityManager &) = delete;
	EntityManager & operator=(const EntityManager &&) = delete;

	void			Add(IEntity * entity)
	{
		*this += entity;
	}
	EntityManager &	operator+=(IEntity * entity)
	{
		this->_content.push_back(entity);
	}

	void			Draw(sf::RenderWindow & renderWindow)
	{
		std::for_each(this->_content.begin(), this->_content.end(), [&](IEntity * entity){ entity->Draw(renderWindow); });
	}
	bool			Behave(void)
	{
		bool bReturn(true);
		std::for_each(this->_content.begin(), this->_content.end(), [&, bReturn](IEntity * entity) mutable { if (!bReturn) return; bReturn = entity->Behave(); });
		return bReturn;
	}

	bool			TicksUp(void)
	{
		return (!(this->_ticksCounter = (this->_ticksCounter == RefreshRate ? 0 : this->_ticksCounter + 1)));
	}

protected:
	std::vector<IEntity *>	_content;
	size_t					_ticksCounter = 0;
};

#endif // __ENTITY_MANAGER__