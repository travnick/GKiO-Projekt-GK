/// @file Model/Point.h
/// @date 01-12-2011
/// @author Mikołaj Milej

#pragma once

#include "Controller/GlobalDefines.h"
#include "Model/ModelDefines.h"
#include "Model/SSEData.h"


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
  };

} /* namespace Model */

