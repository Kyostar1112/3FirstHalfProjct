#ifndef _C_PLAYER_SHOT_H_
#define _C_PLAYER_SHOT_H_

#include"Character.h"
namespace Shot {
	const int SHOT_FIRING_CNT = 100;		//�V���b�g�̔��˂���Ă���̐������Ԃ̍ő�l.
}

class clsPlayerShot
	:public clsCharacter
{
public:
	clsPlayerShot();
	~clsPlayerShot();

	//
	bool GetShotFlg();

	void Fire();

	void FireInterval();

	void FireInit();

private:
	bool m_bShotFlg;//true�Ȃ甭�˂��Ă���.
	int  m_iShotFiringCnt;//true�Ȃ甭�˂��Ă���.
};

#endif  //#ifdef _C_PLAYER_SHOT_H_