#include "binding-util.h"
#include "binding-types.h"
#include "pipe.h"
#include "debugwriter.h"
#include "i18n.h"
#include "define.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL.h>
#include <SDL3_net/SDL_net.h>

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
	return Qnil;
}

RB_METHOD(journalSetLang){
	RB_UNUSED_PARAM;
	const char *lang;
	rb_get_args(argc, argv, "z", &lang RB_ARG_END);
	return Qnil;
}

RB_METHOD(journalActive){
	RB_UNUSED_PARAM;
	return Qfalse;
}

void journalBindingInit(){
	if (!NET_Init()) {
		Debug() << "NET_Init() failed: " << SDL_GetError();
	}
	VALUE module = rb_define_module("Journal");
	_rb_define_module_function(module, "set", journalSet);
	_rb_define_module_function(module, "active?", journalActive);
	_rb_define_module_function(module, "setLang", journalSetLang);
}
