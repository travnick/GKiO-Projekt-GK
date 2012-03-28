/// @file Main/Model/SceneFileManager.cpp
/// @date 22-12-2011
/// @author Mikołaj Milej

#include <boost/foreach.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include "Model/ModelDefines.h"
#include "Model/SceneFileManager.h"
#include "Model/Scene.h"
#include "Model/Camera.h"
#include "Model/Light.h"
#include "Model/Object.h"
#include "Model/Sphere.h"
#include "Model/Material.h"
#include "Model/Vector.h"

namespace Model {

  void SceneFileManager::loadScene (std::istream & is, Scene &scene) throw (std::exception){
    Objects::ObjectType objectType = Objects::None;
    using boost::property_tree::ptree;
    ptree pt;
    read_xml(is, pt, boost::property_tree::xml_parser::no_comments);

    BOOST_FOREACH( ptree::value_type const &value, pt.get_child(Materials::MATERIALS_NAME))
    {
      QSharedPointer <Material> material(new Material);
      Color color;
      worldUnit reflection;

      std::string childName = Materials::MATERIALS_NAME + "." + value.first;
      ptree child;

      ptree::value_type const &colorNode =
          find(pt, child, childName.c_str(), "diffuseColor").dereference();
      getColor(colorNode, color);

      reflection = value.second.get <worldUnit>("reflection");
      if (reflection < 0.0f || reflection > 1.0f)
      {
#ifdef Q_OS_LINUX
        throw std::exception();
#else
        throw std::exception("Wartoć odbicia jest spoza dozwolonego zakresu.");
#endif
      }

      ( *material->getColor()) = (color);
      material->setReflection(reflection);

      scene.addMaterial(material);
    }

    BOOST_FOREACH(
        ptree::value_type const &value, pt.get_child(Lights::LIGHTS_NAME))
    {
      QSharedPointer <Light> object(new Light());
      Point point;
      Color color;

      std::string childName = Lights::LIGHTS_NAME + "." + value.first;
      ptree child;

      ptree::value_type const &colorNode =
          find(pt, child, childName.c_str(), "color").dereference();

      getVPCommon(value, point);
      getColor(colorNode, color);

      object->setPosition(point);
      ( *object) = (color);

      scene.addLight(object);
    }

    if (scene.getMaterials().empty())
    {
#ifdef Q_OS_LINUX
      throw std::exception();
#else
      throw std::exception("Brak zdefiniowanych materiałów");
#endif
    }

    BOOST_FOREACH( ptree::value_type const &value, pt.get_child(Objects::OBJECTS_NAME))
    {
      objectType = Object::getObjectType(value.first);

      switch (objectType) {
        case Objects::Sphere:
        {
          QSharedPointer <Sphere> mainbject;
          Point point;
          worldUnit radius;
          int material;
          int multiplyX;
          int multiplyY;
          int multiplyZ;
          int multiplyXSign = 1;
          int multiplyYSign = 1;
          int multiplyZSign = 1;
          worldUnit offset;

          getVPCommon(value, point);
          radius = value.second.get <worldUnit>("r");
          if ( !(radius > 0))
          {
#ifdef Q_OS_LINUX
            throw std::exception();
#else
            throw std::exception("Promień musi być dodatni.");
#endif
          }
          material = value.second.get <int>("material");
          if (material >= scene.getMaterials().size() || material < 0)
          {
#ifdef Q_OS_LINUX
            throw std::exception();
#else
            throw std::exception("Nie ma takiego materiału. Sprawdź obiekty.");
#endif
          }

          multiplyX = value.second.get <int>("multiplyX", 1);
          multiplyY = value.second.get <int>("multiplyY", 1);
          multiplyZ = value.second.get <int>("multiplyZ", 1);
          offset = value.second.get <worldUnit>("offset", radius * 2);

          mainbject = QSharedPointer <Sphere>(new Sphere(radius));

          mainbject->setPosition(point);
          mainbject->setMaterial(material);

          if (multiplyX < 0)
          {
            multiplyXSign = -1;
            multiplyX = -multiplyX;
          }

          if (multiplyY < 0)
          {
            multiplyYSign = -1;
            multiplyY = -multiplyY;
          }

          if (multiplyZ < 0)
          {
            multiplyZSign = -1;
            multiplyZ = -multiplyZ;
          }

          for (int i = 0; i < multiplyX; ++i)
          {
            for (int j = 0; j < multiplyY; ++j)
            {
              for (int k = 0; k < multiplyZ; ++k)
              {
                QSharedPointer <VisibleObject> object(new Sphere( *mainbject));
                point = object->getPosition();

                point.coords [PX] += offset * i * multiplyXSign;
                point.coords [PY] += offset * j * multiplyYSign;
                point.coords [PZ] += offset * k * multiplyZSign;

                object->setPosition(point);

                scene.addVisibleObject(object);
              }
            }
          }
        }
          break;
        case Objects::Camera:
        {
          if ( !(scene.getCamera().isNull()))
          {
#ifdef Q_OS_LINUX
            throw std::exception();
#else
            throw std::exception("Może być zdefiniowana tylko jedna kamera.");
#endif
          }

          QSharedPointer <Camera> object(new Camera());
          Point point;
          Vector vector;
          Point::dataType screenWidth;
          worldUnit viewDistance;
          Camera::unitType FOV;
          std::string cameraType;

          /**
           * Camera rotating not yet implemented
           */
          vector.set(0, 0, 3);
          //              ptree::value_type const &dir =
          //                  pt.get_child(Objects::OBJECTS_NAME + "." + value.first).find("dir").dereference();
          //              getVPCommon(dir, vector);

          getVPCommon(value, point);

          screenWidth = value.second.get <worldUnit>("screenWidth");
          if ( !(screenWidth > 0))
          {
#ifdef Q_OS_LINUX
            throw std::exception();
#else
            throw std::exception("Parametr screenWidth musi być większy od 0.");
#endif
          }
          FOV = value.second.get <Camera::unitType>("FOV");
          if ( !(FOV > 0 && FOV < 180))
          {
#ifdef Q_OS_LINUX
            throw std::exception();
#else
            throw std::exception("Parametr FOV musi być z zakresu (0;180)");
#endif
          }
          viewDistance = value.second.get <worldUnit>("viewDistance");
          if ( !(viewDistance > 0))
          {
#ifdef Q_OS_LINUX
            throw std::exception();
#else
            throw std::exception("Parametr viewDistance musi być większy od 0.");
#endif
          }
          cameraType = value.second.get <std::string>("type");

          object->setFOV(FOV);
          object->setViewDistance(viewDistance);
          object->setScreenWidth(screenWidth);
          object->setPosition(point);
          object->setDirection(vector);
          object->setType(cameraType);

          scene.setCamera(object);
        }
          break;
        default:
          break;
      }
    }

    if (scene.getCamera().isNull())
    {
#ifdef Q_OS_LINUX
      throw std::exception();
#else
      throw std::exception("Brak kamery w scenie");
#endif
    }

    if (scene.getLights().empty())
    {
#ifdef Q_OS_LINUX
      throw std::exception();
#else
      throw std::exception("Brak świateł w scenie");
#endif
    }
  }

  void SceneFileManager::getColor (const boost::property_tree::ptree::value_type & value,
                                   Color &color){
    color.red = (value.second.get <Color::colorValueDataType>("r"));
    color.green = (value.second.get <Color::colorValueDataType>("g"));
    color.blue = (value.second.get <Color::colorValueDataType>("b"));
  }

  void SceneFileManager::getVPCommon (const boost::property_tree::ptree::value_type & value,
                                      VPCommon &object){
    object.coords [PX] = value.second.get <VPCommon::dataType>("x");
    object.coords [PY] = value.second.get <VPCommon::dataType>("y");
    object.coords [PZ] = value.second.get <VPCommon::dataType>("z");
  }

  boost::property_tree::ptree::assoc_iterator SceneFileManager::find (const boost::property_tree::ptree &root,
                                                                      boost::property_tree::ptree &child,
                                                                      const char *childName,
                                                                      const char *fieldName){
    using boost::property_tree::ptree;

    child = root.get_child(childName);
    ptree::assoc_iterator node = child.find(fieldName);
    if (node == child.not_found())
    {
      std::string message = "Brakuje sekcji ";
      message.append(fieldName).append(" w:<br>").append(childName);
#ifdef Q_OS_LINUX
      throw std::exception();
#else
      throw std::exception(message.c_str());
#endif
    }
    return node;
  }
} /* namespace Model */
