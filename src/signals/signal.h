#pragma once
#include <sigc++/sigc++.h> 
#include "signalconnection.h"

template<typename Result, typename... Args>
class Signal;

// мямямя
template<typename Result, typename... Args>
class Signal<Result(Args...)>{
public:
    template<typename Obj, typename Func>
    SignalConnection Connect(Obj& obj, Func func){
        if constexpr (std::is_base_of_v<sigc::trackable, Obj>)
            return SignalConnection(signal.connect(sigc::track_object(sigc::mem_fun(obj, func), obj)));
        else
            return SignalConnection(signal.connect(sigc::mem_fun(obj, func)));
    }

    template<typename Callable>
    SignalConnection Connect(Callable cb){
        return SignalConnection(signal.connect(std::move(cb)));
    }

    void Emit(Args... args){
        signal(std::forward<Args>(args)...);
    }

    void operator()(Args... args){
        signal(std::forward<Args>(args)...);
    }

    void DisconnectAll(){
        signal.clear();
    }

private:
    sigc::signal<Result(Args...)> signal;
};

class Trackable : public sigc::trackable { /* owo */ };
