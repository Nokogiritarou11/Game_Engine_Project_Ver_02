#include "A-Star.h"
const int AStar::mapHeight;
const int AStar::mapWidth;
int AStar::startNodeX;
int AStar::startNodeZ;
int AStar::goalNodeX;
int AStar::goalNodeZ;
int AStar::nowX;
int AStar::nowZ;
Node AStar::nodes[mapHeight][mapWidth];
std::vector<Node> AStar::openNode;
std::vector<Node> AStar::root;
bool AStar::search;
bool AStar::is_root;

const int DoragonMap[AStar::mapHeight][AStar::mapWidth] = {
	// x -                         x +
//       8	7 6 5 4 3 2 1 0 1 2 3 4 5 6 7 8 
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//9 z + 
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//8    -1500 -900
		{0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0},//7
		{0,0,0,0,1,1,1,1,1,1,1,1,1,0,0,0,0},//6
		{0,0,0,1,1,1,1,1,1,1,1,1,1,1,0,0,0},//5
		{0,0,0,1,1,1,0,1,1,1,1,1,1,1,0,0,0},//4
		{0,0,1,1,1,0,0,0,1,1,1,0,1,1,1,0,0},//3
		{0,0,1,1,1,0,0,0,1,1,0,0,0,1,1,0,0},//2
		{0,0,1,1,1,1,0,1,1,1,0,0,0,1,1,0,0},//1
		{0,0,1,1,1,1,1,1,1,1,1,0,1,1,1,0,0},//0
		{0,0,1,1,1,0,1,1,1,1,1,1,1,1,1,0,0},//1
		{0,0,1,1,0,0,0,1,1,1,0,1,1,1,1,0,0},//2
		{0,0,1,1,0,0,0,1,1,0,0,0,1,1,1,0,0},//3
		{0,0,0,1,1,0,1,1,1,0,0,0,1,1,0,0,0},//4
		{0,0,0,0,1,1,1,1,1,1,0,1,1,1,0,0,0},//5
		{0,0,0,0,1,1,1,1,1,1,1,1,1,0,0,0,0},//6
		{0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0},//7
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//8
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0} //9 z -
};
const int PlayerMap[AStar::mapHeight][AStar::mapWidth] = {
	// x -                         x +
//		8 7 6 5 4 3 2 1 0 1 2 3 4 5 6 7 8 
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//9 z + 
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//8    -1500 -900
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//7
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//6
		{0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0},//5
		{0,0,0,0,1,1,0,1,1,1,1,1,1,0,0,0,0},//4
		{0,0,0,1,1,0,0,0,1,1,1,0,1,1,1,0,0},//3
		{0,0,0,1,1,0,0,0,1,1,0,0,0,1,1,0,0},//2
		{0,0,0,1,1,1,0,1,1,1,0,0,0,1,1,0,0},//1
		{0,0,0,1,1,1,1,1,1,1,1,0,1,1,1,0,0},//0
		{0,0,0,1,1,0,1,1,1,1,1,1,1,1,1,0,0},//1
		{0,0,0,1,0,0,0,1,1,1,0,1,1,1,1,0,0},//2
		{0,0,0,1,0,0,0,1,1,0,0,0,1,1,1,0,0},//3
		{0,0,0,1,1,0,1,1,1,0,0,0,1,1,0,0,0},//4
		{0,0,0,0,1,1,1,1,1,1,0,1,1,1,0,0,0},//5
		{0,0,0,0,1,1,1,1,1,1,1,1,1,0,0,0,0},//6
		{0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0},//7
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//8
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0} //9 z -
};

void AStar::Initialize(Vector3 startPos, Vector3 goalPos)
{
	for (int i = 0; i < mapHeight;i++)
	{
		for (int j = 0; j < mapWidth; j++)
		{
			nodes[i][j].status = NONE;
		}
	}
	root.clear();
	openNode.clear();
	SetStartandGoal(startPos, goalPos);
	CreateStart();
	search = true;

	
}

void AStar::Update()
{
	if (search)
	{
		nodes[nowZ][nowX].is_Open(-1, -1);
		nodes[nowZ][nowX].is_Open(1, 1);
		nodes[nowZ][nowX].is_Open(1, -1);
		nodes[nowZ][nowX].is_Open(-1, 1);
		nodes[nowZ][nowX].is_Open(0, -1);
		nodes[nowZ][nowX].is_Open(-1, 0);
		nodes[nowZ][nowX].is_Open(0, 1);
		nodes[nowZ][nowX].is_Open(1, 0);
		nodes[nowZ][nowX].status = CLOSE;

		if (nowX == goalNodeX && nowZ == goalNodeZ)
		{
			// I—¹
			search = false;

			int a = goalNodeZ;
			int b = goalNodeX;

			for (;;)
			{
				Node* parent = nodes[a][b].parent;

				if (parent == nullptr)
				{
					break;
				}
				
				root.push_back(nodes[a][b]);

				a = parent->z;
				b = parent->x;
				
			}
			is_root = true;
		}
		int num = 100;

		for (auto& n : openNode)
		{
			if (nodes[n.z][n.x].status != CLOSE)
			{
				if (num >= n.GetScore())
				{
					num = n.GetScore();
					nowX = n.x;
					nowZ = n.z;
				}
			}
		}
	}

}


void AStar::CreateStart()
{
	nodes[startNodeZ][startNodeX].status = OPEN;
	int x = startNodeX;
	int z = startNodeZ;
	openNode.push_back(nodes[startNodeZ][startNodeX]);
	nodes[startNodeZ][startNodeX].parent = nullptr;
	nodes[startNodeZ][startNodeX].cost = 0;
	int dx = abs(goalNodeX - x);
	int dz = abs(goalNodeZ - z);
	if (dx < dz)
	{
		nodes[startNodeZ][startNodeX].heuristic = dz;
	}
	else
	{
		nodes[startNodeZ][startNodeX].heuristic = dx;
	}
}

void Node::Create(int _x, int _z, Node* _parent)
{
	status = AStar::OPEN;
	x = _x;
	z = _z;
	parent = _parent;
	cost = parent->cost + 1;

	int dx = abs(AStar::goalNodeX - x);
	int dz = abs(AStar::goalNodeZ - z);
	if (dx < dz)
	{
		heuristic = dz;
	}
	else
	{
		heuristic = dx;
	}
}

bool Node::is_Open(int _x, int _z)
{

	_x = x + _x;
	_z = z + _z;
	if (_x < 0 || _z < 0 || _x > AStar::mapHeight || _z > AStar::mapWidth)
	{
		return false;
	}
	if (DoragonMap[_z][_x] == 0)
	{
		return false;
	}
	if (AStar::nodes[_z][_x].status != AStar::NONE)
	{
		return false;
	}

	AStar::nodes[_z][_x].Create(_x, _z, &AStar::nodes[z][x]);
	AStar::openNode.push_back(AStar::nodes[_z][_x]);


	return true;

}


void AStar::SetStartandGoal(Vector3 startPos, Vector3 goalPos)
{
	startNodeX = startPos.x / 300 + mapWidth / 2;
	startNodeZ = startPos.z / 300 + mapHeight / 2;
	goalNodeX = goalPos.x / 300 + mapWidth / 2;
	goalNodeZ = goalPos.z / 300 + mapHeight / 2;
	
	nowX = startNodeX;
	nowZ = startNodeZ;
	nodes[startNodeZ][startNodeX].x = startNodeX;
	nodes[startNodeZ][startNodeX].z = startNodeZ;

}

Vector3 AStar::GetTarget(bool is_new)
{

	static int count = root.size();
	if (count == -1)
	{
		count = root.size();
	}
	if (is_new)
	{
		if (is_root)
		{
			count -= 1;

			if (count < 0)
			{
				count = -1;
				is_root = false;
				return Vector3((root.at(0).x - mapWidth / 2) * 300.0f,
					0.0f,
					(root.at(0).z - mapHeight / 2) * 300.0f);
			}
		}
		
		return Vector3((root.at(count).x - mapWidth / 2) * 300.0f,
				0.0f,
				(root.at(count).z - mapHeight / 2) * 300.0f);
		
	}
}