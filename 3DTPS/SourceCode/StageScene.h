#ifndef _C_STAGE_SCENE_H_
#define _C_STAGE_SCENE_H_

#include"BaseScene.h"
#include"Sprite2D.h"
#include "PShot.h"
#include "Player.h"
#include "Sphere.h"
#include "Enemy.h"
#include "Player.h"
#include "Num.h"

#include <math.h>

namespace ConstantStageScene {
	const int iShotMax = 100;			//�V���b�g�̍ő吔.
	const int iShotInterValCnt = 30;	//�V���b�g�̊Ԋu.
	const int iEnemyMax = 10;			//��x�̉�ʂɏo�Ă���G�̍ő吔.
	const int iSpawnCnt = 60;			//�G�̃X�|�[������^�C�~���O.
	const int iPlayerHp = 15;			//�v���C���[�̗̑�.
	const int PlayerInvincible = 60;	//�v���C���[�̖��G����.
	const int iPlayerHpNumdigit = 2;	//���@�̗̑͂̌���.
	const int iStageClearTimedigit = 2;	//�N���A�܂ł̐����̌���.
	const int iStageEnemyDowndigit = 4;	//�G�̌��j�����X�R�A�̌���.
	const int iStageClearTimeMax = 60;	//�Q�[���N���A�܂ł̎���.
	const float fTagetAngle = 35.0f;	//�^�[�Q�b�g�̏Ə���F��ς���p�x.
}
namespace CSS = ConstantStageScene;

class clsStageScene
	:public clsSceneBase
{
public:
	clsStageScene();
	~clsStageScene();

	void Create();
	void Init();
	void UpDate();
	void Render();
	void Release();

	vector<unique_ptr<clsEnemy>>		m_vsmpEnemy;	//�{�X.
	vector<unique_ptr<clsPlayerShot>>	m_vsmpShot;	//�e.
	unique_ptr<clsPlayer>		m_smpPlayer;	//���@.
	unique_ptr<clsCharacter>	m_smpGround;	//�n��.

	void RightRoll(float MovePoint);
	void LeftRoll(float MovePoint);
	void Fire();

	SceneMode GetMode()
	{
		return m_stMode;
	}

	void MusicStop()
	{
		m_vsmpBgm[0]->Stop();
		for (UINT i = 0; i < m_vsmpShotSe.size(); i++)
		{
			m_vsmpShotSe[i]->Stop();
		}
		for (UINT i = 0; i < m_vsmpHitSe.size(); i++)
		{
			m_vsmpHitSe[i]->Stop();
		}
	}
	vector<float> InnerProduct;

private:
	void HitTestOfEnemyAndGround();

	void HitTestOfEnemyAndAlignment();

	void HitTestOfEnemyAndBullet();

	void HitTestOfShipAndEnemy();

	int m_icnt;//60�J�E���g�p.

	int m_iHp;	//�v���C��-�̗̑�;
	int m_iClearTime;	//�X�e�[�W�N���A�܂ł̎���.

	int m_iScore;//�G�̌��j�ɂ��X�R�A���Z.

	//�A�j���[�V�����o�ߎ���
	double m_dAnimTime;

	SceneMode m_stMode;

	//HP�\��.
	unique_ptr<clsNum> m_smpHpNum;

	//�c�莞�ԕ\��.
	unique_ptr<clsNum> m_smpClearTimeNum;

	//�X�R�A�\��.
	unique_ptr<clsNum> m_smpScoreNum;

	//�n�[�g�}�[�N.
	unique_ptr<clsSprite2D> m_smpHeart;
	//���v�}�[�N.
	unique_ptr<clsSprite2D> m_smpClock;
	//�X�R�A����.
	unique_ptr<clsSprite2D> m_smpScoreTxt;

	//Hp�Q�[�V�J�p.
	unique_ptr<clsSprite2D> m_smpHpGage;
	//�_���[�V�J�_�ŗp.
	unique_ptr<clsSprite2D> m_smpDamageImg;

	bool m_bPlayerDamage;//true���_���[�V�J���󂯂����.
	int	 m_iPlayerinvincible;//0����͖��G���.

	//BGM.
	vector<unique_ptr<clsSound>> m_vsmpBgm;
	//SE.
	vector<unique_ptr<clsSound>> m_vsmpShotSe;
	vector<unique_ptr<clsSound>> m_vsmpHitSe;

	int m_iShotIntervalCnt;	//�V���b�g�̊Ԋu.

	int m_iEnemyIntervalCnt;//�G�̏o�Ă���Ԋu.

	vector<unique_ptr<clsCharacter>> m_vsmpEnemySphere;
	unique_ptr<clsCharacter> m_smpPlayerSphere;
	vector<unique_ptr<clsCharacter>> m_vsmpShotSphere;

	ModelRenderSet m_stModelRenderSet;

	unique_ptr<clsSprite2D>		m_smpTargetPoint;		//�Ə�.
	unique_ptr<clsSprite2D>		m_smpTargetPointHit;	//�Ə�������ʒu�̂�.
	bool m_bTargetPointHitCheackFlg;

	//�X�t�B�A�쐬.
	HRESULT InitSphere(clsDX9Mesh* pMesh, float fScale = 0.7f);
	//�X�t�B�A�쐬.
	HRESULT InitSphere(D3DXVECTOR3 vPos, float fScale = 0.7f);

	//�����蔻��܂Ƃ�.
	void HitCheak();

	//�Փ˔���.
	bool Collision(clsDX9Mesh* pAttacker, clsDX9Mesh* pTarget);
	bool Collision(clsCharacter* pAttacker, clsCharacter* pTarget);

	D3DXVECTOR3 IntersectionLocation(clsGameObject* AttackObject, clsCharacter* DefenceObject, D3DXVECTOR3 RayOrientation);

	//���C�ƃ��b�V���̓����蔻��.
	bool Intersect(
		clsGameObject* pAttacker, clsCharacter* pTarget,
		float* pfDistance, D3DXVECTOR3* pvIntersect);

	//�����ʒu�̃|���S���̒��_��������.
	HRESULT FindVerticesOnPoly(
		LPD3DXMESH pTarget, DWORD dwPolyIndex,
		D3DXVECTOR3* pVecVertices);

	//���ς��g�p�����p�x�̊Ԃɑ��݊m�F�֐�
	bool InnerProductDot(const D3DXVECTOR3& vPos, const D3DXVECTOR3& vTargetPos, const float& fYAxis, const float& fAngle);

#if 0

	//�o�E���f�B���O�{�b�N�X�쐬.
	HRESULT InitBBox(clsDX9Mesh* pMesh);
	HRESULT InitBBox(clsD3DXSKINMESH* pMesh);

	//�Փ˔���(�o�E���f�B���O�{�b�N�X).
	bool BBoxCollision(clsDX9Mesh* pAttacker, clsDX9Mesh* pTarget);
	bool BBoxCollision(clsD3DXSKINMESH* pAttacker, clsD3DXSKINMESH* pTarget);

#endif // 0
};

#endif  //#ifdef _C_STAGE_H_