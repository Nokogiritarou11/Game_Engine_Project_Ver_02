#pragma once
#include "MonoBehaviour.h"

//コピペ用MonoBehaviourテンプレート
/*
namespace BeastEngine
{
	//他自作コンポーネント使用時の前方宣言



	class Hoge : public MonoBehaviour
	{
	public:
		void Awake();      //初めてアクティブになった時に一回だけ呼ばれる(Enableに依存し、ゲームオブジェクトのActiveを無視する)
		void Start();      //初めてアクティブになった時に一回だけ呼ばれる(ゲームオブジェクトのActiveと自身のEnableに依存する)
		void Update();     //アクティブ時に毎フレーム呼ばれる
		void LateUpdate(); //アクティブ時に毎フレーム、Updateのあとに呼ばれる

		void OnEnable();  //アクティブになった時に呼ばれる
		void OnDisable(); //非アクティブになった時に呼ばれる
		void OnDestroy(); //削除時に呼ばれる

		void OnTrigger_Enter(BeastEngine::Collision& collision);   //このオブジェクトにアタッチされたコライダーが他コライダーに接触したとき
		void OnCollision_Enter(BeastEngine::Collision& collision); //非トリガー時

		void OnTrigger_Stay(BeastEngine::Collision& collision);    //接触がにフレーム以上続いたとき(接触が維持される限り毎フレーム)
		void OnCollision_Stay(BeastEngine::Collision& collision);  //非トリガー時

		void OnTrigger_Exit(BeastEngine::Collision& collision);    //接触していたコライダーと離れたとき
		void OnCollision_Exit(BeastEngine::Collision& collision);  //非トリガー時

		bool Draw_ImGui() override;

	private:


		// シリアライズ関数
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			//シリアライズしたい変数を末尾に追加
			archive(cereal::base_class<BeastEngine::MonoBehaviour>(this));
		}
	};
}

//以下マクロのクラス名部分を書き換え
REGISTER_COMPONENT(Hoge)
CEREAL_REGISTER_TYPE(BeastEngine::Hoge)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::MonoBehaviour, BeastEngine::Hoge)
CEREAL_CLASS_VERSION(BeastEngine::Hoge, 1)

*/