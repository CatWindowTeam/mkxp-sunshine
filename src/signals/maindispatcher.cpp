#include "maindispatcher.h"
#include <utility>

MainDispatcher::MainDispatcher() : mutex(SDL_CreateMutex()) {}

MainDispatcher::~MainDispatcher() {
    SDL_DestroyMutex(mutex);
}

void MainDispatcher::invoke(std::function<void()> fn) {
    SDL_LockMutex(mutex);
    queue.push(std::move(fn));
    SDL_UnlockMutex(mutex);
}

void MainDispatcher::process() {
    std::queue<std::function<void()>> local;

    SDL_LockMutex(mutex);
    std::swap(local, queue);
    SDL_UnlockMutex(mutex);

    while (!local.empty()) {
        local.front()();
        local.pop();
    }
}
