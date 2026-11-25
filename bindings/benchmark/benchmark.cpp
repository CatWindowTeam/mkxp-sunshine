#include "binding.h"
#include "logger.h"
#include "sharedstate.h"
#include "eventthread.h"
#include "graphics.h"
#include "meow.h"
#include "bitmap.h"
#include "etc.h"
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
	Color color = Color(255, 255, 255, 255);
	Tone tone = Tone(255, 255, 255, 255);
	Rect rect = Rect(0, 0, 0, 0);

	BENCHMARK( Debug_Log("TEST"); )
	BENCHMARK( Warn_Log("TEST"); )
	BENCHMARK( Error_Log("TEST"); )
	BENCHMARK( Log("TEST"); )

	BENCHMARK( set_crash_info("%i", loglevel); )

	BENCHMARK( testbitmap->drawText(0, 0, 640, 480, "TEST", 1); )

	BENCHMARK( Log_Debug(testbitmap->width()); )
	BENCHMARK( Log_Debug(testbitmap->height()); )

	BENCHMARK( testbitmap->blt(0, 0, testbitmap, testbitmap->rect()); )
	BENCHMARK( testbitmap->clearRect(0, 0, 640, 480); )
	BENCHMARK( testbitmap->blur(); )
	BENCHMARK( testbitmap->hueChange(100); )
	BENCHMARK( testbitmap->ensureNonMega(); )

	BENCHMARK( shState->checkReset(); )
	BENCHMARK( shState->checkShutdown(); )

	BENCHMARK( color.set(0, 0, 0, 0); )
	BENCHMARK( color.setRed(1); )
	BENCHMARK( color.setGreen(1); )
	BENCHMARK( color.setBlue(1); )
	BENCHMARK( color.setAlpha(1); )

	BENCHMARK( Log_Debug(color.getRed()); )
	BENCHMARK( Log_Debug(color.getGreen()); )
	BENCHMARK( Log_Debug(color.getBlue()); )
	BENCHMARK( Log_Debug(color.getAlpha()); )
	BENCHMARK( Log_Debug(color.serialSize()); )

        BENCHMARK( tone.set(0, 0, 0, 0); )
        BENCHMARK( tone.setRed(1); )
        BENCHMARK( tone.setGreen(1); )
        BENCHMARK( tone.setBlue(1); )
        BENCHMARK( tone.setAlpha(1); )

        BENCHMARK( Log_Debug(tone.getRed()); )
        BENCHMARK( Log_Debug(tone.getGreen()); )
        BENCHMARK( Log_Debug(tone.getBlue()); )
        BENCHMARK( Log_Debug(tone.getAlpha()); )
        BENCHMARK( Log_Debug(tone.serialSize()); )

	BENCHMARK( rect.set(0, 0, 0, 0); )
        BENCHMARK( rect.setX(1); )
        BENCHMARK( rect.setY(1); )
	BENCHMARK( rect.setWidth(1); )
	BENCHMARK( rect.setHeight(1); )
	shState->rtData().rqTermAck.set();
}

static void mriBindingTerminate(){}
static void mriBindingReset(){}
static void mriBindingGc(){}
