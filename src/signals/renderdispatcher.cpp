#include "renderdispatcher.h"

void RenderDispatcher::invoke(std::function<void()> fn){
    std::lock_guard lock(mutex);
    queue.push(std::move(fn));
}

void RenderDispatcher::process(){
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