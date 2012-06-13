/// @file Model/Point.h

#pragma once

#include <QMatrix4x4>
#include <QVector3D>

#include "Controller/GlobalDefines.h"
#include "Model/ModelDefines.h"
#include "Model/SSEData.h"

namespace Model
{

  /**3D Point class
   *
   */
  class Point
  {
    public:
      typedef worldUnit dataType;

      SSEVector data;

      inline Point ()
      {
      }

      /**Constructs point from SSEVector
       *
       * @param newData new coordinates
       */
      inline Point (const SSEVector &newData)
          : data(newData)
      {
      }

      /**Constructs object with given parameters
       *
       * @param x position on x axis in 3D space
       * @param y position on y axis in 3D space
       * @param z position on z axis in 3D space
       */
      inline Point (const dataType &x, const dataType &y, const dataType &z)
          : data(x, y, z)
      {
      }

      /**Sets coordinates of object
       *
       * @param x position on x axis in 3D space
       * @param y position on y axis in 3D space
       * @param z position on z axis in 3D space
       */
      inline void set (dataType x, dataType y, dataType z)
      {
        data [X] = x;
        data [Y] = y;
        data [Z] = z;
      }

      /**Sets coordinates of object
       *
       * @param x position on x axis in 3D space
       * @param y position on y axis in 3D space
       */
      inline void set (const dataType &x, const dataType &y)
      {
        data [X] = x;
        data [Y] = y;
      }

      /**Returns position at given index
       *
       * @param idx index of coordinate to get
       * @return coordinate at given index
       */
      inline float &operator [] (int idx)
      {
        return data [idx];
      }

      /**Returns position at given index
       *
       * @param idx index of coordinate to get
       * @return coordinate at given index
       */
      inline float operator [] (int idx) const
      {
        return data [idx];
      }

      /**Rotates coordinates through angle degrees given in other
       *
       * @param other angles
       */
      void rotate (const Point &other)
      {
        QMatrix4x4 rotateMatrix;
        rotateMatrix.rotate(other [Z], 0, 0, 1); //rotate about Z axis
        rotateMatrix.rotate(other [Y], 0, 1, 0); //rotate about Y axis
        rotateMatrix.rotate(other [X], 1, 0, 0); //rotate about X axis

        QVector3D result(
            rotateMatrix.mapVector(QVector3D(data [X], data [Y], data [Z])));

        data [X] = result.x();
        data [Y] = result.y();
        data [Z] = result.z();
      }

      /**SSEVector typecast operator
       * It provides typecasting Point into SSEVector
       *
       */
      inline &operator SSEVector ()
      {
        return data;
      }

      /**SSEVector typecast operator
       * It provides typecasting Point into SSEVector
       *
       */
      inline &operator const SSEVector () const
      {
        return data;
      }
  };

} /* namespace Model */

