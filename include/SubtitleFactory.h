#pragma once

#include "SAMI.h"
#include "SRT.h"
#include "SSA.h"
#include "Subtitle.h"
#include "TTML.h"
#include "WriteBehavior.h"

#include <string>

class SubtitleFactory
{
public:
  static std::unique_ptr< Subtitle > create(const std::string& format)
  {
    if (format == ".srt")
    {
      auto srt = std::make_unique< SRT >();
      srt->setWriteBehavior(std::make_unique< toSRT >());
      return srt;
    }
    else if (format == ".smi")
    {
      auto sami = std::make_unique< SAMI >();
      sami->setWriteBehavior(std::make_unique< toSAMI >());
      return sami;
    }
    else if (format == ".ass")
    {
      auto ssa = std::make_unique< SSA >();
      ssa->setWriteBehavior(std::make_unique< toSSA >());
      return ssa;
    }
    else if (format == ".ttml")
    {
      auto ttml = std::make_unique< TTML >();
      ttml->setWriteBehavior(std::make_unique< toTTML >());
      return ttml;
    }
    return nullptr;
  }
};
