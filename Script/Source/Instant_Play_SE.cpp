#include "Instant_Play_SE.h"
#include "Player_Camera_Controller.h"

using namespace std;
using namespace BeastEngine;

void Instant_Play_SE::Awake()
{
	//�����o�|�C���^�̎擾
	const auto& s = Get_Component<AudioSource>();
	source = s;
	volume = s->Get_Volume();
	pitch = s->Get_Pitch();
}

void Instant_Play_SE::OnEnable()
{
	//�A�N�e�B�u�����ɃJ�����V�F�C�N���Ă�
	source.lock()->Play_OneShot(volume, pitch);
}

bool Instant_Play_SE::Draw_ImGui()
{
	bool open = false;
	if (!Draw_ImGui_Header("Instant_Play_SE", open)) return false;

	if (open)
	{
		//const float window_center = ImGui::GetWindowContentRegionWidth() * 0.5f;
		ImGui::Text(u8"�ݒ�\�ȃp�����[�^�͂���܂���");
	}
	return true;
}