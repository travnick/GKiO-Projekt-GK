/// @file Model/Scene.h

#pragma once

#include <Model/Object.h>
#include <string>
#include <vector>

#include "Controller/GlobalDefines.h"
#include "Model/ModelDefines.h"
#include "Model/Camera.h"
#include "Model/Light.h"
#include "Model/Material.h"
#include "Model/Sphere.h"

//Forward declarations -->
namespace std
{
  class exception;
}
// <-- Forward declarations

namespace Model
{

  /**3D scene class
   *
   */
  class Scene
  {
    public:
      typedef std::vector <Light> LighContainer;
      typedef std::vector <Material> MaterialContainer;
      typedef std::vector <VisibleObjectUniquePtr> ObjectContainer;

      /**Sets loaded = false
       *
       */
      Scene ();

      /**Initializes scene
       * If scene is loaded and reload == false, then nothing happens.
       * If you want reload scene set reload = true.
       * It throws std::exception when XML file has errors.
       *
       * @param filename file name to load scene from
       * @param reload if true forces reloading scene from file
       * @return if scene is loaded properly or not
       * @throws std::exception from XML parser
       */
      bool init (const QString &filename,
                 bool reload = false) throw (std::exception);

      /**Returns camera
       *
       * @return camera
       */
      inline const Camera &getCamera () const
      {
        return camera;
      }

      /**Returns camera
       *
       * @return camera
       */
      inline Camera &getCamera ()
      {
        return camera;
      }

      /**Returns objects in scene
       *
       * @return objects scene
       */
      inline const ObjectContainer &getObjects () const
      {
        return objects;
      }

      /**Returns lights in scene
       *
       * @return lights
       */
      inline const LighContainer &getLights () const
      {
        return lights;
      }

      /**Returns materials
       *
       * @return materials
       */
      inline const MaterialContainer &getMaterials () const
      {
        return materials;
      }

      /**Returns object count
       *
       * @return object count
       */
      inline int getObjectCount () const
      {
        return objects.size();
      }

      /**Returns world object
       *
       *@return world object
       */
      inline const Sphere &getWorldObject () const
      {
        return world;
      }

      /**Sets image width
       * It sets image width in camera, not in scene
       *
       * @param imageWidth image width
       */
      inline void setImageWidth (imageUnit imageWidth)
      {
        camera.setImageWidth(imageWidth);
      }

      /**Sets image height
       * It sets image height in camera, not in scene
       *
       * @param imageHeight image height
       */
      inline void setImageHeight (imageUnit imageHeight)
      {
        camera.setImageHeight(imageHeight);
      }

      /**Sets camera object in scene
       *
       * @param newCamera camera to set in scene
       */
      inline void setCamera (const Camera &newCamera)
      {
        this->camera = newCamera;
      }

      inline void addLight (const Light &object)
      {
        lights.emplace_back(object);
      }

      inline void addLight (Light &&object)
      {
        lights.emplace_back(std::move(object));
      }

      inline void addMaterial (Material &&object)
      {
        materials.emplace_back(std::move(object));
      }

      inline void addMaterial (const Material &object)
      {
        materials.emplace_back(object);
      }

      inline void addVisibleObject (std::unique_ptr <VisibleObject> &&object)
      {
        objects.emplace_back(std::move(object));
      }

      void updateCamera ();

      /**Returns state of scene.
       * Tells if scene is already loaded or not
       *
       * @return true if scene is already loaded, otherwise false
       */
      inline const bool &isLoaded ()
      {
        return loaded;
      }

    private:
      Sphere world;
      Camera camera;
      LighContainer lights;
      MaterialContainer materials;
      ObjectContainer objects;
      MaterialUniquePtr worldMaterial;
      bool loaded;
  };
}
