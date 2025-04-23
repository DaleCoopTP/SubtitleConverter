#ifndef SRT_H
#define SRT_H

#include "DynamicArray.h"
#include "Subtitle.h"

#include <regex>

class SRT : public Subtitle
{
public:
  Structures::Time timeParse(const string& s) override;
  string dialogueParse(const string& s) override;
  void fileParse(istream& f) override;
  void deleteFormat() override;
  void setFormat() override;
  DynamicArray< Structures::Node > getCollisions() override;
};

#endif
