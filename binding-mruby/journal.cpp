#include "eventthread.h"
#include "debugwriter.h"
#include "config.h"
#include <SDL3/SDL.h>
#include <SDL3_net/SDL_net.h>
#include <mruby.h>
static char lang[4] = "en";

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

static mrb_value journalSet(mrb_state *mrb, mrb_value self){
	const char *name;
	mrb_get_args(mrb, "z", &name);
	static char buffer[1024];
	if(SDL_strcmp(lang, "en") == 0)
		SDL_snprintf(buffer, 1024, "Journal/%s.png", name);
	else
		SDL_snprintf(buffer, 1024, "Journal/%s/%s.png", lang, name);
	SendRaw(buffer);
	return mrb_nil_value();
}


static mrb_value journalLangSet(mrb_state *mrb, mrb_value self){
	const char *name;
    mrb_get_args(mrb, "z", &name);
	SDL_snprintf(lang, 4, "%s", name);
	return mrb_nil_value();
}

static mrb_value journalActive(mrb_state *mrb, mrb_value self){
    constexpr char ping[] = "PING";
    constexpr char expectedPong[] = "PONG";
    NET_StreamSocket *socket = NET_CreateClient(addr, conf.journal_port, 0);
    if (!socket) {
        return mrb_false_value();
    }

    if (NET_WaitUntilConnected(socket, conf.journal_timeout) == NET_FAILURE) {
        NET_DestroyStreamSocket(socket);
        return mrb_false_value();
    }

    if (!NET_WriteToStreamSocket(socket, ping, sizeof(ping))) {
        NET_DestroyStreamSocket(socket);
        return mrb_false_value();
    }

    if (NET_WaitUntilStreamSocketDrained(socket, conf.journal_timeout) < 0) {
        NET_DestroyStreamSocket(socket);
        return mrb_false_value();
    }

    void *sockets[] = { socket };
    if (NET_WaitUntilInputAvailable(sockets, 1, conf.journal_timeout) <= 0) {
        NET_DestroyStreamSocket(socket);
        return mrb_false_value();
    }

    char response[sizeof(expectedPong)/* - 1*/] = {};
    int received = NET_ReadFromStreamSocket(socket, response, sizeof(response));
    NET_DestroyStreamSocket(socket);
    if (received != sizeof(expectedPong)){
        return mrb_false_value();
    }

    return SDL_memcmp(response, expectedPong, sizeof(expectedPong)) == 0 ? mrb_true_value() : mrb_false_value();
}


void journalBindingInit(mrb_state *mrb){
	if(!NET_Init()) {
		Debug() << "NET_Init() failed: " << SDL_GetError();
	}else{
		addr = NET_ResolveHostname(conf.journal_address.c_str());
		NET_WaitUntilResolved(addr, 1000);
	}
	
	struct RClass *m = mrb_define_module(mrb, "Journal");
	mrb_define_module_function(mrb, m, "set", journalSet, MRB_ARGS_REQ(1));
	mrb_define_module_function(mrb, m, "setLang", journalLangSet, MRB_ARGS_REQ(1));
	mrb_define_module_function(mrb, m, "active?", journalActive, MRB_ARGS_NONE());
}
