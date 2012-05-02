/// @file Model/Vector.h
/// @date 01-12-2011
/// @author Mikołaj Milej

#pragma once

#include "Model/Point.h"
#include "Model/PointAndVectorOperations.h"
#include "Controller/GlobalDefines.h"

namespace Model {

  /**3D Vector class
   *
   */
  class Vector: public Point {
    public:
      worldUnit length;

      /**Square length is equal to dot product with self
       *
       */
      worldUnit squareLength;

      Vector ()
          : length(0), squareLength(0){
      }

      /**Normalizes vector
       * Length before normalization is stored to length
       *
       */
      inline void normalize (){
        calculateLength();

        worldUnit constant = 1.0f / length;

        PVOperations::multiply(constant, data, data);
      }

      /**Calculate length of vector
       *
       */
      inline void calculateLength (){
        length = SQRT(dotProduct());
      }

      /**Calculates dot product of vector with self
       * Result is also stored in squareLength
       *
       * It uses SSE4.1 if avaliable or SSE3 instead.
       * No SSE2 support now.
       *
       * @return dotProduct
       */
      inline worldUnit dotProduct (){
        IGNORE_WARNINGS_BEGIN
#ifdef __SSE4_1__
        _mm_store_ss( &squareLength, _mm_dp_ps(data, data, DOT_PROD_MASK));
#else
        __m128 result = _mm_mul_ps(data, data);
        result = _mm_hadd_ps(result, result);
        result = _mm_hadd_ps(result, result);
        _mm_store_ss( &squareLength, result);
#endif
        IGNORE_WARNINGS_END
        return squareLength;
      }

      /**Calculates dot product with other vector or point
       * You should give here Point::data
       *
       * @param vector2 other vector or point
       * @return dotProduct
       */
      inline worldUnit dotProduct (const __m128 &vector2) const{
        return Point::dotProduct(vector2);
      }

      /**Sets parameters of vector
       *
       * @param x length on x axis in 3D space
       * @param y length on y axis in 3D space
       * @param z length on z axis in 3D space
       */
      inline void set (const dataType &x, const dataType &y, const dataType &z){
        Point::set(x, y, z);

        //previous data are invalid
        length = -1;
        squareLength = -1;
      }
  };
}
