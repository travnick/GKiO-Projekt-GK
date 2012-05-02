/// @file Model/Ray.h
/// @date 01-12-2011
/// @author Mikołaj Milej

#pragma once

#include "Model/Point.h"
#include "Model/Vector.h"

namespace Model {

  /**Ray class
   *
   */
  class Ray {
    public:
      inline Ray (){
      }

      /**Creates ray with given start point and direction
       *
       * @param start start point of vector
       * @param direction direction of vector
       */
      inline Ray (const Point &newStart, const Vector &newDirection)
          : start(newStart), direction(newDirection){
      }

      /**Sets ray start point and direction
       *
       * @param start start point of vector
       * @param direction direction of vector
       */
      inline void setParams (const Point &newStart, const Vector &newDirection){
        start = newStart;
        direction = newDirection;
      }

      /**Sets ray start point
       *
       * @param start start point of vector
       */
      inline void setParams (const Point &newStart){
        start = newStart;
      }

      /**Returns start point of vector
       *
       * @return start point of vector
       */
      inline const Point &getStart () const{
        return start;
      }

      /**Returns direction of vector
       *
       * @return direction of vector
       */
      inline Vector &getDir (){
        return direction;
      }

      /**Returns direction of vector
       *
       * @return direction of vector
       */
      inline const Vector &getDir () const{
        return direction;
      }

    private:
      Point start;
      Vector direction;
  };
}

