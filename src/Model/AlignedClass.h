/// @file AlignedClass.h

#pragma once

namespace Model {

  /**Class that provides data alignment
   *
   */
  template <int alignmentSize>
  class AlignedClass {
    public:
      //For x86_64 we don't need this because of default 16B heap alignment (ABI)
#if USE_SSE == 1
#if USE_POINTERS == 1
      //TODO: Implement other operators new and delete

      /**Allocates memory aligned to specified boundary
       *
       * @param size size of data to allocate memory for
       */
      static void* operator new (size_t size)
      {
        return _mm_malloc(size, alignmentSize);
      }

      /**Deallocates aligned memory
       *
       * @param pointer memory to deallocate
       */
      static void operator delete (void *pointer)
      {
        _mm_free(pointer);
      }
#endif
      // Alignment for static object creation
    }__attribute__((aligned(alignmentSize)));
#else
  };
#endif

} /* namespace Model */
