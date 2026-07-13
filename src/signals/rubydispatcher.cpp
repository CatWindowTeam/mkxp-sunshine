#include "rubydispatcher.h"

void RubyDispatcher::invoke(std::function<void()> fn){
    std::lock_guard lock(mutex);
    queue.push(std::move(fn));
}

void RubyDispatcher::process(){
    std::queue<std::function<void()>> local;

    {
        std::lock_guard lock(mutex);
        std::swap(local, queue);
    }

    while (!local.empty())
    {
        local.front()();
        local.pop();
    }
}