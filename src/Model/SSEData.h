/// @file SSEData.h
/// @date 28-04-2012
/// @author Mikołaj Milej

#pragma  once

#include "Model/AlignedClass.h"
#include "Model/ModelDefines.h"

namespace Model {

  /**Class that provides easy access to SSE __m128 data structure
   *
   */
  class SSEData: public AlignedClass <16> {
      /**Internal private struct that provides aligned data for SSE
       *
       */
      class SSEDataPrivate: public AlignedClass <16> {
        public:
          union {
              __m128 data;
              float dataArray [4];
          };

          inline SSEDataPrivate (){
          }

          inline SSEDataPrivate (const __m128 &other){
            data = other;
          }

#ifdef __x86_64__
          inline SSEDataPrivate (const SSEDataPrivate &other){
            data = other.data;
          }

          //Some hacks over pointers;
          inline SSEDataPrivate *operator -> () const{
            return const_cast <SSEDataPrivate*>(this);
          }
#endif
      };

#ifndef __x86_64__
      SSEDataPrivate * data;
#else
      SSEDataPrivate data;
#endif
    public:
      enum Positions {
        X = 3, Y = 2, Z = 1, W = 0, COORDS_COUNT = 4
      };

#ifndef __x86_64__
      SSEData ()
          : data(new SSEDataPrivate){
      }

      SSEData (const SSEData &other)
          : data(new SSEDataPrivate( *other.data)){
      }

      SSEData (const __m128 &newData)
      : data(new SSEDataPrivate(newData)){
      }

      ~SSEData (){
        delete data;
      }

#else
      inline SSEData (){
      }

      inline SSEData (const __m128 &newData)
          : data(newData){
      }
#endif

      inline SSEData & operator = (const SSEData &other){
        data->data = other.data->data;
        return ( *this);
      }

      inline SSEData & operator = (const __m128 &sseData){
        data->data = sseData;
        return ( *this);
      }

      inline float & operator[] (int idx) const{
        return data->dataArray [idx];
      }

      inline & operator __m128 () const{
        return sse();
      }

      inline __m128 & sse () const{
        return data->data;
      }

      inline float & x () const{
        return data->dataArray [X];
      }

      inline float & y () const{
        return data->dataArray [Y];
      }

      inline float & z () const{
        return data->dataArray [Z];
      }

      inline float & w () const{
        return data->dataArray [W];
      }
  };

} /* namespace Model */
