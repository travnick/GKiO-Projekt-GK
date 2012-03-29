/// @file Main/Model/SceneFileManager.h
/// @date 22-12-2011
/// @author Mikołaj Milej

#pragma once

#include "Controller/GlobalDefines.h"
#include <QtXml>

namespace Model {
  //Forward declarations -->
  class Color;
  class Scene;
  class VPCommon;
  // <-- Forward declarations

  /**Scene loader class
   *
   */
  class SceneFileManager {
    public:
      inline SceneFileManager () {
      }

      /**Loads scene from stream
       *
       * @param is stream to load data from
       * @param scene scene to load data in
       */
      void loadScene (QIODevice & io, Scene &scene) throw (std::exception);

    private:
      /**Loads color from value to color obejct
       *
       * @param value xml value
       * @param color color object
       */
      void getColor (QDomElement const &value, Color &color);

      /**Loads data from value to VPcommon object
       * VPcommon is class with commod data for Vector and Point
       *
       * @param value xml value
       * @param object object to load data in
       */
      void getVPCommon (QDomElement const &value, VPCommon &object);
  };
} /* namespace Model */
