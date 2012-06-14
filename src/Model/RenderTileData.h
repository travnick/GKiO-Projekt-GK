/// @file Model/RenderTileData.h

#pragma once

#include <QtGlobal>

#include "Controller/GlobalDefines.h"
#include "Model/Point2D.h"

namespace Model
{

  /**Tile information class
   *
   */
  class RenderTileData
  {
    public:
      /**Amount of the memory consumed by an image
       *
       */
      quint64 imageDataSize;
      Point2D topLeft;
      Point2D bottomRight;
      imageUnit height;
      imageUnit width;
      imageUnit imageHeight;
      imageUnit imageWidth;

      /**Pointer to memory allocated for image
       * Don't delete it until program close
       *
       */
      colorType *imageData;
      bool deleteImageData;

      inline RenderTileData ()
      {
        imageData = 0;
        deleteImageData = true;
      }

      /**Frees image memory only once
       *
       */
      inline ~RenderTileData ()
      {
        if (deleteImageData)
        {
          free(imageData);
          imageData = 0;
        }
      }
  };
}
