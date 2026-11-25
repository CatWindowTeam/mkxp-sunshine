#include "binding.h"
#include "logger.h"
#include "sharedstate.h"
#include "eventthread.h"
#include "graphics.h"
#include "meow.h"
#include "bitmap.h"
#include <chrono>
static void mriBindingExecute();
static void mriBindingTerminate();
static void mriBindingReset();
static void mriBindingGc();
using namespace std;
using namespace std::chrono;

ScriptBinding scriptBindingImpl = {
	mriBindingExecute,
	mriBindingTerminate,
	mriBindingReset,
	mriBindingGc
};
ScriptBinding *scriptBinding = &scriptBindingImpl;

#define BENCHMARK(code) \
{\
    try {\
    	for (int i = 0; i < 10; i++){ code } \
		auto start = high_resolution_clock::now(); \
		for (int i = 0; i < 30; i++){ code } \
		auto stop = high_resolution_clock::now(); \
		auto duration = duration_cast<microseconds>(stop - start); \
		Log(#code, " took: ", duration.count(), " ms"); \
    }catch (const char* msg) { \
        Log(msg); \
    } \
} \


static void mriBindingExecute(){
	Bitmap *testbitmap = new Bitmap(640, 480);
	BENCHMARK( Debug_Log("TEST"); )
	BENCHMARK( Warn_Log("TEST"); )
	BENCHMARK( Error_Log("TEST"); )
	BENCHMARK( Log("TEST"); )
	BENCHMARK( set_crash_info("%i", loglevel); )
	BENCHMARK( testbitmap->drawText(0, 0, 640, 480, "TEST", 1); )
	BENCHMARK( shState->checkReset(); )
	BENCHMARK( shState->checkShutdown(); )
	shState->rtData().rqTermAck.set();
}

static void mriBindingTerminate(){}
static void mriBindingReset(){}
static void mriBindingGc(){}
