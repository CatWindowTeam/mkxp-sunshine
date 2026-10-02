#pragma once
#include <SDL3/SDL_atomic.h>
#include <SDL3/SDL_thread.h>
#include <SDL3/SDL_iostream.h>
#include <string>
#include <iostream>

struct AtomicFlag{
	AtomicFlag(){
		clear();
	}

	AtomicFlag(bool value){
		SDL_SetAtomicInt(&atom, value ? 1 : 0);
	}

	void set(){
		SDL_SetAtomicInt(&atom, 1);
	}

	void clear(){
		SDL_SetAtomicInt(&atom, 0);
	}

	operator bool() const {
		return SDL_GetAtomicInt(&atom);
	}

private:
	mutable SDL_AtomicInt atom;
};

inline bool readFileSDL(const char *path, std::string &out){
	SDL_IOStream *f = SDL_IOFromFile(path, "rb");
	if (!f)
		return false;

	long size = SDL_GetIOSize(f);
	size_t back = out.size();

	out.resize(back+size);
	size_t read = SDL_ReadIO(f, &out[back], size);
	SDL_CloseIO(f);
	if (read != (size_t) size)
		out.resize(back+read);

	return true;
}

template<size_t bufSize = 248, size_t pbSize = 8>
class SDLRWBuf : public std::streambuf{
public:
	SDLRWBuf(SDL_IOStream *ops) : ops(ops){
		char *end = buf + bufSize + pbSize;
		setg(end, end, end);
	}

private:
	int_type underflow(){
		if (!ops)
			return traits_type::eof();

		if (gptr() < egptr())
			return traits_type::to_int_type(*gptr());

		char *base = buf;
		char *start = base;
		if (eback() == base){
			SDL_memmove(base, egptr() - pbSize, pbSize);
			start += pbSize;
		}
		
		size_t n = SDL_ReadIO(ops, start, bufSize - (start - base));
		if (n == 0)
			return traits_type::eof();

		setg(base, start, start + n);
		return underflow();
	}

	SDL_IOStream *ops;
	char buf[bufSize+pbSize];
};

class SDLRWStream{
public:
	SDLRWStream(const char *filename, const char *mode)
	    : ops(SDL_IOFromFile(filename, mode)),
	      buf(ops),
	      s(&buf)
	{}

	~SDLRWStream(){
		if (ops)
			SDL_CloseIO(ops);
	}

	operator bool() const noexcept {
		return ops != 0;
	}

	std::istream &stream() noexcept {
		return s;
	}

private:
	SDL_IOStream *ops;
	SDLRWBuf<> buf;
	std::istream s;
};
