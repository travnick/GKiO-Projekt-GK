/// @file Main/Model/Renderer.cpp
/// @date 01-12-2011
/// @author Mikołaj Milej

#include <cmath>

#include "Model/Renderer.h"
#include "Model/Ray.h"
#include "Model/Point.h"
#include "Model/Vector.h"
#include "Model/Camera.h"
#include "Model/Scene.h"
#include "Model/Color.h"
#include "Model/VisibleObject.h"
#include "Model/PointAndVectorOperations.h"
#include "Model/RenderTileData.h"
#include "Model/Light.h"
#include "Model/Material.h"

#define REFLECTION_DEEP 10
#define E 2.71828183f
#define DEFAULT_INTERSECTION_ERROR_VALUE 0.01f

namespace Model {

  const float LAMBERT_MIN = (0.49f / (COLOR_MAX_VALUE * COLOR_MAX_VALUE));

  Renderer::Renderer (){
    tmpDistance.reset(new Vector);
    pointLightDist.reset(new Vector);
    distanceToIntersection.reset(new Vector);
    lightRay.reset(new Ray);
    normalAtIntersection.reset(new Vector);
    intersection.reset(new Point);
    intersectionErrorValue = DEFAULT_INTERSECTION_ERROR_VALUE;
  }

  Renderer::~Renderer (){
  }

  void Renderer::render (const RenderTileData &tile, const bool &run){
    const Camera *camera = scene->getCamera().data();
    worldUnit viewDistance = scene->getCamera()->getViewDistance();
    Vector direction( *(camera->getDirection()));
    Point start(camera->getPosition());
    Ray ray;
    Color rayResult;
    tilePtr = &tile;

    imageUnit diffToNewLine = BPP * (tile.imageWidth - tile.width);
    imageUnit R = BPP * (tile.topLeft.x + tile.topLeft.y * tile.imageWidth);
    imageUnit G = R + 1;
    imageUnit B = G + 1;

    Camera::unitType rayStartX = camera->getScreenTopLeft()->data.x();
    Camera::unitType rayStartY = camera->getScreenTopLeft()->data.y();

    Camera::unitType line = rayStartY + tile.topLeft.y * camera->getScreenImageWidthRatio();
    Camera::unitType col;

    for (imageUnit iLine = tile.topLeft.y; iLine < tile.bottomRight.y; ++iLine)
    {
      col = rayStartX + tile.topLeft.x * camera->getScreenImageWidthRatio();
      //Checking if thread is allowed to run
      for (imageUnit iCol = tile.topLeft.x; run && iCol < tile.bottomRight.x; ++iCol)
      {
        start.set(col, line);

        if (camera->getType() == Camera::Conic)
        {
          camera->getDirection(start, direction);
        }

        ray.setParams(start, direction);

        rayResult.setDefaultColor();
        shootRay(ray, rayResult, viewDistance);

        tile.imageData [R] = rayResult.red();
        R += BPP;

        tile.imageData [G] = rayResult.green();
        G += BPP;

        tile.imageData [B] = rayResult.blue();
        B += BPP;

        col += camera->getScreenImageWidthRatio();
      }
      line += camera->getScreenImageWidthRatio();

      R += diffToNewLine;
      G += diffToNewLine;
      B += diffToNewLine;
    }
  }

  inline void Renderer::shootRay (Ray & ray, Color &resultColor, worldUnit mainViewDistance) const{
    Scene::ObjectContainerIterator endObjects = scene->getObjects().end();
    Scene::ObjectContainerIterator currentObject = endObjects;
    worldUnit coef = 1;
    worldUnit tmpLightCoef = 0;
    worldUnit viewDistance = mainViewDistance;
    int visibleSize = REFLECTION_DEEP;
//#ifdef USE_MMX
//    Color tmpColor;
//#else
//    Color::dataType tmpColor;
//#endif
    bool inShadow;

    //Because qRound(0.49f * COLOR_MAX_VALUE / COLOR_MAX_VALUE) < 1
    while (visibleSize-- > 0 && coef > (0.45f / COLOR_MAX_VALUE))
    {
      viewDistance = mainViewDistance;
      for (Scene::ObjectContainerIterator object = scene->getObjects().begin(); object < endObjects;
          object++)
      {
        if (( *object)->checkRay(ray, viewDistance, *tmpDistance) == true)
        {
          viewDistance -= intersectionErrorValue;
          currentObject = object;
        }
      }

      if (currentObject != endObjects)
      {
        PVOperations::multiply(viewDistance, ray.getDir().data, distanceToIntersection->data);

        //Calculate intersection point
        PVOperations::move(ray.getStart().data, distanceToIntersection->data, intersection->data);

        //Get normal vector at intersection point
        ( *currentObject)->getNormal( *intersection, *normalAtIntersection);

        Material *currentMat = scene->getMaterials() [( *currentObject)->getMaterial()].data();

        tmpLightCoef = (1.0f - currentMat->getReflection()) * coef * (1.0f / COLOR_COUNT);

        //Calculate light contribution
        Scene::LighContainerIterator endLights = scene->getLights().end();
        for (Scene::LighContainerIterator light = scene->getLights().begin(); light < endLights;
            light++)
        {
          PVOperations::diff(( *light)->getPosition().data, intersection->data,
                             pointLightDist->data);
          if (normalAtIntersection->dotProduct(pointLightDist->data) <= 0.0f)
          {
            continue;
          }

          pointLightDist->normalize();

          if (pointLightDist->length < 0.0f)
          {
            continue;
          }

          lightRay->setParams( *intersection, *pointLightDist);
          // Computation of the shadows
          inShadow = false;

          for (Scene::ObjectContainerIterator object = scene->getObjects().begin();
              object < endObjects; object++)
          {
            if (( *object)->checkRay( *lightRay, pointLightDist->length, *tmpDistance) == true)
            {
              inShadow = true;
              break;
            }
          }

          if ( !inShadow)
          {
            // Lambert lighting model + hacked light attenuation
            worldUnit lambert = lightRay->getDir().dotProduct(normalAtIntersection->data) * 1
                * ( *light)->power * pow(1.01f, -pointLightDist->length) * tmpLightCoef;

            // qRound(0.49f / (COLOR_MAX_VALUE * COLOR_MAX_VALUE)) < 1
            if (lambert > LAMBERT_MIN)
            {
              resultColor += ( * *light) * currentMat->getColor() * lambert;
            }
          }
        }

        coef *= currentMat->getReflection();

        //Calculate reflection vector
        PVOperations::multiply(ray.getDir().dotProduct(normalAtIntersection->data) * 2,
                               normalAtIntersection->data, normalAtIntersection->data);
        PVOperations::subVectors(ray.getDir().data, normalAtIntersection->data, ray.getDir().data);
        ray.getDir().normalize();
        ray.setParams( *intersection);

        currentObject = endObjects;
      }
      else
      {
        return;
      }
    }
  }

  void Renderer::calculateDistancePrecision (){
    intersectionErrorValue = floor(log10(scene->getCamera()->getViewDistance())) - FLOAT_PRECISION;
    intersectionErrorValue = pow(10, intersectionErrorValue);
  }

}
