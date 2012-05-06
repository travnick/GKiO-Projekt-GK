/// @file SSEData.h
/// @date 28-04-2012
/// @author Mikołaj Milej

#pragma  once

#ifdef __x86_64__
#define USE_SSE 1
#endif

#include <x86intrin.h>

#include "Model/AlignedClass.h"
#include "Model/ModelDefines.h"

// Calculate dot product from xmm[3/2/1] and store sesult to xmm[0]
#define  DOT_PROD_MASK  0b11100001

namespace Model {

#if USE_SSE == 1
  typedef __m128 SSEDataBaseType;
#else
  class SSEDataPrivate;
  typedef SSEDataPrivate SSEDataBaseType;
#endif

  enum Positions {
    X = 3, Y = 2, Z = 1, W = 0
  };

  /**Internal private struct that provides aligned data for SSE
   *
   */
  class SSEDataPrivate {
      union {
#if USE_SSE == 1
          __m128 data;
#endif
          float dataArray [4];
      };

    public:

      inline SSEDataPrivate (){
        // It's important, without determining W value there is random performance loss
        ( *this) [W] = 1;
      }

      inline SSEDataPrivate (float x, float y, float z){
        ( *this) [X] = x;
        ( *this) [Y] = y;
        ( *this) [Z] = z;
        // It's important, without determining W value there is random performance loss
        ( *this) [W] = 1;
      }

      inline SSEDataPrivate (const __m128 &other){
        *this = other;
      }

      inline SSEDataPrivate (const SSEDataPrivate &other){
        *this = other;
      }

#if USE_SSE == 1
      inline & operator __m128 (){
        return sse();
      }

      inline __m128 & sse (){
        return data;
      }

      inline const __m128 & sse () const{
        return data;
      }
#endif

      inline SSEDataPrivate & operator = (const SSEDataBaseType &other){
#if USE_SSE == 1
        data = other;
#else
        ( *this) [X] = other [X];
        ( *this) [Y] = other [Y];
        ( *this) [Z] = other [Z];
#endif
        return ( *this);
      }

      inline float & operator[] (int idx){
        return dataArray [idx];
      }

      inline float operator[] (int idx) const{
        return dataArray [idx];
      }

      //Some hacks to act like pointer. I't was needed in version with pointers
      inline const SSEDataPrivate *operator -> () const{
        return this;
      }
      inline SSEDataPrivate *operator -> (){
        return this;
      }

      inline SSEDataPrivate &operator * (){
        return *this;
      }

      inline const SSEDataPrivate &operator * () const{
        return *this;
      }
  };

  /**Class that provides easy access to SSE __m128 data structure
   *
   */
  class SSEData {

      SSEDataPrivate internalData;
    public:

      inline SSEData (){
      }

      inline SSEData (const SSEDataBaseType &newData)
          : internalData(newData){
      }

      inline SSEData (float newX, float newY, float newZ)
          : internalData(newX, newY, newZ){
      }

      inline SSEData & operator = (const SSEData &other){
        *internalData = *other.internalData;
        return ( *this);
      }

      inline SSEData & operator = (const SSEDataBaseType &sseData){
        *internalData = sseData;
        return ( *this);
      }

      inline float & operator[] (int idx){
        return ( *internalData) [idx];
      }

      inline float operator[] (int idx) const{
        return ( *internalData) [idx];
      }

      inline float & x (){
        return ( *internalData) [X];
      }

      inline float & y (){
        return ( *internalData) [Y];
      }

      inline float & z (){
        return ( *internalData) [Z];
      }

      inline float & w (){
        return ( *internalData) [W];
      }

      inline float x () const{
        return ( *internalData) [X];
      }

      inline float y () const{
        return ( *internalData) [Y];
      }

      inline float z () const{
        return ( *internalData) [Z];
      }

      inline float w () const{
        return ( *internalData) [W];
      }

      // operations on data -->

      /**Moves point by a vector and save new point in result
       * You should give here Point::data
       *
       * @param vector __m128
       * @param result __m128
       */
      inline void move (const SSEData &vector, SSEData &result) const{
        result = ( *this) + vector;
      }

      /**Moves point by a reversed vector and save new point in result
       * You should give here Point::data
       *
       * @param vector __m128
       * @param result __m128
       */
      inline void negMove (const SSEData &vector, SSEData &result) const{
        result = ( *this) - vector;
      }

      /**Calculates distance between two points
       * You should give here Point::data
       *
       * @param point2 __m128
       * @param result __m128
       */
      inline void diff (const SSEData &point2, SSEData &result) const{
        result = ( *this) - point2;
      }

      /**Adds two vectors
       * You should give here Point::data
       *
       * @param vector1 __m128
       * @param vector2 __m128
       * @param result __m128
       */
      inline void addVectors (const SSEData &vector2, SSEData &result) const{
        result = ( *this) + vector2;
      }

      /**Subtracts two vectors
       * You should give here Point::data
       *
       * @param vector1 __m128
       * @param vector2 __m128
       * @param result __m128
       */
      inline void subVectors (const SSEData &vector2, SSEData &result) const{
        result = ( *this) - vector2;
      }

      /**Multiplies vector by a constant
       *
       * @param constant worldUnit
       * @param result __m128
       */
      inline void multiply (const worldUnit &constant, SSEData &result) const{
        result = ( *this) * constant;
      }

      /**adds vector by a constant
       * You should give here Point::data
       *
       * @param other SSEData
       * @param result SSEData
       */
      inline SSEDataBaseType operator + (const SSEData &other) const{
#if USE_SSE == 1
        return _mm_add_ps(const_cast <SSEData&>( *this), const_cast <SSEData&>(other));
#else
        return SSEDataBaseType(( *this) [X] + other [X], ( *this) [Y] + other [Y],
            ( *this) [Z] + other [Z]);
#endif
      }

      /**adds vector by a constant
       * You should give here Point::data
       *
       * @param other SSEData
       * @param result SSEData
       */
      inline SSEData &operator += (const SSEData &other){
        *internalData = *this + other;
        return *this;
      }

      /**Subtracts other data from this
       * You should give here Point::data
       *
       * @param other SSEData
       * @param result SSEData
       */
      inline SSEDataBaseType operator - (const SSEData &other) const{
#if USE_SSE == 1
        return _mm_sub_ps(const_cast <SSEData&>( *this), const_cast <SSEData&>(other));
#else
        return SSEDataBaseType(( *this) [X] - other [X], ( *this) [Y] - other [Y],
            ( *this) [Z] - other [Z]);
#endif
      }

      /**Subtracts other data from this and store to this
       * You should give here Point::data
       *
       * @param other SSEData
       * @param result SSEData
       */
      inline SSEData &operator -= (const SSEData &other){
        *internalData = ( *this) - other;
        return *this;
      }

      /**Multiplies vector by a constant
       * You should give here Point::data
       *
       * @param constant worldUnit
       * @param result __m128
       */
      inline SSEDataBaseType operator * (worldUnit constant) const{
        SSEData tmpData;
        tmpData [X] = constant;
        tmpData [Y] = constant;
        tmpData [Z] = constant;
        return *this * tmpData;
      }

      /**Multiplies vector by a constant
       * You should give here Point::data
       *
       * @param constant worldUnit
       * @param result __m128
       */
      inline SSEDataBaseType operator * (const SSEData &other) const{
#if USE_SSE == 1
        return _mm_mul_ps(const_cast <SSEData&>( *this), const_cast <SSEData&>(other));
#else
        return SSEDataBaseType(( *this) [X] * other [X], ( *this) [Y] * other [Y],
            ( *this) [Z] * other [Z]);
#endif
      }

      /**Multiplies vector by a constant
       * You should give here Point::data
       *
       * @param constant worldUnit
       * @param result __m128
       */
      inline SSEData &operator *= (worldUnit constant){
        *internalData = ( *this) * constant;
        return *this;
      }

      /**Multiplies vector by a constant
       * You should give here Point::data
       *
       * @param constant worldUnit
       * @param result __m128
       */
      inline SSEData &operator *= (const SSEData &other){
        *internalData = *this * other;
        return *this;
      }

      /**Calculates dot product of two vectors
       * You should give here Point::date
       *
       * It uses SSE4.1 if avaliable or SSE3 instead.
       * No SSE2 support now.
       *
       * @param other __m128 secondVector
       * @return dotProduct
       */
      inline float dotProduct (const SSEData &other) const{
        float dotProd;
        IGNORE_WARNINGS_BEGIN

#if USE_SSE == 1
#ifdef __SSE4_1__
        _mm_store_ss(
            &dotProd,
            _mm_dp_ps(const_cast <SSEData&>( *this), const_cast <SSEData&>(other), DOT_PROD_MASK));
#else
        __m128 result = _mm_mul_ps(const_cast <SSEData&>( *this), const_cast <SSEData&>(other));
        result = _mm_hadd_ps(result, result);
        result = _mm_hadd_ps(result, result);
        _mm_store_ss( &dotProd, result);
#endif
#else
        dotProd = ( *this) [X] * other [X] + ( *this) [Y] * other [Y] + ( *this) [Z] * other [Z];
#endif
        IGNORE_WARNINGS_END
        return dotProd;
      }

      /**Calculates cross product of two vectors
       * You should give here Point::date
       *
       * @param other __m128 secondVector
       * @return dotProduct
       */
      inline SSEData crossProduct (const SSEData &other) const{
        SSEData crossProd;

        crossProd [X] = ( *this) [Y] * other [Z] - ( *this) [Z] * other [Y];
        crossProd [Y] = ( *this) [Z] * other [X] - ( *this) [X] * other [Z];
        crossProd [Z] = ( *this) [X] * other [Y] - ( *this) [Y] * other [X];

        return crossProd;
      }

      /**Calculates dot product of vector with self
       * Result is also stored in squareLength
       *
       * It uses SSE4.1 if avaliable or SSE3 instead.
       * No SSE2 support now.
       *
       * @return dotProduct
       */
      inline float dotProduct () const{
        return dotProduct( *this);
      }
      // <-- operations on data

      inline SSEDataBaseType operator - () const{
        return SSEDataPrivate( -( *this) [X], -( *this) [Y], -( *this) [Z]);
      }

      inline void negate (){
        SSEData tmp(0, 0, 0);
        *this = tmp - *this;
      }

    private:
#if USE_SSE == 1
      inline & operator __m128 (){
        return sse();
      }

      inline const __m128 & sse () const{
        return internalData->sse();
      }

      inline __m128 & sse (){
        return *internalData;
      }
#endif
  };

} /* namespace Model */
