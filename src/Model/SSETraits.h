#pragma once

#include <QMatrix4x4>
#include <QVector3D>

#include "common.h"
#include "Controller/GlobalDefines.h"
#include "Model/SSEData.h"

namespace Model
{
  class DummyClass
  {
  };

  template <typename T, typename Inherit = DummyClass>
  class SSEVectorTraits: public SSEVector, public Inherit
  {
    public:
      typedef T dataType;

      SSEVectorTraits ()
      {
      }

      /**Constructs point from SSEVector
       *
       * @param newData new coordinates
       */
      inline SSEVectorTraits (const SSEVector &newData)
          : SSEVector(newData)
      {
      }

      /**Constructs object with given parameters
       *
       * @param x position on x axis in 3D space
       * @param y position on y axis in 3D space
       * @param z position on z axis in 3D space
       */
      inline SSEVectorTraits (dataType x, dataType y, dataType z)
          : SSEVector(x, y, z)
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
        (*this) [X] = x;
        (*this) [Y] = y;
        (*this) [Z] = z;
      }

      /**Sets coordinates of object
       *
       * @param x position on x axis in 3D space
       * @param y position on y axis in 3D space
       */
      inline void set (dataType x, dataType y)
      {
        (*this) [X] = x;
        (*this) [Y] = y;
      }

      /**Rotates coordinates through angle degrees given in other
       *
       * @param other angles
       */
      template <typename U>
      void rotate (const U &other)
      {
        QMatrix4x4 rotateMatrix;
        rotateMatrix.rotate(other [Z], 0, 0, 1); //rotate about Z axis
        rotateMatrix.rotate(other [Y], 0, 1, 0); //rotate about Y axis
        rotateMatrix.rotate(other [X], 1, 0, 0); //rotate about X axis

        QVector3D result(
            rotateMatrix.mapVector(
                QVector3D( (*this) [X], (*this) [Y], (*this) [Z])));

        set(result.x(), result.y(), result.z());
      }
  };

} /* namespace Model */
