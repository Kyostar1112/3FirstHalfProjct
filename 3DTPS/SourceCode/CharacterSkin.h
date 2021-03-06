#ifndef _C_CHARACTER_SKIN_H_
#define _C_CHARACTER_SKIN_H_

#include"D3DXSKINMESH.h"
#include"GameObject.h"

/*****************************************************************************************************
*	キャラクタクラス（スキンメッシュクラス）
*/
class clsCharacterSkin
	: public clsGameObject
{
public:
	clsCharacterSkin();
	~clsCharacterSkin();

	//モデルデータ関連付け.
	virtual void AttachModel(clsD3DXSKINMESH* pModel);
	//モデルデータ関連付け解除.
	virtual void DetachModel();

	//レンダリング.
	virtual void Render(D3DXMATRIX& mView, D3DXMATRIX& mProj,
		D3DXVECTOR3& vLight, D3DXVECTOR3& vEye);

	//アニメーション最大数を取得する.
	int GetAnimSetMax();
	//アニメーション切り替え.
	virtual void ChangeAnimSet(int index, double dStartPos = 0.0f);
	virtual double GetAnimPeriod(int index)
	{
		float fMotionAdjustment = 0.036f;//レンダーの位置に合わせるために.
		return m_pSkinModel->GetAnimPeriod(index) - fMotionAdjustment;
	}

	double m_dAnimSpeed;
	double m_dAnimTime;
	int m_iAnimNo;

	//位置更新関数.
	virtual void UpdatePos();
	enDirection		m_enDir;	//方向.
	float			m_fSpd;

	virtual clsD3DXSKINMESH* GetPointor() {
		return m_pSkinModel;
	}
	//座標や回転の更新.
	virtual void Update();//仮想関数.

protected:
	clsD3DXSKINMESH*			m_pSkinModel;
	LPD3DXANIMATIONCONTROLLER	m_pAnimCtrl;
};

#endif //#ifndef _C_CHARACTER_SKIN_H_.
