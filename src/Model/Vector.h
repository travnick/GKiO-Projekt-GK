/// @file Model/Vector.h

#pragma once

#include "Controller/GlobalDefines.h"
#include "Model/Point.h"

namespace Model
{

  /**3D Vector class
   *
   */
  class Vector: public Point
  {
    public:
      worldUnit length;

      /**Square length is equal to dot product with self
       *
       */
      worldUnit squareLength;

      Vector ()
          : length(0), squareLength(0)
      {
      }

      Vector (const SSEVector &newData)
          : Point(newData), length(0), squareLength(0)
      {
      }

      /**Constructs object with given parameters
       *
       * @param x position on x axis in 3D space
       * @param y position on y axis in 3D space
       * @param z position on z axis in 3D space
       */
      inline Vector (const dataType &x, const dataType &y, const dataType &z)
          : Point(x, y, z), length(0), squareLength(0)
      {
      }

      /**Normalizes vector
       * Length before normalization is stored to length
       *
       */
      inline void normalize ()
      {
        calculateLength();

        worldUnit constant = 1.0f / length;

        data *= constant;
      }

      /**Creates new normalized vector
       * Length before normalization is stored to length
       *
       */
      inline Vector toNormal () const
      {
        Vector other(*this);
        other.normalize();
        return other;
      }

      /**Calculate length of vector
       *
       */
      inline void calculateLength ()
      {
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
      inline worldUnit dotProduct ()
      {
        return squareLength = data.dotProduct();
      }

      /**Calculates dot product with other vector or point
       * You should give here Point::data
       *
       * @param vector2 other vector or point
       * @return dotProduct
       */
      inline worldUnit dotProduct (const SSEVector &vector2) const
      {
        return data.dotProduct(vector2);
      }

      /**Sets parameters of vector
       *
       * @param x length on x axis in 3D space
       * @param y length on y axis in 3D space
       * @param z length on z axis in 3D space
       */
      inline void set (const dataType &x, const dataType &y, const dataType &z)
      {
        Point::set(x, y, z);

        //previous data are invalid
        length = -1;
        squareLength = -1;
      }

      inline Vector operator * (const Vector &other) const
      {
        return data.crossProduct(other.data);
      }

      inline Vector &operator *= (const Vector &other)
      {
        data = data.crossProduct(other.data);
        return *this;
      }
  };
}
