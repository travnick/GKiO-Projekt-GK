/// @file Main/Model/Point.h
/// @date 01-12-2011
/// @author Mikołaj Milej

#pragma once

#include "Model/VpCommon.h"

namespace Model {

  /**3D Point class
   *
   */
  class Point: public VPCommon {
    public:
      inline Point (){
      }

      /**Copy constructor
       * It does deep copy of another point
       *
       * @param other point to copy data from
       */
      inline Point (const Point &other)
          : VPCommon(){
        VPCommon::operator =(other);
      }

      /**Constructs Point with given parameters
       *
       * @param x position on x axis in 3D space
       * @param y position on y axis in 3D space
       * @param z position on z axis in 3D space
       */
      inline Point (const dataType &x, const dataType &y, const dataType &z)
          : VPCommon(x, y, z){
      }

      inline virtual ~Point (){
      }

      /**Copy operator
       * It does deep copy of another point
       *
       * @param other point to copy data from
       * @return self
       */
      inline Point &operator= (const Point &other){
        VPCommon::operator =(other);
        return *this;
      }
  };
}
