/// @file Model/Light.h

#pragma once

#include "Model/Color.h"
#include "Model/Object.h"

namespace Model
{

  /**Light class
   *
   */
  class Light: public Object, public Color
  {
    public:
      float power;

      Light ()
          : power(1)
      {
      }

      /**Sets color of light
       *
       * @return this
       */
      inline Light &operator= (const Color &color)
      {
        Color::operator =(color);
        return *this;
      }

      /**Returns color of light
       *
       * @return color of light
       */
      inline const Color &getColor () const
      {
        return *this;
      }
  };
}
