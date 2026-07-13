#pragma once

#include <functional>
#include <mutex>
#include <queue>

class RenderDispatcher{
public:
    RenderDispatcher() {};

    void invoke(std::function<void()> fn);
    void process();

private:
    std::mutex mutex;
    std::queue<std::function<void()>> queue;
};