/// @file Model/SceneFileManager.h

#pragma once

#include "Controller/GlobalDefines.h"

class QDomElement;
class QString;

namespace Model {
  //Forward declarations -->
  class Color;
  class Point;
  class Scene;
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
      void loadScene (QIODevice & io, Scene &scene);

    private:
      /**Loads color from value to color object
       *
       * @param value xml value
       * @param color color object
       */
      static void getColor (const QDomElement &value, Color &color);

      /**Loads data from value to Point object
       * Point is class with common data for Vector and Point
       *
       * @param value xml value
       * @param object object to load data in
       */
      static void getVPCommon (const QDomElement &value, Point &object);

      static int getInt (const QDomElement &elem, const QString &name);
      static float getFloat (const QDomElement &elem, const QString &name);
      static double getDouble (const QDomElement &elem, const QString &name);
  };
} /* namespace Model */
