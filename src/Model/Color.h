/// @file Main/Model/Color.h
/// @date 01-12-2011
/// @author Mikołaj Milej

#pragma once

//#include <x86intrin.h> // SIMD functions
#include "Controller/GlobalDefines.h"

#define DEFAULT_COLOR_VALUE 0

namespace Model {

  /**Color class
   *
   */
  class Color {
    private:
      typedef unsigned short int privateColorValueDataType;

      class ColorValue {
        public:
          ColorValue (int value);

          ColorValue ();

          ColorValue & operator= (const int& value);

          ColorValue & operator+= (const int& value);

          ColorValue & operator+= (const ColorValue& value);

          inline operator colorType () const;

        private:
          privateColorValueDataType data;
      };

    public:
      typedef privateColorValueDataType colorValueDataType;

      typedef ColorValue dataType;

      inline Color (){
        setDefaultColor();
      }

      /**Sets color
       * Each value should be in range 0 <= value <= 255
       *
       * @param r red
       * @param g green
       * @param b blue
       */
      inline void setColor (const dataType & r, const dataType & g, const dataType & b){
        red = r;
        green = g;
        blue = b;
      }

      /**Reset color to default
       * Default:
       *  r = 0
       *  g = 0
       *  b = 0
       *
       */
      inline void setDefaultColor (){
        setColor(DEFAULT_COLOR_VALUE, DEFAULT_COLOR_VALUE, DEFAULT_COLOR_VALUE);
      }

      dataType red;
      dataType green;
      dataType blue;
  };

  inline Color::ColorValue::ColorValue (int value)
      : data(value){
  }

  inline Color::ColorValue::ColorValue (){
  }

  inline Color::ColorValue & Color::ColorValue::operator= (const int& value){
    data = value;
    if (data > COLOR_MAX_VALUE)
    {
      data = COLOR_MAX_VALUE;
    }
    return *this;
  }

  inline Color::ColorValue & Color::ColorValue::operator+= (const int& value){
    data += value;
    if (data > COLOR_MAX_VALUE)
    {
      data = COLOR_MAX_VALUE;
    }

    return *this;
  }

  inline Color::ColorValue & Color::ColorValue::operator+= (const ColorValue& value){

    return ( *this) += value.data;
  }

  inline Color::ColorValue::operator colorType () const{
    return static_cast <colorType>(data);
  }

}
