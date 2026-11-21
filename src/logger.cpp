#include "logger.h"
#include "meow.h"
#include <cstdio>
#include <vector>
#include <sstream>
#include <cstdarg>

//for C logs
void LogC(const char *fmt, ...){
    va_list args;
    va_start(args, fmt);
    va_list args_copy;
    va_copy(args_copy, args);
    
    int len = SDL_vsnprintf(NULL, 0, fmt, args);
    char* data = (char*)SDL_malloc(len);
    
	SDL_vsnprintf(data, len + 1, fmt, args_copy);

	// we already do formatting, so use puts instead printf must be more optimized way?
	puts(data);
	logs.emplace_back(data);

	SDL_free(data);
	va_end(args);
}

template<typename... Args>
void LogCXX(Args&&... args){
	std::ostringstream buf;
    (buf << ... << args) << '\n';
    logs.push_back(buf.view());
    std::cout << buf.view();
}
