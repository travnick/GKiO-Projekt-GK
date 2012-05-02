/// @file Model/Point.h
/// @date 01-12-2011
/// @author Mikołaj Milej

#pragma once

#include <x86intrin.h>

#include "Controller/GlobalDefines.h"
#include "Model/ModelDefines.h"
#include "Model/SSEData.h"

// Calculate dot product from xmm[3/2/1] and store sesult to xmm[0]
#define  DOT_PROD_MASK  0b11100001

namespace Model {

  /**3D Point class
   *
   */
  class Point {
    public:
      typedef worldUnit dataType;
      typedef float coordsUnit;

      SSEData data;

      inline Point (){
      }

      /**Constructs object with given parameters
       *
       * @param x position on x axis in 3D space
       * @param y position on y axis in 3D space
       * @param z position on z axis in 3D space
       */
      inline Point (const dataType &x, const dataType &y, const dataType &z){
        set(x, y, z);
      }

      /**Sets coordinates of object
       *
       * @param x position on x axis in 3D space
       * @param y position on y axis in 3D space
       * @param z position on z axis in 3D space
       */
      inline void set (dataType x, dataType y, dataType z){
        data.x() = x;
        data.y() = y;
        data.z() = z;
      }

      /**Sets coordinates of object
       *
       * @param x position on x axis in 3D space
       * @param y position on y axis in 3D space
       */
      inline void set (const dataType &x, const dataType &y){
        data.x() = x;
        data.y() = y;
      }

      /**Calculates dot product of two vectors
       * You should give here Point::date
       *
       * It uses SSE4.1 if avaliable or SSE3 instead.
       * No SSE2 support now.
       *
       * @param vector2 __m128 secondVector
       * @return dotProduct
       */
      inline worldUnit dotProduct (const __m128 &vector2) const{
        float dotProd;
        IGNORE_WARNINGS_BEGIN
#ifdef __SSE4_1__
        _mm_store_ss( &dotProd, _mm_dp_ps(data, vector2, DOT_PROD_MASK));
#else
        __m128 result = _mm_mul_ps(data, vector2);
        result = _mm_hadd_ps(result, result);
        result = _mm_hadd_ps(result, result);
        _mm_store_ss( &dotProd, result);
#endif
        IGNORE_WARNINGS_END
        return dotProd;
      }
  };

} /* namespace Model */

