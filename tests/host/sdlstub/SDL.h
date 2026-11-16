/* Host test stand-in for SDL.h: only what the Amiga IRandom.cpp asks of it,
 * with a performance counter the test sets.
 * Copyright (c) 2026 AmiCradle Team. GPL 2 or later, like Mojotron. */
#ifndef SDLSTUB_SDL_H
#define SDLSTUB_SDL_H
#include <cstdint>
typedef std::uint64_t Uint64;
extern Uint64 sdlstub_counter;
inline Uint64 SDL_GetPerformanceCounter() { return sdlstub_counter; }
#endif
