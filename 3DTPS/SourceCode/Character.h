#ifndef _C_CHARACTER_H_
#define _C_CHARACTER_H_

//�C���N���[�h�t�@�C��.
#include "DX9Mesh.h"
#include "GameObject.h"

/***********************************************************************************
*
*	�L�����N�^�N���X.
*
**/
class clsCharacter
	: public clsGameObject
{
public:
	clsCharacter();
	virtual ~clsCharacter();

	//���f���f�[�^�֘A�t��.
	void AttachModel(clsDX9Mesh* pModel);
	//���f���f�[�^�֘A�t������.
	void DetachModel();

	//�`��.
	void Render(D3DXMATRIX& mView, D3DXMATRIX& mProj,
		D3DXVECTOR3& vLight, D3DXVECTOR3& vEye,
		D3DXVECTOR4 vColor = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f),
		bool alphaFlg = false);

	clsDX9Mesh* GetPointor() {
		return m_pModel;
	}

	LPD3DXMESH GetMesh() {
		return m_pModel->m_pMesh;
	}

	LPD3DXMESH GetMeshForRay() {
		return m_pModel->m_pMeshForRay;
	}

	//�ʒu�X�V�֐�.
	void UpdatePos();

	//���W���]�l���X�V����.
	void Update();

	enDirection		m_enDir;	//����.
	float			m_fSpd;
	SPHERE			m_Sphere;	//�X�t�B�A�\����.

private:

	clsDX9Mesh*		m_pModel;
};

#endif //#ifndef _C_CHARACTER_H_
