#pragma once
#include "Behaviour.h"
#include "Animator_Controller.h"

namespace BeastEngine
{
	class GameObject;
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
		void Reset_Trigger(std::string key);

		std::shared_ptr<BeastEngine::Animator_Controller> controller;

	private:
		void Initialize(std::shared_ptr<BeastEngine::GameObject> obj) override;
		bool Draw_ImGui() override;
		bool Can_Multiple() override { return false; };

		void Update(); //çXêV

		bool playing = true;

		std::string controller_path;

		friend class BeastEngine::Animator_Manager;
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(cereal::base_class<BeastEngine::Behaviour>(this), controller_path);
		}
	};
}

CEREAL_REGISTER_TYPE(BeastEngine::Animator)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::Behaviour, BeastEngine::Animator)