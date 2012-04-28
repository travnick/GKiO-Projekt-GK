/// @file Main/Model/Point.h
/// @date 01-12-2011
/// @author Mikołaj Milej

#pragma once

#include <x86intrin.h>

#include "Controller/GlobalDefines.h"
#include "Model/ModelDefines.h"
#include "Model/SSEData.h"

//#define PX 0
//#define PY 1
//#define PZ 2
//#define PW 3
#define PX 3
#define PY 2
#define PZ 1
#define PW 0
#define SSE_INSTR

namespace Model {

  /**3D Point class
   *
   */
  class Point {
    public:
      typedef worldUnit dataType;
      typedef float coordsUnit;
      enum COORDS {
        X = 0, Y = 1, Z = 2, W = 3, COORDS_COUNT = 4
      };

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
        data [PX] = x;
        data [PY] = y;
        data [PZ] = z;
        data [PW] = 0; // Zeroing is important for dot product

        data = _mm_set_ps(x, y, z, 0);
      }

      /**Sets coordinates of object
       *
       * @param x position on x axis in 3D space
       * @param y position on y axis in 3D space
       */
      inline void set (const dataType &x, const dataType &y){
        data [PX] = x;
        data [PY] = y;
      }

      /**Calculates dot product of two vectors
       * You should give here VPCommon::coords
       *
       * It uses SSE4.1 if avaliable or SSE3 instead.
       * No SSE2 support now.
       *
       * @param vector2 __m128 secondVector
       * @return dotProduct
       */
      inline worldUnit dotProduct (const __m128 &vector2) const{
        float dotProd;
#ifdef __SSE4_1__
        _mm_store_ss( &dotProd, _mm_dp_ps(data, vector2, 0b11100001));
#else
        __m128 result = _mm_mul_ps(data, vector2);
        result = _mm_hadd_ps(result, result);
        result = _mm_hadd_ps(result, result);
        _mm_store_ss( &dotProd, result);
#endif

        return dotProd;
      }
  };

} /* namespace Model */

