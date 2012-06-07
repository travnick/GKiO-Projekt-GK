/// @file Model/Renderer.cpp

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

namespace Model
{

//Because qRound(0.49f * COLOR_MAX_VALUE / COLOR_MAX_VALUE) < 1
  const float COLOR_MIN_VALUE = 0.5f / COLOR_MAX_VALUE;

  Renderer::Renderer (const Controller::RenderParams &newRenderParams)
      : tmpDistance(new Vector), pointLightDist(new Vector), distanceToIntersection(
          new Vector), lightRay(new Ray)
  {
    setRenderParams( &newRenderParams);
  }

  Renderer::~Renderer ()
  {
  }

  void Renderer::render (const RenderTileData &tile)
  {
    const Camera *camera = renderParams->scene->getCamera().data();
    worldUnit viewDistance =
        renderParams->scene->getCamera()->getViewDistance();
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

        int refractionDepth = renderParams->refractionDeep;

        rayResult.setDefaultColor();
        shootRay(ray, rayResult, viewDistance, refractionDepth);

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
                                  worldUnit mainViewDistance,
                                  int refractionDepth) const
  {
    Scene::ObjectContainerIterator endObjects =
        renderParams->scene->getObjects().end();
    Scene::ObjectContainerIterator currentObject = endObjects;
    worldUnit coef = 1;
    worldUnit tmpLightCoef = 0;
    worldUnit viewDistance = mainViewDistance;
    int reflecionDeep = renderParams->reflectionDeep;
    bool inShadow;

    while (reflecionDeep-- > 0)
    {
      viewDistance = mainViewDistance;
      for (Scene::ObjectContainerIterator object =
          renderParams->scene->getObjects().begin(); object < endObjects;
          object++)
      {
        if ( ( *object)->checkRay(ray, viewDistance, *tmpDistance) == true)
        {
          viewDistance -= DEFAULT_INTERSECTION_ERROR_VALUE;
          currentObject = object;
        }
      }

      if (currentObject != endObjects)
      {
        QScopedPointer <Vector> normalAtIntersection(new Vector);
        QScopedPointer <Point> intersection(new Point);

        ray.getDir().data.multiply(viewDistance, distanceToIntersection->data);

        //Calculate intersection point
        ray.getStart().data.move(distanceToIntersection->data,
                                 intersection->data);

        //Get normal vector at intersection point
        ( *currentObject)->getNormal( *intersection, *normalAtIntersection);

        Material *currentMat =
            renderParams->scene->getMaterials() [ ( *currentObject)->getMaterial()].data();

        //(1.0f / COLOR_COUNT) because few lines bellow we do color * color
        tmpLightCoef = (1.0f - currentMat->getReflection()) * coef
            * (1.0f / COLOR_COUNT);

        //Calculate light contribution
        Scene::LighContainerIterator endLights =
            renderParams->scene->getLights().end();
        for (Scene::LighContainerIterator light =
            renderParams->scene->getLights().begin(); light < endLights;
            light++)
        {
          ( *light)->getPosition().data.diff(intersection->data,
                                             pointLightDist->data);
          //if angle between light and normal vector at intersection is higher than 90 degrees
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
            for (Scene::ObjectContainerIterator object =
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
            worldUnit lambert = lightRay->getDir().dotProduct(
                normalAtIntersection->data);

            //light attenuation -->
            worldUnit lightPower = (0.01 * pointLightDist->length
                + 0.001 * pointLightDist->length * pointLightDist->length);

            worldUnit lightPowerSpecular = ( *light)->power
                * pow(1.01f, -pointLightDist->length);

            if (lightPower < 1.0)
            {
              lightPower = 1.0;
            }

            worldUnit specular = pow(
                pointLightDist->data.dotProduct(normalAtIntersection->data),
                currentMat->getSpecularPower());

            lightPower = ( *light)->power / lightPower;
            //<--light attenuation

            worldUnit transparency =
                renderParams->scene->getMaterials() [ ( *currentObject)->getMaterial()]->getTransparency();

            Color transpColor(0, 0, 0);

            if ( (transparency > 0.01f) && (refractionDepth > 0))
            {
              tmpLightCoef *= (1.0f - transparency);

              Ray newRay(ray);

              int refrResult = calculateRefraction(newRay, * *currentObject, normalAtIntersection.data());

              if (refrResult == 0)
              {
                newRay.setParams(
                    Point(
                        SSEData(
                            intersection->data
                                + ray.getDir().data
                                    * DEFAULT_INTERSECTION_ERROR_VALUE)
                            + newRay.getDir().data
                                * DEFAULT_INTERSECTION_ERROR_VALUE));
                shootRay(newRay, transpColor, mainViewDistance,
                         refractionDepth - 1);
              }
            }

            lightPower *= lambert * tmpLightCoef;
            lightPowerSpecular *= specular * tmpLightCoef;

            resultColor += ( * *light) * currentMat->getColor() * lightPower;
            // resultColor += ( * *light) * currentMat->getSpecularColor()
            //     * lightPowerSpecular;
            resultColor += transpColor * transparency;
          }
        }

        coef *= currentMat->getReflection();

        if (coef < COLOR_MIN_VALUE)
        {
          break;
        }

        //Calculate reflection vector
        normalAtIntersection->data *= ray.getDir().dotProduct(
            normalAtIntersection->data) * 2;
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

  inline int Renderer::calculateRefraction (Ray &ray,
                                            VisibleObject &currentObject, Vector *normalAtIntersection) const
  {

    float ior =
        renderParams->scene->getMaterials() [currentObject.getMaterial()]->getIOR();

//    Vector abackRayDir(ray.getDir());
//    abackRayDir.data.negate();

    float cos_alpha = -ray.getDir().dotProduct( *normalAtIntersection);
    //float cos_alpha = -abackRayDir.dotProduct( *normalAtIntersection);

    //it is ior_in/ior_out
    float ir;


    //we are going into the sphere
    if (cos_alpha > 0.0f)
    {
      ir = 1.0f / ior;
    }
    else
    {
      ir = ior;
    }

    float sin_betha2 = ir * ir * (1.0f - cos_alpha * cos_alpha);

    //
//    float cosA = 1;
//    float tmp;

    //float cos_betha = 1.0f - ir*ir *(1.0f -(cos_alpha * cos_alpha));
    //ray.getDir().data = SSEData(ray.getDir().data * ir) + SSEData(normalAtIntersection->data * (ir * cos_alpha - sqrt(abs(cos_betha))));

    if (sin_betha2 < 1.0f)
    {
      /*
       //we are in sphere
       if (cos_alpha < 0.0f)
       {
       ir = ior;
       tmp = 1.0f - ir * ir * (1.0f - cos_alpha * cos_alpha);
       cosA = -1.0f;

       cos_alpha = -cos_alpha;

       ray.getDir().data = SSEData(abackRayDir.data * ir)
       - SSEData(
       normalAtIntersection->data
       * (cosA * (ir * cos_alpha - (sqrt(tmp)))));
       ray.getDir().normalize();
       */

      //ray.getDir().data = SSEData(ray.getDir().data * ir)
      //    - SSEData(
      //       normalAtIntersection->data
      //            * (ir * (cos_alpha) + sqrt(1.0f - sin_betha2)));
      ray.getDir().data = SSEData(ray.getDir().data * ir)
          - SSEData(
              normalAtIntersection->data
                  * (ir * (cos_alpha) + sqrt(1.0f - sin_betha2)));
      ray.getDir().normalize();
      //}
      /*
       else
       {
       ir = 1.0f / ior;
       tmp = 1.0f - ir * ir * (1.0f - cos_alpha * cos_alpha);
       ray.getDir().data = SSEData(ray.getDir().data * ir)
       - SSEData(
       normalAtIntersection->data
       * (cosA * (ir * cos_alpha - (sqrt(tmp)))));
       ray.getDir().normalize();
       }

       //if (tmp > 0.0f)
       {
       */
      /*
       ray.getDir().data = SSEData(
       normalAtIntersection->data * (cosA * (ir * cos_alpha - sqrt(tmp))))
       - SSEData(abackRayDir.data * ir);

       */
      /*
       ray.getDir().data = SSEData(abackRayDir.data * ir)
       - SSEData(
       normalAtIntersection->data
       * (cosA * (ir * cos_alpha - (sqrt(tmp)))));
       ray.getDir().normalize();
       */
      //}
      return 0;
    }

    return -1;

  }

}
