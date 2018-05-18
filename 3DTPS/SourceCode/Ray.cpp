#include "Ray.h"

//�V�F�[�_�t�@�C����(�f�B���N�g�����܂�)
const char SHADER_NAME[] = "Shader\\Ray.hlsl";

//�R���X�g���N�^.
clsRay::clsRay()
{
	ZeroMemory(this, sizeof(clsRay));
}

//�f�X�g���N�^.
clsRay::~clsRay()
{
	SAFE_RELEASE(m_pVertexBuffer);
	SAFE_RELEASE(m_pConstantBuffer);
	SAFE_RELEASE(m_pPixelShader);
	SAFE_RELEASE(m_pVertexLayout);
	SAFE_RELEASE(m_pVertexShader);
}

//������.
HRESULT clsRay::Init(
	ID3D11Device* pDevice11,
	ID3D11DeviceContext* pContext11)
{
	m_pDevice11 = pDevice11;
	m_pDeviceContext11 = pContext11;

	if (FAILED(InitShader()))
	{
		return E_FAIL;
	}
	if (FAILED(InitLine()))
	{
		return E_FAIL;
	}

	return S_OK;
}

//======================================
//	HLSL�t�@�C����ǂݍ��݃V�F�[�_���쐬����.
//	HLSL:HIGH-LEVEL-SHADER-LANGUAGE.
//======================================
HRESULT clsRay::InitShader()
{
	ID3DBlob* pCompiledShader = NULL;
	ID3DBlob* pErrors = NULL;
	UINT uCompileFlag = 0;

#ifdef _DEBUG
	uCompileFlag
		= D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION;
#endif//#ifdef _DEBUG

	//HLSL����o�[�e�b�N�X�V�F�[�_�̃u���u���쐬.
	if (FAILED(
		D3DX11CompileFromFile(
			SHADER_NAME,	//�V�F�[�_�t�@�C����(HLSL�t�@�C��)
			NULL,			//�}�N����`�̔z��ւ̃|�C���^(���g�p)
			NULL,			//�C���N���[�h�t�@�C���������C���^�[�t�F�C�X�ւ̃|�C���^(���g�p)
			"VS",			//�V�F�[�_�G���g���[�|�C���g�֐��̖��O.
			"vs_5_0",		//�V�F�[�_�̃��f�����w�肷�镶����(�v���t�@�C��)
			uCompileFlag,	//�V�F�[�_�R���p�C���t���O.
			0,				//�G�t�F�N�g�R���p�C���t���O(���g�p)
			NULL,			//�X���b�h�|���v�C���^�[�t�F�C�X�ւ̃|�C���^(���g�p)
			&pCompiledShader,//�u���u���i�[���郁�����ւ̃|�C���^.
			&pErrors,		//�G���[�ƌx���ꗗ���i�[���郁�����ւ̃|�C���^.
			NULL)))			//�߂�l�ւ̃|�C���^(���g�p)
	{
		MessageBox(NULL, "hlsl�Ǎ����s", "�G���[", MB_OK);
		return E_FAIL;
	}
	SAFE_RELEASE(pErrors);

	//��L�ō쐬�����u���u����u�o�[�e�b�N�X�V�F�[�_�v���쐬.
	if (FAILED(
		m_pDevice11->CreateVertexShader(
			pCompiledShader->GetBufferPointer(),
			pCompiledShader->GetBufferSize(),
			NULL,
			&m_pVertexShader)))//(out)�o�[�e�b�N�X�V�F�[�_.
	{
		MessageBox(NULL, "vs�쐬���s", "�G���[", MB_OK);
		return E_FAIL;
	}

	//���_�C���v�b�g���C�A�E�g���`.
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{
			"POSITION",				//�ʒu.
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,//DXGI�̃t�H�[�}�b�g(32bit float�^*3)
			0,
			0,							//�f�[�^�̊J�n�ʒu.
			D3D11_INPUT_PER_VERTEX_DATA, 0
		}
	};
	//���_�C���v�b�g���C�A�E�g�̔z��v�f�����Z�o.
	UINT numElements = sizeof(layout) / sizeof(layout[0]);

	//���_�C���v�b�g���C�A�E�g�̍쐬.
	if (FAILED(
		m_pDevice11->CreateInputLayout(
			layout,
			numElements,
			pCompiledShader->GetBufferPointer(),
			pCompiledShader->GetBufferSize(),
			&m_pVertexLayout)))//(out)���_�C���v�b�g���C�A�E�g.
	{
		MessageBox(NULL, "���_�C���v�b�g���C�A�E�g�쐬���s", "�G���[", MB_OK);
		return E_FAIL;
	}
	SAFE_RELEASE(pCompiledShader);

	//HLSL����s�N�Z���V�F�[�_�̃u���u���쐬.
	if (FAILED(
		D3DX11CompileFromFile(
			SHADER_NAME,	//�V�F�[�_�t�@�C����(HLSL�t�@�C��)
			NULL,
			NULL,
			"PS",			//�V�F�[�_�G���g���[�|�C���g�֐��̖��O.
			"ps_5_0",		//�V�F�[�_�̃��f�����w�肷�镶����(�v���t�@�C��)
			uCompileFlag,	//�V�F�[�_�R���p�C���t���O.
			0,
			NULL,
			&pCompiledShader,//�u���u���i�[���郁�����ւ̃|�C���^.
			&pErrors,
			NULL)))
	{
		MessageBox(NULL, "hlsl(ps)�Ǎ����s", "�G���[", MB_OK);
		return E_FAIL;
	}
	SAFE_RELEASE(pErrors);

	//��L�ō쐬�����u���u����u�s�N�Z���V�F�[�_�v���쐬.
	if (FAILED(
		m_pDevice11->CreatePixelShader(
			pCompiledShader->GetBufferPointer(),
			pCompiledShader->GetBufferSize(),
			NULL,
			&m_pPixelShader)))//(out)�s�N�Z���V�F�[�_.
	{
		MessageBox(NULL, "ps�쐬���s", "�G���[", MB_OK);
		return E_FAIL;
	}
	SAFE_RELEASE(pCompiledShader);//�u���u���.

	//�R���X�^���g(�萔)�o�b�t�@�쐬 �V�F�[�_�ɓ���̐��l�𑗂�o�b�t�@.
	//�����ł͕ϊ��s��n���p.
	//�V�F�[�_��World,View,Projection�s���n��.
	D3D11_BUFFER_DESC cb;
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;//�R���X�^���g�o�b�t�@���w��.
	cb.ByteWidth = sizeof(SHADER_CONSTANT_BUFFER);//�R���X�^���g�o�b�t�@�̃T�C�Y.
	cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;//�������݂ŃA�N�Z�X.
	cb.MiscFlags = 0;//���̑��̃t���O(���g�p)
	cb.StructureByteStride = 0;//�\���̂̃T�C�Y(���g�p)
	cb.Usage = D3D11_USAGE_DYNAMIC;//�g�p���@:���ڏ�������.

	//�R���X�^���g�o�b�t�@�쐬.
	if (FAILED(
		m_pDevice11->CreateBuffer(
			&cb,
			NULL,
			&m_pConstantBuffer)))
	{
		MessageBox(NULL, "�R���X�^���g�o�b�t�@�쐬���s", "�G���[", MB_OK);
		return E_FAIL;
	}

	return S_OK;
}

//���C������.
HRESULT clsRay::InitLine()
{
	//���C�̃o�[�e�b�N�X�o�b�t�@�쐬.
	D3D11_BUFFER_DESC	bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(MODEL_VERTEX) * 2;//2�_��.
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = m_Ray.vPoint; //���C�̍��W���Z�b�g.
	if (FAILED(
		m_pDevice11->CreateBuffer(
			&bd, &InitData, &m_pVertexBuffer)))
	{
		ERR_MSG("���_�o�b�t�@�쐬���s", "Error");
		return E_FAIL;
	}
	return S_OK;
}

//�`��.
void clsRay::Render(D3DXMATRIX& mView, D3DXMATRIX& mProj)
{
	//���[���h�s��.
	D3DXMATRIX	mWorld;
	//��]�s��A�ړ��s��.
	D3DXMATRIX	mRot, mTran;

	//��]�s��쐬.
	D3DXMatrixRotationY(&mRot, m_Ray.fYaw);

	//���s�ړ��s��쐬.
	D3DXMatrixTranslation(&mTran,
		m_Ray.vPos.x, m_Ray.vPos.y, m_Ray.vPos.z);

	//���[���h�s��쐬.
	mWorld = mRot * mTran;

	//�g�p����V�F�[�_�̓o�^.
	m_pDeviceContext11->VSSetShader(m_pVertexShader, NULL, 0);
	m_pDeviceContext11->PSSetShader(m_pPixelShader, NULL, 0);

	//�V�F�[�_�̃R���X�^���g�o�b�t�@�Ɋe��f�[�^��n��.
	D3D11_MAPPED_SUBRESOURCE pData;
	SHADER_CONSTANT_BUFFER cd;	//�R���X�^���g�o�b�t�@.
	//�o�b�t�@���̃f�[�^�̏������J�n����map.
	if (SUCCEEDED(
		m_pDeviceContext11->Map(
			m_pConstantBuffer, 0,
			D3D11_MAP_WRITE_DISCARD, 0, &pData)))
	{
		//���[���h,�J����,�v���W�F�N�V�����s���n��.
		D3DXMATRIX m = mWorld * mView * mProj;
		D3DXMatrixTranspose(&m, &m);	//�s���]�u����.
		//�s��̌v�Z���@��DirectX��GPU�ňقȂ��.
		cd.mWVP = m;

		//�J���[.
		cd.vColor = D3DXVECTOR4(1.0f, 0.0f, 1.0f, 1.0f);

		memcpy_s(pData.pData, pData.RowPitch,
			(void*)(&cd), sizeof(cd));

		m_pDeviceContext11->Unmap(m_pConstantBuffer, 0);
	}

	//���̃R���X�^���g�o�b�t�@���ǂ̃V�F�[�_�Ŏg�����H.
	m_pDeviceContext11->VSSetConstantBuffers(
		0, 1, &m_pConstantBuffer);
	m_pDeviceContext11->PSSetConstantBuffers(
		0, 1, &m_pConstantBuffer);

	//���_�o�b�t�@���Z�b�g.
	UINT stride = sizeof(MODEL_VERTEX);	//�f�[�^�̊Ԋu.
	UINT offset = 0;
	m_pDeviceContext11->IASetVertexBuffers(
		0, 1, &m_pVertexBuffer, &stride, &offset);

	//���_�C���v�b�g���C�A�E�g���Z�b�g.
	m_pDeviceContext11->IASetInputLayout(m_pVertexLayout);

	//�v���~�e�B�u�E�g�|���W�[���Z�b�g.
	m_pDeviceContext11->IASetPrimitiveTopology(
		D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	//�v���~�e�B�u�������_�����O.
	m_pDeviceContext11->Draw(2, 0);
}