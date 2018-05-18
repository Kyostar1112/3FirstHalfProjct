#ifndef _C_SPHERE_H_
#define _C_SPHERE_H_

#include"Character.h"

class clsSphere
	:public clsCharacter
{
public:
	clsSphere();
	~clsSphere();
	void InitSphere(D3DXVECTOR3 vPos, float fScale)
	{
		//���S�Ɣ��a���\���̂ɐݒ�.
		SetPosition(vPos);
		SetPositionY(1.0f);
		SetScale(0.5f);
	}
	SPHERE m_stSphere;
private:
	//�X�t�B�A�쐬.
};

#endif //_C_SPHERE_H_
