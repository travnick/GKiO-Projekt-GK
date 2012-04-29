/// @file Main/Model/Color.h
/// @date 01-12-2011
/// @author Mikołaj Milej

#pragma once

#include <x86intrin.h> // SIMD functions
#include "Controller/GlobalDefines.h"
#include "Model/ModelDefines.h"
#include "Model/SSEData.h"

#define DEFAULT_COLOR_VALUE 0

namespace Model {

  /**Color class
   *
   */
  class Color {
    public:
      enum ColorEnum {
        R = 3, G = 2, B = 1
      };
      typedef float dataType;

      inline Color (){
        setDefaultColor();
      }

      inline Color (const __m128 &newData)
          : data(newData){
      }

      /**Sets color
       * Each value should be in range 0 <= value <= COLOR_MAX_VALUE
       *
       * @param r red
       * @param g green
       * @param b blue
       */
      inline void setColor (dataType r, dataType g, dataType b){
        data = _mm_set_ps(r, g, b, 0);
      }

      /**Reset color to DEFAULT_COLOR_VALUE
       *
       * Default:
       *  r = 0
       *  g = 0
       *  b = 0
       *
       */
      inline void setDefaultColor (){
        setColor(DEFAULT_COLOR_VALUE, DEFAULT_COLOR_VALUE, DEFAULT_COLOR_VALUE);
      }

      /**
       *
       * @param mulValue
       * @return
       */
      inline Color operator * (float mulValue) const{
        __m128 value = _mm_set1_ps(mulValue);
        return _mm_mul_ps(data, value);
      }

      /**
       *
       * @param mulValue
       * @return
       */
      inline Color &operator *= (float mulValue){
        __m128 value = _mm_set1_ps(mulValue);
        data = _mm_mul_ps(data, value);
        return *this;
      }

      /**
       *
       * @param other
       * @return
       */
      inline Color operator * (const Color& other) const{
        return _mm_mul_ps(data, other.data);
      }

      /**
       *
       * @param other
       * @return
       */
      inline Color &operator *= (const Color& other){
        data = _mm_mul_ps(data, other.data);
        return *this;
      }

      /**
       *
       * @param other
       * @return
       */
      inline Color &operator += (const Color& other){
        data = _mm_add_ps(data, other.data);
        return *this;
      }

      inline colorType red () const{
        return retCol(data [R]);
      }

      inline colorType green () const{
        return retCol(data [G]);
      }

      inline colorType blue () const{
        return retCol(data [B]);
      }

    private:
      SSEData data;

      /**Returns color value saturated to COLOR_MAX_VALUE
       *
       * @param color float representation of color
       * @return color value
       */
      inline colorType retCol (float color) const{
        if (color > COLOR_MAX_VALUE)
        {
          color = COLOR_MAX_VALUE;
        }

        return uRound <colorType>(color);
      }
  };
}
