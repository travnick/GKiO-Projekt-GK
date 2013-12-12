/// @file Model/Color.h

#pragma once

#include <QColor>

#include "Controller/GlobalDefines.h"
#include "Model/ModelDefines.h"
#include "Model/SSEData.h"

#define DEFAULT_COLOR_VALUE 0

namespace Model
{

  /**Color class
   *
   */
  class Color
  {
    public:
      enum ColorEnum
      {
        R = X, G = Y, B = Z
      };

      typedef float dataType;

      inline Color ()
      {
      }

      /**Constructs color from QRgb
       *
       * @param color
       */
      inline Color (QRgb color)
          : data(qRed(color), qGreen(color), qBlue(color))
      {
      }

      /**Constructs Color from r, g, b components
       *
       * @param r red
       * @param g green
       * @param b blue
       *
       */
      inline Color (dataType r, dataType g, dataType b)
          : data(r, g, b)
      {
      }

      /**Constructs Color from BaseSSEData
       *
       */
      inline Color (const SSEVector &newData)
          : data(newData)
      {
      }

      /**Sets color
       * Each value should be in range 0 <= value <= COLOR_MAX_VALUE
       *
       * @param r red
       * @param g green
       * @param b blue
       */
      inline void setColor (dataType r, dataType g, dataType b)
      {
        data [R] = r;
        data [G] = g;
        data [B] = b;
      }

      /**Reset color to DEFAULT_COLOR_VALUE
       *
       * Default:
       *  r = 0
       *  g = 0
       *  b = 0
       *
       */
      inline void setDefaultColor ()
      {
        setColor(DEFAULT_COLOR_VALUE, DEFAULT_COLOR_VALUE, DEFAULT_COLOR_VALUE);
      }

      /**All color components are multiplied by mulValue
       *
       * @param mulValue
       * @return
       */
      inline Color operator * (float mulValue) const
      {
        return data * mulValue;
      }

      /**All color components are multiplied by mulValue
       *
       * @param mulValue
       * @return
       */
      inline Color &operator *= (float mulValue)
      {
        data *= mulValue;
        return *this;
      }

      /**There occurs:
       * this[R] * other[R]
       * this[G] * other[G]
       * this[B] * other[B]
       *
       * @param other
       * @return
       */
      inline Color operator * (const Color& other) const
      {
        return data * other.data;
      }

      /**There occurs:
       * this[R] *= other[R]
       * this[G] *= other[G]
       * this[B] *= other[B]
       *
       * @param other
       * @return
       */
      inline Color &operator *= (const Color& other)
      {
        data *= other.data;
        return *this;
      }

      /**Adds two colors
       *
       * @param other
       * @return
       */
      inline Color &operator += (const Color& other)
      {
        data += other.data;
        return *this;
      }

      /** Returns a red component of the color
       *
       *  @return red component
       */
      inline colorType red () const
      {
        return retColor(data [R]);
      }

      /** Returns a green component of the color
       *
       *  @return green component
       */
      inline colorType green () const
      {
        return retColor(data [G]);
      }

      /** Returns a blue component of the color
       *
       *  @return blue component
       */
      inline colorType blue () const
      {
        return retColor(data [B]);
      }

    private:
      SSEVector data;

      /**Returns color value saturated to COLOR_MAX_VALUE
       *
       * @param color float representation of color
       * @return color value
       */
      inline colorType retColor (float color) const
      {
        saturate(color);

        return uRound <colorType>(color);
      }

      /**Saturates given color to COLOR_MAX_VALUE
       *
       * @param color RGB data
       */
      inline void saturate (SSEVector &color) const
      {
        saturate(color [R]);
        saturate(color [G]);
        saturate(color [B]);
      }

      /**Saturates given color value to COLOR_MAX_VALUE
       *
       * @param color float representation of color
       */
      inline void saturate (float &color) const
      {
        if (color > COLOR_MAX_VALUE)
        {
          color = COLOR_MAX_VALUE;
        }
      }
  };
}
