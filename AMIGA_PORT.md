Mojotron: Robot Wars for GFOS
=============================

A port of Mojotron: Robot Wars, Dennis Payne's SDL 2 version of Craig
Timpany's Mojotron (after the arcade Robotron), from
https://gitlab.com/dulsi/mojotron at commit
6ee4b12db33417d0d857abb621b366ffb62d0d32 (version 1.1), for GFOS: AmigaOS
3.2 with GFOpenUp, on a 68040 or 68060 with an FPU, on GFOpenUp's SDL 2.

Clear each arena of robots: move with one set of keys and aim with
another, collect fruit in threes for powerups, and beat the clock to
skip ahead.

Status: built in Cradle's os32-gcc16-open6 stove; not yet run in a lab.


What you need
-------------

- GFOS: AmigaOS 3.2 with GFOpenUp 0.8.26 or later, which installs
  LIBS:GFOpenGPU/SDL2.module, SDL2_image.module and SDL2_mixer.module.
- A 68040 or 68060 with an FPU (not an LC040 or EC040), an RTG screen
  with room for a 640 x 480 window (or the AmiCradle lab), and about
  12 MB of free memory.
- Nothing else: the pictures, sounds and music are in the drawer.


How to play
-----------

Install it one of three ways:

- GFOpenUp: `GFOpenUpTool INSTALL Parts/Mojotron` puts it in
  SYS:Games/Mojotron.
- Run Install-Mojotron from the archive (it offers Work:Games).
- Or copy the Mojotron drawer where you want it.

Then double-click Mojotron. The keys are in the drawer's Controls file;
upstream's ReadMe-Mojotron and Guide tell the rest.

The settings and high scores are kept in the drawer's User drawer;
Install-Mojotron and GFOpenUp keep it when they update the game.


Building
--------

    sh package/build-amiga.sh        (STOVE=/path/to/os32-gcc16-open6)

or, in Cradle, stove_build with the os32-gcc16-open6 stove and target
package. It fetches upstream at the commit package/mojotron.lock names,
applies amiga/patches, builds with amiga/Makefile.amigaos and packs
Mojotron-VERSION-68040.lha and the GFOpenUp part Parts/Mojotron.

Host test: `sh tests/host/test_xmlreader.sh` reads tests/host/samples.xml
and Mojotron's own res.xml and robotwar.xml with Expat and with
amiga/xmlreader.cpp, and checks both give the same events.
`sh tests/host/test_amiga_patches.sh` builds the patched IRandom.cpp on the
host as for AmigaOS and rolls it, and checks the image decoder hint is set
on AmigaOS only, before SDL_Init and the pictures.


What was changed
----------------

- amiga/xmlreader.cpp and amiga/include/expat.h: our own small XML
  reader in place of Expat, behind upstream's ExpatXMLParser class.
- amiga/patches/0001-user-path-amigaos.patch (ConfigFile.cpp): the
  settings in PROGDIR:User, with no std::filesystem.
- amiga/patches/0002-image-decoder-amigaos.patch (main.cpp): the
  pictures are decoded by SDL2_image's own 68k decoder (hint
  SDL_IMAGE_DECODER cpu). Sent one by one to the services card's cores
  they took minutes on a busy Cradle (a blank window); decoded here they
  take seconds. SetEnv SDL_IMAGE_DECODER auto gives the cores back.
- amiga/patches/0003-random-seed-amigaos.patch (IRandom.cpp): libstdc++
  has no random device on AmigaOS (std::random_device throws and the
  game stopped at start); the seed is the time of day and SDL's
  performance counter.
- amiga/amiga_start.c: 256 KB of stack, the $VER string, the drawer as
  the current directory and PROGDIR:User made at start.
- No Gamerzilla (it is optional upstream).
- amiga/: the Makefile, the GlowIcons (our own art), the Installer script
  and its spec. package/: the build, the release archive and the GFOpenUp
  part.

The AmiCradle Team.
