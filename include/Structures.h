#ifndef STRUCTURES_H
#define STRUCTURES_H
#include <string>
using namespace std;

namespace Structures
{
struct Time
{
  int layer = 0;
  int start = 0;
  int end = 0;

  bool isEmpty() const { return start == 0 && end == 0; }

  Time() = default;
  Time(int l, int s, int e) : layer(l), start(s), end(e) {}

  static int timeConverter(const string& s)
  {
    const int hours = stoi(s.substr(0, 2));
    const int minutes = stoi(s.substr(3, 2));
    const int seconds = stoi(s.substr(6, 2));
    const int centiseconds = stoi(s.substr(9, 2));
    const int milliseconds = centiseconds * 10;
    const int totalMilliseconds = hours * 3600000 + minutes * 60000 + seconds * 1000 + milliseconds;
    return totalMilliseconds;
  }
};

struct Node
{
  Time time;
  string dialogue = "";

  Node() = default;
  Node(const Time& t, const string& d) : time(t), dialogue(d) {}
};
};

#endif
