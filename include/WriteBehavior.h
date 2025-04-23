#ifndef WRITEBEHAVIOR_H
#define WRITEBEHAVIOR_H

#include "DynamicArray.h"
#include "Structures.h"

#include <cstdio>
#include <fstream>
#include <string>

class WriteBehavior
{
  protected:
	std::string timeFormat(int totalMilliseconds, char format)
	{
		int hours = totalMilliseconds / 3600000;
		totalMilliseconds %= 3600000;
		int minutes = totalMilliseconds / 60000;
		totalMilliseconds %= 60000;
		int seconds = totalMilliseconds / 1000;
		int milliseconds = totalMilliseconds % 1000;
		char buffer[16];
		sprintf(buffer, "%02d:%02d:%02d%c%03d", hours, minutes, seconds, format, milliseconds);
		return std::string(buffer);
	}

  public:
	virtual void write(std::ostream &out, const DynamicArray< Structures::Node > &v) = 0;
	virtual ~WriteBehavior() = default;
};

class toSRT : public WriteBehavior
{
  public:
	void write(std::ostream &out, const DynamicArray< Structures::Node > &v) override
	{
		for (size_t i = 0; i < v.size(); ++i)
		{
			out << i + 1 << "\n";
			out << timeFormat(v[i].time.start, ',') << " --> " << timeFormat(v[i].time.end, ',') << "\n";
			out << v[i].dialogue << "\n\n";
		}
	}
};

class toSAMI : public WriteBehavior
{
  public:
	void write(std::ostream &out, const DynamicArray< Structures::Node > &v) override
	{
		out << "<SAMI>\n";
		out << "<BODY>\n";
		for (size_t i = 0; i < v.size(); ++i)
		{
			out << "<SYNC Start=" << v[i].time.start << " End=" << v[i].time.end << ">\n";
			if (v[i].dialogue.find("<P") != std::string::npos)
				out << v[i].dialogue << "\n";
			else
				out << "<P>" << v[i].dialogue << "</P>\n";
		}
		out << "</BODY>\n";
		out << "</SAMI>\n";
	}
};

class toSSA : public WriteBehavior
{
  public:
	void write(std::ostream &out, const DynamicArray< Structures::Node > &v) override
	{
		out << "[Script Info]\n";
		out << "Title: Converted Subtitle\n";
		out << "ScriptType: v4.00+\n";
		out << "Collisions: Normal\n\n";

		out << "[V4+ Styles]\n";
		out << "Format: Name, Fontname, Fontsize, PrimaryColour, BackColour, Bold, Italic, Alignment, MarginL, "
			   "MarginR, MarginV, Encoding\n";
		out << "Style: Default,Arial,20,&H00FFFFFF,&H00000000,0,0,2,10,10,10,0\n\n";

		out << "[Events]\n";
		out << "Format: Marked, Start, End, Style, Name, MarginL, MarginR, MarginV, Text\n";

		for (const auto &node : v)
		{
			out << "Dialogue: Marked=0," << timeFormat(node.time.start, '.') << "," << timeFormat(node.time.end, '.')
				<< ",Default,,0,0,0," << node.dialogue << "\n";
		}
	}
};

class toTTML : public WriteBehavior
{
  public:
	void write(std::ostream &out, const DynamicArray< Structures::Node > &v) override
	{
		out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
		out << "<tt xmlns=\"http://www.w3.org/ns/ttml\">\n";
		out << "<body>\n";
		out << "<div>\n";

		for (size_t i = 0; i < v.size(); ++i)
		{
			out << "<p begin=\"" << timeFormat(v[i].time.start, '.') << "\" end=\"" << timeFormat(v[i].time.end, '.') << "\">";
			out << v[i].dialogue;
			out << "</p>\n";
		}

		out << "</div>\n";
		out << "</body>\n";
		out << "</tt>\n";
	}
};

#endif
