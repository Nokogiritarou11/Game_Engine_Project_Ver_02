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
	enum class Condition_Type
	{
		Int,
		Float,
		Bool,
		Trigger
	};

	union Controller_Parameter_Value
	{
		int value_int;
		float value_float;
		bool value_bool;
	};

	struct Controller_Parameter
	{
		Controller_Parameter_Value value;
		Condition_Type type;
	private:
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(value, type);
		}
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

	struct Condition //ëJà⁄èåè
	{
		std::string key;
		Condition_Type type;
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
}