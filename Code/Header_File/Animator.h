#pragma once
#include "Behaviour.h"
#include "Animator_Controller.h"

namespace BeastEngine
{
	class GameObject;
	class Transform;
	class Animator_Manager;

	class Animator final : public Behaviour
	{
	public:

		void Play();  //çƒê∂
		void Stop();  //í‚é~
		void Pause(); //àÍéûí‚é~

		void Set_Int(const std::string& key, const int& value) const;
		void Set_Float(const std::string& key, const float& value) const;
		void Set_Bool(const std::string& key, const bool& value) const;
		void Set_Trigger(const std::string& key) const;

		[[nodiscard]] int Get_Int(const std::string& key) const;
		[[nodiscard]] float Get_Float(const std::string& key) const;
		[[nodiscard]] bool Get_Bool(const std::string& key) const;
		void Reset_Trigger(const std::string& key) const;

		std::shared_ptr<Animator_Controller> controller;

	private:
		struct Animation_Target
		{
			std::weak_ptr<Transform> target;
			Vector3 position;
			Quaternion rotation;
			Vector3 scale;
		};

		void Initialize(const std::shared_ptr<GameObject>& obj) override;
		bool Draw_ImGui() override;
		bool Can_Multiple() override { return false; };

		void Set_Default_Pose();
		void Update(); //çXêV

		bool playing = true;

		std::string controller_path;

		std::unordered_map<std::string, Animation_Target> animation_data;
		std::unordered_map<std::string, Animation_Target> pose_default;
		std::unordered_map<std::string, Animation_Target> pose_playing;
		std::unordered_map<std::string, Animation_Target> pose_next;
		std::unordered_map<std::string, Animation_Target> pose_interrupt;

		friend class Animator_Manager;
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<Behaviour>(this), controller_path);
		}
	};
}

REGISTER_COMPONENT(Animator)
CEREAL_REGISTER_TYPE(BeastEngine::Animator)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::Behaviour, BeastEngine::Animator)
CEREAL_CLASS_VERSION(BeastEngine::Animator, 1)