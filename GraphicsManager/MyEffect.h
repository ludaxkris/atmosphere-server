#pragma once
#include "MyEffect.h"
#include <d3d9.h>
#include <d3dx9.h>
#include <dxerr.h>

class MyEffect 
{

	public:
		ID3DXEffect * effect;

		MyEffect();
		void CompileEffect(LPDIRECT3DDEVICE9 g_pd3dDevice, const char * filename);
};
