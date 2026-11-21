#pragma once
#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
#include "meow.h"

template<typename... Args>
void Log(Args&&... args){
	std::ostringstream buf;

    (buf << ... << args) << '\n';
    const std::string message = buf.str();

    logs.push_back(message);
    std::cout << message;
}
