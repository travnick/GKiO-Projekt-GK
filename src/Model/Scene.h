/// @file Main/Model/Scene.h
/// @date 01-12-2011
/// @author Mikołaj Milej

#pragma once

#include <string>
#include <vector>

#include <QSharedPointer>

#include "Controller/GlobalDefines.h"
#include "Model/ModelDefines.h"
#include "Model/Camera.h"

//Forward declarations -->
namespace std {
  class exception;
}
// <-- Forward declarations

namespace Model {

  //Forward declarations -->
  class Light;
  class Material;
  class VisibleObject;
  // <-- Forward declarations

  /**3D scene class
   *
   */
  class Scene {
    public:
      typedef QSharedPointer <Camera> CameraType;
      typedef QSharedPointer <Light> LightType;
      typedef QSharedPointer <Material> MaterialType;
      typedef QSharedPointer <VisibleObject> ObjectType;

      typedef std::vector <LightType> LighContainer;
      typedef LighContainer::const_iterator LighContainerIterator;

      typedef std::vector <MaterialType> MaterialContainer;
      typedef MaterialContainer::const_iterator MaterialContainerIterator;

      typedef std::vector <ObjectType> ObjectContainer;
      typedef ObjectContainer::const_iterator ObjectContainerIterator;

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
       * @throws std::exception from rapidxml
       */
      bool init (const QString &filename, bool reload = false) throw (std::exception);

      /**Returns camera
       *
       * @return camera
       */
      inline const CameraType &getCamera () const{
        return camera;
      }

      /**Returns objects in scene
       *
       * @return objects scene
       */
      inline const ObjectContainer &getObjects () const{
        return objects;
      }

      /**Returns lights in scene
       *
       * @return lights
       */
      inline const LighContainer &getLights () const{
        return lights;
      }

      /**Returns materials
       *
       * @return materials
       */
      inline const MaterialContainer &getMaterials () const{
        return materials;
      }

      /**Returns object count
       *
       * @return object count
       */
      inline int getObjectCount () const{
        return objects.size();
      }

      /**Sets image width
       * It sets image width in camera, not in scene
       *
       * @param imageWidth image width
       */
      inline void setImageWidth (imageUnit imageWidth){
        camera->setImageWidth(imageWidth);
      }

      /**Sets image height
       * It sets image height in camera, not in scene
       *
       * @param imageHeight image height
       */
      inline void setImageHeight (imageUnit imageHeight){
        camera->setImageHeight(imageHeight);
      }

      /**Sets camera object in scene
       *
       * @param camera camera to set in scene
       */
      inline void setCamera (const CameraType &newCamera){
        this->camera = newCamera;
      }

      /**Adds light to scene
       *
       * @param object light to add
       */
      inline void addLight (const LightType &object){
        lights.push_back(object);
      }

      /**Adds material to scene
       *
       * @param object material to add
       */
      inline void addMaterial (const MaterialType &object){
        materials.push_back(object);
      }

      /**Adds visible object to scene
       *
       * @param object visible object to add
       */
      inline void addVisibleObject (const ObjectType &object){
        objects.push_back(object);

        calculateObjectSizeOnImage(object);
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
      inline const bool &isLoaded (){
        return loaded;
      }

    private:
      bool loaded;
      CameraType camera;
      LighContainer lights;
      MaterialContainer materials;
      ObjectContainer objects;

      void calculateObjectSizeOnImage (const ObjectType &object);
  };
}

