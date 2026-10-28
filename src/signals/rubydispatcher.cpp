#include "rubydispatcher.h"
#include <utility>

RubyDispatcher::RubyDispatcher() : mutex(SDL_CreateMutex()){
    if (mutex == nullptr) {
        // TODO: error handling
    }
}

RubyDispatcher::~RubyDispatcher(){
    SDL_DestroyMutex(mutex);
}

void RubyDispatcher::invoke(std::function<void()> fn){
    SDL_LockMutex(mutex);

    try {
        queue.push(std::move(fn));
    } catch (...) {
        SDL_UnlockMutex(mutex);
        throw;
    }

    SDL_UnlockMutex(mutex);
}

void RubyDispatcher::process(){
    std::queue<std::function<void()>> local;

    SDL_LockMutex(mutex);
    queue.swap(local);
    SDL_UnlockMutex(mutex);

    while (!local.empty()) {
        local.front()();
        local.pop();
    }
}
