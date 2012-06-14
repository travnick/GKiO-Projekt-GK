/// @file SSEData.h

#pragma  once

#define USE_SSE 1

#ifdef __x86_64__

# if USE_SSE == 1
#   define USE_POINTERS 0
# endif

#else

//There is no gain with using SSE and wasting time for mallocs.
# undef USE_SSE

# if USE_SSE == 1
#   define USE_POINTERS 1
# endif

#endif

//Include for SIMD operations
#include <x86intrin.h>

#include "Model/ModelDefines.h"

// Calculate dot product from xmm[3/2/1] and store result to xmm[0]
#define  DOT_PROD_MASK  0b11100001

namespace Model
{
  union SSEData;
  typedef SSEData BaseSSEData;

  class SSEVector;

  enum Axis
  {
    X = 3, Y = 2, Z = 1, W = 0
  };

  /**Multiplies two vectors
   *
   * @param first T
   * @param other T
   * @return BaseSSEData
   */
  template <typename T>
  inline BaseSSEData operator * (const T &first, const T &other);

  /**Multiplies vector by a constant
   *
   * @param first T
   * @param constant float
   * @return BaseSSEData
   */
  template <typename T>
  inline BaseSSEData operator * (const T &first, float constant);

  /**Multiplies vector by a constant
   *
   * @param constant float
   * @param other T
   * @return BaseSSEData
   */
  template <typename T>
  inline BaseSSEData operator * (float constant, const T &other);

  /**Multiplies two vectors
   *
   * @param first BaseSSEData
   * @param other SSEVector
   * @return BaseSSEData
   */
  inline BaseSSEData operator * (const BaseSSEData &first,
                                 const SSEVector &other);

  /**Multiplies two vectors
   *
   * @param first SSEVector
   * @param other BaseSSEData
   * @return BaseSSEData
   */
  inline BaseSSEData operator * (const SSEVector &first,
                                 const BaseSSEData &other);

  /**Adds two vectors
   *
   * @param first T
   * @param other T
   * @return BaseSSEData
   */
  template <typename T>
  inline BaseSSEData operator + (const T &first, const T &other);

  /**Subtracts two vectors
   *
   * @param first T
   * @param other T
   * @return BaseSSEData
   */
  template <typename T>
  inline BaseSSEData operator - (const T &first, const T &other);

  //---------------------------------------------------------------------------------

  /**union that provides aligned data for SSE with easy access to them
   *
   * __m128 and float [4] are nearly the same but compiler require __m128
   * to SSE operations so there is hack to do easy access to manipulate
   * this data without shuffles, moves etc.
   */
  union SSEData
  {
      __m128 data;
      float dataArray [4];

      //------------------

      inline SSEData ()
      {
        setDefaults();
      }

      /**Constructs SSEData from other
       *
       * @param other object to copy data from
       */
      inline SSEData (const SSEData &other)
      {
#if USE_SSE == 1
        data = other.data;
#else
        ( *this) [X] = other [X];
        ( *this) [Y] = other [Y];
        ( *this) [Z] = other [Z];
#endif
      }

      /**Constructs SSEData with given parameters
       *
       * @param x
       * @param y
       * @param z
       */
      inline SSEData (float x, float y, float z)
      {
        setDefaults();

        (*this) [X] = x;
        (*this) [Y] = y;
        (*this) [Z] = z;
      }

#if USE_SSE == 1
      /**Constructs SSEData from __m128 data
       *
       * @param other object to copy data from
       */
      inline SSEData (const __m128 &other)
      {
        data = other;
      }
#endif

      /**Copies parameters from other
       *
       * @param other object to copy data from
       * @return this
       */
      inline SSEData & operator = (const SSEData &other)
      {
#if USE_SSE == 1
        data = other.data;
#else
        ( *this) [X] = other [X];
        ( *this) [Y] = other [Y];
        ( *this) [Z] = other [Z];
#endif
        return (*this);
      }

      /**Sets default values
       * Zeroing is important when using SSE.
       * Without determining W value there is performance loss
       * because of operating on NaNs etc.
       */
      inline void setDefaults ()
      {
#if USE_SSE == 1
        (*this) [W] = 0;
#endif
      }

      /**Returns value at given index
       *
       * @param idx index of value to get
       * @return value at given index
       */
      inline float & operator[] (int idx)
      {
        return dataArray [idx];
      }

      /**Returns value at given index
       *
       * @param idx index of value to get
       * @return value at given index
       */
      inline float operator[] (int idx) const
      {
        return dataArray [idx];
      }

      //Some hacks to act like pointer. It's needed in version with pointers
      inline const SSEData *operator -> () const
      {
        return this;
      }

      inline SSEData *operator -> ()
      {
        return this;
      }

      inline SSEData &operator * ()
      {
        return *this;
      }

      inline const SSEData &operator * () const
      {
        return *this;
      }
      //End of hacks

      /**__m128 typecast operator
       * It provides typecasting SSEData into __m128
       *
       */
      inline & operator __m128 ()
      {
        return data;
      }
  };

  //---------------------------------------------------------------------------------

  /**Class that provides operations on SSE Data
   *
   */
  class SSEVector
  {
#if USE_POINTERS == 1
      SSEData *internalData;
#else
      SSEData internalData;
#endif

    public:

#if USE_POINTERS == 1
      SSEVector ()
      : internalData(new SSEData)
      {
      }

      SSEVector (const SSEVector &other)
      : internalData(new SSEData(other.getInternalData()))
      {
      }

      SSEVector (const BaseSSEData &other)
      : internalData(new SSEData(other))
      {
      }

      inline SSEVector (float newX, float newY, float newZ)
      : internalData(new SSEData(newX, newY, newZ))
      {
      }

      ~SSEVector ()
      {
        delete internalData;
      }

      inline SSEVector & operator = (const SSEVector &other)
      {
        *internalData = other.getInternalData();
        return ( *this);
      }

#else

      inline SSEVector ()
      {
      }

      /**Constructs SSEVector from BaseSSEData
       *
       * @param newData object to copy data from
       */
      inline SSEVector (const BaseSSEData &newData)
          : internalData(newData)
      {
      }

      /**Constructs SSEVector with given data
       *
       * @param newX
       * @param newY
       * @param newZ
       */
      inline SSEVector (float newX, float newY, float newZ)
          : internalData(newX, newY, newZ)
      {
      }

#endif

      /**Copies data from other
       *
       * @param newData object to copy data from
       * @return this
       */
      inline SSEVector & operator = (const BaseSSEData &other)
      {
        *internalData = other;
        return (*this);
      }

      /**Returns value at given index
       *
       * @param idx index of value to get
       * @return value at given index
       */
      inline float & operator[] (int idx)
      {
        return (*internalData) [idx];
      }

      /**Returns value at given index
       *
       * @param idx index of value to get
       * @return value at given index
       */
      inline float operator[] (int idx) const
      {
        return (*internalData) [idx];
      }

// operations on data -->

      /**Moves point by a vector and save new point in result
       * You should give here Point::data
       *
       * @param vector SSEVector
       * @param result SSEVector
       */
      inline void move (const SSEVector &vector, SSEVector &result) const
      {
        result = *this + vector;
      }

      /**Moves point by a reversed vector and save new point in result
       * You should give here Point::data
       *
       * @param vector SSEVector
       * @param result SSEVector
       */
      inline void negMove (const SSEVector &vector, SSEVector &result) const
      {
        result = *this - vector;
      }

      /**Calculates distance between two points
       * You should give here Point::data
       *
       * @param point2 SSEVector
       * @param result SSEVector
       */
      inline void diff (const SSEVector &point2, SSEVector &result) const
      {
        result = *this - point2;
      }

      /**Adds vector2 to self and store it in result
       * You should give here Point::data
       *
       * @param vector2 SSEVector
       * @param result SSEVector
       */
      inline void addVectors (const SSEVector &vector2, SSEVector &result) const
      {
        result = *this + vector2;
      }

      /**Subtracts vector2 from self and store it in result
       * You should give here Point::data
       *
       * @param vector2 SSEVector
       * @param result SSEVector
       */
      inline void subVectors (const SSEVector &vector2, SSEVector &result) const
      {
        result = *this - vector2;
      }

      /**Multiplies self by a constant
       *
       * @param constant float
       * @param result SSEVector
       */
      inline void multiply (float constant, SSEVector &result) const
      {
        result = *this * constant;
      }

      /**Adds vector to self
       * You should give here Point::data
       *
       * @param other SSEVector
       * @return this SSEVector
       */
      inline SSEVector &operator += (const SSEVector &other)
      {
        *internalData = *this + other;
        return *this;
      }

      /**Subtracts other vector from this
       * You should give here Point::data
       *
       * @param other SSEVector
       * @return this SSEVector
       */
      inline SSEVector &operator -= (const SSEVector &other)
      {
        *internalData = (*this) - other;
        return *this;
      }

      /**Multiplies self by a constant
       *
       * @param constant worldUnit
       * @return this SSEVector
       */
      inline SSEVector &operator *= (float constant)
      {
        *internalData = *this * constant;
        return *this;
      }

      /**Multiplies self by other vector
       * You should give here Point::data
       *
       * @param other SSEVector
       * @return this SSEVector
       */
      inline SSEVector &operator *= (const SSEVector &other)
      {
        *internalData = *this * (other);
        return *this;
      }

      /**Calculates dot product of vector with self
       * Result is also stored in squareLength
       *
       * @return dotProduct
       */
      inline float dotProduct () const
      {
        return dotProduct(*this);
      }

      IGNORE_WARNINGS_BEGIN
      /**Calculates dot product with other vector
       * You should give here Point::date
       *
       * @param other SSEVector secondVector
       * @return dotProduct
       */
      inline float dotProduct (const BaseSSEData &other) const
      {
        float dotProd;

#if __SSE4_1__ == 1 &&  USE_SSE == 1
        _mm_store_ss(
            &dotProd,
            _mm_dp_ps(const_cast <__m128 &>(internalData->data), const_cast <__m128 &>(other.data), DOT_PROD_MASK));
#elif __SSE3__ == 1 &&  USE_SSE == 1
        __m128 result = _mm_mul_ps(const_cast <SSEVector&>( *this), const_cast <BaseSSEData&>(other));
        result = _mm_hadd_ps(result, result);
        result = _mm_hadd_ps(result, result);
        _mm_store_ss( &dotProd, result);
#else
        dotProd = ( *this) [X] * other [X] + ( *this) [Y] * other [Y]
        + ( *this) [Z] * other [Z];
#endif
        return dotProd;
      }
      IGNORE_WARNINGS_END

      /**Calculates cross product with other vector
       * You should give here Point::data
       *
       * @param other SSEVector secondVector
       * @return crossProduct SSEVector
       */
      inline SSEVector crossProduct (const SSEVector &other) const
      {
        SSEVector crossProd;

        crossProd [X] = (*this) [Y] * other [Z] - (*this) [Z] * other [Y];
        crossProd [Y] = (*this) [Z] * other [X] - (*this) [X] * other [Z];
        crossProd [Z] = (*this) [X] * other [Y] - (*this) [Y] * other [X];

        return crossProd;
      }

      inline BaseSSEData operator - () const
      {
        return BaseSSEData(- (*this) [X], - (*this) [Y], - (*this) [Z]);
      }

      /**Negates vector
       *
       */
      inline void negate ()
      {
        (*this) [X] = - (*this) [X];
        (*this) [Y] = - (*this) [Y];
        (*this) [Z] = - (*this) [Z];
      }
// <-- operations on data

      /**SSEData typecast operator
       * It provides typecasting SSEVector into SSEData
       *
       */
      inline & operator SSEData ()
      {
        return *internalData;
      }

      /**SSEData typecast operator
       * It provides typecasting SSEVector into SSEData
       *
       */
      inline & operator const SSEData () const
      {
        return *internalData;
      }

      /**__m128 typecast operator
       * It provides typecasting SSEVector into __m128
       *
       */
      inline & operator __m128 ()
      {
        return internalData->data;
      }

    private:

      //Some hacks to act like pointer. It's needed in version with pointers
      inline const SSEVector *operator -> () const
      {
        return this;
      }

      inline SSEVector *operator -> ()
      {
        return this;
      }

      inline SSEVector &operator * ()
      {
        return *this;
      }

      inline const SSEVector &operator * () const
      {
        return *this;
      }
      //End of hacks
  };

  //---------------------------------------------------------------------------------

  template <typename T>
  inline BaseSSEData operator * (const T &first, const T &other)
  {
#if USE_SSE == 1
    return _mm_mul_ps(const_cast <T&>(first), const_cast <T&>(other));
#else
    return BaseSSEData(first [X] * other [X], first [Y] * other [Y],
        first [Z] * other [Z]);
#endif
  }

  template <typename T>
  inline BaseSSEData operator * (const T &first, float constant)
  {
    T tmpData(constant, constant, constant);
    return tmpData * first;
  }

  template <typename T>
  inline BaseSSEData operator * (float constant, const T &other)
  {
    return other * constant;
  }

  inline BaseSSEData operator * (const BaseSSEData &first,
                                 const SSEVector &other)
  {
    return first * other;
  }

  inline BaseSSEData operator * (const SSEVector &first,
                                 const BaseSSEData &other)
  {
    return other * first;
  }

  template <typename T>
  inline BaseSSEData operator + (const T &first, const T &other)
  {
#if USE_SSE == 1
    return _mm_add_ps(const_cast <T&>(first), const_cast <T&>(other));
#else
    return BaseSSEData(first [X] + other [X], first [Y] + other [Y],
        first [Z] + other [Z]);
#endif
  }

  template <typename T>
  inline BaseSSEData operator - (const T &first, const T &other)
  {
#if USE_SSE == 1
    return _mm_sub_ps(const_cast <T&>(first), const_cast <T&>(other));
#else
    return BaseSSEData(first [X] - other [X], first [Y] - other [Y],
        first [Z] - other [Z]);
#endif
  }

} /* namespace Model */
