/// @file Controller/RendererThread.cpp

#include "Controller/RendererThread.h"
#include "Model/Renderer.h"
#include "Model/RenderTileData.h"

namespace Controller
{

  RendererThread::RendererThread (const std::shared_ptr <RenderParams> &newRenderParams)
      : renderer(new Model::Renderer(*newRenderParams))
  {
  }

  RendererThread::~RendererThread ()
  {
  }

  void RendererThread::run ()
  {
    renderer->render(*tile);
  }

} /* namespace Controller */
