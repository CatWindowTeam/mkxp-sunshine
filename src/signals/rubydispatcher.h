#pragma once

#include <functional>
#include <mutex>
#include <queue>

class RubyDispatcher{
public:
    RubyDispatcher() {};

    void invoke(std::function<void()> fn);
    void process();

private:
    std::mutex mutex;
    std::queue<std::function<void()>> queue;
};