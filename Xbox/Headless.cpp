// Headless version of PPSSPP, for testing using http://code.google.com/p/pspautotests/ .
// See headless.txt.
// To build on non-windows systems, just run CMake in the SDL directory, it will build both a normal ppsspp and the headless version.

#include <stdio.h>

#include "Core/Config.h"
#include "Core/Core.h"
#include "Core/CoreTiming.h"
#include "Core/System.h"
#include "Core/HLE/sceUtility.h"
#include "Core/MIPS/MIPS.h"
#include "Core/Host.h"
#include "Log.h"
#include "LogManager.h"
#include "native/input/input_state.h"

#include "Compare.h"
#include "StubHost.h"
#include "WindowsHeadlessHost.h"


// Bad !!
extern LPDIRECT3DDEVICE9 pD3Ddevice;
void DirectxInit();

// 1 megabyte
#define MB	(1024*1024)

// Add one line of text to the output buffer.
#define AddStr(a,b) (pstrOut += wsprintf( pstrOut, a, b ))

void displaymem()
{
    MEMORYSTATUS stat;
    CHAR strOut[1024], *pstrOut;

    // Get the memory status.
    GlobalMemoryStatus( &stat );

    // Setup the output string.
    pstrOut = strOut;
    AddStr( "%4u total MB of virtual memory.\n", stat.dwTotalVirtual / MB );
    AddStr( "%4u  free MB of virtual memory.\n", stat.dwAvailVirtual / MB );
    AddStr( "%4u total MB of physical memory.\n", stat.dwTotalPhys / MB );
    AddStr( "%4u  free MB of physical memory.\n", stat.dwAvailPhys / MB );
    AddStr( "%4u total MB of paging file.\n", stat.dwTotalPageFile / MB );
    AddStr( "%4u  free MB of paging file.\n", stat.dwAvailPageFile / MB );
    AddStr( "%4u  percent of memory is in use.\n", stat.dwMemoryLoad );

    // Output the string.
    OutputDebugString( strOut );
}


class PrintfLogger : public LogListener
{
public:
	void Log(LogTypes::LOG_LEVELS level, const char *msg)
	{
		switch (level)
		{
		case LogTypes::LVERBOSE:
			fprintf(stderr, "V %s", msg);
			break;
		case LogTypes::LDEBUG:
			fprintf(stderr, "D %s", msg);
			break;
		case LogTypes::LINFO:
			fprintf(stderr, "I %s", msg);
			break;
		case LogTypes::LERROR:
			fprintf(stderr, "E %s", msg);
			break;
		case LogTypes::LWARNING:
			fprintf(stderr, "W %s", msg);
			break;
		case LogTypes::LNOTICE:
		default:
			fprintf(stderr, "N %s", msg);
			break;
		}
	}
};


struct InputState;
// Temporary hack around annoying linking error.
void GL_SwapBuffers() { 
	pD3Ddevice->Present(0, 0, 0, 0);
	DebugBreak();
}
void NativeUpdate(InputState &input_state) { }
void NativeRender() { }

#ifndef _WIN32
InputState input_state;
#endif

void printUsage(const char *reason)
{
	if (reason != NULL)
		fprintf(stderr, "Error: %s\n\n", reason);
	fprintf(stderr, "PPSSPP Headless\n");
	fprintf(stderr, "This is primarily meant as a non-interactive test tool.\n\n");
	fprintf(stderr, "Options:\n");
	fprintf(stderr, "  -m, --mount umd.cso   mount iso on umd:\n");
	fprintf(stderr, "  -l, --log             full log output, not just emulated printfs\n");

	HEADLESSHOST_CLASS h1;
	HeadlessHost h2;
	if (typeid(h1) != typeid(h2))
	{
		fprintf(stderr, "  --graphics            use the full gpu backend (slower)\n");
		fprintf(stderr, "  --screenshot=FILE     compare against a screenshot\n");
	}

	fprintf(stderr, "  -i                    use the interpreter\n");
	fprintf(stderr, "  -j                    use jit (default)\n");
	fprintf(stderr, "  -c, --compare         compare with output in file.expected\n");
	fprintf(stderr, "\nSee headless.txt for details.\n");
}



int main(int argc, const char* argv[])
{
	bool fullLog = true;
	bool useJit = true;
	bool autoCompare = false;
	bool useGraphics = false;
	
	const char *bootFilename = 0;
	const char *mountIso = 0;
	const char *screenshotFilename = 0;
	bool readMount = false;

	displaymem();

	useJit = false;
	//bootFilename = "game:\\cube.elf";
	//bootFilename = "game:\\string.prx";
	bootFilename = "game:\\PSP\\GAME\\PSPRICK\\EBOOT.PBP";

	if (!bootFilename)
	{
		printUsage("No executable specified");
		return 1;
	}

	DirectxInit();

	HeadlessHost *headlessHost = new HEADLESSHOST_CLASS();
	host = headlessHost;

	std::string error_string;
	bool glWorking = host->InitGL(&error_string);

	LogManager::Init();
	LogManager *logman = LogManager::GetInstance();
	
	
	/*
	PrintfLogger *printfLogger = new PrintfLogger();
	for (int i = 0; i < LogTypes::NUMBER_OF_LOGS; i++)
	{
		LogTypes::LOG_TYPE type = (LogTypes::LOG_TYPE)i;
		logman->SetEnable(type, fullLog);
		logman->SetLogLevel(type, LogTypes::LDEBUG);
		logman->AddListener(type, printfLogger);
	}
	*/
	CoreParameter coreParameter;
	coreParameter.cpuCore = CPU_INTERPRETER;
	coreParameter.gpuCore = GPU_NULL;
	coreParameter.enableSound = false;
	coreParameter.fileToStart = bootFilename;
	coreParameter.mountIso = mountIso ? mountIso : "";
	coreParameter.startPaused = false;
	coreParameter.enableDebugging = false;
	coreParameter.printfEmuLog = true;
	coreParameter.headLess = true;
	coreParameter.renderWidth = 480;
	coreParameter.renderHeight = 272;
	coreParameter.outputWidth = 480;
	coreParameter.outputHeight = 272;
	coreParameter.pixelWidth = 480;
	coreParameter.pixelHeight = 272;
	coreParameter.unthrottle = true;

	g_Config.bEnableSound = false;
	g_Config.bFirstRun = false;
	g_Config.bIgnoreBadMemAccess = true;
	// Never report from tests.
	g_Config.sReportHost = "";
	g_Config.bAutoSaveSymbolMap = false;
	g_Config.bBufferedRendering = true;
	g_Config.bHardwareTransform = false;
#ifdef USING_GLES2
	g_Config.iAnisotropyLevel = 0;
#else
	g_Config.iAnisotropyLevel = 8;
#endif
	g_Config.bVertexCache = true;
	g_Config.bTrueColor = true;
	g_Config.ilanguage = PSP_SYSTEMPARAM_LANGUAGE_ENGLISH;
	g_Config.iTimeFormat = PSP_SYSTEMPARAM_TIME_FORMAT_24HR;
	g_Config.bEncryptSave = true;
	g_Config.sNickName = "shadow";
	g_Config.iTimeZone = 60;
	g_Config.iDateFormat = PSP_SYSTEMPARAM_DATE_FORMAT_DDMMYYYY;
	g_Config.iButtonPreference = PSP_SYSTEMPARAM_BUTTON_CROSS;
	g_Config.iLockParentalLevel = 9;

#if defined(ANDROID)
#elif defined(BLACKBERRY) || defined(__SYMBIAN32__)
#elif !defined(_WIN32)
	g_Config.memCardDirectory = std::string(getenv("HOME"))+"/.ppsspp/";
	g_Config.flashDirectory = g_Config.memCardDirectory+"/flash/";
#endif

	if (!PSP_Init(coreParameter, &error_string)) {
		fprintf(stderr, "Failed to start %s. Error: %s\n", coreParameter.fileToStart.c_str(), error_string.c_str());
		printf("TESTERROR\n");
		return 1;
	}

	host->BootDone();

	if (screenshotFilename != 0)
		headlessHost->SetComparisonScreenshot(screenshotFilename);

	coreState = CORE_RUNNING;
	while (coreState == CORE_RUNNING)
	{
		// Run for a frame at a time, just because.
		u64 nowTicks = CoreTiming::GetTicks();
		u64 frameTicks = usToCycles(1000000/60);
		mipsr4k.RunLoopUntil(nowTicks + frameTicks);

		// If we were rendering, this might be a nice time to do something about it.
		if (coreState == CORE_NEXTFRAME) {
			coreState = CORE_RUNNING;
			headlessHost->SwapBuffers();
		}
	}

	host->ShutdownGL();
	PSP_Shutdown();

	delete host;
	host = NULL;
	headlessHost = NULL;

	if (autoCompare)
		CompareOutput(bootFilename);

	return 0;
}

