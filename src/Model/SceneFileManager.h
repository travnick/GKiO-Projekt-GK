/// @file Main/Model/SceneFileManager.h
/// @date 22-12-2011
/// @author Mikołaj Milej

#pragma once

#include <boost/property_tree/ptree.hpp>

#include "Controller/GlobalDefines.h"

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
      inline SceneFileManager (){
      }

      /**Loads scene from stream
       *
       * @param is stream to load data from
       * @param scene scene to load data in
       */
      void loadScene (std::istream &is, Scene &scene) throw (std::exception);

    private:
      /**Loads color from value to color obejct
       *
       * @param value xml value
       * @param color color object
       */
      void getColor (boost::property_tree::ptree::value_type const &value, Color &color);

      /**Loads data from value to VPcommon object
       * VPcommon is class with commod data for Vector and Point
       *
       * @param value xml value
       * @param object object to load data in
       */
      void getVPCommon (boost::property_tree::ptree::value_type const &value, VPCommon &object);

      boost::property_tree::ptree::assoc_iterator find (const boost::property_tree::ptree &root,
                                                        boost::property_tree::ptree &child,
                                                        const char *childName,
                                                        const char *fieldName);
  };
} /* namespace Model */
