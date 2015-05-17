#ifndef __COLLISION_ENGINE__
# define __COLLISION_ENGINE__

// [Todo] : Tuto prog C pour nicolas

// [Todo] : CollisionNotification
// [Todo] : OnCollisionPolicy -> std::unordered_map<CollisionCode or type2Int, std::function> + NeedDelete ?

# include "IEntity.h"
# include <queue>

struct ICollisionEngine
{
	// [Register entities]
	virtual ICollisionEngine &	operator+=(std::vector<HitBox*> &) = 0;
	virtual ICollisionEngine &	operator+=(IEntity &) = 0;
	// [Basis]
	virtual void				Calculate(void) = 0;
	virtual void				Calculate(HitBox &) = 0;
};

struct QuadTreeCollisionEngine : public ICollisionEngine
{
	explicit QuadTreeCollisionEngine(const HitBox & screen)
	{
	}
	QuadTreeCollisionEngine(const QuadTreeCollisionEngine &) = delete;
	QuadTreeCollisionEngine(const QuadTreeCollisionEngine &&) = delete;
	QuadTreeCollisionEngine & operator=(const QuadTreeCollisionEngine &) = delete;
	QuadTreeCollisionEngine & operator=(const QuadTreeCollisionEngine &&) = delete;
	~QuadTreeCollisionEngine() = default;


};

#endif // __COLLISION_ENGINE__