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

#include <stdio.h>
#include <locale.h>

#if defined(_WIN32) && defined(_DEBUG) && !defined(_XBOX)
#include <windows.h>
#endif

#ifdef _XBOX
#include <xtl.h>
#endif

#include "../ge_constants.h"
#include "../GPUState.h"
#include "../../Core/Config.h"

#include "VertexShaderGenerator.h"

#undef WRITE

#define WRITE p+=sprintf

bool CanUseHardwareTransform(int prim) {
	if (!g_Config.bHardwareTransform)
		return false;
	return !gstate.isModeThrough() && prim != GE_PRIM_RECTANGLES;
}

// prim so we can special case for RECTANGLES :(
void ComputeVertexShaderID(VertexShaderID *id, int prim, bool useHWTransform) {
	const u32 vertType = gstate.vertType;
	int doTexture = gstate.isTextureMapEnabled() && !gstate.isModeClear();
	bool doTextureProjection = gstate.getUVGenMode() == 1;

	bool hasColor = (vertType & GE_VTYPE_COL_MASK) != 0;
	bool hasNormal = (vertType & GE_VTYPE_NRM_MASK) != 0;
	bool hasBones = (vertType & GE_VTYPE_WEIGHT_MASK) != 0;
	bool enableFog = gstate.isFogEnabled() && !gstate.isModeThrough() && !gstate.isModeClear();
	bool lmode = (gstate.lmode & 1) && gstate.isLightingEnabled();

	memset(id->d, 0, sizeof(id->d));
	id->d[0] = lmode & 1;
	id->d[0] |= ((int)gstate.isModeThrough()) << 1;
	id->d[0] |= ((int)enableFog) << 2;
	id->d[0] |= doTexture << 3;
	id->d[0] |= (hasColor & 1) << 4;
	if (doTexture) {
		id->d[0] |= (gstate_c.flipTexture & 1) << 5;
		id->d[0] |= (doTextureProjection & 1) << 6;
	}

	if (useHWTransform) {
		id->d[0] |= 1 << 8;
		id->d[0] |= (hasNormal & 1) << 9;
		id->d[0] |= (hasBones & 1) << 10;

		// UV generation mode
		id->d[0] |= gstate.getUVGenMode() << 16;

		// The next bits are used differently depending on UVgen mode
		if (gstate.getUVGenMode() == 1) {
			id->d[0] |= gstate.getUVProjMode() << 18;
		} else if (gstate.getUVGenMode() == 2) {
			id->d[0] |= gstate.getUVLS0() << 18;
			id->d[0] |= gstate.getUVLS1() << 20;
		}

		// Bones
		id->d[0] |= (gstate.getNumBoneWeights() - 1) << 22;

		// Okay, d[1] coming up. ==============

		id->d[1] |= gstate.isLightingEnabled() << 24;
		id->d[1] |= ((vertType & GE_VTYPE_WEIGHT_MASK) >> GE_VTYPE_WEIGHT_SHIFT) << 25;
		if (gstate.isLightingEnabled() || gstate.getUVGenMode() == 2) {
			// Light bits
			for (int i = 0; i < 4; i++) {
				id->d[1] |= (gstate.ltype[i] & 3) << (i * 4);
				id->d[1] |= ((gstate.ltype[i] >> 8) & 3) << (i * 4 + 2);
			}
			id->d[1] |= (gstate.materialupdate & 7) << 16;
			for (int i = 0; i < 4; i++) {
				id->d[1] |= (gstate.lightEnable[i] & 1) << (20 + i);
			}
		}
	}
}

static const char * const boneWeightAttrDecl[8] = {
	"attribute mediump float a_w1;\n",
	"attribute mediump vec2 a_w1;\n",
	"attribute mediump vec3 a_w1;\n",
	"attribute mediump vec4 a_w1;\n",
	"attribute mediump vec4 a_w1;\nattribute mediump float a_w2;\n",
	"attribute mediump vec4 a_w1;\nattribute mediump vec2 a_w2;\n",
	"attribute mediump vec4 a_w1;\nattribute mediump vec3 a_w2;\n",
	"attribute mediump vec4 a_w1;\nattribute mediump vec4 a_w2;\n",
};

enum DoLightComputation {
	LIGHT_OFF,
	LIGHT_SHADE,
	LIGHT_FULL,
};

void GenerateVertexShader(int prim, char *buffer, bool useHWTransform) {
}

