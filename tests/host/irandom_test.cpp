/* IRandom with the Amiga seed (amiga/patches/0003) on the host: no
 * std::random_device, every roll in range, every value comes up, and a
 * different E-clock gives a different game.
 * Copyright (c) 2026 AmiCradle Team. GPL 2 or later, like Mojotron. */
#include "IRandom.hh"
#include <SDL.h>
#include <cstdio>
#include <vector>

Uint64 sdlstub_counter;

static std::vector<int> rolls(Uint64 counter)
{
    sdlstub_counter = counter;
    IRandom::init();
    std::vector<int> v;
    for (int i = 0; i < 64; i++)
        v.push_back(IRandom::roll(1000));
    return v;
}

int main()
{
    int fail = 0;
    sdlstub_counter = 0x0123456789abcdefULL;
    IRandom::init();
    std::vector<int> seen(6, 0);
    bool inrange = true;
    for (int i = 0; i < 6000; i++) {
        int r = IRandom::roll(6);
        if (r < 0 || r >= 6) inrange = false; else seen[r]++;
    }
    bool all = true;
    for (int c : seen) if (c == 0) all = false;
    std::printf("%s  rolls in range\n", inrange ? "pass" : "FAIL"); fail |= !inrange;
    std::printf("%s  every value comes up\n", all ? "pass" : "FAIL"); fail |= !all;
    bool differ = rolls(1000) != rolls(1001);
    std::printf("%s  another E-clock, another game\n", differ ? "pass" : "FAIL"); fail |= !differ;
    bool high = rolls(1ULL << 40) != rolls(2ULL << 40);
    std::printf("%s  the counter's high half counts too\n", high ? "pass" : "FAIL"); fail |= !high;
    return fail;
}
