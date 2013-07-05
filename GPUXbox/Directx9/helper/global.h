#pragma once
#include <xtl.h>
#include <d3d9.h>
#include <d3dx9.h>

// Used on xbox to create a linear format
#define D3DFMT(x)	(D3DFORMAT)MAKELINFMT(x)

extern LPDIRECT3DDEVICE9 pD3Ddevice;

extern LPDIRECT3DVERTEXSHADER9      pFramebufferVertexShader; // Vertex Shader
extern LPDIRECT3DPIXELSHADER9       pFramebufferPixelShader;  // Pixel Shader
extern IDirect3DVertexDeclaration9* pFramebufferVertexDecl;

#define D3DBLEND_UNK	D3DSTENCILOP_FORCE_DWORD