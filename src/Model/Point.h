/// @file Model/Point.h

#pragma once

#include "Controller/GlobalDefines.h"
#include "Model/SSETraits.h"

namespace Model
{
  class Point: public SSEVectorTraits <worldUnit>
  {
      typedef SSEVectorTraits <worldUnit> BaseType;

    public:
      using BaseType::SSEVectorTraits;
      typedef BaseType::dataType dataType;
  };
}
