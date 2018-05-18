#ifndef _C_RESURCE_H_
#define _C_RESURCE_H_
#include"Global.h"
#include"DX9Mesh.h"
#include"D3DXSKINMESH.h"
#include"File.h"
#include<memory>

/**************************************************
*	���\�[�X�N���X.
*/
struct ModelRenderSet
{
	D3DXMATRIX  mView;
	D3DXMATRIX  mProj;
	D3DXVECTOR3 vLight;
	D3DXVECTOR3 vEye;
};
struct SpriteRenderSet
{
	HWND hWnd;
	ID3D11Device*  pDevice11;
	ID3D11DeviceContext* pContext11;
};
class clsResource
{
public:
	//�X�^�e�B�b�N���f�����.
	enum enStaticModel
	{
		enStaticModel_Min = 0,	//�����Ƀ��f�������Ȃ���.

		enStaticModel_Plane,
		enStaticModel_Stage1,
		enStaticModel_Stage2,
		enStaticModel_Shot,
		enStaticModel_Sphere,

		enStaticModel_Max,		//�����Ƀ��f�������Ȃ���.
	};

	//�X�L�����f�����.
	enum enSkinModel
	{
		enSkinModel_Min = 0,	//�����Ƀ��f�������Ȃ���.

		enSkinModel_Player,

		enSkinModel_Boss,

		enSkinModel_Max,		//�����Ƀ��f�������Ȃ���.
	};
	//�C���X�^���X�擾(�B��̃A�N�Z�X�o�H).
	static clsResource* GetInstance()
	{
		//�B��̃C���X�^���X���쐬����.
		//(static�ō쐬���ꂽ�̂�2��ڂ͖��������).
		static clsResource s_Instance;
		return &s_Instance;
	}
	//	clsResource();
	~clsResource();

	HRESULT Init(HWND hWnd,
		ID3D11Device* pDevice,
		ID3D11DeviceContext* pContext);

	HRESULT CreateStaticModel(
		LPSTR FileName, enStaticModel enModel);
	HRESULT ReleaseStaticModel(enStaticModel enModel);
	clsDX9Mesh* GetStaticModels(enStaticModel enModel);

	//�X�L�����f���p.
	HRESULT CreateSkinModel(
		LPSTR FileName, enSkinModel enModel);
	HRESULT ReleaseSkinModel(enSkinModel enModel);
	clsD3DXSKINMESH* GetSkinModels(enSkinModel enModel);

	ModelRenderSet GetModelRenderSet()
	{
		return m_stModelRenderSet;
	}

	SpriteRenderSet GetSpriteRenderSet()
	{
		return m_stSpriteRenderSet;
	}

	void SetModelRender(D3DXMATRIX  mView, D3DXMATRIX  mProj, D3DXVECTOR3 vLight, D3DXVECTOR3 vEye)
	{
		m_stModelRenderSet.mView = mView;
		m_stModelRenderSet.mProj = mProj;
		m_stModelRenderSet.vLight = vLight;
		m_stModelRenderSet.vEye = vEye;
	}

	void SetSpriteRender(HWND hWnd, ID3D11Device*  pDevice11, ID3D11DeviceContext* pContext11)
	{
		m_stSpriteRenderSet.hWnd = hWnd;
		m_stSpriteRenderSet.pDevice11 = pDevice11;
		m_stSpriteRenderSet.pContext11 = pContext11;

		m_si.hWnd = m_stSpriteRenderSet.hWnd;
		m_si.pDevice = m_stSpriteRenderSet.pDevice11;
		m_si.pDeviceContext = m_stSpriteRenderSet.pContext11;

	}
	unique_ptr<clsFile> m_smpFile;
	vector<int> m_viScore;
	void ScoreComparison()
	{
		for (size_t i = m_viScore.size() - 1; i >= 0; i--)
		{
			for (size_t j = m_viScore.size() - 1; j >= 0; j--)
			{
				if (m_viScore[i] == m_viScore[j])
				{
					m_viScore.erase(m_viScore.begin() + j);
				}
			}
		}
	}
	//�[�x�e�X�g(Z�e�X�g) ON/OFF�ؑ�.
	void SetDepth(bool bFlg);
private:
	//�[�x(Z)�e�X�g�ݒ�.
	ID3D11DepthStencilState* m_pDepthStencilState;

	//������R�s�[���֎~����.
	clsResource();
	clsResource(const clsResource& rhs);
	clsResource& operator = (const clsResource& rhs);

	ModelRenderSet m_stModelRenderSet;
	SpriteRenderSet m_stSpriteRenderSet;

	CD3DXSKINMESH_INIT m_si;

	vector<unique_ptr<clsDX9Mesh>> 		m_vsmpStaticModels;	//�X�^�e�B�b�N���f��.
	vector<unique_ptr<clsD3DXSKINMESH>>	m_vsmpSkinModels;	//�X�L�����f��.
	bool IsRangeStaticModel(enStaticModel enModel);
	bool IsRangeSkinModel(enSkinModel enModel);
};

#endif//#ifndef _C_RESURCE_H_
