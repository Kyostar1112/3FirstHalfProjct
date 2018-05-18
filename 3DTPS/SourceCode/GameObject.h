#ifndef _C_GAME_OBJECT_H_
#define _C_GAME_OBJECT_H_

#include "Global.h"

class clsGameObject
{
public:
	clsGameObject();
	virtual ~clsGameObject();//�p���ړI�Ȃ牼�z�����Ă����Ȃ��ƌp����̃f�X�g���N�^���Ƃ����Ă���Ȃ�.

	//�ʒu�ݒ�.
	virtual void SetPosition(D3DXVECTOR3 vPos);//SpiaMge��virtual.
	void SetPositionX(float fPosX);
	void SetPositionY(float fPosY);
	void SetPositionZ(float fPosZ);
	//�擾.
	D3DXVECTOR3 GetPosition();
	float GetPositionX();
	float GetPositionY();
	float GetPositionZ();
	//��������.
	void AddPosition(D3DXVECTOR3 vMove);
	void AddPositionX(float fMoveX);
	void AddPositionY(float fMoveY);
	void AddPositionZ(float fMoveZ);

	//��]�ݒ�.
	//�ݒ�.
	void SetRotation(D3DXVECTOR3 vRot);
	void SetRotationY(float fRotY);
	void SetRotationZ(float fRotZ);
	//�擾.
	D3DXVECTOR3 GetRotation();
	float GetRotationX();
	float GetRotationY();
	float GetRotationZ();
	//��������.
	void AddRotation(D3DXVECTOR3 vMove);
	void AddRotationY(float fMoveY);
	void AddRotationZ(float fMoveZ);

	//�g�k�ݒ�.
	void SetScale(float fScale);
	float GetScale();

	D3DXVECTOR3 m_vRay;
	D3DXVECTOR3 m_vAxis;

private:

protected://
	D3DXVECTOR3		m_vPos;
	D3DXVECTOR3		m_vRot;
	float			m_fScale;

	//���W���]�̍X�V.
	virtual void Update() = 0; //�������z�֐�.
};

#endif//#ifndef _C_GAME_OBJECT_H_.