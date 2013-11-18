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

const float E = 2.7182818284590452354L;
//const float PI = M_PI;

using namespace Model;

//Because qRound(0.49f * COLOR_MAX_VALUE / COLOR_MAX_VALUE) < 1
const float COLOR_MIN_VALUE = 0.5f / COLOR_MAX_VALUE;

Renderer::Renderer (const Controller::RenderParams &newRenderParams)
    : tmpDistance(new Vector), pointLightDist(new Vector), rayStartIntersect(
        new Vector), lightRay(new Ray)
{
  setRenderParams(&newRenderParams);
}

Renderer::~Renderer ()
{
}

void Renderer::render (const RenderTileData &tile)
{
  const Camera &camera = renderParams->scene->getCamera();
  worldUnit viewDistance = camera.getViewDistance();
  Vector direction;
  Point startOnScreen(camera.getScreenTopLeft());
  Point currentOnScreen;
  Ray ray;
  Color rayResult;
  const VisibleObject *objectWeAreIn = &renderParams->scene->getWorldObject();

  const imageUnit diffToNewLine = BPP * (tile.imageWidth - tile.width);
  imageUnit R = BPP * (tile.topLeft.x + tile.topLeft.y * tile.imageWidth);
  imageUnit G = R + 1;
  imageUnit B = G + 1;

  startOnScreen.data += camera.screenWidthDelta.data * tile.topLeft.x;
  startOnScreen.data += camera.screenHeightDelta.data * tile.topLeft.y;

  for (imageUnit iLine = tile.topLeft.y; iLine < tile.bottomRight.y; ++iLine)
  {
    currentOnScreen.data = startOnScreen.data;

    for (imageUnit iCol = tile.topLeft.x;
        renderParams->allowRunning && iCol < tile.bottomRight.x; ++iCol)
    //Checking if thread is allowed to run: renderParams->allowRunning
    {
      if (camera.getType() == Camera::Conic)
      {
        camera.getDirection(currentOnScreen, direction);
      }

      ray.setParams(currentOnScreen, direction);

      int refractionDepth = renderParams->refractionDeep;

      rayResult.setDefaultColor();
      shootRay(ray, rayResult, viewDistance, refractionDepth, objectWeAreIn);

      tile.imageData [R] = rayResult.red();
      R += BPP;

      tile.imageData [G] = rayResult.green();
      G += BPP;

      tile.imageData [B] = rayResult.blue();
      B += BPP;

      currentOnScreen.data += camera.screenWidthDelta.data;
    }

    startOnScreen.data += camera.screenHeightDelta.data;

    R += diffToNewLine;
    G += diffToNewLine;
    B += diffToNewLine;
  }
}

inline void Renderer::shootRay (Ray & ray,
                                Color &resultColor,
                                worldUnit mainViewDistance,
                                int refractionDepth,
                                const VisibleObject *objectWeAreIn) const
{
  Scene::ObjectIt endObjects = renderParams->scene->getObjects().end();
  ObjectPtr currentObject;
  float reflectionCoef = 1;
  float lightContrCoef = 0;
  worldUnit rayStartIntersectDist = mainViewDistance;
  int reflecionDeep = renderParams->reflectionDeep;
  bool inShadow;

  while (reflecionDeep-- >= 0)
  {
    rayStartIntersectDist = mainViewDistance;
    //Find intersection
    for (const ObjectPtr &object : renderParams->scene->getObjects())
    {
      if (object->checkRay(ray, rayStartIntersectDist, *tmpDistance))
      {
        currentObject = object;
      }
    }

    //If there is any intersection?
    if (currentObject != nullptr)
    {
      QScopedPointer <Vector> normalAtIntersection(new Vector);
      QScopedPointer <Point> intersection(new Point);

      ray.getDir().data.multiply(rayStartIntersectDist,
                                 rayStartIntersect->data);

      //Calculate intersection point
      ray.getStart().data.move(rayStartIntersect->data, intersection->data);

      //Get normal vector at intersection point
      currentObject->getNormal(*intersection, *normalAtIntersection);

      //move intersection point by epsilon, needed for error correction
      Vector correction(normalAtIntersection->data * FLOAT_EPSILON);
      intersection->data += correction;

      const Material &currentMaterial = currentObject->getMaterial();

      //(1.0f / COLOR_COUNT) because few lines bellow we do color * color
      lightContrCoef = reflectionCoef * (1.0f / COLOR_COUNT);

      if (renderParams->reflectionDeep > 1)
      {
        lightContrCoef *= (1.0f - currentMaterial.getReflection());
      }

      float transparency = currentObject->getMaterial().getTransparency();

      //calculate refracted ray and transparent sphere color
      Color transpColor = shootRefractedRay(ray, transparency, refractionDepth,
                                            mainViewDistance,
                                            rayStartIntersectDist,
                                            lightContrCoef, correction,
                                            *normalAtIntersection,
                                            *intersection, *currentObject,
                                            objectWeAreIn);

      resultColor += transpColor;

      Color textureColor = currentMaterial.getTextureColor(
          *normalAtIntersection.data());

      //Calculate reflection vector
      Vector reflectedRay(ray.getDir());
      Model::SSEVector normalCopy = normalAtIntersection->data;
      normalCopy *= reflectedRay.dotProduct(normalCopy) * 2;
      reflectedRay.data -= normalCopy;
      //reflectedRay is still normalized;

      //Calculate light contribution
      for (const Light &light : renderParams->scene->getLights())
      {
        light.getPosition().data.diff(intersection->data, pointLightDist->data);
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

        lightRay->setParams(*intersection, *pointLightDist);
        // Computation of the shadows
        inShadow = false;

        //TODO: Can we check this once before starting rendering?
        if (renderParams->shadows)
        {
          for (Scene::ObjectIt object =
              renderParams->scene->getObjects().begin(); object < endObjects;
              object++)
          {
            if ( (*object)->checkRay(*lightRay, pointLightDist->length,
                                     *tmpDistance))
            {
              inShadow = true;
              break;
            }
          }
        }

        if (!inShadow)
        {
          // Lambert lighting model
          float lambert = lightRay->getDir().dotProduct(
              normalAtIntersection->data);

          //light attenuation
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

          float rayLigthRayAngleCos = reflectedRay.dotProduct(
              lightRay->getDir());

          float specular = pow(rayLigthRayAngleCos,
                               currentMaterial.getSpecularPower());
          lightPower = light.power / lightPower;

          lightPower *= lambert * lightContrCoef;
          lightPowerSpecular *= specular * lightContrCoef;

          //Add diffuse component
          resultColor += light * currentMaterial.getColor()
              * (textureColor * (1.0 / COLOR_MAX_VALUE)) * lightPower;

          if (rayLigthRayAngleCos > 0.0f)
          {
            resultColor += light * currentMaterial.getSpecularColor()
                * lightPowerSpecular;
          }
        }
      }

      //multiply by current reflection contribution
      reflectionCoef *= currentMaterial.getReflection();

      if ( (reflectionCoef < COLOR_MIN_VALUE)
          || (objectWeAreIn == currentObject.get()))
      {
        break;
      }

      ray.getDir().data = reflectedRay.data;
      ray.setParams(*intersection);
      //ray is still normalized;

      currentObject = nullptr;
    }
    else
    {
      return;
    }
  }
}

inline int Renderer::calculateRefraction (Ray &ray,
                                          VisibleObject &currentObject,
                                          const VisibleObject *objectWeAreIn,
                                          Vector *normalAtIntersection) const
{

  float ior = currentObject.getMaterial().getIOR();

  //get cosine between normal and ray going into the sphere
  float cos_alpha = -ray.getDir().dotProduct(*normalAtIntersection);

  //it is ior_in/ior_out
  float ir;
  float a = 1;

  //we are going into the sphere
  if (cos_alpha >= 0.0f)
  {
    ir = objectWeAreIn->getMaterial().getIOR() / ior;
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
                                          worldUnit rayStartIntersectDist,
                                          float &lightContrCoef,
                                          Vector &correction,
                                          Vector &normalAtIntersection,
                                          Point &intersection,
                                          VisibleObject &currentObject,
                                          const VisibleObject *objectWeAreIn) const
{
  //default color is black
  Color transpColor(0, 0, 0);

  //checking if there is any transparency or if we still need to calculate transparency
  if ( (transparency > 0.01f) && (refractionDepth > 0))
  {
    lightContrCoef *= (1.0f - transparency);

    Ray newRay(ray);

    int refrResult = calculateRefraction(newRay, currentObject, objectWeAreIn,
                                         &normalAtIntersection);

    //there was refraction
    if (refrResult == 0)
    {
      newRay.setParams(intersection);

      //adding some corrections to ray start point
      newRay.getStart().data -= correction.data;
      newRay.getStart().data += newRay.getDir().data * FLOAT_EPSILON;

      //changing object to the sphere we are going into
      objectWeAreIn = &currentObject;

      shootRay(newRay, transpColor, mainViewDistance, refractionDepth - 1,
               objectWeAreIn);

      transpColor *= transparency;
    }
  }

  return transpColor;
}
