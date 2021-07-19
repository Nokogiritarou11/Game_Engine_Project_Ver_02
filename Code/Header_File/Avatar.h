#pragma once
#include <unordered_map>
#include <string>
#include "Object.h"

namespace BeastEngine
{
	enum class Humanoid_Rig
	{
		None,                   //����
		Hips,                   //�K
		LeftUpperLeg,           //������_��
		RightUpperLeg,			//������_�E
		LeftLowerLeg,			//�ӂ���͂�_��
		RightLowerLeg,			//�ӂ���͂�_�E
		LeftFoot,				//��_��
		RightFoot,				//��_�E
		Spine,					//��
		Chest,					//�w��(����)
		UpperChest,				//�w��(���b��)
		Neck,					//��
		Head,					//��
		LeftShoulder,			//��_��
		RightShoulder,			//��_�E
		LeftUpperArm,			//��r_��
		RightUpperArm,			//��r_�E
		LeftLowerArm,			//�O�r_��
		RightLowerArm,			//�O�r_�E
		LeftHand,				//���_��
		RightHand,				//���_�E
		LeftToes,				//�ܐ�_��
		RightToes,				//�ܐ�_�E
		LeftEye,				//��_��
		RightEye,				//��_�E
		Jaw,					//�{
		LeftThumbProximal,		//�e�w_��3_��
		LeftThumbIntermediate,	//�e�w_��2_��
		LeftThumbDistal,		//�e�w_��1_��
		LeftIndexProximal,		//�l�����w_��3_��
		LeftIndexIntermediate,	//�l�����w_��2_��
		LeftIndexDistal,		//�l�����w_��1_��
		LeftMiddleProximal,		//���w_��3_��
		LeftMiddleIntermediate,	//���w_��2_��
		LeftMiddleDistal,		//���w_��1_��
		LeftRingProximal,		//��w_��3_��
		LeftRingIntermediate,	//��w_��2_��
		LeftRingDistal,			//��w_��1_��
		LeftLittleProximal,		//���w_��3_��
		LeftLittleIntermediate,	//���w_��2_��
		LeftLittleDistal,		//���w_��1_��
		RightThumbProximal,		//�e�w_��3_�E
		RightThumbIntermediate,	//�e�w_��2_�E
		RightThumbDistal,		//�e�w_��1_�E
		RightIndexProximal,		//�l�����w_��3_�E
		RightIndexIntermediate,	//�l�����w_��2_�E
		RightIndexDistal,		//�l�����w_��1_�E
		RightMiddleProximal,	//���w_��3_�E
		RightMiddleIntermediate,//���w_��2_�E
		RightMiddleDistal,		//���w_��1_�E
		RightRingProximal,		//��w_��3_�E
		RightRingIntermediate,	//��w_��2_�E
		RightRingDistal,		//��w_��1_�E
		RightLittleProximal,	//���w_��3_�E
		RightLittleIntermediate,//���w_��2_�E
		RightLittleDistal,      //���w_��1_�E
	};

	class Avatar : public Object
	{
	public:
		std::unordered_map<BeastEngine::Humanoid_Rig, std::string> Humanoid_Data;

		static std::shared_ptr<Avatar> Load_Avatar(std::string fullpath);

	private:

		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(cereal::base_class<BeastEngine::Object>(this), Humanoid_Data);
		}
	};
}

CEREAL_REGISTER_TYPE(BeastEngine::Avatar)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::Object, BeastEngine::Avatar)