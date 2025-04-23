#include "TTML.h"

#include <regex>
#include <string>

Structures::Time TTML::timeParse(const string &s)
{
	Structures::Time t;
	std::regex pattern("<p begin=\"(\\d{2}):(\\d{2}):(\\d{2})\\.(\\d{2,3})\" "
					   "end=\"(\\d{2}):(\\d{2}):(\\d{2})\\.(\\d{2,3})\"");
	std::smatch match;
	if (!std::regex_search(s, match, pattern))
	{
		return t;
	}

	std::string startStr = match.str(1) + ":" + match.str(2) + ":" + match.str(3) + "." + match.str(4);
	std::string endStr = match.str(5) + ":" + match.str(6) + ":" + match.str(7) + "." + match.str(8);

	t.start = Structures::Time::timeConverter(startStr);
	t.end = Structures::Time::timeConverter(endStr);
	return t;
}

string TTML::dialogueParse(const string &s)
{
	string dialogue = s;
	return dialogue;
}

void TTML::fileParse(istream &f)
{
	string line, fullContent;

	while (getline(f, line))
	{
		fullContent += line + "\n";
	}

	std::regex pattern("<p begin=\"([^\"]+)\" end=\"([^\"]+)\">(.*?)</p>");
	std::smatch match;

	auto begin = fullContent.cbegin();
	while (std::regex_search(begin, fullContent.cend(), match, pattern))
	{
		Structures::Node sub;
		sub.time = timeParse(match.str(1));
		sub.dialogue = match.str(3);

		if (!sub.time.isEmpty() && !sub.dialogue.empty())
		{
			contents.push_back(sub);
		}
		begin = match[0].second;
	}
}

void TTML::deleteFormat()
{
	std::regex pattern("<[^>]+>");
	for (auto &k : contents)
	{
		k.dialogue = std::regex_replace(k.dialogue, pattern, "");
	}
}

void TTML::setFormat()
{
	for (auto &k : contents)
	{
		k.dialogue = "<span style=\"italic\">" + k.dialogue + "</span>";
	}
}

DynamicArray< Structures::Node > TTML::getCollisions()
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
