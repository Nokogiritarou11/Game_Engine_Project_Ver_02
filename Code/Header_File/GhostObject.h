#pragma once
#include "BulletCollision/CollisionDispatch/btGhostObject.h"
#include "LinearMath/btTransform.h"
#include <memory>

namespace BeastEngine
{
	class Transform;
	class Collider;

	class GhostObject
	{
	public:
		~GhostObject();

	private:
		void Initialize(std::shared_ptr<Collider> col);
		void Create();
		void Resize() const;
		void Remove();
		void Get_BtTransform(btTransform& t) const;
		void Set_BtTransform(const btTransform& t) const;
		void Set_Debug_Draw(bool value) const;
		bool Get_Debug_Drawed() const;

		std::unique_ptr<btGhostObject> ghost{};
		std::weak_ptr<Collider> collider;

		friend class Collider;
	};
}