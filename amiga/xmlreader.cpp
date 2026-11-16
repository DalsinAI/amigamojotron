/* ExpatXMLParser (upstream's expatcpp.h) without Expat: a small reader of
 * the XML Mojotron's resource files use. It calls startElement with the
 * attributes as name, value pairs ending in a null, endElement (also for
 * an empty element <a/>) and characterData for the text between tags.
 * It skips comments, <?...?> and <!...>, and decodes &lt; &gt; &amp;
 * &quot; &apos; and &#N; / &#xN; (to one byte; the files are ASCII).
 * Copyright (c) 2026 AmiCradle Team. GPL 2 or later, like Mojotron. */
#include "expatcpp.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

ExpatXMLParser::ExpatXMLParser()
{
}

static std::string decode(const char *s, size_t n)
{
	std::string out;
	out.reserve(n);
	for (size_t i = 0; i < n; i++)
	{
		if (s[i] != '&')
		{
			out += s[i];
			continue;
		}
		const char *semi = (const char *)memchr(s + i, ';', n - i);
		if (!semi)
		{
			out += s[i];
			continue;
		}
		std::string ent(s + i + 1, semi - (s + i + 1));
		if (ent == "lt") out += '<';
		else if (ent == "gt") out += '>';
		else if (ent == "amp") out += '&';
		else if (ent == "quot") out += '"';
		else if (ent == "apos") out += '\'';
		else if (ent.size() > 1 && ent[0] == '#')
			out += (char)strtol(ent.c_str() + (ent[1] == 'x' ? 2 : 1), NULL, ent[1] == 'x' ? 16 : 10);
		else
		{
			out.append(s + i, semi - (s + i) + 1);
		}
		i = semi - s;
	}
	return out;
}

static bool is_space(char c)
{
	return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

static bool is_name(char c)
{
	return c && !is_space(c) && c != '=' && c != '>' && c != '/' && c != '"' && c != '\'';
}

void ExpatXMLParser::parse(const char *filename)
{
	FILE *f = fopen(filename, "rb");
	if (!f)
		return;
	std::string doc;
	char buf[4096];
	size_t got;
	while ((got = fread(buf, 1, sizeof buf, f)) > 0)
		doc.append(buf, got);
	fclose(f);

	const char *p = doc.c_str(), *end = p + doc.size();
	while (p < end)
	{
		const char *lt = (const char *)memchr(p, '<', end - p);
		const char *text_end = lt ? lt : end;
		if (text_end > p)
		{
			std::string text = decode(p, text_end - p);
			characterData(text.c_str(), (int)text.size());
		}
		if (!lt)
			break;
		p = lt;
		if (!strncmp(p, "<!--", 4))
		{
			const char *e = strstr(p + 4, "-->");
			p = e ? e + 3 : end;
			continue;
		}
		if (p[1] == '?' || p[1] == '!')
		{
			const char *e = (const char *)memchr(p, '>', end - p);
			p = e ? e + 1 : end;
			continue;
		}
		if (p[1] == '/')
		{
			const char *n = p + 2, *e = n;
			while (e < end && is_name(*e))
				e++;
			std::string name(n, e - n);
			endElement(name.c_str());
			const char *gt = (const char *)memchr(e, '>', end - e);
			p = gt ? gt + 1 : end;
			continue;
		}
		const char *n = p + 1, *e = n;
		while (e < end && is_name(*e))
			e++;
		std::string name(n, e - n);
		std::vector<std::string> attrs;
		bool empty = false;
		p = e;
		while (p < end)
		{
			while (p < end && is_space(*p))
				p++;
			if (p >= end)
				break;
			if (*p == '>')
			{
				p++;
				break;
			}
			if (*p == '/')
			{
				empty = true;
				p++;
				continue;
			}
			const char *an = p;
			while (p < end && is_name(*p))
				p++;
			std::string aname(an, p - an);
			while (p < end && is_space(*p))
				p++;
			std::string value;
			if (p < end && *p == '=')
			{
				p++;
				while (p < end && is_space(*p))
					p++;
				if (p < end && (*p == '"' || *p == '\''))
				{
					char q = *p++;
					const char *v = p;
					while (p < end && *p != q)
						p++;
					value = decode(v, p - v);
					if (p < end)
						p++;
				}
			}
			if (aname.empty())
			{
				p++; /* a stray character: step over it */
				continue;
			}
			attrs.push_back(aname);
			attrs.push_back(value);
		}
		std::vector<const XML_Char *> atts;
		for (auto &a : attrs)
			atts.push_back(a.c_str());
		atts.push_back(NULL);
		startElement(name.c_str(), atts.data());
		if (empty)
			endElement(name.c_str());
	}
}
