/// @file UsableCpp/include/PointAndVectorOperations.h
/// @date 01-12-2011
/// @author Mikołaj Milej

#pragma once

#include <x86intrin.h> // SIMD functions
#include <cmath>
#include <Model/ModelDefines.h>
#include <Model/Point.h>

namespace Model {
  namespace PVOperations {

    /**Moves point by a vector and save new point in result
     * You should give here Point::data
     *
     * @param point __m128
     * @param vector __m128
     * @param result __m128
     */
    inline void move (const __m128 &point, const __m128 &vector, __m128 &result){
      result = _mm_add_ps(point, vector);
    }

    /**Moves point by a reversed vector and save new point in result
     * You should give here Point::data
     *
     * @param point __m128
     * @param vector __m128
     * @param result __m128
     */
    inline void negMove (const __m128 &point, const __m128 &vector, __m128 &result){
      result = _mm_sub_ps(point, vector);
    }

    /**Calculates distance between two points
     * You should give here Point::data
     *
     * @param point1 __m128
     * @param point2 __m128
     * @param result __m128
     */
    inline void diff (const __m128 &point1, const __m128 &point2, __m128 &result){
      result = _mm_sub_ps(point1, point2);
    }

    /**Adds two vectors
     * You should give here Point::data
     *
     * @param vector1 __m128
     * @param vector2 __m128
     * @param result __m128
     */
    inline void addVectors (const __m128 &vector1, const __m128 &vector2, __m128 &result){
      result = _mm_add_ps(vector1, vector2);
    }

    /**Subtracts two vectors
     * You should give here Point::data
     *
     * @param vector1 __m128
     * @param vector2 __m128
     * @param result __m128
     */
    inline void subVectors (const __m128 &vector1, const __m128 &vector2, __m128 &result){
      result = _mm_sub_ps(vector1, vector2);
    }

    /**Multiplies vector by a constant
     * You should give here Point::data
     *
     * @param constant worldUnit
     * @param vector __m128
     * @param result __m128
     */
    inline void multiply (const worldUnit &constant, const __m128 &vector, __m128 &result){
      result = _mm_mul_ps(vector, _mm_set1_ps(constant));
    }
  }
}
