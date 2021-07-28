#pragma once
#include "btBulletDynamicsCommon.h"
#include "BulletCollision/CollisionDispatch/btGhostObject.h"
#include "LinearMath/btVector3.h"
#include "LinearMath/btQuaternion.h"
#include "LinearMath/btTransform.h"
#include "cereal/cereal.hpp"
#include "cereal/access.hpp"
#include "cereal/archives/binary.hpp"
#include <memory>
#include "Original_Math.h"

namespace BeastEngine
{
	class Transform;
	class Collider;
	class Box_Collider;

	class GhostObject
	{
	public:
		~GhostObject();

	private:
		void Initialize(std::shared_ptr<BeastEngine::Collider> col);
		void Create();
		void Resize();
		void Remove();
		void Get_btTransform(btTransform& t);
		void Set_btTransform(btTransform& t);
		void Draw_ImGui();

		std::unique_ptr<btGhostObject> ghost;
		std::weak_ptr<BeastEngine::Collider> collider;

		friend class BeastEngine::Collider;
		friend class BeastEngine::Box_Collider;
	};
}