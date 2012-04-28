/// @file SSEData.h
/// @date 28-04-2012
/// @author Mikołaj Milej

#pragma  once

#include "Model/AlignedClass.h"

namespace Model {

  /**Class that provides easy access to SSE __m128 data structure
   *
   */
  class SSEData: AlignedClass <16> {
      /**Internal private struct that provides aligned data for SSE
       *
       */
      struct SSEDataPrivate: public AlignedClass <16> {
          union {
              __m128 data;
              float dataArray [4];
          };
      };

      SSEDataPrivate * data;
    public:

      SSEData ()
          : data(new SSEDataPrivate){
      }

      SSEData (const SSEData &other)
          : data(new SSEDataPrivate( *other.data)){
      }

      SSEData (const __m128 &newData)
          : data(new SSEDataPrivate){
        data->data = newData;
      }

      ~SSEData (){
        delete data;
      }

      SSEData & operator = (const SSEData &other){
        data->data = other.data->data;
        return ( *this);
      }

      SSEData & operator = (const __m128 &sseData){
        data->data = sseData;
        return ( *this);
      }

      inline float & operator[] (int idx) const{
        return data->dataArray [idx];
      }

      inline & operator __m128 () const{
        return data->data;
      }

      inline __m128& sse ()const
      {
        return data->data;
      }
    };

  } /* namespace Model */
