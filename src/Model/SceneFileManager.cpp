/// @file Main/Model/SceneFileManager.cpp
/// @date 29-03-2012
/// @author Piotr Jawniak

#include <QIODevice>
#include <QtXml>
#include <stdexcept>

#include "Model/ModelDefines.h"
#include "Model/SceneFileManager.h"
#include "Model/Scene.h"
#include "Model/Camera.h"
#include "Model/Light.h"
#include "Model/Object.h"
#include "Model/Sphere.h"
#include "Model/Plane.h"
#include "Model/Material.h"
#include "Model/Vector.h"

namespace Model {

  void SceneFileManager::loadScene (QIODevice & io, Scene &scene)
  {
    Objects::ObjectType objectType = Objects::None;
    QString tmpname;
    QDomNode node;
    QDomElement root;
    QDomDocument d;

    d.setContent(&io);
    root = d.documentElement();

    // Wczytywanie materiałów
    tmpname = QString::fromStdString(Materials::MATERIALS_NAME);	// potrzebny QString, a jest std::string
    node = root.elementsByTagName(tmpname).item(0).firstChild();

	for ( ; !node.isNull(); node = node.nextSibling())
	{
		if(!node.isElement())
			continue;

		QSharedPointer <Material> material(new Material());
		Color color;
		worldUnit reflection;
		QDomElement elem = node.toElement();

		reflection = static_cast<worldUnit>(getFloat(elem, "reflection"));

		if(reflection < 0.0f || reflection > 1.0f)
			throw std::logic_error("Wartość odbicia jest spoza zakresu.");

		getColor(node.firstChildElement("diffuseColor").toElement(), color);

		( *material->getColor()) = color;
		material->setReflection(reflection);

		scene.addMaterial(material);
	}

	if (scene.getMaterials().empty())
		throw std::logic_error("Brak zdefiniowanych materiałów");

	// Wczytywanie świateł
	tmpname = QString::fromStdString(Lights::LIGHTS_NAME);
	node = root.elementsByTagName(tmpname).item(0).firstChild();

	for ( ; !node.isNull(); node = node.nextSibling())
	{
		if(!node.isElement())
			continue;

		QSharedPointer <Light> object(new Light());
		Point point;
		Color color;
		QDomElement elem = node.toElement();

		getColor(node.firstChildElement("color").toElement(), color);
		getVPCommon(node.firstChildElement("position").toElement(), point);

		object->setPosition(point);
		( *object) = (color);

		scene.addLight(object);
	}

	// Wczytywanie obiektów
	tmpname = QString::fromStdString(Objects::OBJECTS_NAME);
	node = root.elementsByTagName(tmpname).item(0).firstChild();

	for ( ; !node.isNull(); node = node.nextSibling())
	{
		if(!node.isElement())
			continue;

	  QDomElement elem = node.toElement();
      objectType = Object::getObjectType(elem.tagName().toStdString());

	  switch (objectType) {
      case Objects::Sphere:
      {
        QSharedPointer <Sphere> mainbject;
        Point point;
        worldUnit radius;
        worldUnit offset;
        int material;
        int multiplyX = 1, multiplyY = 1, multiplyZ = 1;
        int multiplyXSign = 1, multiplyYSign = 1, multiplyZSign = 1;

        getVPCommon(node.firstChildElement("position").toElement(), point);
        radius = getFloat(elem, "r");

        if(!(radius > 0)) {
          throw std::logic_error("Promień musi być dodatni.");
        }

        offset = radius * 2;
        if(elem.hasAttribute("offset")) {
        	offset = static_cast<worldUnit>(getFloat(elem, "offset"));
        }

        material = getInt(elem, "material");

        if(material >= scene.getMaterials().size() || material < 0)
          throw std::logic_error("Nie ma takiego materiału. Sprawdź obiekty.");

        QDomNode mul = node.firstChildElement("multiply");
        if(mul.isElement()) {
          QDomElement mulelem = mul.toElement();
          multiplyX = getInt(mulelem, "x");
          multiplyY = getInt(mulelem, "y");
          multiplyZ = getInt(mulelem, "z");
        }

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
 
		case Objects::Plane:
		{
			QSharedPointer<Plane> object(new Plane());
			Vector angles;
			int material;

			// TODO tymczasowo dopisywanie rzez coords
			angles.coords[PX] = getFloat(elem, "angleX");
			angles.coords[PY] = getFloat(elem, "angleY");
			angles.coords[PZ] = getFloat(elem, "angleZ");
			angles.coords[PW] = getFloat(elem, "d");
			material = getInt(elem, "material");

			object->setAngles(angles);
			object->setMaterial(material);
			scene.addVisibleObject(object);
		}
		break;

      case Objects::Camera:
      {
        if ( !(scene.getCamera().isNull()))
        {
          throw std::logic_error("Może być zdefiniowana tylko jedna kamera.");
        }

        QSharedPointer<Camera> object(new Camera());
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
        getVPCommon(node.firstChildElement("position").toElement(), point);

        screenWidth = static_cast<Point::dataType>(getFloat(elem, "screenWidth"));
        if (!(screenWidth > 0))
          throw std::logic_error("Parametr screenWidth musi być większy od 0.");

        FOV = static_cast<Camera::unitType>(getDouble(elem, "fov"));
        if (!(FOV > 0 && FOV < 180))
          throw std::logic_error("Parametr FOV musi być z zakresu (0;180)");

        viewDistance = static_cast<worldUnit>(getFloat(elem, "viewDistance"));
        if (!(viewDistance > 0))
         throw std::logic_error("Parametr viewDistance musi być większy od 0.");

        cameraType = elem.attribute("type").toStdString();
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
	  throw std::logic_error("Brak kamery w scenie");

	if (scene.getLights().empty())
	  throw std::logic_error("Brak świateł w scenie");
  }

  void SceneFileManager::getColor (const QDomElement & value, Color &color){
    color.red = getInt(value, "r");
    color.green = getInt(value, "g");
    color.blue = getInt(value, "b");
  }

  void SceneFileManager::getVPCommon (const QDomElement & value, VPCommon &object) {
    object.coords [PX] = getFloat(value, "x");
    object.coords [PY] = getFloat(value, "y");
    object.coords [PZ] = getFloat(value, "z");
  }

  int SceneFileManager::getInt(const QDomElement &elem, const QString &name)
  {
	  bool ok;
	  int val;
	  val = elem.attribute(name).toInt(&ok, 10);
	  if(!ok) throw std::logic_error("Błąd podczas wczytywania elementu " + name.toStdString());
	  return val;
  }

  float SceneFileManager::getFloat(const QDomElement &elem, const QString &name)
  {
	  bool ok;
	  float val;
	  val = elem.attribute(name).toFloat(&ok);
	  if(!ok) throw std::logic_error("Błąd podczas wczytywania elementu " + name.toStdString());
	  return val;
  }

  double SceneFileManager::getDouble(const QDomElement &elem, const QString &name)
  {
	  bool ok;
	  double val;
	  val = elem.attribute(name).toDouble(&ok);
	  if(!ok) throw std::logic_error("Błąd podczas wczytywania elementu " + name.toStdString());
	  return val;
  }
}/* namespace Model */
