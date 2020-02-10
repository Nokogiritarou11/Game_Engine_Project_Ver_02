#pragma once
#include <stdio.h>
#include <string>
#include <DirectXMath.h>
#include <memory>

class Transform;
class GameObject;

class Object : public std::enable_shared_from_this<Object>
{
public:
	std::string name;

	Object();
	~Object();

	std::string ToString();
	static void Destroy(std::shared_ptr<GameObject> obj);
	static std::shared_ptr<GameObject> Instantiate(std::string name);
	static std::shared_ptr<GameObject> Instantiate(std::string name, std::shared_ptr<Transform> parent);
	static std::shared_ptr<GameObject> Instantiate(std::string name, std::shared_ptr<Transform> parent, bool instantiateInWorldSpace);
	static std::shared_ptr<GameObject> Instantiate(std::string name, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 euler);
	static std::shared_ptr<GameObject> Instantiate(std::string name, DirectX::XMFLOAT3 position, DirectX::XMFLOAT4 rotation);
	static std::shared_ptr<GameObject> Instantiate(std::string name, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 euler, std::shared_ptr<Transform> parent);
	static std::shared_ptr<GameObject> Instantiate(std::string name, DirectX::XMFLOAT3 position, DirectX::XMFLOAT4 rotation, std::shared_ptr<Transform> parent);

private:

};