#pragma once
#include <vector>
#include "Original_Math.h"

struct Node;

class AStar
{
public:
	enum STATUS
	{
		NONE = 0,
		OPEN,
		CLOSE = 2,
	};

	const static int mapWidth = 17;
	const static int mapHeight = 19;

	static Node nodes[mapHeight][mapWidth];
	static std::vector<Node> openNode;
	static int goalNodeX;
	static int goalNodeZ;
private:

	static int startNodeX;
	static int startNodeZ;


	static int nowX;
	static int nowZ;


public:

	static std::vector<Node> root;	// ç≈íZåoòHÇÃï€ë∂

	static bool search; // íTçıíÜÇµÇƒÇ¢ÇÈÇ©Ç«Ç§Ç©
	static bool is_root;
	static void SetStartandGoal(Vector3 startPos,Vector3 goalPos);
	static void CreateStart();



	static void Initialize(Vector3 startPos, Vector3 endPos);

	static void Update();
	
	static Vector3 GetTarget(bool is_new = false);

};

struct Node
{
	AStar::STATUS status = AStar::NONE;
	int cost;
	int heuristic;
	Node* parent;
	int x;
	int z;

	
	int GetScore()
	{
		return cost + heuristic;
	}
	void Create(int _x, int _z, Node* _parent);

	bool is_Open(int _x, int _z);
};