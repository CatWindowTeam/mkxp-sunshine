#pragma once

#include <sigc++/signal.h>

#include "signalconnection.h"

// мямямя
template<typename Signature>
class Signal
{
public:
    size_t listeners = 0;

    template<typename Obj>
    SignalConnection Connect(Obj& obj, void (Obj::*func)())
    {
        return SignalConnection(signal.connect([&obj, func](auto&&...) { (obj.*func)(); }));
    }

    template<typename Obj, typename Func>
    SignalConnection Connect(Obj& obj, Func func)
    {
        listeners++;
        return SignalConnection(signal.connect(sigc::mem_fun(obj, func)));
    }

    template<typename Callable>
    SignalConnection Connect(Callable cb)
    {
        return SignalConnection(signal.connect(cb));
    }

    template<typename... Args>
    void Emit(Args... args)
    {
        signal(args...);
    }

    template<typename... Args>
    void operator()(Args... args)
    {
        signal(args...);
    }

    void DisconnectAll()
    {
        signal.clear();
    }

private:
    sigc::signal<Signature> signal;
};