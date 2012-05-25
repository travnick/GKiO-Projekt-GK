/// @file Model/Scene.cpp

#include <exception>
#include <QFile>

#include "Model/Camera.h"
#include "Model/Light.h"
#include "Model/Object.h"
#include "Model/Scene.h"
#include "Model/SceneFileManager.h"
#include "Model/Sphere.h"

using namespace std;

namespace Model
{

  Scene::Scene ()
  {
    loaded = false;
  }

  bool Scene::init (const QString &filename, bool reload) throw (std::exception)
  {
    bool result = false;
    QFile infile(filename);

    if (loaded && !reload)
    {
      return loaded;
    }

    if (infile.exists())
    {
      camera.clear();
      lights.clear();
      materials.clear();
      objects.clear();

      SceneFileManager fileManager;
      fileManager.loadScene(infile, *this);
      infile.close();
      result = true;
    }
    loaded = result;

    return result;
  }

  void Scene::updateCamera ()
  {
    camera->calibrate();
  }

  void Scene::calculateObjectSizeOnImage (const ObjectPtr &)
  {
    //TODO: implement
  }
}
