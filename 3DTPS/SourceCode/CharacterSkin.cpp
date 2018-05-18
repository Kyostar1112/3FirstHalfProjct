#include "CharacterSkin.h"

//�R���X�g���N�^.
clsCharacterSkin::clsCharacterSkin()
	: m_pSkinModel(NULL)
	, m_pAnimCtrl(NULL)
	, m_dAnimSpeed(0.1f)
{
	m_fSpd = 1.0f;
}
//�f�X�g���N�^.
clsCharacterSkin::~clsCharacterSkin()
{
}
//���f���f�[�^�֘A�t��.
void clsCharacterSkin::AttachModel(clsD3DXSKINMESH* pModel)
{
	if (pModel == nullptr) {
		return;
	}

	//���f���̐ݒ�.
	m_pSkinModel = pModel;
	//�A�j���[�V�������x.
	m_dAnimSpeed = m_pSkinModel->GetAnimSpeed();
	//�X�P�[���ݒ�.
	m_fScale = m_pSkinModel->m_fScale;

	//�A�j���[�V�����R���g���[���[�̃N���[��.
	LPD3DXANIMATIONCONTROLLER pAC
		= m_pSkinModel->GetAnimCtrl();

	pAC->CloneAnimationController(
		pAC->GetMaxNumAnimationOutputs(),
		pAC->GetMaxNumAnimationSets(),
		pAC->GetMaxNumTracks(),
		pAC->GetMaxNumEvents(),
		&m_pAnimCtrl);	//(out)�A�j���[�V�����R���g���[���[.
}
//���f���f�[�^�֘A�t������.
void clsCharacterSkin::DetachModel()
{
	if (m_pSkinModel != nullptr) {
		m_pSkinModel = NULL;

		if (m_pAnimCtrl != NULL) {
			m_pAnimCtrl->Release();
			m_pAnimCtrl = NULL;
		}
	}
}

//�����_�����O�֐�.
void clsCharacterSkin::Render(D3DXMATRIX& mView, D3DXMATRIX& mProj,
	D3DXVECTOR3& vLight, D3DXVECTOR3& vEye)
{
	if (m_pSkinModel == nullptr && m_pAnimCtrl == nullptr) {
		return;
	}

	Update();

	m_pSkinModel->Render(
		mView, mProj, vLight, vEye,
		m_pAnimCtrl);
}

//���W���]�̍X�V.
void clsCharacterSkin::Update()
{
	if (m_pSkinModel == nullptr) {
		return;
	}
	m_pSkinModel->m_vPos = m_vPos;
	m_pSkinModel->m_fYaw = m_vRot.y;
	m_pSkinModel->m_fPitch = m_vRot.x;
	m_pSkinModel->m_fRoll = m_vRot.z;

	m_pSkinModel->m_fScale = m_fScale;

	m_pSkinModel->m_dAnimSpeed = m_dAnimSpeed;
}
//============================================================
//	�ʒu�X�V�֐�.
//============================================================
void clsCharacterSkin::UpdatePos()
{
	//�L�����N�^�ړ�����(���W�R��)
	D3DXMATRIX mYaw;
	D3DXMatrixRotationY(&mYaw, m_vRot.y);

	//Z���x�N�g����p��.
	D3DXVECTOR3	vecAxisZ(0.0f, 0.0f, 1.0f);

	//Z���x�N�g�����̂��̂���]��Ԃɂ��ϊ�����.
	D3DXVec3TransformCoord(
		&vecAxisZ,	//(out)
		&vecAxisZ,
		&mYaw);	//Y����]�s��.

	//�����ɂ���Đi�s������l��ݒ�.
	switch (m_enDir)
	{
	case enDirection_Stop:
		break;
	case enDirection_Foward:	//�O�i.
		//����������*�i�߂�l(0.1f).
		m_vPos += vecAxisZ * 0.15f * m_fSpd;
		//		m_fWalkLangth -= 0.15f * m_fSpd;
		break;
	case enDirection_Backward:	//���.
		m_vPos -= vecAxisZ * 0.15f * m_fSpd;
		//		m_fWalkLangth -= 0.15f * m_fSpd;
		break;
		//case enDirection_LeftTurn:
		//	break;
		//case enDirection_RightTurn:
		//	break;
		//default:
		//	break;
	}
	m_enDir = enDirection_Stop;//��~.
}
//�A�j���[�V�����ő吔���擾����.
int clsCharacterSkin::GetAnimSetMax()
{
	if (m_pAnimCtrl != NULL) {
		return (int)m_pAnimCtrl->GetMaxNumAnimationSets();
	}
	return -1;
}

//�A�j���[�V�����؂�ւ�.
void clsCharacterSkin::ChangeAnimSet(int index, double dStartPos)
{
	//�A�j���[�V�����͈͓̔����`�F�b�N.
	if (index < 0 || index >= GetAnimSetMax()) {
		return;
	}
	m_pSkinModel->ChangeAnimSet_StartPos(
		index, dStartPos, m_pAnimCtrl);
}