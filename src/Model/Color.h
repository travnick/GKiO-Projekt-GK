/// @file Main/Model/Color.h
/// @date 01-12-2011
/// @author Mikołaj Milej

#pragma once

#include <x86intrin.h> // SIMD functions
#include "Controller/GlobalDefines.h"
#include "Model/ModelDefines.h"
#include "Model/SSEData.h"

#define DEFAULT_COLOR_VALUE 0
#define USE_MMX

inline colorType uRound (float d){
  return colorType(d + 0.5f);
}

namespace Model {

  /**Color class
   *
   */
  class Color {
    public:
      typedef float dataType;

      inline Color (){
        setDefaultColor();
      }

      inline Color (const __m128 &newData)
          : data(newData){
      }

      /**Sets color
       * Each value should be in range 0 <= value <= 255
       *
       * @param r red
       * @param g green
       * @param b blue
       */
      inline void setColor (dataType r, dataType g, dataType b){
        data = _mm_set_ps(r, g, b, 0);
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

      inline Color operator * (float mulValue) const{
        __m128 value = _mm_set1_ps(mulValue);
        return _mm_mul_ps(data, value);
      }

      inline Color operator * (const Color& other) const{
        return _mm_mul_ps(data, other.data);
      }

      inline Color &operator += (const Color& other){
        data = _mm_add_ps(data, other.data);
        return *this;
      }

      inline colorType red () const{
        return retCol(data [3]);
      }

      inline colorType green () const{
        return retCol(data [2]);
      }

      inline colorType blue () const{
        return retCol(data [1]);
      }

    private:
//      union {
//          __m128 data;
//          __attribute__((aligned(16))) float dataArray [4];
//      };

      SSEData data;

      inline colorType retCol (float color) const{
        if (color > COLOR_MAX_VALUE)
        {
          color = COLOR_MAX_VALUE;
        }

        return uRound(color);
      }
  };
}
