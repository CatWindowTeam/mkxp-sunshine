#pragma once

#include <sigc++/connection.h>
#include "debugwriter.h"

class SignalConnection{
public:
    SignalConnection() = default;
    SignalConnection(sigc::connection conn)
        : connection(std::move(conn))
    {}
 
    void Disconnect()
    {
        if (&connection != nullptr)
            //if (connection.connected())
                connection.disconnect();
            //else
            //    Debug() << "Double disconnect!";
        else
            Debug() << "Couldn't disconnect nullptr connection!";
    }

    bool Connected() const
    {
        if (&connection == nullptr)
            Debug() << "nullptr connection!";
            return false;
        return connection.connected();
    }

private:
    sigc::connection connection;

    template<typename>
    friend class Signal;
};