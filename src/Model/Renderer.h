/// @file Main/Model/Renderer.h
/// @date 01-12-2011
/// @author Mikołaj Milej

#pragma once

#include <QScopedPointer>

#include "Controller/GlobalDefines.h"
#include "Model/ModelDefines.h"

namespace Model {
  //Forward declarations -->
  class Ray;
  class Scene;
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
      Renderer ();
      ~Renderer ();

      /**Renders part of image which is described by tile
       * "run" is checked at the beginning of each loop.
       * If run == false then rendering stops
       *
       * @param tile part of image
       * @param run permission for rendering
       */
      void render (const RenderTileData &tile, const bool &run);

      /**Sets 3D scene to render
       *
       * @param scene 3d scene to render
       */
      inline void setScene (const Model::Scene *newScene){
        scene = newScene;
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

      const RenderTileData *tilePtr;

      const Scene *scene;

      /**It checks if given ray intersects with any object in scene
       * If there is no intersection with given ray then returned color
       * is equal to default color
       *
       * @param ray ray to check intersection with
       * @param resultColor color at the intersection point
       * @param viewDistance maximum range to check intersections
       */
      void shootRay (Ray & ray, Color &resultColor, worldUnit viewDistance) const;

      Q_DISABLE_COPY (Renderer)
  };
}

