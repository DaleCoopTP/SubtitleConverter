#ifndef SUBTITLE_H
#define SUBTITLE_H

#include "DynamicArray.h"
#include "Structures.h"
#include "WriteBehavior.h"

#include <fstream>
#include <memory>
#include <string>
using namespace std;

class Subtitle
{
protected:
  DynamicArray< Structures::Node > contents;

  unique_ptr< WriteBehavior > write_behavior;

  static void deltaStart(Structures::Node& n, const int det) { n.time.start += det; }
  static void deltaEnd(Structures::Node& n, const int det) { n.time.end += det; }
  static void deltaSE(Structures::Node& n, const int det)
  {
    n.time.start += det;
    n.time.end += det;
  }

public:
  DynamicArray< Structures::Node >& getContents() { return contents; }
  const DynamicArray< Structures::Node >& getContents() const { return contents; }

  void setWriteBehavior(std::unique_ptr< WriteBehavior > behavior) { write_behavior = std::move(behavior); }

  virtual void write(ofstream& out) const
  {
    if (write_behavior)
      write_behavior->write(out, contents);
  }

  virtual ~Subtitle() = default;

  virtual Structures::Time timeParse(const string& s) = 0;
  virtual string dialogueParse(const string& s) = 0;
  virtual DynamicArray< Structures::Node > getCollisions() = 0;
  virtual void deleteFormat() = 0;
  virtual void setFormat() = 0;

  virtual void fileParse(istream& f) = 0;
};

#endif
