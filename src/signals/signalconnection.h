#pragma once
#include <sigc++/sigc++.h> 

class SignalConnection{
public:
    SignalConnection() = default;
    SignalConnection(sigc::connection conn) : connection(std::move(conn)){}
 
    void Disconnect(){
        if (Connected())
            connection.disconnect();
    }

    bool Connected() const{
        return connection.connected();
    }

private:
    sigc::connection connection;

    template<typename, typename...>
    friend class Signal;
};
