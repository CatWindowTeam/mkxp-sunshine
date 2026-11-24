#pragma once
#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
#include "meow.h"
#ifdef SUNSHINE_COLORED_TERMINAL_OUTPUT
	#include "rang.hpp"
#endif

template<typename... Args>
void Debug_Log(Args&&... args){
	#ifndef NDEBUG
		std::ostringstream buf;
		buf << "[DEBUG] ";
	    (buf << ... << args) << '\n';
    	auto message = buf.str();
    	#ifdef SUNSHINE_COLORED_TERMINAL_OUTPUT
    		std::cout << rang::fgB::cyan << message << rang::fg::reset;
    	#else
    		std::cout << message;
    	#endif
    	logs.push_back(std::move(message));
    #endif
}

template<typename... Args>
void Log(Args&&... args){
	std::ostringstream buf;
	buf << "[INFO] ";
    (buf << ... << args) << '\n';
    auto message = buf.str();

	std::cout << message;
    logs.push_back(std::move(message));
}

template<typename... Args>
void Warn_Log(Args&&... args){
	std::ostringstream buf;
	buf << "[WARN] ";
	
    (buf << ... << args) << '\n';
    auto message = buf.str();

	#ifdef SUNSHINE_COLORED_TERMINAL_OUTPUT
		std::cout << rang::fgB::yellow << message << rang::fg::reset;
	#else
    	std::cout << message;
    #endif
    logs.push_back(std::move(message));
}

template<typename... Args>
void Error_Log(Args&&... args){
	std::ostringstream buf;
	buf << "[ERR] ";
	
    (buf << ... << args) << '\n';
    auto message = buf.str();

	#ifdef SUNSHINE_COLORED_TERMINAL_OUTPUT
		std::cout << rang::fgB::red << message << rang::fg::reset;
	#else
    	std::cout << message;
    #endif
    logs.push_back(std::move(message));
}
