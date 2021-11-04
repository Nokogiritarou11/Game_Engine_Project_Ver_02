#pragma once
#include <string>

#include "cereal/cereal.hpp"
#include "cereal/access.hpp"
#include "cereal/types/array.hpp"

namespace BeastEngine
{
	//�V�[�����ݒ�
	struct Project_Settings
	{
		float shadow_bias = 0.01f;         //�V�F�[�_�[���Ŏg�p����V���h�E�o�C�A�X
		float shadow_distance = 15.0f;     //�J�������猩�ĉe��`�悷�鋗��
		std::vector<std::string> tag{};    //�Q�[���I�u�W�F�N�g�ɐݒ�ł���^�O
		std::array<std::string, 32> layer; //�Փ˔���ȂǂɎg�p�ł��郌�C���[
		std::array<int, 32> layer_mask;    //���C���[���Ƃ̏Փ˔���ݒ�

	private:
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(shadow_bias, shadow_distance, tag, layer, layer_mask);
		}

	};
}

CEREAL_CLASS_VERSION(BeastEngine::Project_Settings, 1)