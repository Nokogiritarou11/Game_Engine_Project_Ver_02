#pragma once
#include "Behaviour.h"
#include "Animator_Controller.h"

namespace BeastEngine
{
	class GameObject;
	class Transform;
	class Animator_Manager;

	class Animator : public BeastEngine::Behaviour
	{
	public:

		void  Play();  //çƒê∂
		void  Stop();  //í‚é~
		void  Pause(); //àÍéûí‚é~

		void Set_Int(std::string key, int value);
		void Set_Float(std::string key, float value);
		void Set_Bool(std::string key, bool value);
		void Set_Trigger(std::string key);

		int Get_Int(std::string key);
		float Get_Float(std::string key);
		bool Get_Bool(std::string key);
		void Reset_Trigger(std::string key);

		void Set_Parameter_Int(std::string key, int value);
		void Set_Parameter_Float(std::string key, float value);
		void Set_Parameter_Bool(std::string key, bool value);
		int Get_Parameter_Int(std::string key);
		float Get_Parameter_Float(std::string key);
		bool Get_Parameter_Bool(std::string key);

		std::shared_ptr<BeastEngine::Animator_Controller> controller;

	private:
		struct Animation_Target
		{
			std::weak_ptr<BeastEngine::Transform> target;
			BeastEngine::Vector3 position;
			BeastEngine::Quaternion rotation;
			BeastEngine::Vector3 scale;
		};

		void Initialize(std::shared_ptr<BeastEngine::GameObject> obj) override;
		bool Draw_ImGui() override;
		bool Can_Multiple() override { return false; };

		void Set_Default_Pose();
		void Add_Parameter(std::string& p_name, BeastEngine::Parameter_Type type);
		void Judge_Event(Animation_Event& eve);
		void Update(); //çXêV

		bool playing = true;

		std::string controller_path;

		std::unordered_map<std::string, Animation_Parameter> parameters;
		std::unordered_map<std::string, Animation_Target> animation_data;
		std::unordered_map<std::string, Animation_Target> pose_default;
		std::unordered_map<std::string, Animation_Target> pose_playing;
		std::unordered_map<std::string, Animation_Target> pose_next;
		std::unordered_map<std::string, Animation_Target> pose_interrupt;

		friend class BeastEngine::Animator_Manager;
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(cereal::base_class<BeastEngine::Behaviour>(this), controller_path, parameters);
		}
	};
}

REGISTER_COMPONENT(Animator)
CEREAL_REGISTER_TYPE(BeastEngine::Animator)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::Behaviour, BeastEngine::Animator)