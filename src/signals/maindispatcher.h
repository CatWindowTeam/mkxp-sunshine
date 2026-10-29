#pragma once
#include <SDL3/SDL.h>
#include <functional>
#include <queue>

class MainDispatcher {
public:
    MainDispatcher();
    ~MainDispatcher();

    void invoke(std::function<void()> fn);
    void process();
private:
    SDL_Mutex* mutex;
    std::queue<std::function<void()>> queue;
};
