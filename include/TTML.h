#ifndef TTML_H
#define TTML_H

#include "DynamicArray.h"
#include "Subtitle.h"

#include <regex>

class TTML : public Subtitle
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
