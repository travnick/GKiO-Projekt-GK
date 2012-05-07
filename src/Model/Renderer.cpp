/// @file Model/Renderer.cpp
/// @date 01-12-2011
/// @author Mikołaj Milej

#include "Model/Renderer.h"
#include "Model/Ray.h"
#include "Model/Point.h"
#include "Model/Vector.h"
#include "Model/Camera.h"
#include "Model/Scene.h"
#include "Model/Color.h"
#include "Model/VisibleObject.h"
#include "Model/RenderTileData.h"
#include "Model/Light.h"
#include "Model/Material.h"
#include "Controller/RendererThread.h"

const float E = 2.71828183f;

namespace Model {

  //Because qRound(0.49f * COLOR_MAX_VALUE / COLOR_MAX_VALUE) < 1
  const float COLOR_MIN_VALUE = 0.5f / COLOR_MAX_VALUE;

  Renderer::Renderer (const Controller::RenderParams &newRenderParams)
      : tmpDistance(new Vector), pointLightDist(new Vector), distanceToIntersection(new Vector), lightRay(
          new Ray), normalAtIntersection(new Vector), intersection(new Point){
    setRenderParams( &newRenderParams);
  }

  Renderer::~Renderer (){
  }

  void Renderer::render (const RenderTileData &tile){
    const Camera *camera = renderParams->scene->getCamera().data();
    worldUnit viewDistance = renderParams->scene->getCamera()->getViewDistance();
    Vector direction;
    Point startOnScreen(camera->getScreenTopLeft());
    Point currentOnScreen;
    Ray ray;
    Color rayResult;

    imageUnit diffToNewLine = BPP * (tile.imageWidth - tile.width);
    imageUnit R = BPP * (tile.topLeft.x + tile.topLeft.y * tile.imageWidth);
    imageUnit G = R + 1;
    imageUnit B = G + 1;

    startOnScreen.data += camera->screenWidthDelta.data * tile.topLeft.x;
    startOnScreen.data += camera->screenHeightDelta.data * tile.topLeft.y;

    for (imageUnit iLine = tile.topLeft.y; iLine < tile.bottomRight.y; ++iLine)
    {
      currentOnScreen.data = startOnScreen.data;

      for (imageUnit iCol = tile.topLeft.x; renderParams->allowRunning && iCol < tile.bottomRight.x;
          ++iCol)
      //Checking if thread is allowed to run: renderParams->allowRunning
      {
        if (camera->getType() == Camera::Conic)
        {
          camera->getDirection(currentOnScreen, direction);
        }

        ray.setParams(currentOnScreen, direction);

        rayResult.setDefaultColor();
        shootRay(ray, rayResult, viewDistance);

        tile.imageData [R] = rayResult.red();
        R += BPP;

        tile.imageData [G] = rayResult.green();
        G += BPP;

        tile.imageData [B] = rayResult.blue();
        B += BPP;

        currentOnScreen.data += camera->screenWidthDelta.data;
      }

      startOnScreen.data += camera->screenHeightDelta.data;

      R += diffToNewLine;
      G += diffToNewLine;
      B += diffToNewLine;
    }
  }

  inline void Renderer::shootRay (Ray & ray, Color &resultColor, worldUnit mainViewDistance) const{
    Scene::ObjectContainerIterator endObjects = renderParams->scene->getObjects().end();
    Scene::ObjectContainerIterator currentObject = endObjects;
    worldUnit coef = 1;
    worldUnit tmpLightCoef = 0;
    worldUnit viewDistance = mainViewDistance;
    int visibleSize = renderParams->reflectionDeep;
    bool inShadow;

    while (visibleSize-- > 0)
    {
      viewDistance = mainViewDistance;
      for (Scene::ObjectContainerIterator object = renderParams->scene->getObjects().begin();
          object < endObjects; object++)
      {
        if (( *object)->checkRay(ray, viewDistance, *tmpDistance) == true)
        {
          viewDistance -= DEFAULT_INTERSECTION_ERROR_VALUE;
          currentObject = object;
        }
      }

      if (currentObject != endObjects)
      {
        ray.getDir().data.multiply(viewDistance, distanceToIntersection->data);

        //Calculate intersection point
        ray.getStart().data.move(distanceToIntersection->data, intersection->data);

        //Get normal vector at intersection point
        ( *currentObject)->getNormal( *intersection, *normalAtIntersection);

        Material *currentMat =
            renderParams->scene->getMaterials() [( *currentObject)->getMaterial()].data();

        //(1.0f / COLOR_COUNT) because few lines bellow we do color * color
        tmpLightCoef = (1.0f - currentMat->getReflection()) * coef * (1.0f / COLOR_COUNT);

        //Calculate light contribution
        Scene::LighContainerIterator endLights = renderParams->scene->getLights().end();
        for (Scene::LighContainerIterator light = renderParams->scene->getLights().begin();
            light < endLights; light++)
        {
          ( *light)->getPosition().data.diff(intersection->data, pointLightDist->data);
          if (normalAtIntersection->dotProduct(pointLightDist->data) <= 0.0f)
          {
            continue;
          }

          pointLightDist->normalize();

          if (pointLightDist->length < DEFAULT_INTERSECTION_ERROR_VALUE)
          {
            continue;
          }

          lightRay->setParams( *intersection, *pointLightDist);
          // Computation of the shadows
          inShadow = false;

          //TODO: Can we check this once before starting rendering?
          if (renderParams->shadows)
          {
            for (Scene::ObjectContainerIterator object = renderParams->scene->getObjects().begin();
                object < endObjects; object++)
            {
              if (( *object)->checkRay( *lightRay, pointLightDist->length, *tmpDistance) == true)
              {
                inShadow = true;
                break;
              }
            }
          }

          if ( !inShadow && pointLightDist->length < 1500)
          //because pow(1.01f, - (>1500)) gives too small result and float underflow
          {
            // Lambert lighting model + hacked light attenuation
            worldUnit lambert = lightRay->getDir().dotProduct(normalAtIntersection->data) * 1
                * ( *light)->power * pow(1.01f, -pointLightDist->length) * tmpLightCoef;

            resultColor += ( * *light) * currentMat->getColor() * lambert;
          }
        }

        coef *= currentMat->getReflection();

        if (coef < COLOR_MIN_VALUE)
        {
          break;
        }

        //Calculate reflection vector
        normalAtIntersection->data *= ray.getDir().dotProduct(normalAtIntersection->data) * 2;
        ray.getDir().data -= normalAtIntersection->data;
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

}
