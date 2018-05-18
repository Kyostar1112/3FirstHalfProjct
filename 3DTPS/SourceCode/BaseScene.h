#ifndef _C_SCENE_H_
#define _C_SCENE_H_

#define _CRT_SECURE_NO_WARNINGS

#include "Resource.h"

#include "Character.h"		//�L�����N�^�N���X.
#include "CharacterSkin.h"	//�L�����N�^�X�L���N���X.

#include "Sprite2D.h"		//Ui�N���X.

#include "Sound.h"			//���y�N���X.
#include <algorithm>

#define Resource clsResource::GetInstance()

namespace SceneBase {
}

//======================================
//	�񋓑�.
//======================================
//�V�[�����[�h.
enum SceneMode
{
	Title = 0,
	Stage,
	Over,
	Clear,
	Result,
};
class clsSceneBase
{
public:
	clsSceneBase();
	virtual ~clsSceneBase();

	virtual void Create() = 0;//�Q�[���N�����̏������֐�.
	virtual void Init() = 0;//�V�[�����Ƃ̏������֐�.
	virtual void UpDate() = 0;//�����Ɖ񂷂�.
	virtual void Render() = 0;//�\��.
	virtual void MusicStop() = 0;//���y�~�߂�.
	virtual void Release() = 0;//�j�������.

	void BlackOut();
	void BlackStart(float ChaAmo, bool BFlg = true);

	//�Ó]�@�\�������Ă��邩�ǂ���byInoue.
	bool GetBlackIdle() {
		if (m_smpBlack->m_BlackMode == Idle &&
			m_smpWhite->m_BlackMode == Idle)
		{
			return true;
		}
		return false;
	}

	//�Ó]���������u�Ԃ�m�点��byTahara.
	bool GetBlackEnd() {
		if (m_smpBlack->m_BlackMode == Change ||
			m_smpWhite->m_BlackMode == Change)
		{
			return true;
		}
		return false;
	}

	//ON(True)/Off(false)
	void SetDepth(bool bFlg)
	{
		Resource->SetDepth(bFlg);
	}

private:


	unique_ptr<clsSprite2D> m_smpBlack;	//�Ó]�p.
	unique_ptr<clsSprite2D> m_smpWhite;	//�Ó]�p.

	float	m_fChangePoint;
	bool	m_bColor;
};

#endif  //#ifdef _C_SCENE_H_