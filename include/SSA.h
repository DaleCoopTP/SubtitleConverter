#ifndef SSA_H
#define SSA_H

#include "DynamicArray.h"
#include "Subtitle.h"

#include <regex>

class SSA : public Subtitle
{
public:
  Structures::Time timeParse(const string &s) override;
  string dialogueParse(const string &s) override;
  void fileParse(istream &f) override;
  DynamicArray< Structures::Node > getCollisions() override;
  void setFormat() override;
  void deleteFormat() override;
};

#endif
