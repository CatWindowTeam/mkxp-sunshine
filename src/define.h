#pragma once
#include <SDL3/SDL_platform_defines.h>
#if !defined(SDL_PLATFORM_ANDROID) && (defined(SDL_PLATFORM_AIX) || defined(SDL_PLATFORM_BSDI) || defined(SDL_PLATFORM_FREEBSD) || \
    defined(SDL_PLATFORM_HPUX) || defined(SDL_PLATFORM_HURD) || defined(SDL_PLATFORM_IRIX) || \
    defined(SDL_PLATFORM_LINUX) || defined(SDL_PLATFORM_MACOS) || defined(SDL_PLATFORM_NETBSD) || \
    defined(SDL_PLATFORM_OPENBSD) || defined(SDL_PLATFORM_SOLARIS) || defined(SDL_PLATFORM_UNIX) || \
    defined(SDL_PLATFORM_APPLE) || defined(SDL_PLATFORM_OSF))
	#define unix_like 1
#endif

#if defined(SDL_PLATFORM_WINDOWS) || defined(SDL_PLATFORM_WIN32)
	#define windows 1
#endif

#if SDL_PLATFORM_ANDROID
	#define android 1
#endif

#if defined(SDL_PLATFORM_APPLE)
	#define apple 1
#endif

#if SDL_PLATFORM_EMSCRIPTEN
	#define web 1
#endif

#if SDL_PLATFORM_HAIKU
	#define haiku 1
#endif

#if SDL_PLATFORM_PS2
	#define ps2 1
#endif

#if SDL_PLATFORM_VITA
	#define vita 1
#endif
