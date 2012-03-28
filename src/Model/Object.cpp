/// @file Main/Model/Object.cpp
/// @date 22-12-2011
/// @author Mikołaj Milej

#include <map>

#include "Model/ModelDefines.h"
#include "Model/Object.h"

namespace Model {

  typedef std::map <std::string, Objects::ObjectType> objectTypesMap;
  typedef std::map <Objects::ObjectType, std::string> objectTypeNamesMap;

  Objects::ObjectType Object::getObjectType (const std::string & typeName){
    static objectTypesMap objectTypes;
    objectTypesMap::iterator iterator;
    Objects::ObjectType objectType = Objects::None;

    if (objectTypes.empty())
    {
      objectTypes [Objects::CAMERA_NAME] = Objects::Camera;
      objectTypes [Objects::LIGHT_NAME] = Objects::Light;
      objectTypes [Objects::SPHERE_NAME] = Objects::Sphere;
    }

    iterator = objectTypes.find(typeName);

    if (iterator != objectTypes.end())
    {
      objectType = iterator->second;
    }

    return objectType;
  }

  const std::string & Object::getObjectTypeName (const Objects::ObjectType objectType){
    static objectTypeNamesMap objectTypeNames;
    objectTypeNamesMap::iterator iterator;
    static std::string objectTypeName;
    objectTypeName = "";

    if (objectTypeNames.empty())
    {
      objectTypeNames [Objects::Camera] = Objects::CAMERA_NAME;
      objectTypeNames [Objects::Light] = Objects::LIGHT_NAME;
      objectTypeNames [Objects::Sphere] = Objects::SPHERE_NAME;
    }

    iterator = objectTypeNames.find(objectType);

    if (iterator != objectTypeNames.end())
    {
      objectTypeName = iterator->second;
    }

    return objectTypeName;
  }

} /* namespace Model */
