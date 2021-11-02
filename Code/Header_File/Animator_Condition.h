#pragma once
#include <string>
#include "cereal/cereal.hpp"
#include "cereal/access.hpp"
#include "cereal/types/unordered_map.hpp"

namespace BeastEngine
{
	//�A�j���[�V�����p�����[�^�[�̌^
	enum class Parameter_Type
	{
		Int,
		Float,
		Bool,
		Trigger
	};

	//����^�C�v
	enum class Condition_Mode
	{
		If,
		IfNot,
		Greater,
		Less,
		Equals,
		NotEquals
	};

	//�X�e�[�g�C�x���g�^�C�v
	enum class State_Event_Type
	{
		Enter,
		Exit
	};

	//�A�j���[�V�����p�����[�^�[�p���ʍ\����
	struct Animation_Parameter
	{
		int value_int = 0;
		float value_float = 0;
		bool value_bool = false;
		Parameter_Type type;
	private:
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(value_int, value_float, value_bool, type);
		}
	};

	//�J�ڏ����f�[�^�p�\����
	struct Condition
	{
		std::string key;
		Parameter_Type type;
		Condition_Mode mode;
		float threshold = 0;

	private:
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(key, type, mode, threshold);
		}
	};

	//�A�j���[�V�����C�x���g�\����
	struct Animation_Event
	{
		std::string key;
		bool called = false;
		int frame = 0;
		Animation_Parameter parameter;

	private:
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(key, frame, parameter);
		}
	};

	//�X�e�[�g�C�x���g�\����
	struct State_Event
	{
		std::string key;
		State_Event_Type type;
		Animation_Parameter parameter;

	private:
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(key, type, parameter);
		}
	};
}

CEREAL_CLASS_VERSION(BeastEngine::Animation_Parameter, 1)
CEREAL_CLASS_VERSION(BeastEngine::Condition, 1)
CEREAL_CLASS_VERSION(BeastEngine::Animation_Event, 1)
CEREAL_CLASS_VERSION(BeastEngine::State_Event, 1)