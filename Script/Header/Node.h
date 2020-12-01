#pragma once
#include <vector>
#include <string>
#include <memory>

#include "Actions.h"
#include "ExecJudgment.h"

class ExecJudgment;
class BehaviorData;
class Action;

class Node
{
protected:
	std::string name; // ���O
	const int priority; // ���ԑw�ł̗D�揇��
	Action* action; // �s��
	Node* parent;  // �e�m�[�h�@
	std::vector<std::shared_ptr<Node>> child;// �q�m�[�h
	int hierarchyNo; // �K�w�ԍ�

public:

	Node(std::string name,
		Node* parent,
		int priority,
		ExecJudgment* execJudgment,
		Action* action, int hierarchyNo) :
		name(name),
		parent(parent),
		priority(priority),
		execJudgment(execJudgment),
		action(action),
		hierarchyNo(hierarchyNo),
		child(NULL)
	{
	}

	ExecJudgment* execJudgment;

	// ���O�Q�b�^�[
	std::string getName() { return name; }

	// �e�m�[�h�Q�b�^�[
	Node* getParent() { return parent; }
	
	// �q�m�[�h�Q�b�^�[
	Node* getChild(int index) { return child[index].get(); }

	// �q�m�[�h�Q�b�^�[(����)
	Node* getLastChild() { return child[child.size() - 1].get(); }

	// �q�m�[�h�Q�b�^�[(�擪)
	Node* getTopChild() { return child[0].get(); }

	// �K�w�ԍ��Q�b�^�[
	int getHirerchyNo() { return hierarchyNo; }

	// �D�揇�ʃQ�b�^�[
	int getPriority() { return priority; }

	// �e�m�[�h�Z�b�^�[
	void setParent(Node* newParent) { parent = newParent; }

	// �q�m�[�h�ǉ�
	void addChild(std::shared_ptr<Node> newChild) { child.push_back(newChild); }

	// �s���f�[�^�������Ă��邩
	bool hasAction() { if (action) { return true; } return false; }

	// ���s�۔���
	bool judgment();

	// �D�揇�ʑI��
	Node* selectPriority(std::vector<Node*>* list);

	// �m�[�h����
	Node* searchNode(std::string searchName);

	// �m�[�h���_
	Node* childNodeInference(BehaviorData* data);
	 
	int Run();

};