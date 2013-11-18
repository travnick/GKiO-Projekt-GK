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

      Light (const Light &light)
          : Object(light), Color(light), power(light.power)
      {
      }

      Light (const Object &object, const Color &color, float newPower)
          : Object(object), Color(color), power(newPower)
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
