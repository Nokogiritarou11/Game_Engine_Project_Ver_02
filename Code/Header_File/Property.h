#pragma once
#include <algorithm>
#include <stdlib.h>
#include <functional>
//https://takachan.hatenablog.com/entry/2018/07/28/215502
// �v���p�e�B�\�����x������N���X
template<class T> class Property
{
public:
	T& r;
	std::function<void(T value)> set = nullptr; // ���ǉ�
	std::function<T()> get = nullptr; // ���ǉ�

	operator T()
	{
		// get���ݒ肳��Ă�Γo�^���ꂽ�ق����Ă�
		return get ? this->get() : this->r;
	}

	// ���ڒ��g���Q�Ƃł���悤�ɃA���[���Z�q���I�[�o�[���[�h
	T operator ->()
	{
		return get ? this->get() : this->r;
	}

	void operator =(const T v)
	{
		if (set) // set���ݒ肳��Ă���Γo�^���ꂽ�ق����Ă�
		{
			this->set(v);
		}
		else
		{
			r = v;
		}
	}
};

/*//////////////////////////////////////////////////////////////////////////////////
	// �����ݒ肵�Ȃ�
Property<int> a{ _a };

// set�����w��
Property<int> b{ _b, [this](int v) { _b = v * 2; �{�ŕۑ� }, nullptr };

// get�����w��
Property<int> c{ _c, nullptr, [this]() { return _c * 3;  �{�Ŏ擾 } };

// �I�u�W�F�N�g�ւ̎Q��
Property<Sample*> sample{ _sample };
*/