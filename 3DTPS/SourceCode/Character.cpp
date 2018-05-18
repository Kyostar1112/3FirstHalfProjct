#include"Character.h"

clsCharacter::clsCharacter()
	: m_pModel(NULL)
{
	m_fSpd = 1.0f;
	m_Sphere.fRadius = 0.7f;
}

clsCharacter::~clsCharacter()
{
}

//���f���f�[�^�֘A�t��.
void clsCharacter::AttachModel(clsDX9Mesh* pModel)
{
	if (pModel != NULL)
	{
		m_pModel = pModel;
	}
}

//���f���f�[�^�֘A�t������.
void clsCharacter::DetachModel()
{
	if (m_pModel != NULL)
	{
		m_pModel = NULL;
	}
}
#if 0

//�ʒu�X�V�֐�.
void clsDX9Mesh::UpdatePos()
{
	//============================================
	//�L�����N�^�ړ�����(���W�R��)
	D3DXMATRIX mYaw;
	D3DXMatrixRotationY(&mYaw, m_fYaw);//Y����].

	//Z���x�N�g����p��.
	D3DXVECTOR3	vecAxisZ(0.0f, 0.0f, 1.0f);

	//Z���x�N�g�����̂��̂���]��Ԃɂ��ϊ�����.
	D3DXVec3TransformCoord(
		&vecAxisZ,	//(out)
		&vecAxisZ,
		&mYaw);	//Y����]�s��.

	//�����ɂ���Đi�s������l��ݒ�.
	switch (m_enDir) {
	case enDirection_Foward:	//�O�i.
		//�����������~�i�߂�l(0.1f)
		m_vPos += vecAxisZ * 0.1f;
		break;
	case enDirection_Backward:	//���.
		m_vPos -= vecAxisZ * 0.1f;
		break;
	}
	m_enDir = enDirection_Stop;	//��~.
	//============================================
}

#endif // 0

//�`��.
void clsCharacter::Render(D3DXMATRIX &mView, D3DXMATRIX &mProj,
	D3DXVECTOR3 &vLight, D3DXVECTOR3 &vEye,
	D3DXVECTOR4 vColor, bool alphaFlg)
{
	//���W�X�V����.
	Update();
	//���̂܂ܕ`��֐����Ă�.
	m_pModel->Render(mView, mProj, vLight, vEye, vColor, alphaFlg);
}

//���W���]�l���X�V����.
void clsCharacter::Update()
{
	if (m_pModel == NULL) {
		return;
	}

	m_pModel->m_vPos = m_vPos;
	m_Sphere.vCenter = m_vPos;
	m_pModel->m_fYaw = m_vRot.y;
	m_pModel->m_fPitch = m_vRot.x;
	m_pModel->m_fRoll = m_vRot.z;
	m_pModel->m_fScale = m_fScale;
}

//============================================================
//	�ʒu�X�V�֐�.
//============================================================
void clsCharacter::UpdatePos()
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