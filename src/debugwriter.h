/*
** debugwriter.h
**
** This file is part of mkxp.
**
** Copyright (C) 2013 Jonas Kulla <Nyocurio@gmail.com>
**
** mkxp is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** mkxp is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with mkxp.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef DEBUGWRITER_H
#define DEBUGWRITER_H

#include <iostream>
#include <sstream>
#include <vector>
#include <ruby.h>
#include <source_location>
#include "meow.h"
#undef vsnprintf
#undef snprintf

#ifdef __ANDROID__
	#include <android/SDL_log.h>
#elif __EMSCRIPTEN__
	#include <emscripten/console.h>
#endif

class Debug{
public:
	explicit Debug(const std::source_location location = std::source_location::current()) : location(location){
	    buf << std::boolalpha;
	}

	template<typename T>
	Debug &operator<<(const T &t){
		buf << t;
		buf << " ";

		return *this;
	}

	template<typename T>
	Debug &operator<<(const std::vector<T> &v){
		for (size_t i = 0; i < v.size(); ++i)
			buf << v[i] << " ";

		return *this;
	}

	template<typename T>
	Debug &operator<<(const VALUE &v){
		buf << rb_inspect(v);
		return *this;
	}

	~Debug(){
		std::ostringstream result;
		result << "[" << location.line() << ":" << location.function_name() << "] " << buf.view();
#ifdef __ANDROID__
		__android_log_write(ANDROID_LOG_DEBUG, "sunshine", result.view().c_str());
#elif __EMSCRIPTEN__
		emscripten_console_log(result.view().c_str());				
#else
		logs.emplace_back(result.view());
		std::cout << std::move(result).str() << '\n';
#endif
	}

private:
	std::ostringstream buf;
	std::source_location location;
};

#endif // DEBUGWRITER_H
