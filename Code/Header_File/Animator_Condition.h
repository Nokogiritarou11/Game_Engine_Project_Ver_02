#pragma once
#include <string>
#include "cereal/cereal.hpp"
#include "cereal/access.hpp"
#include "cereal/archives/binary.hpp"
#include "cereal/types/vector.hpp"
#include "cereal/types/unordered_map.hpp"
#include "cereal/types/string.hpp"
#include "cereal/types/utility.hpp"
#include "cereal/types/polymorphic.hpp"
#include "cereal/types/base_class.hpp"
#include "cereal/types/memory.hpp"

namespace BeastEngine
{
	enum class Parameter_Type
	{
		Int,
		Float,
		Bool,
		Trigger
	};

	enum class Condition_Mode
	{
		If,
		IfNot,
		Greater,
		Less,
		Equals,
		NotEquals
	};

	struct Animation_Parameter
	{
		int value_int;
		float value_float;
		bool value_bool;
		Parameter_Type type;
	private:
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(value_int, value_float, value_bool, type);
		}
	};

	struct Condition //ëJà⁄èåè
	{
		std::string key;
		Parameter_Type type;
		Condition_Mode mode;
		float threshold;

	private:
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(key, type, mode, threshold);
		}
	};

	struct Animation_Event
	{
		std::string key;
		bool trigger = false;
		bool called = false;
		float time;
		Animation_Parameter parameter;

	private:
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(key, time, parameter);
		}
	};
}