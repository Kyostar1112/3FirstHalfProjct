#include "Sprite.h"

//�V�F�[�_�t�@�C����(�f�B���N�g�����܂�)
const char SHADER_NAME[] = "Shader\\Mesh.hlsl";

//�R���X�g���N�^.
clsSprite::clsSprite()
{
	//������.
	ZeroMemory(this, sizeof(clsSprite));
}

//�f�X�g���N�^.
clsSprite::~clsSprite()
{
	//�����ł͉�����Ȃ�.
	m_pDevice11 = NULL;
	m_pDeviceContext11 = NULL;
}

//������.
HRESULT clsSprite::Init()
{
	m_pDevice11 = clsResource::GetInstance()->GetSpriteRenderSet().pDevice11;
	m_pDeviceContext11 = clsResource::GetInstance()->GetSpriteRenderSet().pContext11;

	//�V�F�[�_�쐬.
	if (FAILED(InitShader())) {
		return E_FAIL;
	}
	//�|���S���쐬.
	if (FAILED(InitModel())) {
		return E_FAIL;
	}

	return S_OK;
}

//======================================
//	HLSL�t�@�C����ǂݍ��݃V�F�[�_���쐬����.
//	HLSL:HIGH-LEVEL-SHADER-LANGUAGE.
//======================================
HRESULT clsSprite::InitShader()
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
			"VS_Ita",		//�V�F�[�_�G���g���[�|�C���g�֐��̖��O.
			"vs_5_0",		//�V�F�[�_�̃��f�����w�肷�镶����(�v���t�@�C��)
			uCompileFlag,	//�V�F�[�_�R���p�C���t���O.
			0,				//�G�t�F�N�g�R���p�C���t���O(���g�p)
			NULL,			//�X���b�h�|���v�C���^�[�t�F�C�X�ւ̃|�C���^(���g�p)
			&pCompiledShader,//�u���u���i�[���郁�����ւ̃|�C���^.
			&pErrors,		//�G���[�ƌx���ꗗ���i�[���郁�����ւ̃|�C���^.
			NULL)))		//�߂�l�ւ̃|�C���^(���g�p)
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
		},
		{
			"TEXCOORD",		//�e�N�X�`���ʒu.
			0,
			DXGI_FORMAT_R32G32_FLOAT,	//DXGI�̃t�H�[�}�b�g(32bit float�^*2)
			0,
			12,							//�e�N�X�`���f�[�^�̊J�n�ʒu.
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
			"PS_Ita",		//�V�F�[�_�G���g���[�|�C���g�֐��̖��O.
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
	cb.ByteWidth = sizeof(SPRITESHADER_CONSTANT_BUFFER);//�R���X�^���g�o�b�t�@�̃T�C�Y.
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

//================================================
//	���f���쐬.
//================================================
HRESULT clsSprite::InitModel()
{
	float itaW = 2.0f;
	float w, h;
	w = h = (1.0f / 8.0f);

	//�|��(�l�p�`)�̒��_���쐬.
	SpriteVertex vertices[] =
	{
		//���_���W(x,y,z)				UV���W(u,v)
		D3DXVECTOR3(-itaW, -itaW, 0.0f), D3DXVECTOR2(0.0f, h),//���_1.
		D3DXVECTOR3(-itaW, itaW, 0.0f), D3DXVECTOR2(0.0f, 0.0f),//���_2.
		D3DXVECTOR3(itaW, -itaW, 0.0f), D3DXVECTOR2(w, h),//���_3.
		D3DXVECTOR3(itaW, itaW, 0.0f), D3DXVECTOR2(w, 0.0f)	//���_4.
	};
	//�ő�v�f�����Z�o����.
	UINT uVerMax = sizeof(vertices) / sizeof(vertices[0]);

	//�o�b�t�@�\����.
	D3D11_BUFFER_DESC bd;
	bd.Usage = D3D11_USAGE_DEFAULT;//�g�p��(�f�t�H���g)
	bd.ByteWidth = sizeof(SpriteVertex) * uVerMax;//���_�T�C�Y(���_�~3)
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;//���_�o�b�t�@�Ƃ��Ĉ���.
	bd.CPUAccessFlags = 0;	//CPU����̓A�N�Z�X���Ȃ�.
	bd.MiscFlags = 0;	//���̑��̃t���O(���g�p)
	bd.StructureByteStride = 0;//�\���̂̃T�C�Y(���g�p)

	//�T�u���\�[�X�f�[�^�\����.
	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = vertices;//�|���̒��_���Z�b�g.

	//���_�o�b�t�@�̍쐬.
	if (FAILED(
		m_pDevice11->CreateBuffer(
			&bd, &InitData, &m_pVertexBuffer)))
	{
		MessageBox(NULL, "���_�o�b�t�@�쐬���s", "�G���[", MB_OK);
		return E_FAIL;
	}

	//���_�o�b�t�@���Z�b�g.
	UINT stride = sizeof(SpriteVertex);//�f�[�^�Ԋu.
	UINT offset = 0;
	m_pDeviceContext11->IASetVertexBuffers(
		0, 1,
		&m_pVertexBuffer, &stride, &offset);

	//�e�N�X�`���p�̃T���v���[�\����.
	D3D11_SAMPLER_DESC SamDesc;
	ZeroMemory(&SamDesc, sizeof(SamDesc));
	//���j�A�t�B���^-(���`���)
	//	POINT:���������e��.
	SamDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	//���b�s���O���[�h.
	//	WRAP  :�J��Ԃ�
	//	MIRROR:���]�J��Ԃ�.
	//	CLAMP :�[�̖͗l�������L�΂�.
	//	BORDER:�ʓr���E�F�����߂�.
	SamDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	SamDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	SamDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

	//�T���v���[�쐬.
	if (FAILED(
		m_pDevice11->CreateSamplerState(
			&SamDesc, &m_pSampleLinear)))//(out)�T���v���[.
	{
		MessageBox(NULL, "�T���v���쐬���s", "�G���[", MB_OK);
		return E_FAIL;
	}

	//�e�N�X�`���쐬.
	if (FAILED(
		D3DX11CreateShaderResourceViewFromFile(
			m_pDevice11,		//���\�[�X���g�p����f�o�C�X�ւ̃|�C���^.
			"Data\\Image\\Explosion.png",//�t�@�C����.
			NULL, NULL,
			&m_pTexture,	//(out)�e�N�X�`��.
			NULL)))
	{
		MessageBox(NULL, "�e�N�X�`���쐬���s", "�G���[", MB_OK);
		return E_FAIL;
	}

	return S_OK;
}

//�|���S������ʂɃ����_�����O.
void clsSprite::Render()
{
	D3DXMATRIX  mView = clsResource::GetInstance()->GetModelRenderSet().mView;
	D3DXMATRIX  mProj = clsResource::GetInstance()->GetModelRenderSet().mProj;
	D3DXVECTOR3 vEye = clsResource::GetInstance()->GetModelRenderSet().vEye;

	//���[���h�s��.
	D3DXMATRIX	mWorld;

	//���[���h�ϊ�(�\���ʒu��ݒ肷��)
	D3DXMatrixIdentity(&mWorld);	//������:�P�ʍs��쐬.

	//���s�ړ�.
	D3DXMatrixTranslation(&mWorld,
		m_vPos.x, m_vPos.y, m_vPos.z);

	//�g�p����V�F�[�_�̓o�^.
	m_pDeviceContext11->VSSetShader(m_pVertexShader, NULL, 0);
	m_pDeviceContext11->PSSetShader(m_pPixelShader, NULL, 0);

	//�r���{�[�h�p.
	D3DXMATRIX CancelRotation = mView;//�r���[(�J����)�s��.
	CancelRotation._41 =
		CancelRotation._42 = CancelRotation._43 = 0;//xyz��0�ɂ���.
	//CancelRotation �̋t�s������߂�.
	D3DXMatrixInverse(&CancelRotation, NULL, &CancelRotation);
	mWorld = CancelRotation * mWorld;

	//�V�F�[�_�̃R���X�^���g�o�b�t�@�Ɋe��f�[�^��n��.
	D3D11_MAPPED_SUBRESOURCE pData;
	SPRITESHADER_CONSTANT_BUFFER cd;	//�R���X�^���g�o�b�t�@.
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
		static D3DXVECTOR4 vColor(1.0f, 1.0f, 1.0f, 1.0f);
		cd.vColor = vColor;

		//�e�N�X�`�����W.
		if (m_AnimCount % 8 == 0) {
			m_texUV.x += 1.0f / 8.0f;
			m_AnimCount = 0;
			if (m_texUV.x >= 1.0f) {
				m_texUV.x = 0.0f;
				m_texUV.y += 1.0f / 8.0f;
			}
			if (m_texUV.y >= 1.0f)
			{
				m_texUV = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f);
				m_bDispFlg = false;
			}
		}
		m_AnimCount += 4;

		cd.vUV = m_texUV;

		memcpy_s(pData.pData, pData.RowPitch,
			(void*)(&cd), sizeof(cd));

		m_pDeviceContext11->Unmap(m_pConstantBuffer, 0);
	}

	//���̃R���X�^���g�o�b�t�@���ǂ̃V�F�[�_�Ŏg�����H.
	m_pDeviceContext11->VSSetConstantBuffers(
		2, 1, &m_pConstantBuffer);
	m_pDeviceContext11->PSSetConstantBuffers(
		2, 1, &m_pConstantBuffer);

	//���_�o�b�t�@���Z�b�g.
	UINT stride = sizeof(SpriteVertex);	//�f�[�^�̊Ԋu.
	UINT offset = 0;
	m_pDeviceContext11->IASetVertexBuffers(
		0, 1, &m_pVertexBuffer, &stride, &offset);

	//���_�C���v�b�g���C�A�E�g���Z�b�g.
	m_pDeviceContext11->IASetInputLayout(m_pVertexLayout);

	//�v���~�e�B�u�E�g�|���W�[���Z�b�g.
	m_pDeviceContext11->IASetPrimitiveTopology(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//�e�N�X�`�����V�F�[�_�ɓn��.
	m_pDeviceContext11->PSSetSamplers(
		0, 1, &m_pSampleLinear);	//�T���v��-���Z�b�g.
	m_pDeviceContext11->PSSetShaderResources(
		0, 1, &m_pTexture);		//�e�N�X�`�����V�F�[�_�ɓn��.

	//�A���t�@�u�����h�p�u�����h�X�e�[�g�쐬���ݒ�.
	SetBlend(true);

	//�v���~�e�B�u�������_�����O.
	m_pDeviceContext11->Draw(4, 0);

	//�A���t�@�u�����h�𖳌��ɂ���.
	SetBlend(false);
}