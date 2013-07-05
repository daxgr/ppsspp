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

#include "WindowsHeadlessHost.h"
#include "Compare.h"

#include <stdio.h>
#include <xtl.h>
#include <io.h>

#include "file/vfs.h"
#include "file/zip_read.h"

// Bad !!
extern LPDIRECT3DDEVICE9 pD3Ddevice;
void DirectxInit();

const bool WINDOW_VISIBLE = false;
const int WINDOW_WIDTH = 480;
const int WINDOW_HEIGHT = 272;

void WindowsHeadlessHost::LoadNativeAssets()
{
	/*
	VFSRegister("", new DirectoryAssetReader("assets/"));
	VFSRegister("", new DirectoryAssetReader(""));
	VFSRegister("", new DirectoryAssetReader("../"));
	VFSRegister("", new DirectoryAssetReader("../Windows/assets/"));
	VFSRegister("", new DirectoryAssetReader("../Windows/"));

	// See SendDebugOutput() for how things get back on track.
	*/
}

void WindowsHeadlessHost::SendDebugOutput(const std::string &output)
{
	OutputDebugString(output.c_str());
}

void WindowsHeadlessHost::SendDebugScreenshot(const u8 *pixbuf, u32 w, u32 h)
{
	
}

void WindowsHeadlessHost::SetComparisonScreenshot(const std::string &filename)
{
	comparisonScreenshot = filename;
}

bool WindowsHeadlessHost::InitGL(std::string *error_message)
{
	
	LoadNativeAssets();

	return ResizeGL();
}

void WindowsHeadlessHost::ShutdownGL()
{
}

bool WindowsHeadlessHost::ResizeGL()
{
	return true;
}

void WindowsHeadlessHost::SwapBuffers()
{
	pD3Ddevice->Present(0, 0, 0, 0);
	
	pD3Ddevice->Clear(0, NULL, D3DCLEAR_STENCIL|D3DCLEAR_TARGET |D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1, 0);
}
