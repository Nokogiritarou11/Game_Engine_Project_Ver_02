#pragma once
#include <algorithm>
#include <stdlib.h>
#include <functional>
//https://takachan.hatenablog.com/entry/2018/07/28/215502
// プロパティ構文を支援するクラス
template<class T> class Property
{
public:
	T& r;
	std::function<void(T value)> set = nullptr; // ★追加
	std::function<T()> get = nullptr; // ★追加

	operator T()
	{
		// getが設定されてれば登録されたほうを呼ぶ
		return get ? this->get() : this->r;
	}

	// 直接中身を参照できるようにアロー演算子もオーバーロード
	T operator ->()
	{
		return get ? this->get() : this->r;
	}

	void operator =(const T v)
	{
		if (set) // setが設定されていれば登録されたほうを呼ぶ
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
	// 何も設定しない
Property<int> a{ _a };

// setだけ指定
Property<int> b{ _b, [this](int v) { _b = v * 2; 倍で保存 }, nullptr };

// getだけ指定
Property<int> c{ _c, nullptr, [this]() { return _c * 3;  倍で取得 } };

// オブジェクトへの参照
Property<Sample*> sample{ _sample };
*/