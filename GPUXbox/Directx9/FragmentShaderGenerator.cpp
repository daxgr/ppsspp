// Copyright (c) 2012- PPSSPP Project.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, version 2.0 or later versions.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License 2.0 for more details.

// A copy of the GPL 2.0 should have been included with the program.
// If not, see http://www.gnu.org/licenses/

// Official git repository and contact information can be found at
// https://github.com/hrydgard/ppsspp and http://www.ppsspp.org/.

#if defined(USING_GLES2)
#define GLSL_ES_1_0
#else
#define GLSL_1_3

// SDL 1.2 on Apple does not have support for OpenGL 3 and hence needs
// special treatment in the shader generator.
#if defined(__APPLE__)
#define FORCE_OPENGL_2_0
#endif
#endif

#include "FragmentShaderGenerator.h"
#include "../ge_constants.h"
#include "../GPUState.h"
#include <cstdio>

#define WRITE p+=sprintf

// #define DEBUG_SHADER

// GL_NV_shader_framebuffer_fetch looks interesting....

static bool IsAlphaTestTriviallyTrue() {
	int alphaTestFunc = gstate.alphatest & 7;
	int alphaTestRef = (gstate.alphatest >> 8) & 0xFF;
	
	switch (alphaTestFunc) {
	case GE_COMP_ALWAYS:
		return true;
	case GE_COMP_GEQUAL:
		if (alphaTestRef == 0)
			return true;

	// This breaks the trees in MotoGP, for example.
	// case GE_COMP_GREATER:
	//if (alphaTestRef == 0 && (gstate.alphaBlendEnable & 1) && gstate.getBlendFuncA() == GE_SRCBLEND_SRCALPHA && gstate.getBlendFuncB() == GE_SRCBLEND_INVSRCALPHA)
	//	return true;

	case GE_COMP_LEQUAL:
		if (alphaTestRef == 255)
			return true;
	default:
		return false;
	}
}

static bool IsColorTestTriviallyTrue() {
	int colorTestFunc = gstate.colortest & 3;
	switch (colorTestFunc) {
	case GE_COMP_ALWAYS:
		return true;
	default:
		return false;
	}
}

static bool CanDoubleSrcBlendMode() {
	if (!gstate.isAlphaBlendEnabled()) {
		return false;
	}

	int funcA = gstate.getBlendFuncA();
	int funcB = gstate.getBlendFuncB();
	if (funcA != GE_SRCBLEND_DOUBLESRCALPHA) {
		funcB = funcA;
		funcA = gstate.getBlendFuncB();
	}
	if (funcA != GE_SRCBLEND_DOUBLESRCALPHA) {
		return false;
	}

	// One side should be doubled.  Let's check the other side.
	// LittleBigPlanet, for example, uses 2.0 * src, 1.0 - src, which can't double.
	switch (funcB) {
	case GE_DSTBLEND_SRCALPHA:
	case GE_DSTBLEND_INVSRCALPHA:
		return false;

	default:
		return true;
	}
}


// Here we must take all the bits of the gstate that determine what the fragment shader will
// look like, and concatenate them together into an ID.
void ComputeFragmentShaderID(FragmentShaderID *id) {
	memset(&id->d[0], 0, sizeof(id->d));
	if (gstate.clearmode & 1) {
		// We only need one clear shader, so let's ignore the rest of the bits.
		id->d[0] = 1;
	} else {
		int lmode = (gstate.lmode & 1) && gstate.isLightingEnabled();
		bool enableFog = gstate.isFogEnabled() && !gstate.isModeThrough();
		bool enableAlphaTest = gstate.isAlphaTestEnabled() && !IsAlphaTestTriviallyTrue();
		bool enableColorTest = gstate.isColorTestEnabled() && !IsColorTestTriviallyTrue();
		bool enableColorDoubling = (gstate.texfunc & 0x10000) != 0;
		// This isn't really correct, but it's a hack to get doubled blend modes to work more correctly.
		bool enableAlphaDoubling = CanDoubleSrcBlendMode();
		bool doTextureProjection = gstate.getUVGenMode() == 1;
		bool doTextureAlpha = (gstate.texfunc & 0x100) != 0;

		// All texfuncs except replace are the same for RGB as for RGBA with full alpha.
		if (gstate_c.textureFullAlpha && (gstate.texfunc & 0x7) != GE_TEXFUNC_REPLACE)
			doTextureAlpha = false;

		// id->d[0] |= (gstate.clearmode & 1);
		if (gstate.isTextureMapEnabled()) {
			id->d[0] |= 1 << 1;
			id->d[0] |= (gstate.texfunc & 0x7) << 2;
			id->d[0] |= (doTextureAlpha & 1) << 5; // rgb or rgba
		}
		id->d[0] |= (lmode & 1) << 7;
		id->d[0] |= gstate.isAlphaTestEnabled() << 8;
		if (enableAlphaTest)
			id->d[0] |= (gstate.alphatest & 0x7) << 9;	 // alpha test func
		id->d[0] |= gstate.isColorTestEnabled() << 12;
		if (enableColorTest)
			id->d[0] |= (gstate.colortest & 0x3) << 13;	 // color test func
		id->d[0] |= (enableFog & 1) << 15;
		id->d[0] |= (doTextureProjection & 1) << 16;
		id->d[0] |= (enableColorDoubling & 1) << 17;
		id->d[0] |= (enableAlphaDoubling & 1) << 18;
	}
}

// Missing: Z depth range
// Also, logic ops etc, of course. Urgh.
void GenerateFragmentShader(char *buffer) {
	char *p = buffer;
}

