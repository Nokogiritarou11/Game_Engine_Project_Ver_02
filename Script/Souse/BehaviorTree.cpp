#include "BehaviorTree.h"

Node* BehaviorTree::activeNodeInference(BehaviorData* data)
{
	// �f�[�^�����Z�b�g���ĊJ�n
	data->init();
	return root->childNodeInference(data);
}

void BehaviorTree::addNode(std::string parentName, std::string entryName, int priority, ExecJudgment* judgment, Action* action)
{
	// ���[�g�m�[�h����łȂ��ꍇ
	if (parentName != "")
	{
		// ���[�g�m�[�h����q�����Ă���m�[�h������
		Node* search_node = root->searchNode(parentName);
		if (search_node != NULL)
		{
			// �ǉ��������m�[�h�쐬
			std::shared_ptr<Node> add_node = std::make_shared<Node>(entryName, search_node, priority, judgment, action, search_node->getHirerchyNo() + 1);
			// �m�[�h�o�^
			search_node->addChild(add_node);
		}
	}
	else {
		// ���[�g�m�[�h����̏ꍇ
		if (root == NULL)
		{
			// ���[�g�m�[�h�ɃG���g���[�m�[�h��o�^
			root = std::make_unique<Node>(entryName, nullptr, priority, judgment, action, 1);
		}
	}
}
// �V�[�P���X�m�[�h����̐��_�J�n
Node* BehaviorTree::sequenceBack(Node* sequence_node, BehaviorData* data)
{
	return sequence_node->childNodeInference(data);
}
Node* BehaviorTree::run(Node* actionNode, BehaviorData* data)
{
	// �m�[�h���s���A�߂�l����m�[�h�̊����A�p���A
	int state = actionNode->Run();

	// ����I��
	if (state == ActionSTATE::END)
	{
		bool loop = false;
		do {
			//�V�[�P���X�m�[�h�����o��
			Node* sequenceNode = data->popSequenceNode();
			loop = false;

			// �r������Ȃ��Ȃ�I��
			// �V�[�P���X�m�[�h��NULL�ł���ΑS�Ċ����̂��߁Areturn NULL
			if (sequenceNode == NULL)
			{
				return NULL;
			}
			else {
				// NULL�łȂ����sequenceBack()���s�����̃V�[�P���X�m�[�h�����s�������ʂ�return���Ȃ���
				Node* node = sequenceBack(sequenceNode, data);
				if (node != NULL)
				{
					// ���s�������ʂ�return������
					return node;
				}
				// node��NULL�̏ꍇ�A�X�^�b�N�̃T�C�Y�𒲂ׂďI�����Ă��邩���f
				// 0�̏ꍇ�A�I�����Ă��邽�߁ANULL�����^�[��
//				if (data->sequenceStack.size() == 0) {
//					return NULL;
//				}
				// ���[�v�p�������邽�߁A�ϐ�loop��true����
				loop = true;
			}
		} while (loop);
	}
	else if (state == ActionSTATE::FAILED)
	{
		return NULL;
	}
	// ����ێ�
	return actionNode;
}