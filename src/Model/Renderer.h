/// @file Model/Renderer.h

#pragma once

#include <QScopedPointer>
#include <QImage>
#include "Controller/GlobalDefines.h"
#include "Model/ModelDefines.h"
#include "Model/Vector.h"

class QString;

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

      /**Constructs renderer with given rendering parameters
       *
       * @param renderParams
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

      /**Sets rendering parameters
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
       * @param objectWeAreIn object in which current ray starts
       */
      void shootRay (Ray & ray,
                     Color &resultColor,
                     worldUnit viewDistance,
                     int refractionDepth,
                     const VisibleObject *objectWeAreIn) const;

      /**Used to calculate color of transparent object; shoots refracted rays
       *
       * @param ray ray going into the object
       * @param transparency transparency coefficient
       * @param refractionDepth how many times ray can pass through transparent surface
       * @param mainViewDistance whole distance on the scene
       * @param rayStartIntersectDist distance between start of the ray and nearest intersection
       * @param lightContrCoef coefficient which determines light contribution
       * @param correction needed to correct new ray's starting point
       * @param normalAtIntersection normal at intersection point
       * @param intersection point of intersection
       * @param currentObject object which ray has collision with
       * @param objectWeAreIn ray goes from that object to another one (the one ray has collision with)
       * @return color of transparency
       */
      Color shootRefractedRay (const Ray &ray,
                               float transparency,
                               int refractionDepth,
                               worldUnit mainViewDistance,
                               worldUnit rayStartIntersectDist,
                               float &lightContrCoef,
                               Vector &correction,
                               Vector &normalAtIntersection,
                               Point &intersection,
                               const VisibleObject &currentObject,
                               const VisibleObject *objectWeAreIn) const;

      /**Calculates refracted ray direction
       *
       * @param ray ray which goes into the object
       * @param currentObject object which ray has collision with
       * @param objectWeAreIn ray goes from that object to another one (the one ray has collision with)
       * @param normalAtIntersection normal at intersection point
       * @return returns 0 when refraction had place; -1 in other case
       */
      int calculateRefraction (Ray &ray,
                               const VisibleObject &currentObject,
                               const VisibleObject &objectWeAreIn,
                               const Vector &normalAtIntersection) const;

      Q_DISABLE_COPY (Renderer)
  };
}

