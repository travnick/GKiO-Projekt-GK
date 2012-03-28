/// @file Main/Model/VpCommon.h
/// @date 18-01-2012
/// @author Mikołaj Milej

#pragma once

#include <malloc.h>
#include "Controller/GlobalDefines.h"
#include "Model/ModelDefines.h"

#define PX 0
#define PY 1
#define PZ 2
#define PW 3

namespace Model {

  /**Class for common data of 3D Vector and 3D Point
   *
   */
  class VPCommon {
    public:
      typedef worldUnit dataType;
      typedef float coordsUnit;
      enum COORDS {
        X = 0, Y = 1, Z = 2, L = 3, COORDS_COUNT = 4
      };

      coordsUnit coords [COORDS_COUNT];

      /**Sets coordinates of object
       *
       * @param x position on x axis in 3D space
       * @param y position on y axis in 3D space
       * @param z position on z axis in 3D space
       */
      inline void set (const dataType &x, const dataType &y, const dataType &z){
        this->coords [PX] = x;
        this->coords [PY] = y;
        this->coords [PZ] = z;
      }

      /**Sets coordinates of object
       *
       * @param x position on x axis in 3D space
       * @param y position on y axis in 3D space
       */
      inline void set (const dataType &x, const dataType &y){
        this->coords [PX] = x;
        this->coords [PY] = y;
      }

    protected:
      /**Allocates memory for coordinates
       *
       */
      inline VPCommon (){
        this->coords [PW] = -1;
      }

      /**Frees memory allocated for coordinates
       *
       */
      inline virtual ~VPCommon (){
      }

      /**Constructs object with given parameters
       *
       * @param x position on x axis in 3D space
       * @param y position on y axis in 3D space
       * @param z position on z axis in 3D space
       */
      inline VPCommon (const dataType &x, const dataType &y, const dataType &z){
        set(x, y, z);
      }

      /**Copy operator
       * Performs deep copy of other object
       *
       * @param other object to copy data from
       * @return self
       */
      inline VPCommon &operator= (const VPCommon &other){
        this->set(other.coords [PX], other.coords [PY], other.coords [PZ]);
        this->coords [PW] = other.coords [PW];

        return *this;
      }

    private:
      /**Prevent copying pure VPCommon object
       *
       */
      inline VPCommon (const VPCommon &){
      }
  };

} /* namespace Model */
