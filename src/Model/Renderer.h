/// @file Model/Renderer.h

#pragma once

#include <QScopedPointer>

#include "Controller/GlobalDefines.h"
#include "Model/ModelDefines.h"

namespace Controller
{
  //Forward declarations -->
  struct RenderParams;
// <-- Forward declarations
}

namespace Model
{
  //Forward declarations -->
  class Color;
  class Point;
  class VisibleObject;
  class Ray;
  class Vector;
  struct RenderTileData;
  // <-- Forward declarations

  /**Renderer class
   *
   */
  class Renderer
  {
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

      /**
       *
       * @param newRenderParams
       */
      inline void setRenderParams (const Controller::RenderParams * newRenderParams)
      {
        renderParams = newRenderParams;
      }

    private:
      //Internal temporary -->
      QScopedPointer <Ray> lightRay;
      /**Vector from ray start point do intersection point
       *
       */
      QScopedPointer <Vector> rayStartIntersect;
      /**Vector from intersection point to light position
       *
       */
      QScopedPointer <Vector> pointLightDist;
      /**It is used for temporary calculations
       *
       */
      QScopedPointer <Vector> tmpDistance;
      // <-- Internal temporary

      const Controller::RenderParams * renderParams;

      /**It checks if given ray intersects with any object in scene
       * If there is no intersection with given ray then returned color
       * is equal to default color
       *
       * @param ray ray to check intersection with
       * @param resultColor color at the intersection point
       * @param viewDistance maximum range to check intersections
       * @param refractionDepth maximum depth of refraction
       */
      void shootRay (Ray & ray,
                     Color &resultColor,
                     worldUnit viewDistance,
                     int refractionDepth) const;

      /**
       *
       */
      Color shootRefractedRay (const Ray &ray,
                               float transparency,
                               int refractionDepth,
                               worldUnit mainViewDistance,
                               worldUnit viewDistance,
                               worldUnit &tmpLightCoef,
                               Vector &correction,
                               Vector &normalAtIntersection,
                               Point &intersection,
                               VisibleObject &currentObject) const;

      /**
       *
       */
      int calculateRefraction (Ray &ray,
                               VisibleObject &currentObject,
                               Vector *normalAtIntersection) const;

      Q_DISABLE_COPY (Renderer)
  };
}

