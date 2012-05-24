/// @file Model/Renderer.cpp

#include "Controller/RendererThread.h"
#include "Model/Camera.h"
#include "Model/Color.h"
#include "Model/Light.h"
#include "Model/Material.h"
#include "Model/Point.h"
#include "Model/Ray.h"
#include "Model/Renderer.h"
#include "Model/RenderTileData.h"
#include "Model/Scene.h"
#include "Model/Vector.h"
#include "Model/VisibleObject.h"

const float E = 2.71828183f;

using namespace Model;

//Because qRound(0.49f * COLOR_MAX_VALUE / COLOR_MAX_VALUE) < 1
const float COLOR_MIN_VALUE = 0.5f / COLOR_MAX_VALUE;

Renderer::Renderer (const Controller::RenderParams &newRenderParams)
    : tmpDistance(new Vector), pointLightDist(new Vector), distanceToIntersection(
        new Vector), lightRay(new Ray), normalAtIntersection(new Vector), intersection(
        new Point) {
  setRenderParams( &newRenderParams);
}

Renderer::~Renderer () {
}

void Renderer::render (const RenderTileData &tile) {
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

    for (imageUnit iCol = tile.topLeft.x;
        renderParams->allowRunning && iCol < tile.bottomRight.x; ++iCol)
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

inline void Renderer::shootRay (Ray & ray,
                                Color &resultColor,
                                worldUnit mainViewDistance) const {
  Scene::ObjectIt endObjects = renderParams->scene->getObjects().end();
  Scene::ObjectIt currentObject = endObjects;
  worldUnit coef = 1;
  worldUnit tmpLightCoef = 0;
  worldUnit viewDistance = mainViewDistance;
  int reflecionDeep = renderParams->reflectionDeep;
  bool inShadow;

  while (reflecionDeep-- > 0)
  {
    viewDistance = mainViewDistance;
    //Find intersection
    for (Scene::ObjectIt object = renderParams->scene->getObjects().begin();
        object < endObjects; object++)
    {
      if ( ( *object)->checkRay(ray, viewDistance, *tmpDistance) == true)
      {
        viewDistance -= DEFAULT_INTERSECTION_ERROR_VALUE;
        currentObject = object;
      }
    }

    //If there is any intersection?
    if (currentObject != endObjects)
    {
      ray.getDir().data.multiply(viewDistance, distanceToIntersection->data);

      //Calculate intersection point
      ray.getStart().data.move(distanceToIntersection->data,
                               intersection->data);

      //Get normal vector at intersection point
      ( *currentObject)->getNormal( *intersection, *normalAtIntersection);

      //(1.0f / COLOR_COUNT) because few lines bellow we do color * color
      tmpLightCoef = (1.0f - ( *currentObject)->getMaterial()->getReflection())
          * coef * (1.0f / COLOR_COUNT);

      //Calculate light contribution
      Scene::LighIt endLights = renderParams->scene->getLights().end();
      for (Scene::LighIt light = renderParams->scene->getLights().begin();
          light < endLights; light++)
      {
        ( *light)->getPosition().data.diff(intersection->data,
                                           pointLightDist->data);
        //If angle is >= 90 degrees
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
          for (Scene::ObjectIt object =
              renderParams->scene->getObjects().begin(); object < endObjects;
              object++)
          {
            if ( ( *object)->checkRay( *lightRay, pointLightDist->length,
                                      *tmpDistance) == true)
            {
              inShadow = true;
              break;
            }
          }
        }

        if ( !inShadow)
        {
          // Lambert lighting model
          float lambert = lightRay->getDir().dotProduct(
              normalAtIntersection->data);

          //light attenuation -->
          float lightPower = (0.01 * pointLightDist->length
              + 0.001 * pointLightDist->length * pointLightDist->length);

          float lightPowerSpecular = ( *light)->power
              * pow(1.01f, -pointLightDist->length);

          if (lightPower < 1.0)
          {
            lightPower = 1.0;
          }

          float specular = pow(
              pointLightDist->data.dotProduct(normalAtIntersection->data),
              ( *currentObject)->getMaterial()->getSpecularPower());

          lightPower = ( *light)->power / lightPower;
          //<--light attenuation

          lightPower *= lambert * tmpLightCoef;
          lightPowerSpecular *= specular * tmpLightCoef;

          resultColor += ( * *light)
              * ( *currentObject)->getMaterial()->getColor() * lightPower;
          resultColor += ( * *light)
              * ( *currentObject)->getMaterial()->getSpecularColor()
              * lightPowerSpecular;
        }
      }

      coef *= ( *currentObject)->getMaterial()->getReflection();

      if (coef < COLOR_MIN_VALUE)
      {
        break;
      }

      //Calculate reflection vector
      normalAtIntersection->data *= ray.getDir().dotProduct(
          normalAtIntersection->data) * 2;
      ray.getDir().data -= normalAtIntersection->data;
      ray.setParams( *intersection);
      //ray is still normalized;

      currentObject = endObjects;
    }
    else
    {
      return;
    }
  }
}
