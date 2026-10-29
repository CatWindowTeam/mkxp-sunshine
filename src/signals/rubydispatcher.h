#pragma once
#include <SDL3/SDL.h>
#include <functional>
#include <queue>

class RubyDispatcher {
public:
    RubyDispatcher();
    ~RubyDispatcher();

    RubyDispatcher(const RubyDispatcher&) = delete;
    RubyDispatcher& operator=(const RubyDispatcher&) = delete;

    void invoke(std::function<void()> fn);
    void process();
private:
    SDL_Mutex* mutex;
    std::queue<std::function<void()>> queue;
};
