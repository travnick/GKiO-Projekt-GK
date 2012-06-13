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
    : tmpDistance(new Vector), pointLightDist(new Vector), rayStartIntersect(
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
  Scene::ObjectIt endObjects = renderParams->scene->getObjects().end();
  Scene::ObjectIt currentObject = endObjects;
  float coef = 1;
  float tmpLightCoef = 0;
  worldUnit rayStartIntersectDist = mainViewDistance;
  int reflecionDeep = renderParams->reflectionDeep;
  bool inShadow;

  while (reflecionDeep-- > 0)
  {
    rayStartIntersectDist = mainViewDistance;
    //Find intersection
    for (Scene::ObjectIt object = renderParams->scene->getObjects().begin();
        object < endObjects; object++)
    {
      if ( ( *object)->checkRay(ray, rayStartIntersectDist, *tmpDistance))
      {
        currentObject = object;
      }
    }

    //If there is any intersection?
    if (currentObject != endObjects)
    {
      QScopedPointer <Vector> normalAtIntersection(new Vector);
      QScopedPointer <Point> intersection(new Point);

      ray.getDir().data.multiply(rayStartIntersectDist,
                                 rayStartIntersect->data);

      //Calculate intersection point
      ray.getStart().data.move(rayStartIntersect->data, intersection->data);

      //Get normal vector at intersection point
      ( *currentObject)->getNormal( *intersection, *normalAtIntersection);

      //move intersection point by epsilon, needed for error correction
      Vector correction(normalAtIntersection->data * FLOAT_EPSILON);
      intersection->data += correction;

      Material *currentMaterial = ( *currentObject)->getMaterial().data();

      //(1.0f / COLOR_COUNT) because few lines bellow we do color * color
      tmpLightCoef = coef * (1.0f / COLOR_COUNT);

      if (renderParams->reflectionDeep > 1)
      {
        tmpLightCoef *= (1.0f - currentMaterial->getReflection());
      }

      Vector trololo(ray.getDir());
      Model::SSEVector trololo2 = normalAtIntersection->data;
      //Calculate reflection vector
      trololo2 *= trololo.dotProduct(trololo2) * 2;
      trololo.data -= trololo2;
      //trololo.normalize();
      //trololo is still normalized;

      float transparency = ( *currentObject)->getMaterial()->getTransparency();

      //calculate refracted ray and transparent sphere color
      Color transpColor = shootRefractedRay(ray, transparency, refractionDepth,
                                            mainViewDistance,
                                            rayStartIntersectDist, tmpLightCoef,
                                            correction, *normalAtIntersection,
                                            *intersection, * *currentObject);

      resultColor += transpColor;

      //Calculate light contribution
      Scene::LighIt endLights = renderParams->scene->getLights().end();

      for (Scene::LighIt light = renderParams->scene->getLights().begin();
          light < endLights; light++)
      {
        ( *light)->getPosition().data.diff(intersection->data,
                                           pointLightDist->data);
        //if angle between light and normal vector at intersection is higher than 90 degrees
        if (normalAtIntersection->dotProduct(pointLightDist->data) <= 0.0f)
        {
          continue;
        }

        pointLightDist->normalize();

        if (pointLightDist->length < FLOAT_EPSILON)
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
                                      *tmpDistance))
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

          float lightPowerSpecular = 1;

          if (lightPower < 1.0)
          {
            lightPower = 1.0;
          }

          if (lightPowerSpecular < 1.0)
          {
            lightPowerSpecular = 1.0;
          }

          float specular = pow(trololo.dotProduct(lightRay->getDir()),
                               currentMaterial->getSpecularPower());
          lightPower = ( *light)->power / lightPower;
//          lightPowerSpecular = ( *light)->power / lightPowerSpecular;
          //<--light attenuation

          lightPower *= lambert * tmpLightCoef;
          lightPowerSpecular *= specular * tmpLightCoef;

          //Add diffuse component
          resultColor += ( * *light) * currentMaterial->getColor() * lightPower;

          //TODO: do something like trololo.dotProduct(lightRay->getDir()) < max angle then contribute
          // else continue
          if (normalAtIntersection->dotProduct(trololo) <= 0.25f)
          {
            continue;
          }
          else
          {
            resultColor += ( * *light) * currentMaterial->getSpecularColor()
                * lightPowerSpecular;
          }
        }
      }

      //multiply by current reflection contribution
      coef *= currentMaterial->getReflection();

      if (coef < COLOR_MIN_VALUE)
      {
        break;
      }

      //Calculate reflection vector
//      normalAtIntersection->data *= ray.getDir().dotProduct(
//          normalAtIntersection->data) * 2;
//      ray.getDir().data -= normalAtIntersection->data;
      ray.getDir().data = trololo.data;
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

inline int Renderer::calculateRefraction (Ray &ray,
                                          VisibleObject &currentObject,
                                          Vector *normalAtIntersection) const
{

  float ior = currentObject.getMaterial()->getIOR();

  //TODO: not sure if it is proper comment - needs verification
  //get cosinus between normal and ray going into the sphere
  float cos_alpha = -ray.getDir().dotProduct( *normalAtIntersection);

  //it is ior_in/ior_out
  float ir;
  float a = 1;

  //we are going into the sphere
  if (cos_alpha >= 0.0f)
  {
    ir = 1.0f / ior;
  }
  else
  //going outside of the sphere
  {
    ir = ior;
    a = -a;
    cos_alpha = -cos_alpha;
  }

  float sin_betha2 = ir * ir * (1.0f - cos_alpha * cos_alpha);

  //checking if there is no total internal reflection
  if (sin_betha2 < 1.0f)
  {
    ray.getDir().data = ray.getDir().data * ir
        - normalAtIntersection->data
            * (a * (ir * cos_alpha + sqrt(1.0f - sin_betha2)));

    ray.getDir().normalize();

    return 0;
  }
  return -1;
}

inline Color Renderer::shootRefractedRay (const Ray &ray,
                                          float transparency,
                                          int refractionDepth,
                                          worldUnit mainViewDistance,
                                          worldUnit viewDistance,
                                          worldUnit &tmpLightCoef,
                                          Vector &correction,
                                          Vector &normalAtIntersection,
                                          Point &intersection,
                                          VisibleObject &currentObject) const
{
  //default color is black
  Color transpColor(0, 0, 0);

  //checking if there is any transparency or if we still need to calculate transparency
  if ( (transparency > 0.01f) && (refractionDepth > 0))
  {
    tmpLightCoef *= (1.0f - transparency);

    Ray newRay(ray);

    int refrResult = calculateRefraction(newRay, currentObject,
                                         &normalAtIntersection);

    //there was refraction
    if (refrResult == 0)
    {
      newRay.setParams(intersection);

      //adding some corrections to ray start point
      newRay.getStart().data -= correction.data;
      newRay.getStart().data += newRay.getDir().data * FLOAT_EPSILON;

      shootRay(newRay, transpColor, mainViewDistance, refractionDepth - 1);
      transpColor *= transparency;
    }
  }

  return transpColor;
}
