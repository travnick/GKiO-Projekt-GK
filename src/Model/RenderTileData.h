/// @file Model/RenderTileData.h
/// @date 29-12-2011
/// @author Mikołaj Milej

#pragma once

#include <QtGlobal>

#include "Controller/GlobalDefines.h"
#include "Model/Point2D.h"

namespace Model {

  /**Tile information class
   *
   */
  class RenderTileData {
    public:
      quint64 imageDataSize;
      Point2D topLeft;
      Point2D bottomRight;
      imageUnit width;
      imageUnit height;
      imageUnit imageWidth;
      imageUnit imageHeight;
      colorType *imageData;
      bool deleteImageData;

      inline RenderTileData (){
        imageData = 0;
        deleteImageData = true;
      }

      inline ~RenderTileData (){
        if (deleteImageData)
        {
          free(imageData);
          imageData = 0;
        }
      }
  };
}
