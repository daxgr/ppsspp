#pragma once

#include <functional>
#include <string.h>
#include "global.h"

// OpenGL state cache. Should convert all code to use this instead of directly calling glEnable etc,
// as GL state changes can be expensive on some hardware.
class DirectxState
{
private:
	template<D3DRENDERSTATETYPE cap, bool init>
	class BoolState {
		bool _value;
	public:
		BoolState() : _value(init) {
			DirectxState::state_count++;
        }

		inline void set(bool value) {
			if(value && value != _value) {
				_value = value;
				pD3Ddevice->SetRenderState(cap, true);
			}
			if(!value && value != _value) {
				_value = value;
				pD3Ddevice->SetRenderState(cap, false);
			}
		}
		inline void enable() {
			set(true);
		}
		inline void disable() {
			set(false);
		}
		operator bool() const {
			return isset();
		}
		inline bool isset() {
			return _value;
		}
		void restore() {
			if(_value)
				pD3Ddevice->SetRenderState(cap, true);
			else
				pD3Ddevice->SetRenderState(cap, false);
		}
	};

#define STATE1(state, p1def) \
	class SavedState1_##state { \
		DWORD p1; \
	public: \
		SavedState1_##state() : p1(p1def) { \
			DirectxState::state_count++; \
        }; \
		void set(DWORD newp1) { \
			if(newp1 != p1) { \
				p1 = newp1; \
				pD3Ddevice->SetRenderState(state, p1); \
			} \
		} \
		void restore() { \
			pD3Ddevice->SetRenderState(state, p1); \
		} \
	}

#define STATE2(state1, p1def, state2, p2def) \
	class SavedState2_##state1 { \
		DWORD p1; \
		DWORD p2; \
	public: \
		SavedState2_##state1() : p1(p1def), p2(p2def) { \
			DirectxState::state_count++; \
        }; \
		inline void set(DWORD newp1, DWORD newp2) { \
			if(newp1 != p1 || newp2 != p2) { \
				p1 = newp1; \
				p2 = newp2; \
				pD3Ddevice->SetRenderState(state1, p1); \
				pD3Ddevice->SetRenderState(state2, p2); \
			} \
		} \
		inline void restore() { \
			pD3Ddevice->SetRenderState(state1, p1); \
			pD3Ddevice->SetRenderState(state2, p2); \
		} \
	}

	#define STATE3(state1, p1def, state2, p2def, state3, p3def) \
	class SavedState3_##state1 { \
		DWORD p1; \
		DWORD p2; \
		DWORD p3; \
	public: \
		SavedState3_##state1() : p1(p1def), p2(p2def), p3(p3def) { \
			DirectxState::state_count++; \
        }; \
		inline void set(DWORD newp1, DWORD newp2, DWORD newp3) { \
			if(newp1 != p1 || newp2 != p2 || newp3 != p3) { \
				p1 = newp1; \
				p2 = newp2; \
				p3 = newp3; \
				pD3Ddevice->SetRenderState(state1, p1); \
				pD3Ddevice->SetRenderState(state2, p2); \
				pD3Ddevice->SetRenderState(state3, p3); \
			} \
		} \
		inline void restore() { \
			pD3Ddevice->SetRenderState(state1, p1); \
			pD3Ddevice->SetRenderState(state2, p2); \
			pD3Ddevice->SetRenderState(state3, p3); \
		} \
	}

	#define STATE4(func, p1type, p2type, p3type, p4type, p1def, p2def, p3def, p4def) \
	class SavedState4_##func { \
		p1type p1; \
		p2type p2; \
		p3type p3; \
		p4type p4; \
	public: \
		SavedState4_##func() : p1(p1def), p2(p2def), p3(p3def), p4(p4def) { \
			DirectxState::state_count++; \
        }; \
		inline void set(p1type newp1, p2type newp2, p3type newp3, p4type newp4) { \
			if(newp1 != p1 || newp2 != p2 || newp3 != p3 || newp4 != p4) { \
				p1 = newp1; \
				p2 = newp2; \
				p3 = newp3; \
				p4 = newp4; \
				func(p1, p2, p3, p4); \
			} \
		} \
		inline void restore() { \
			func(p1, p2, p3, p4); \
		} \
	}

#define STATEFLOAT4(func, def) \
	class SavedState4_##func { \
		float p[4]; \
	public: \
		SavedState4_##func() { \
			for (int i = 0; i < 4; i++) {p[i] = def;} \
			DirectxState::state_count++; \
		}; \
		inline void set(const float v[4]) { \
			if(memcmp(p,v,sizeof(float)*4)) { \
				memcpy(p,v,sizeof(float)*4); \
				func(p[0], p[1], p[2], p[3]); \
			} \
		} \
		inline void restore() { \
			func(p[0], p[1], p[2], p[3]); \
		} \
	}
	class SavedBlendFactor {
		DWORD c;
	public:
		SavedBlendFactor() {
			c = 0xFFFFFFFF;
			DirectxState::state_count++;
		}
		inline void set(const float v[4]) {
			DWORD c;
			c = D3DCOLOR_COLORVALUE(v[0] * 255, v[1] * 255, v[2] * 255, v[3] * 255);

			if (c!=this->c) {
				this->c = c;				
				pD3Ddevice->SetRenderState(D3DRS_BLENDFACTOR, c);
			}
		}
		inline void restore() {
			pD3Ddevice->SetRenderState(D3DRS_BLENDFACTOR, c);
		}
	};

	class SavedColorMask {
		DWORD mask;
	public:
		SavedColorMask() {
			mask = D3DCOLORWRITEENABLE_ALL;
			DirectxState::state_count++;
		}

		inline void set(bool r, bool g, bool b, bool a) {
			DWORD mask = 0;
			if (r) {
				mask |=D3DCOLORWRITEENABLE_RED;
			}
			if (g) {
				mask |=D3DCOLORWRITEENABLE_GREEN;
			}
			if (b) {
				mask |=D3DCOLORWRITEENABLE_BLUE;
			}
			if (a) {
				mask |=D3DCOLORWRITEENABLE_ALPHA;
			}
			if (mask != this->mask) {
				this->mask = mask;
				//pD3Ddevice->SetRenderState(D3DRS_COLORWRITEENABLE, mask);
			}
		}
		inline void restore() {
			//pD3Ddevice->SetRenderState(D3DRS_COLORWRITEENABLE, mask);
		}
	};


	class BoolUnused {
	public:
		BoolUnused() {
			DirectxState::state_count++;
		}
		inline void set(bool) {
			
		}
		inline void restore() {
			
		}

		inline void enable() {
			set(true);
		}
		inline void disable() {
			set(false);
		}
	};

	class State1Unused {
	public:
		State1Unused() {			
			DirectxState::state_count++;
		}
		inline void set(unsigned int v) {
			
		}
		inline void restore() {
			
		}
	};

	class StateDepthRange {
		float n, f;
	public:
		StateDepthRange() {
			n = 0;
			f = 1;
			DirectxState::state_count++;
		}

		inline void set(float nn, float ff) {
			n = nn;
			f = ff;
			if (n != nn || f != ff) {

			}
		}

		inline void restore() {
			
		}
	};

	class StateVp {
		D3DVIEWPORT9 viewport;
	public:
		inline void set(int x, int y, int w, int h) {

		}

		inline void restore() {

		}
	};

	bool initialized;

public:
	static int state_count;
	DirectxState() : initialized(false) {}
	void Initialize();
	void Restore();

	// When adding a state here, don't forget to add it to DirectxState::Restore() too
	BoolState<D3DRS_ALPHABLENDENABLE, false> blend;
	STATE2(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA, D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA) blendFunc;
	STATE1(D3DRS_BLENDOP, D3DBLENDOP_ADD) blendEquation;
	SavedBlendFactor blendColor;

	BoolState<D3DRS_SCISSORTESTENABLE, false> scissorTest;

	//BoolState<GL_CULL_FACE, false> cullFace;
	//BoolState<GL_DITHER, false> dither;

	BoolUnused cullFace;
	BoolUnused dither;

	//STATE1(glCullFace, GLenum, GL_FRONT) cullFaceMode;
	//STATE1(glFrontFace, GLenum, GL_CCW) frontFace;

	State1Unused cullFaceMode;
	State1Unused frontFace;

	BoolState<D3DRS_ZENABLE, false> depthTest;

	STATE1(D3DRS_ZFUNC, D3DCMP_LESS) depthFunc;
	STATE1(D3DRS_ZWRITEENABLE, TRUE) depthWrite;

	SavedColorMask colorMask;
	

	StateDepthRange depthRange;

	//STATE4(glViewport, GLint, GLint, GLsizei, GLsizei, 0, 0, 128, 128) viewport;
	//STATE4(glScissor, GLint, GLint, GLsizei, GLsizei, 0, 0, 128, 128) scissorRect;

	StateVp viewport;
	StateVp scissorRect;

	BoolState<D3DRS_STENCILENABLE, false> stencilTest;
	STATE3(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP, D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP, D3DRS_STENCILPASS, D3DSTENCILOP_KEEP) stencilOp;
	STATE3(D3DRS_STENCILFUNC, D3DCMP_ALWAYS, D3DRS_STENCILREF, 0, D3DRS_STENCILMASK, 0xFFFFFFFF) stencilFunc;

	// Only works on Win32, all other platforms are "force-vsync"
	void SetVSyncInterval(int interval);  // one of the above VSYNC, or a higher number for multi-frame waits (could be useful for 30hz games)
};

#undef STATE1
#undef STATE2

extern DirectxState dxstate;

struct GLExtensions {
	bool OES_depth24;
	bool OES_packed_depth_stencil;
	bool OES_depth_texture;
	bool EXT_discard_framebuffer;
	bool FBO_ARB;
	bool FBO_EXT;
};

extern GLExtensions gl_extensions;

void CheckGLExtensions();
