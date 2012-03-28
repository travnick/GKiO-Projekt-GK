/// @file Main/Model/Scene.cpp
/// @date 01-12-2011
/// @author Mikołaj Milej

#include <string>
#include <fstream>
#include <iostream>
#include <assert.h>

#include "Model/Scene.h"
#include "Model/SceneFileManager.h"
#include "Model/Object.h"
#include "Model/Camera.h"
#include "Model/Light.h"
#include "Model/Sphere.h"

using namespace std;

namespace Model {

  Scene::Scene (){
    loaded = false;
  }

  bool Scene::init (string &filename, bool reload) throw (std::exception){
    bool result = false;
    ifstream infile;

    if (loaded && !reload)
    {
      return loaded;
    }

    infile.open(filename.c_str(), ifstream::in);

    if (infile.good())
    {
      camera.clear();
      lights.clear();
      materials.clear();
      objects.clear();

      SceneFileManager fileManager;
      fileManager.loadScene(infile, *this);

      result = true;
    }
    else
    {
      result = false;
    }
    infile.close();

    loaded = result;

    return result;
  }

  void Scene::updateCamera (){
    camera->calibrate();
  }

}
