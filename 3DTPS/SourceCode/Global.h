#ifndef _GLOBAL_H_	//�C���N���[�h�K�[�h.
#define _GLOBAL_H_

//�x���ɂ��ẴR�[�h���͂𖳌��ɂ���.4005:�Ē�`.
#pragma warning( disable:4005 )

//======================================
//	�C���N���[�h.
//======================================
#include <Windows.h>

#include <D3DX11.h>
#include <D3D11.h>
#include <D3DX10.h>//�uD3DX�`�v�̒�`�g�p���ɕK�v.
#include <D3D10.h>
#include <memory>
#include <vector>

#include "MyMacro.h"

using namespace std;

//======================================
//	���C�u����.
//======================================
#pragma comment( lib, "winmm.lib" )
#pragma comment( lib, "d3dx11.lib" )
#pragma comment( lib, "d3d11.lib" )
#pragma comment( lib, "d3dx10.lib" )//�uD3DX�`�v�̒�`�g�p���ɕK�v.

//======================================
//	�萔.
//======================================
#define WND_W	1280	//�E�B���h�E��.
#define WND_H	720		//�E�B���h�E����.

#endif//#ifndef _GLOBAL_H_