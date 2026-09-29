#include "binding-util.h"
#include "binding-types.h"
#include "sharedstate.h"
#include "eventthread.h"
#include "debugwriter.h"
#include "define.h"
#include <SDL3/SDL.h>
#include "config.h"
static char lang[4] = "en";

#ifdef JOURNAL_ENABLED

#include <SDL3_net/SDL_net.h>
NET_Address* addr = NULL;
void SendRaw(const char *raw){
    if(NET_WaitUntilResolved(addr, -1) == NET_FAILURE){
        Debug() << "NET_FAILURE " << conf.journal_address << " - " <<  SDL_GetError();
        return;
    }

    NET_StreamSocket *socket = NET_CreateClient(addr, conf.journal_port, 0);
    if (!socket) {
        Debug() << "Failed to create connection: " <<  SDL_GetError();
        return;
    }

    if (NET_WaitUntilConnected(socket, -1) == NET_FAILURE) {
        Debug() << "Failed to connect to " <<  conf.journal_address << ":" <<  conf.journal_port << " " << SDL_GetError();
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

static VALUE journalSet(int argc, VALUE *argv, VALUE self){
	const char *name;
	rb_get_args(argc, argv, "z", &name RB_ARG_END);
	static char buffer[1024];
	if(SDL_strcmp(lang, "en") == 0)
		SDL_snprintf(buffer, 1024, "Journal/%s.png", name);
	else
		SDL_snprintf(buffer, 1024, "Journal/%s/%s.png", lang, name);
	SendRaw(buffer);
	return Qnil;
}


static VALUE journalLangSet(int argc, VALUE *argv, VALUE self){
	const char *name;
    rb_get_args(argc, argv, "z", &name RB_ARG_END);
	SDL_snprintf(lang, 4, "%s", name);
	return Qnil;
}

static VALUE journalActive(VALUE self) {
    constexpr char ping[] = "PING";
    constexpr char expectedPong[] = "PONG";
    NET_StreamSocket *socket = NET_CreateClient(addr, conf.journal_port, 0);
    if (!socket) {
        return Qfalse;
    }

    if (NET_WaitUntilConnected(socket, 5000) == NET_FAILURE) {
        NET_DestroyStreamSocket(socket);
        return Qfalse;
    }

    if (!NET_WriteToStreamSocket(socket, ping, sizeof(ping) - 1)) {
        NET_DestroyStreamSocket(socket);
        return Qfalse;
    }

    if (NET_WaitUntilStreamSocketDrained(socket, 5000) < 0) {
        NET_DestroyStreamSocket(socket);
        return Qfalse;
    }

    void *sockets[] = { socket };
    if (NET_WaitUntilInputAvailable(sockets, 1, 5000) <= 0) {
        NET_DestroyStreamSocket(socket);
        return Qfalse;
    }

    char response[sizeof(expectedPong) - 1] = {};
    int received = NET_ReadFromStreamSocket(socket, response, sizeof(response) - 1);
    NET_DestroyStreamSocket(socket);
    if (received != sizeof(expectedPong) - 1) {
        return Qfalse;
    }

    return SDL_memcmp(response, expectedPong, sizeof(expectedPong) - 1) == 0 ? Qtrue : Qfalse;
}


#else
	static VALUE journalSet(int argc, VALUE *argv, VALUE self){ return Qnil; }
	static VALUE journalLangSet(int argc, VALUE *argv, VALUE self){ return Qnil; }
	static VALUE journalActive(VALUE self){ return Qtrue; }
#endif

void journalBindingInit(){
	#ifdef JOURNAL_ENABLED
		if (!NET_Init()) {
			Debug() << "NET_Init() failed: " << SDL_GetError();
		}else{
			addr = NET_ResolveHostname(conf.journal_address.c_str());
			NET_WaitUntilResolved(addr, 1000);
		}
	#endif
	VALUE module = rb_define_module("Journal");
	rb_define_module_function(module, "set", RUBY_METHOD_FUNC(journalSet), -1);
	rb_define_module_function(module, "setLang", RUBY_METHOD_FUNC(journalLangSet), -1);
	rb_define_module_function(module, "active?", RUBY_METHOD_FUNC(journalActive), 0);
}
