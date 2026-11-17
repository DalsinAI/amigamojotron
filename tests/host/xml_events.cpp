/* Prints every event an ExpatXMLParser gives for a file, one per line:
 * built once with upstream's expatcpp.cpp (Expat) and once with
 * amiga/xmlreader.cpp, the two outputs must match.
 * Copyright (c) 2026 AmiCradle Team. GPL 2 or later, like Mojotron. */
#include "expatcpp.h"
#include <cstdio>
#include <string>

class Dump : public ExpatXMLParser
{
	std::string text;
	void flush()
	{
		/* Expat may split text into pieces: join them, and drop text that
		 * is only white space (Mojotron's files use none) */
		if (text.find_first_not_of(" \t\r\n") != std::string::npos)
			printf("T %s\n", text.c_str());
		text.clear();
	}

public:
	void startElement(const XML_Char *name, const XML_Char **atts) override
	{
		flush();
		printf("S %s", name);
		for (int i = 0; atts[i]; i += 2)
			printf(" [%s=%s]", atts[i], atts[i + 1]);
		printf("\n");
	}
	void endElement(const XML_Char *name) override
	{
		flush();
		printf("E %s\n", name);
	}
	void characterData(const XML_Char *s, int len) override
	{
		text.append(s, len);
	}
	void done()
	{
		flush();
	}
};

int main(int argc, char **argv)
{
	for (int i = 1; i < argc; i++)
	{
		Dump d;
		d.parse(argv[i]);
		d.done();
	}
	return 0;
}
