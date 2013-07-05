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

#ifdef _WIN32
#define SHADERLOG
#endif

#include <map>

#include "math/lin/matrix4x4.h"

#include "Core/Reporting.h"
#include "GPUXbox/GPUState.h"
#include "GPUXbox/ge_constants.h"
#include "GPUXbox/Directx9/ShaderManager.h"
#include "GPUXbox/Directx9/TransformPipeline.h"

Shader::Shader(const char *code, uint32_t shaderType, bool useHWTransform) : failed_(false), useHWTransform_(useHWTransform) {
}

Shader::~Shader() {
}

LinkedShader::LinkedShader(Shader *vs, Shader *fs, bool useHWTransform)
		: program(0), dirtyUniforms(0), useHWTransform_(useHWTransform) {
}

LinkedShader::~LinkedShader() {
}

// Utility
static void SetColorUniform3(int uniform, u32 color) {
}

static void SetColorUniform3Alpha(int uniform, u32 color, u8 alpha) {
}

// This passes colors unscaled (e.g. 0 - 255 not 0 - 1.)
static void SetColorUniform3Alpha255(int uniform, u32 color, u8 alpha) {
}

static void SetColorUniform3ExtraFloat(int uniform, u32 color, float extra) {
}

static void ConvertMatrix4x3To4x4(const float *m4x3, float *m4x4) {
}

static void SetMatrix4x3(int uniform, const float *m4x3) {
}

void LinkedShader::use() {
}

void LinkedShader::stop() {
}

void LinkedShader::updateUniforms() {
}

ShaderManager::ShaderManager() : lastShader(NULL), globalDirty(0xFFFFFFFF), shaderSwitchDirty(0) {
}

ShaderManager::~ShaderManager()  {
}


void ShaderManager::DirtyUniform(u32 what) {
}

void ShaderManager::Clear() {
}

void ShaderManager::ClearCache(bool deleteThem) {
}


void ShaderManager::DirtyShader() {
}

void ShaderManager::EndFrame() { 
}


LinkedShader *ShaderManager::ApplyShader(int prim) {
	return NULL;
}
