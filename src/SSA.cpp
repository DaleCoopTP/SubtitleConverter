#include "SSA.h"

#include "DynamicArray.h"

#include <regex>
#include <string>

Structures::Time SSA::timeParse(const string &s)
{
  Structures::Time time;
  regex pattern(R"(Dialogue:\s*(\d+),(\d+):(\d{2}):(\d{2})\.(\d{2}),(\d+):(\d{2}):(\d{2})\.(\d{2}))");
  smatch match;
  if (!regex_search(s, match, pattern))
  {
    return time;
  }
  time.layer = stoi(match.str(1));
  string begin = match.str(2) + ":" + match.str(3) + ":" + match.str(4) + "." + match.str(5);
  string finish = match.str(6) + ":" + match.str(7) + ":" + match.str(8) + "." + match.str(9);
  time.start = Structures::Time::timeConverter(begin);
  time.end = Structures::Time::timeConverter(finish);
  return time;
}

string SSA::dialogueParse(const string &s)
{
  regex pattern(R"(^Dialogue:\s*(?:[^,]*,){9}(.*)$)");
  smatch match;
  if (!regex_search(s, match, pattern))
  {
    return " ";
  }
  return match.str(1);
}

void SSA::fileParse(istream &f)
{
  string line;
  Structures::Node sub;
  while (getline(f, line))
  {
    if (line.find("Dialogue") != string::npos)
    {
      sub.time = timeParse(line);
      sub.dialogue = dialogueParse(line);
      contents.push_back(sub);
      sub = Structures::Node();
    }
  }
}

DynamicArray< Structures::Node > SSA::getCollisions()
{
  DynamicArray< Structures::Node > collisions;
  const int n = contents.size();

  for (int i = 0; i < n; ++i)
  {
    for (int j = i + 1; j < n; ++j)
    {
      const Structures::Node &first = contents[i];
      const Structures::Node &second = contents[j];

      if (first.time.layer == second.time.layer && first.time.start < second.time.end &&
              second.time.start < first.time.end)
      {
        collisions.push_back(first);
        collisions.push_back(second);
      }
    }
  }
  return collisions;
}

void SSA::setFormat()
{
  for (auto &k : contents)
  {
    k.dialogue = "{\\b1}" + k.dialogue + "{\\b0}";
  }
}

void SSA::deleteFormat()
{
  regex pattern(R"((\{.*?\}|<.*?>))");
  for (auto &k : contents)
  {
    k.dialogue = regex_replace(k.dialogue, pattern, "");
  }
}
