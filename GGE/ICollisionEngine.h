#ifndef __COLLISION_ENGINE__
# define __COLLISION_ENGINE__

# include "IEntity.h"
# include <queue>
# include <vector>
# include <GCL_CPP/Vector.h>
# include "EntityEvent.h"

namespace CollisionEngine
{
	namespace Algorythms
	{
		struct AABB final
		{
			static bool	IsCollision(const HitBox & a, const HitBox & b)
			{
				return (
					a.GetPosition().first < b.GetPosition().first + b.GetSize().first
					&& a.GetPosition().first + a.GetSize().first > b.GetPosition().first
					&& a.GetPosition().second < b.GetPosition().second + b.GetSize().second
					&& a.GetSize().second + a.GetPosition().second > b.GetPosition().second
					);
			}
		};
	}

	namespace Events
	{
		const T_EntityNotifiable<>::T_EventID CollisionEvent = "Collision detected";
	}

	struct Interface
	{
		// [Register entities]
		virtual Interface &			operator+=(std::vector<HitBox*> &) = 0;
		virtual Interface &			operator+=(HitBox &) = 0;
		virtual void				RemoveUnregistered(void) = 0;
		virtual void				Unload(void) = 0;
		// [Basis]
		virtual void				Calculate(void) = 0;
		virtual void				Calculate(HitBox &) = 0;
		// [Events]
		virtual void				ApplyOnCollisionEvents(void) = 0;
	};

	namespace Implem
	{
		template <typename T_CollisionAlgorythm>
		struct Linear : public Interface
		{
			virtual Interface &		operator+=(std::vector<HitBox*> & hbs)
			{
				this->_entities.insert(_entities.end(), hbs.begin(), hbs.end());
				return *this;
			}
			virtual Interface &		operator+=(HitBox & e)
			{
				this->_entities.push_back(&e);
				return *this;
			}
			void					RemoveUnregistered(void)
			{
				std::remove_if(_entities.begin(), _entities.end(), [=](HitBox * hb) -> bool { return hb->DoesRequierUnregisterFromCollisionEngine(); });
			}
			void					Unload(void)
			{
				_entities.clear();
			}

			virtual void			Calculate(void)
			{
				for (auto & elem : _entities)
					this->Calculate(*elem);
			}
			virtual void			Calculate(HitBox & hb)
			{
				for (auto & elem : _entities)
				{
					if (elem != &hb && T_CollisionAlgorythm::IsCollision(*elem, hb))
					{
						elem->NotifyCollision(hb);
						hb.NotifyCollision(elem);
					}
				}
			}
			__declspec(deprecated("deprecated function : To replace with T_EntityNotifiable"))
			void					ApplyOnCollisionEvents(void)
			{
				for (auto & elem : _entities)
					if (elem->HasCollisions())
					{
						elem->TriggerEvent(Events::CollisionEvent);
						elem->ClearCollisions();
					}
			}

		protected:
			std::vector<HitBox*>	_entities;
		};

		//	-----------
		//	| NW | NE |
		//	|---------|
		//	| SW | SE |
		//	-----------
		template <typename T_CollsionAlgorythm, int deepth>
		struct QuadTree : public Interface, public HitBox
		{
			using Node = QuadTree < T_CollsionAlgorythm, deepth - 1 >;
			using NodePtr = Node*;

			const bool	IsLast = (deepth == 0);

			QuadTree(const QuadTree &) = delete;
			QuadTree(const QuadTree &&) = delete;
			QuadTree & operator=(const QuadTree &) = delete;
			QuadTree & operator=(const QuadTree &&) = delete;
			~QuadTree() = default;

			explicit QuadTree(const HitBox & screen)
				: HitBox(screen)
				, _NW(0x0)
				, _NE(0x0)
				, _SW(0x0)
				, _SE(0x0)
			{
				if (deepth == 0)
					return;
				if (screen.GetSize().first % 2 != 0 || screen.GetSize().second % 2 != 0)
					throw std::logic_error("CollisionEngine::QuadTree : Size cannot be divided by 2");

				const HitBox::SizeType &	oldSize = screen.GetSize();
				HitBox::SizeType			newNodeSize(oldSize.first / 2, oldSize.second / 2);

				_NW = new QuadTree<deepth - 1>(HitBox(HitBox::PositionType(0, 0), newNodeSize));
				_NE = new QuadTree<deepth - 1>(HitBox(HitBox::PositionType(newNodeSize.first, 0), newNodeSize));
				_SW = new QuadTree<deepth - 1>(HitBox(HitBox::PositionType(0, newNodeSize.second), newNodeSize));
				_SE = new QuadTree<deepth - 1>(HitBox(HitBox::PositionType(newNodeSize.first, newNodeSize.second), newNodeSize));
			}

			// [Register entities]
			virtual Interface &	operator+=(std::vector<HitBox*> &) = 0;
			virtual Interface &	operator+=(IEntity &) = 0;

			virtual void				Calculate(void) = 0;
			virtual void				Calculate(HitBox &) = 0;


		protected:
			NodePtr	_NW, _NE, _SW, _SE;
		};
	}
}


#endif // __COLLISION_ENGINE__