#ifndef _C_CHARACTER_SKIN_H_
#define _C_CHARACTER_SKIN_H_

#include"D3DXSKINMESH.h"
#include"GameObject.h"

/*****************************************************************************************************
*	�L�����N�^�N���X�i�X�L�����b�V���N���X�j
*/
class clsCharacterSkin
	: public clsGameObject
{
public:
	clsCharacterSkin();
	~clsCharacterSkin();

	//���f���f�[�^�֘A�t��.
	virtual void AttachModel(clsD3DXSKINMESH* pModel);
	//���f���f�[�^�֘A�t������.
	virtual void DetachModel();

	//�����_�����O.
	virtual void Render(D3DXMATRIX& mView, D3DXMATRIX& mProj,
		D3DXVECTOR3& vLight, D3DXVECTOR3& vEye);

	//�A�j���[�V�����ő吔���擾����.
	int GetAnimSetMax();
	//�A�j���[�V�����؂�ւ�.
	virtual void ChangeAnimSet(int index, double dStartPos = 0.0f);
	virtual double GetAnimPeriod(int index)
	{
		float fMotionAdjustment = 0.036f;//�����_�[�̈ʒu�ɍ��킹�邽�߂�.
		return m_pSkinModel->GetAnimPeriod(index) - fMotionAdjustment;
	}

	double m_dAnimSpeed;
	double m_dAnimTime;
	int m_iAnimNo;

	//�ʒu�X�V�֐�.
	virtual void UpdatePos();
	enDirection		m_enDir;	//����.
	float			m_fSpd;

	virtual clsD3DXSKINMESH* GetPointor() {
		return m_pSkinModel;
	}
	//���W���]�̍X�V.
	virtual void Update();//���z�֐�.

protected:
	clsD3DXSKINMESH*			m_pSkinModel;
	LPD3DXANIMATIONCONTROLLER	m_pAnimCtrl;
};

#endif //#ifndef _C_CHARACTER_SKIN_H_.
