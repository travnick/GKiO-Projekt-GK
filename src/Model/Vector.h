/// @file Model/Vector.h

#pragma once

#include "Controller/GlobalDefines.h"
#include "Model/SSETraits.h"

namespace Model
{

  /**3D Vector class
   *
   */
  class VectorPartial
  {
    public:
      typedef worldUnit dataType;
      dataType length;

      VectorPartial ()
          : length(0)
      {
      }
  };

  class Vector: public SSEVectorTraits <VectorPartial::dataType, VectorPartial>
  {
      typedef SSEVectorTraits <VectorPartial::dataType, VectorPartial> BaseType;

    public:
      using BaseType::SSEVectorTraits;
      typedef BaseType::dataType dataType;

      /**Sets parameters of vector
       *
       * @param x length on x axis in 3D space
       * @param y length on y axis in 3D space
       * @param z length on z axis in 3D space
       */
      inline void set (dataType x, dataType y, dataType z)
      {
        BaseType::set(x, y, z);

        //previous data are invalid
        length = -1;
      }

      /**Calculate length of vector
       *
       */
      inline void calculateLength ()
      {
        length = SQRT(dotProduct());
      }

      /**Normalizes vector
       * Length before normalization is stored to length
       *
       */
      inline void normalize ()
      {
        calculateLength();

        worldUnit constant = 1.0f / length;

        SSEVector::operator *=(constant);
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

      inline Vector multiply (float constant) const
      {
        return SSEVector::multiply(constant);
      }

      /**Multiplies vector by other and returns result
       *
       * @param other object to multiply with
       * @return result of multiplying
       */
      inline Vector operator * (const Vector &other) const
      {
        return crossProduct(other);
      }

      inline Vector operator * (float constant) const
      {
        return SSEVector::multiply(constant);
      }

      inline Vector &operator *= (float constant)
      {
        SSEVector::operator *=(constant);
        return *this;
      }

      /**Multiplies vector by other
       *
       * @param other object to multiply with
       * @return this
       */
      inline Vector &operator *= (const Vector &other)
      {
        *this = crossProduct(other);
        return *this;
      }
  };

}
