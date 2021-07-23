#include "Collider_Manager.h"
#include "Box_Collider.h"
#include "GameObject.h"
#include "Transform.h"

using namespace std;
using namespace BeastEngine;

void Collider_Manager::Add(shared_ptr<Box_Collider>& collider)
{
	box_list.emplace_back(collider);
}

void Collider_Manager::Update()
{
	shared_ptr<Box_Collider> box;
	bool expired = false;
	for (auto& b : box_list)
	{
		if (!b.expired())
		{
			box = b.lock();
			if (box->gameobject->Get_Active_In_Hierarchy())
			{
			}
		}
		else
		{
			expired = true;
		}
	}
	if (expired)
	{
		auto removeIt = remove_if(box_list.begin(), box_list.end(), [](weak_ptr<Box_Collider> b) { return b.expired(); });
		box_list.erase(removeIt, box_list.end());
	}
}

// 分離軸に投影された軸成分から投影線分長を算出
float LenSegOnSeparateAxis(Vector3& Sep, Vector3& e1, Vector3& e2)
{
	// 3つの内積の絶対値の和で投影線分長を計算
	// 分離軸Sepは標準化されていること
	float r1 = fabs(Sep.Dot(e1));
	float r2 = fabs(Sep.Dot(e2));
	return r1 + r2;
}
float LenSegOnSeparateAxis(Vector3& Sep, Vector3& e1, Vector3& e2, Vector3& e3)
{
	float r1 = fabs(Sep.Dot(e1));
	float r2 = fabs(Sep.Dot(e2));
	float r3 = fabs(Sep.Dot(e3));
	return r1 + r2 + r3;
}


bool Collider_Manager::Judge(std::shared_ptr<BeastEngine::Box_Collider>& box_1, std::shared_ptr<BeastEngine::Box_Collider>& box_2)
{
	// 各方向ベクトルの確保
   // （N***:標準化方向ベクトル）
	Vector3 size_1 = box_1->transform->Get_Scale(), size_2 = box_2->transform->Get_Scale();
	Vector3 NAe1 = box_1->transform->Get_Right(), Ae1 = NAe1 * size_1.x;
	Vector3 NAe2 = box_1->transform->Get_Up(), Ae2 = NAe2 * size_1.y;
	Vector3 NAe3 = box_1->transform->Get_Forward(), Ae3 = NAe3 * size_1.z;
	Vector3 NBe1 = box_2->transform->Get_Right(), Be1 = NBe1 * size_2.x;
	Vector3 NBe2 = box_2->transform->Get_Up(), Be2 = NBe2 * size_2.y;
	Vector3 NBe3 = box_2->transform->Get_Forward(), Be3 = NBe3 * size_2.z;
	Vector3 Interval = box_1->transform->Get_Position() - box_2->transform->Get_Position();

	// 分離軸 : Ae1
	float rA = Ae1.Length();
	float rB = LenSegOnSeparateAxis(NAe1, Be1, Be2, Be3);
	float L = fabs(Interval.Dot(NAe1));
	if (L > rA + rB) return false;

	// 分離軸 : Ae2
	rA = Ae2.Length();
	rB = LenSegOnSeparateAxis(NAe2, Be1, Be2, Be3);
	L = fabs(Interval.Dot(NAe2));
	if (L > rA + rB) return false;

	// 分離軸 : Ae3
	rA = Ae3.Length();
	rB = LenSegOnSeparateAxis(NAe3, Be1, Be2, Be3);
	L = fabs(Interval.Dot(NAe3));
	if (L > rA + rB) return false;

	// 分離軸 : Be1
	rA = LenSegOnSeparateAxis(NBe1, Ae1, Ae2, Ae3);
	rB = Be1.Length();
	L = fabs(Interval.Dot(NBe1));
	if (L > rA + rB) return false;

	// 分離軸 : Be2
	rA = LenSegOnSeparateAxis(NBe2, Ae1, Ae2, Ae3);
	rB = Be2.Length();
	L = fabs(Interval.Dot(NBe2));
	if (L > rA + rB) return false;

	// 分離軸 : Be3
	rA = LenSegOnSeparateAxis(NBe3, Ae1, Ae2, Ae3);
	rB = Be3.Length();
	L = fabs(Interval.Dot(NBe3));
	if (L > rA + rB) return false;

	// 分離軸 : C11
	Vector3 Cross;
	NAe1.Cross(NBe1, Cross);
	rA = LenSegOnSeparateAxis(Cross, Ae2, Ae3);
	rB = LenSegOnSeparateAxis(Cross, Be2, Be3);
	L = fabs(Interval.Dot(Cross));
	if (L > rA + rB) return false;

	// 分離軸 : C12
	NAe1.Cross(NBe2, Cross);
	rA = LenSegOnSeparateAxis(Cross, Ae2, Ae3);
	rB = LenSegOnSeparateAxis(Cross, Be1, Be3);
	L = fabs(Interval.Dot(Cross));
	if (L > rA + rB) return false;

	// 分離軸 : C13
	NAe1.Cross(NBe3, Cross);
	rA = LenSegOnSeparateAxis(Cross, Ae2, Ae3);
	rB = LenSegOnSeparateAxis(Cross, Be1, Be2);
	L = fabs(Interval.Dot(Cross));
	if (L > rA + rB) return false;

	// 分離軸 : C21
	NAe2.Cross(NBe1, Cross);
	rA = LenSegOnSeparateAxis(Cross, Ae1, Ae3);
	rB = LenSegOnSeparateAxis(Cross, Be2, Be3);
	L = fabs(Interval.Dot(Cross));
	if (L > rA + rB) return false;

	// 分離軸 : C22
	NAe2.Cross(NBe2, Cross);
	rA = LenSegOnSeparateAxis(Cross, Ae1, Ae3);
	rB = LenSegOnSeparateAxis(Cross, Be1, Be3);
	L = fabs(Interval.Dot(Cross));
	if (L > rA + rB) return false;

	// 分離軸 : C23
	NAe2.Cross(NBe3, Cross);
	rA = LenSegOnSeparateAxis(Cross, Ae1, Ae3);
	rB = LenSegOnSeparateAxis(Cross, Be1, Be2);
	L = fabs(Interval.Dot(Cross));
	if (L > rA + rB) return false;

	// 分離軸 : C31
	NAe3.Cross(NBe1, Cross);
	rA = LenSegOnSeparateAxis(Cross, Ae1, Ae2);
	rB = LenSegOnSeparateAxis(Cross, Be2, Be3);
	L = fabs(Interval.Dot(Cross));
	if (L > rA + rB) return false;

	// 分離軸 : C32
	NAe3.Cross(NBe2, Cross);
	rA = LenSegOnSeparateAxis(Cross, Ae1, Ae2);
	rB = LenSegOnSeparateAxis(Cross, Be1, Be3);
	L = fabs(Interval.Dot(Cross));
	if (L > rA + rB) return false;

	// 分離軸 : C33
	NAe3.Cross(NBe3, Cross);
	rA = LenSegOnSeparateAxis(Cross, Ae1, Ae2);
	rB = LenSegOnSeparateAxis(Cross, Be1, Be2);
	L = fabs(Interval.Dot(Cross));
	if (L > rA + rB) return false;

	// 分離平面が存在しないので「衝突している」
	return true;
}