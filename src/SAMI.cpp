#include "SAMI.h"

#include "DynamicArray.h"

#include <regex>
#include <string>

Structures::Time SAMI::timeParse(const string &s)
{
	Structures::Time t;
	regex pattern(R"(<SYNC Start=(\d+)>)");
	smatch match;
	if (regex_search(s, match, pattern))
	{
		t.start = stoi(match.str(1));
		t.end = t.start;
		return t;
	}
	return t;
}

string SAMI::dialogueParse(const string &line)
{
	smatch match;
	regex pattern(R"(<P[^>]*>(.*?)<\/P>)", regex_constants::icase);
	if (!regex_search(line, match, pattern))
		return "";
	string text = match.str(1);
	size_t pos;
	while ((pos = text.find("<br>")) != string::npos)
		text.replace(pos, 4, "\n");
	return text;
}

void SAMI::fileParse(istream &f)
{
	string line;
	Structures::Node sub;
	bool inSubtitle = false;
	bool inParagraph = false;
	string paragraphBuffer;

	while (getline(f, line))
	{
		if (line.find("<SYNC Start=") != string::npos)
		{
			if (inSubtitle)
			{
				if (!paragraphBuffer.empty())
				{
					string text = dialogueParse(paragraphBuffer);
					if (!sub.dialogue.empty())
						sub.dialogue += '\n';
					sub.dialogue += text;
					paragraphBuffer.clear();
					inParagraph = false;
				}
				contents.push_back(sub);
				sub = Structures::Node();
			}
			sub.time = timeParse(line);
			inSubtitle = true;
			sub.dialogue = "";
		}
		else if (line.find("<P") != string::npos)
		{
			if (line.find("Class=ENUSCC") != string::npos || line.find("Class=FRFRCC") != string::npos)
			{
				paragraphBuffer = line;
				inParagraph = true;
				if (line.find("</P>") != string::npos)
				{
					string text = dialogueParse(paragraphBuffer);
					if (!sub.dialogue.empty())
						sub.dialogue += '\n';
					sub.dialogue += text;
					paragraphBuffer.clear();
					inParagraph = false;
				}
			}
		}
		else if (inParagraph)
		{
			paragraphBuffer += line;
			if (line.find("</P>") != string::npos)
			{
				string text = dialogueParse(paragraphBuffer);
				if (!sub.dialogue.empty())
					sub.dialogue += '\n';
				sub.dialogue += text;
				paragraphBuffer.clear();
				inParagraph = false;
			}
		}
	}
	if (inSubtitle)
	{
		if (!paragraphBuffer.empty())
		{
			string text = dialogueParse(paragraphBuffer);
			if (!sub.dialogue.empty())
				sub.dialogue += '\n';
			sub.dialogue += text;
		}
		contents.push_back(sub);
	}
}

DynamicArray< Structures::Node > SAMI::getCollisions()
{
	DynamicArray< Structures::Node > collisions;
	const int n = contents.size();

	for (int i = 0; i < n; ++i)
	{
		for (int j = i + 1; j < n; ++j)
		{
			const Structures::Node &first = contents[i];
			const Structures::Node &second = contents[j];

			if (first.time.start >= second.time.start)
			{
				collisions.push_back(first);
				collisions.push_back(second);
			}
		}
	}
	return collisions;
}

void SAMI::setFormat()
{
	for (auto &k : contents)
	{
		k.dialogue = "<i>" + k.dialogue + "</i>";
	}
}

void SAMI::deleteFormat()
{
	regex pattern(R"((\{.*?\}|<.*?>))");
	for (auto &k : contents)
	{
		k.dialogue = regex_replace(k.dialogue, pattern, "");
	}
}
