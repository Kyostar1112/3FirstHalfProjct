#include"Enemy.h"

const float PI = 3.141592f;
namespace ConstantEnemy
{
	const float fSpawnLength = 30.0f;//�G���o�Ă��鋗��.
	const int   iExpImg = 3;	//�����摜�̐�.
	const int	iAttakInterval = 180;	//�U���Ԋu.
}

namespace CE = ConstantEnemy;

clsEnemy::clsEnemy()
{
	m_vsmpExplosion.reserve(CE::iExpImg);
	for (size_t i = 0; i < CE::iExpImg; i++)
	{
		m_vsmpExplosion.push_back(make_unique<clsExplosion>());
		m_vsmpExplosion[i]->Create();
	}
}

clsEnemy::~clsEnemy()
{
}

void clsEnemy::Loop()
{
	//�A�j���[�V�������x���Z�͐��.
	m_dAnimTime += m_dAnimSpeed;

	switch (m_EnemyState)
	{
	case clsEnemy::State::Move:
		Move();
		break;
	case clsEnemy::State::ChangeAttack:
		ChangeAttack();
		break;
	case clsEnemy::State::InitAttack:
		InitAttack();
		break;
	case clsEnemy::State::Attak:
		Attack();
		break;
	case clsEnemy::State::IdleAttak:
		IdleAttack();
		break;
	default:
		break;
	}
	UpdatePos();
}

void clsEnemy::Spawn()
{
	ChangeAnimSet(02);
	m_iAttakInterval = CE::iAttakInterval;
	float fZ, fX;

	float fDeg = RandamAmong(0.0f, 360.0f);//�p�x.

	m_fLen = CE::fSpawnLength;//����.

	float rad = D3DXToRadian(fDeg);

	fX = m_fLen*cosf(rad);
	fZ = m_fLen*sinf(rad);

	D3DXVECTOR3 vTmp = m_vSpawnPos;

	vTmp.x += fX;
	vTmp.z += fZ;
	SetPosition(vTmp);

	m_dAnimSpeed = 0.01f;
	m_iAnimNo = 0;

	m_bEnableFlg = true;

	//�ړ���ԂɕύX.
	m_EnemyState = State::Move;
}

void clsEnemy::Move()
{
	D3DXVECTOR3 vTargtPos = m_vSpawnPos;
	D3DXVECTOR3 vTmpPos = { 0.0f, 0.0f, 0.0f };	//�^�[�Q�b�g�Ǝ����̕ω���.
	float TargetRot = 0.0f;
	vTmpPos = vTargtPos - GetPosition();
	//�����ɕϊ�����.
	float Length = D3DXVec3Length(&vTmpPos);

	if (Length > 5.0f)
	{
		TargetRot = atan2f(vTmpPos.x, vTmpPos.z);	//�^�[�Q�b�g�ւ̊p�x.

		m_enDir = enDirection_Foward;

		SetRotationY(TargetRot);
	}
	else
	{
		//�U��������ԂɕύX.
		ChangeAnimSet(03);
		m_dAnimTime = 0.0f;
		m_EnemyState = State::ChangeAttack;
	}
}

void clsEnemy::ChangeAttack()
{
	//�ҋ@��Ԃ�.
	if (m_dAnimTime >= GetAnimPeriod(03))
	{
		//�U���ҋ@��ԂɕύX.
		m_EnemyState = State::InitAttack;
		m_dAnimSpeed = 0.0f;
	}
}

void clsEnemy::InitAttack()
{
	//�A�j���[�V�����؂�ւ�.
	ChangeAnimSet(05);
	m_dAnimTime = 0;
	m_dAnimSpeed = 0.01f;
	m_iAttakInterval = CE::iAttakInterval;
	//�U����ԂɕύX.
	m_EnemyState = State::Attak;
}

void clsEnemy::Attack()
{
	//�U���̃��[�V���������H.
	if (m_dAnimTime >= GetAnimPeriod(05))
	{
		m_bAttakFlg = true;
		ChangeAnimSet(06);
		m_dAnimTime = 0;
		//�U���ҋ@��ԂɕύX.
		m_EnemyState = State::IdleAttak;
	}
}

void clsEnemy::IdleAttack()
{
	//�U���̃��[�V���������H.
	if (m_dAnimTime >= GetAnimPeriod(06))
	{
		m_dAnimSpeed = 0.0f;
		ChangeAnimSet(00);
	}

	if (m_iAttakInterval < 0)
	{
		m_iAttakInterval = CE::iAttakInterval;
		//�U���ҋ@��ԂɕύX.
		m_EnemyState = State::InitAttack;
	}
	m_iAttakInterval--;
	m_dAnimSpeed = 0.01f;
}

void clsEnemy::Damage()
{
	for (size_t i = 0; i < m_vsmpExplosion.size(); i++)
	{
		if (!m_vsmpExplosion[i]->m_bDispFlg)
		{
			m_vsmpExplosion[i]->Fire(m_vPos);
			break;
		}
	}
}

void clsEnemy::ExpRender()	//�����̕\������.
{
	for (size_t i = 0; i < m_vsmpExplosion.size(); i++)
	{
		if (m_vsmpExplosion[i]->m_bDispFlg)
		{
			m_vsmpExplosion[i]->Render();
		}
	}
}