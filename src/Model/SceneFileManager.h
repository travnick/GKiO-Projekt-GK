/// @file Model/SceneFileManager.h

#pragma once

#include "Controller/GlobalDefines.h"

class QDomElement;
class QString;

namespace Model
{
  //Forward declarations -->
  class Color;
  class Point;
  class Scene;
  // <-- Forward declarations

  /**Scene loader class
   *
   */
  class SceneFileManager
  {
    public:
      inline SceneFileManager ()
      {
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

      /** Loads integer data from an xml element to the name variable
       *
       * @param elem an xml element
       * @param name a name of the xml property to load data from
       */
      static int getInt (const QDomElement &elem, const QString &name);

      /** Loads floating point data from an xml element to the name variable
       *
       * @param elem an xml element
       * @param name a name of the xml property to load data from
       */
      static float getFloat (const QDomElement &elem, const QString &name);

      /** Loads double precision floating point data from an xml element to the name variable
       *
       * @param elem an xml element
       * @param name a name of the xml property to load data from
       */
      static double getDouble (const QDomElement &elem, const QString &name);
  };
} /* namespace Model */
