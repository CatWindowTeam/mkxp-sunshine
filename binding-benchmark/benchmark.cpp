#include "binding.h"
#include "debugwriter.h"
#include "sharedstate.h"
#include "eventthread.h"
#include "graphics.h"
#include <chrono>
#include <stdio.h>
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
		Debug() << #code << " took: " << duration.count() << " ms"; \
    }catch (const char* msg) { \
        printf(msg); \
    } \
} \


static void mriBindingExecute(){
	BENCHMARK( Debug() << "TEST"; )
	BENCHMARK( shState->checkReset(); )
	BENCHMARK( shState->checkShutdown(); )
	shState->rtData().rqTermAck.set();
}

static void mriBindingTerminate(){}
static void mriBindingReset(){}
static void mriBindingGc(){}
