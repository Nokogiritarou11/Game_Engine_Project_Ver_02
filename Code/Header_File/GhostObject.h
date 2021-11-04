#pragma once
#include "BulletCollision/CollisionDispatch/btGhostObject.h"
#include "LinearMath/btTransform.h"
#include <memory>

namespace BeastEngine
{
	class Transform;
	class Collider;

	//�S�[�X�g(�g���K�[)�R���C�_�[
	class GhostObject
	{
	public:
		~GhostObject();

	private:
		void Initialize(std::shared_ptr<Collider> col); //������
		void Create();       //�쐬
		void Resize() const; //���T�C�Y
		void Remove();       //�폜
		void Get_BtTransform(btTransform& t) const;       //Bullet���ł̍��W���擾����
		void Set_BtTransform(const btTransform& t) const; //Bullet���̍��W��ύX����
		void Set_Debug_Draw(bool value) const;            //�f�o�b�O�`��ɓo�^����
		[[nodiscard]] bool Get_Debug_Drawed() const;      //�f�o�b�O�`��ɓo�^����Ă��邩�擾����

		std::unique_ptr<btGhostObject> ghost{};
		std::weak_ptr<Collider> collider;

		friend class Collider;
	};
}