#pragma once
#include <unordered_map>
#include <string>
#include "Object.h"

namespace BeastEngine
{
	enum class Humanoid_Rig
	{
		None,                   //無し
		Hips,                   //尻
		LeftUpperLeg,           //太もも_左
		RightUpperLeg,			//太もも_右
		LeftLowerLeg,			//ふくらはぎ_左
		RightLowerLeg,			//ふくらはぎ_右
		LeftFoot,				//足_左
		RightFoot,				//足_右
		Spine,					//腰
		Chest,					//背骨(腹部)
		UpperChest,				//背骨(肩甲骨)
		Neck,					//首
		Head,					//頭
		LeftShoulder,			//肩_左
		RightShoulder,			//肩_右
		LeftUpperArm,			//上腕_左
		RightUpperArm,			//上腕_右
		LeftLowerArm,			//前腕_左
		RightLowerArm,			//前腕_右
		LeftHand,				//手首_左
		RightHand,				//手首_右
		LeftToes,				//つま先_左
		RightToes,				//つま先_右
		LeftEye,				//目_左
		RightEye,				//目_右
		Jaw,					//顎
		LeftThumbProximal,		//親指_第3_左
		LeftThumbIntermediate,	//親指_第2_左
		LeftThumbDistal,		//親指_第1_左
		LeftIndexProximal,		//人差し指_第3_左
		LeftIndexIntermediate,	//人差し指_第2_左
		LeftIndexDistal,		//人差し指_第1_左
		LeftMiddleProximal,		//中指_第3_左
		LeftMiddleIntermediate,	//中指_第2_左
		LeftMiddleDistal,		//中指_第1_左
		LeftRingProximal,		//薬指_第3_左
		LeftRingIntermediate,	//薬指_第2_左
		LeftRingDistal,			//薬指_第1_左
		LeftLittleProximal,		//小指_第3_左
		LeftLittleIntermediate,	//小指_第2_左
		LeftLittleDistal,		//小指_第1_左
		RightThumbProximal,		//親指_第3_右
		RightThumbIntermediate,	//親指_第2_右
		RightThumbDistal,		//親指_第1_右
		RightIndexProximal,		//人差し指_第3_右
		RightIndexIntermediate,	//人差し指_第2_右
		RightIndexDistal,		//人差し指_第1_右
		RightMiddleProximal,	//中指_第3_右
		RightMiddleIntermediate,//中指_第2_右
		RightMiddleDistal,		//中指_第1_右
		RightRingProximal,		//薬指_第3_右
		RightRingIntermediate,	//薬指_第2_右
		RightRingDistal,		//薬指_第1_右
		RightLittleProximal,	//小指_第3_右
		RightLittleIntermediate,//小指_第2_右
		RightLittleDistal,      //小指_第1_右
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