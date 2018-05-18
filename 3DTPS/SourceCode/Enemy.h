#ifndef _C_ENEMY_H_
#define _C_ENEMY_H_

#include"CharacterSkin.h"
#include<math.h>
#include <random>
#include "Explosin.h"

class clsEnemy
	:public clsCharacterSkin
{
public:
	clsEnemy();
	~clsEnemy();

	enum class State
	{
		Spawn = 0,		//�G����.
		Move,			//�G�ړ�.
		ChangeAttack,	//�ړ�����U�����[�h�ւ̕ύX.
		InitAttack,		//�U�����Ƃ̏�����.
		Attak,			//�U������.
		IdleAttak,		//�U���̃C���^�[�o��.
	};

	bool GetEnableFlg()
	{
		return m_bEnableFlg;
	}

	void SetEnableFlg(bool EnableFlg)
	{
		m_bEnableFlg = EnableFlg;
	}
	void InitEnemy(bool EnableFlg, D3DXVECTOR3 SpawnPos)
	{
		m_bEnableFlg = EnableFlg;
		m_vSpawnPos = SpawnPos;
		m_fSpd = 0.5f;
	}
	void Spawn();

	void Loop();

	void Damage();		//�U���𓖂Ă�ꂽ���̏���.
	void ExpRender();	//�����̕\������.

	bool m_bAttakFlg;

	//�G�̔��j�p.
	vector<unique_ptr<clsExplosion>> m_vsmpExplosion;
private:

	void Move();

	void ChangeAttack();

	void InitAttack();

	void Attack();

	void IdleAttack();

	State m_EnemyState;

	float m_fLen;//����.

	bool m_bEnableFlg;//true�����̃��f�����g�p���Ă���.

	int  m_iAttakInterval;//�U���̊Ԋu.

	D3DXVECTOR3 m_vSpawnPos;	//�G���o���ׂ̌��̈ʒu.
};

#endif  //#ifdef _C_ENEMY_H_