#include "SRT.h"

#include "DynamicArray.h"

#include <regex>
#include <string>

Structures::Time SRT::timeParse(const string &s)
{
	Structures::Time t;
	regex pattern(R"((\d{2}):(\d{2}):(\d{2}),(\d{3}) --> (\d{2}):(\d{2}):(\d{2}),(\d{3}))");
	smatch match;
	if (!regex_search(s, match, pattern))
	{
		return t;
	}
	string st = match.str(1) + ':' + match.str(2) + ':' + match.str(3) + ',' + match.str(4);
	string en = match.str(5) + ':' + match.str(6) + ':' + match.str(7) + ',' + match.str(8);
	t.start = Structures::Time::timeConverter(st);
	t.end = Structures::Time::timeConverter(en);
	return t;
}

string SRT::dialogueParse(const string &s)
{
	regex pattern("[A-Z]");
	if (regex_search(s, pattern))
	{
		return s;
	}
	return "";
}

void SRT::fileParse(istream &f)
{
	string line;
	Structures::Node sub;
	while (getline(f, line))
	{
		if (line.empty())
			continue;

		string timeLine;
		if (!getline(f, timeLine))
			break;
		sub.time = timeParse(timeLine);
		string dialogue;
		while (getline(f, line) && !line.empty())
		{
			if (!dialogue.empty())
				dialogue += "\n";
			dialogue += dialogueParse(line);
		}
		sub.dialogue = dialogue;
		if (!sub.time.isEmpty() && !sub.dialogue.empty())
		{
			contents.push_back(sub);
			sub = Structures::Node();
		}
	}
}

void SRT::deleteFormat()
{
	regex pattern(R"((\{.*?\}|<.*?>))");
	for (auto &k : contents)
	{
		k.dialogue = regex_replace(k.dialogue, pattern, "");
	}
}

void SRT::setFormat()
{
	for (auto &k : contents)
	{
		k.dialogue = "<i>" + k.dialogue + "</i>";
	}
}

DynamicArray< Structures::Node > SRT::getCollisions()
{
	DynamicArray< Structures::Node > collisions;
	const int n = contents.size();

	for (int i = 0; i < n; ++i)
	{
		for (int j = i + 1; j < n; ++j)
		{
			const Structures::Node &first = contents[i];
			const Structures::Node &second = contents[j];

			if (first.time.start < second.time.end && second.time.start < first.time.end)
			{
				collisions.push_back(first);
				collisions.push_back(second);
			}
		}
	}
	return collisions;
}
