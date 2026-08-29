#include "binding-util.h"
#include "binding-types.h"
#include "pipe.h"
#include "debugwriter.h"
#include "i18n.h"
#include "define.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL.h>
#include <SDL3_net/SDL_net.h>
#include "sharedstate.h"
#include "eventthread.h"
#include "config.h"
static char lang[4] = "en";
static char addr[65] = "127.0.0.1";
static unsigned int port = 0;
void SendRaw(const char *address, int port, const char *raw){
    NET_Address *addr = NET_ResolveHostname(address);
    if (!addr) {
        Debug() << "Failed to resolve " << address << " : " <<  SDL_GetError();
        return;
    }

    if (NET_WaitUntilResolved(addr, -1) == NET_FAILURE) {
        Debug() << "NET_FAILURE " << address << " : " <<  SDL_GetError();
        return;
    }

    NET_StreamSocket *socket = NET_CreateClient(addr, port, 0);
    if (!socket) {
        Debug() << "Failed to create connection: " <<  SDL_GetError();
        return;
    }

    if (NET_WaitUntilConnected(socket, -1) == NET_FAILURE) {
        Debug() << "Failed to connect to " <<  address << ":" <<  port << " " << SDL_GetError();
        NET_DestroyStreamSocket(socket);
        return;
    }

    int length = (int)SDL_strlen(raw);

    if (!NET_WriteToStreamSocket(socket, raw, length)) {
       Debug() << "Failed to send: " <<  SDL_GetError();
    } else if (NET_WaitUntilStreamSocketDrained(socket, -1) < 0) {
        Debug() << "Error: " <<  SDL_GetError();
    }
    NET_DestroyStreamSocket(socket);
}

RB_METHOD(journalSet){
	RB_UNUSED_PARAM;
	const char *name;
	rb_get_args(argc, argv, "z", &name RB_ARG_END);
	static char buffer[1024];
	if(SDL_strcmp(lang, "en") == 0)
		SDL_snprintf(buffer, 1024, "Journal/%s.png", name);
	else
		SDL_snprintf(buffer, 1024, "Journal/%s/%s.png", lang, name);
	SendRaw(addr, port, buffer);
	return Qnil;
}

RB_METHOD(journalLangSet){
        RB_UNUSED_PARAM;
        const char *name;
        rb_get_args(argc, argv, "z", &name RB_ARG_END);
	SDL_snprintf(lang, 4, "%s", name);
        return Qnil;
}

RB_METHOD(journalActive){
	RB_UNUSED_PARAM;
	return Qtrue;
}

void journalBindingInit(){
	const static Config &conf = shState->rtData().config;
	SDL_snprintf(addr, 65, "%s", conf.journal_address.c_str());
	port = conf.journal_port;
	if (!NET_Init()) {
		Debug() << "NET_Init() failed: " << SDL_GetError();
	}
	VALUE module = rb_define_module("Journal");
	_rb_define_module_function(module, "set", journalSet);
	_rb_define_module_function(module, "setLang", journalLangSet);
	_rb_define_module_function(module, "active?", journalActive);
}
