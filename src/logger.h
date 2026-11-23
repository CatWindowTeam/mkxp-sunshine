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
    auto message = buf.str();

    std::cout << message;
    logs.push_back(std::move(message));
}
