/// @file Model/Scene.h

#pragma once

#include <string>
#include <vector>

#include <QSharedPointer>

#include "Controller/GlobalDefines.h"
#include "Model/Camera.h"
#include "Model/ModelDefines.h"

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
      typedef std::vector <LightPtr> LighContainer;
      typedef LighContainer::const_iterator LighIt;

      typedef std::vector <MaterialPtr> MaterialContainer;
      typedef MaterialContainer::const_iterator MaterialIt;

      typedef std::vector <ObjectPtr> ObjectContainer;
      typedef ObjectContainer::const_iterator ObjectIt;

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
      inline const CameraPtr &getCamera () const
      {
        return camera;
      }

      /**Returns camera
       *
       * @return camera
       */
      inline CameraPtr &getCamera ()
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

      /**Sets image width
       * It sets image width in camera, not in scene
       *
       * @param imageWidth image width
       */
      inline void setImageWidth (imageUnit imageWidth)
      {
        camera->setImageWidth(imageWidth);
      }

      /**Sets image height
       * It sets image height in camera, not in scene
       *
       * @param imageHeight image height
       */
      inline void setImageHeight (imageUnit imageHeight)
      {
        camera->setImageHeight(imageHeight);
      }

      /**Sets camera object in scene
       *
       * @param newCamera camera to set in scene
       */
      inline void setCamera (const CameraPtr &newCamera)
      {
        this->camera = newCamera;
      }

      /**Adds light to scene
       *
       * @param object light to add
       */
      inline void addLight (const LightPtr &object)
      {
        lights.push_back(object);
      }

      /**Adds material to scene
       *
       * @param object material to add
       */
      inline void addMaterial (const MaterialPtr &object)
      {
        materials.push_back(object);
      }

      /**Adds visible object to scene
       *
       * @param object visible object to add
       */
      inline void addVisibleObject (const ObjectPtr &object)
      {
        objects.push_back(object);
      }

      /**Updates camera
       * It calls camera->calibrate()
       *
       */
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
      CameraPtr camera;
      LighContainer lights;
      MaterialContainer materials;
      ObjectContainer objects;
      bool loaded;
  };
}
