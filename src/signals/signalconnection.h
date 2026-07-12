#pragma once

#include <sigc++/connection.h>

class SignalConnection{
public:
    SignalConnection() = default;
 
    void Disconnect()
    {
        connection.disconnect();
    }

    bool Connected() const
    {
        return connection.connected();
    }

private:
    SignalConnection(sigc::connection conn)
        : connection(std::move(conn))
    {}

    sigc::connection connection;

    template<typename>
    friend class Signal;
};