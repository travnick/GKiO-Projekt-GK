/// @file Main/Model/Vector.h
/// @date 01-12-2011
/// @author Mikołaj Milej

#pragma once

#include "Model/VpCommon.h"
#include "Model/PointAndVectorOperations.h"

namespace Model {

  /**3D Vector class
   *
   */
  class Vector: public VPCommon {
    public:

      /**Sets current vector length to -1
       * Length of vector is checked during normalization.
       * If length >= 0 then it means that normalization was done already.
       *
       */
      inline Vector (){
      }

      /**Copy constructor
       * It does deep copy of another vector
       *
       * @param other vector to copy data from
       */
      inline Vector (const Vector &other)
          : VPCommon(){
        VPCommon::operator =(other);
      }

      inline virtual ~Vector (){
      }

      /**Normalizes vector
       * Current length is stored to coords[PW].
       *
       */
      inline void normalize (){
        if (coords [PW] < 0)
        {
          PVOperations::normalize(coords);
        }
      }

      /**Copy operator
       * It does deep copy of another vector
       *
       * @param other vector to copy data from
       * @return self
       */
      inline Vector &operator= (const Vector &other){
        VPCommon::operator =(other);
        return *this;
      }

      /**Sets parameters of vector
       *
       * @param x length on x axis in 3D space
       * @param y length on y axis in 3D space
       * @param z length on z axis in 3D space
       */
      inline void set (const dataType &x, const dataType &y, const dataType &z){
        VPCommon::set(x, y, z);
        this->coords [PW] = -1;
      }
  };
}
