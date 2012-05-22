/// @file Model/Renderer.h

#pragma once

#include <QScopedPointer>

#include "Controller/GlobalDefines.h"
#include "Model/ModelDefines.h"

namespace Controller {
  //Forward declarations -->
  struct RenderParams;
// <-- Forward declarations
}

namespace Model {
  //Forward declarations -->
  class Ray;
  class Color;
  struct RenderTileData;
  class Vector;
  class Point;
  // <-- Forward declarations

  /**Renderer class
   *
   */
  class Renderer {
    public:

      /**Initializes temporary fields
       *
       */
      Renderer (const Controller::RenderParams &renderParams);

      /**Needed for QScopedPointer
       *
       */
      ~Renderer ();

      /**Renders part of image which is described by tile
       *
       * @param tile part of image
       */
      void render (const RenderTileData &tile);

      /**Sets 3D scene to render
       *
       * @param scene 3d scene to render
       */
      inline void setRenderParams (const Controller::RenderParams * newRenderParams){
        renderParams = newRenderParams;
      }

    private:
      //Internal temporary -->
      QScopedPointer <Vector> tmpDistance;
      QScopedPointer <Vector> pointLightDist;
      QScopedPointer <Vector> distanceToIntersection;
      QScopedPointer <Ray> lightRay;
      QScopedPointer <Vector> normalAtIntersection;
      QScopedPointer <Point> intersection;
      // <-- Internal temporary

      const Controller::RenderParams * renderParams;

      /**It checks if given ray intersects with any object in scene
       * If there is no intersection with given ray then returned color
       * is equal to default color
       *
       * @param ray ray to check intersection with
       * @param resultColor color at the intersection point
       * @param viewDistance maximum range to check intersections
       */
      void shootRay (Ray & ray,
                     Color &resultColor,
                     worldUnit viewDistance) const;

      Q_DISABLE_COPY (Renderer)
  };
}

