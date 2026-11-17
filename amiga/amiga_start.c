/* The Amiga port's start: the stack, the version string, the game's
 * drawer as the current directory (Mojotron reads robotwar.xml, graphics/
 * and the rest from where it runs, which a Shell start need not be) and
 * PROGDIR:User (the settings and high scores), all before main().
 * Copyright (c) 2026 AmiCradle Team. GPL 2 or later, like Mojotron. */
#include <sys/stat.h>
#include <unistd.h>

/* A Shell gives 4 KB of stack; SDL 2 and C++ want more. libnix's startup
 * sets this much up. */
unsigned long __stack = 256 * 1024;
static const char version[] __attribute__((used)) =
    "$VER: Mojotron 1.1 (16.11.2026)";

static void __attribute__((constructor)) amiga_drawer(void)
{
	chdir("PROGDIR:");
	mkdir("PROGDIR:User", 0755);
}
