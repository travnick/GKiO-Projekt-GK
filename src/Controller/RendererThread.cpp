/// @file Main/Controller/RendererThread.cpp
/// @date 06-01-2012
/// @author Mikołaj Milej

#include "Controller/RendererThread.h"
#include "Model/Renderer.h"
#include "Model/RenderTileData.h"

namespace Controller {

  RendererThread::RendererThread (){
    renderer.reset(new Model::Renderer);
  }

  RendererThread::~RendererThread (){
  }

  void RendererThread::setRenderParams (const Model::Scene &scene, const bool *newAllowRunning){
    renderer->setScene( &scene);
    allowRunning = newAllowRunning;
  }

  void RendererThread::run (){
    renderer->render( *tile, *allowRunning);
  }

} /* namespace Controller */
