/// @file Controller/RendererThread.h

#pragma once

#include <QScopedPointer>

#include <QObject>
#include <QRunnable>

#include "Model/ModelDefines.h"

namespace Controller
{

  struct RenderParams
  {
      Model::ScenePtr scene;
      bool allowRunning;
      bool randomRender;
      bool shadows;
      int maxThreadCount;
      int reflectionDeep;
      int refractionDeep;
  };

  /**Single render thread
   * It renders part of image which is described in tile parameter
   *
   */
  class RendererThread: public QObject, public QRunnable
  {
    Q_OBJECT

    public:

      /**Initializes renderer
       *
       */
      RendererThread (const std::shared_ptr <RenderParams> &renderParams);

      /**Needed for QScopedPointer
       *
       */
      ~RendererThread ();

      /**Sets tile to render
       *
       * @param newTile description of image part to render
       */
      void setTile (const std::shared_ptr <Model::RenderTileData> &newTile)
      {
        tile = newTile;
      }

    public slots:
      /**Runs rendering
       * It's called by thread manager
       *
       */
      void run ();

    private:
      /**Description of image part to render
       *
       */
      std::shared_ptr <Model::RenderTileData> tile;

      /**Renderer
       *
       */
      QScopedPointer <Model::Renderer> renderer;

      /**Disables copying of object
       *
       */
      Q_DISABLE_COPY (RendererThread)
  };

} /* namespace Controller */
