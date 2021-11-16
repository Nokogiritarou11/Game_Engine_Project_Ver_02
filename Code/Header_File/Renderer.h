#pragma once
#include "DxSystem.h"
#include "Component.h"
#include <DirectXMath.h>
#include "Original_Math.h"
#include "Bounds.h"
#include <string>
#include <wrl.h>
#include <memory>
#include <vector>

namespace BeastEngine
{
	class Material;

	//�����_�����O�����R���|�[�l���g�̊��N���X
	class Renderer : public Component
	{
	public:
		void Set_Enabled(bool value);                               //�\�����邩
		[[nodiscard]] bool Get_Enabled() const { return enabled; };	//���݃A�N�e�B�u��

		std::vector<std::shared_ptr<Material>> material; //�g�p����}�e���A��
		std::vector<std::string> material_paths; //�}�e���A���̃p�X
		Bounds bounds; //�o�E���f�B���O�{�b�N�X

	protected:
		bool Draw_ImGui_Header(const std::string& component_name, bool& open); //ImGui�`�掞���ʃw�b�_�[�`��

		bool is_called = false;   //���Ƀ}�l�[�W���[�ɓo�^����Ă��邩
		bool enabled = true;      //�`�悷�邩
		bool enabled_old = false; //�ύX���g���K�[�p�L���b�V��
		bool can_render = false;  //�����_�����O�\��(�f�[�^�������Ă��邩)

		int subset_count = 0; //�T�u�Z�b�g�̐�
		std::vector<int> subset_material_index; //�T�u�Z�b�g�̃}�e���A���C���f�b�N�X
		Matrix world_old; //���[���h�s��̑O�t���[���L���b�V��

	private:
		virtual void Render(const int subset_number = 0) {}; //�`����s
		virtual void Render_Shadow(const int subset_number = 0) {}; //�V���h�E�}�b�v�`����s
		virtual void Recalculate_Frame() {}; //���_�f�[�^���Čv�Z����

		friend class Render_Manager;
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<Component>(this), enabled, bounds, material_paths);
		}
	};
}

CEREAL_REGISTER_TYPE(BeastEngine::Renderer)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::Component, BeastEngine::Renderer)
CEREAL_CLASS_VERSION(BeastEngine::Renderer, 1)