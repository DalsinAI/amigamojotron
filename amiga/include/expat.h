/* The one thing upstream's expatcpp.h takes from Expat: its character
 * type. The reading is done by amiga/xmlreader.cpp, our own, in place of
 * Expat (GFOS ports use only our own code).
 * Copyright (c) 2026 AmiCradle Team. GPL 2 or later, like Mojotron. */
#ifndef AMIGA_EXPAT_H
#define AMIGA_EXPAT_H
typedef char XML_Char;
#endif
