/// @file Controller/RendererThread.h

#pragma once

#include <QSharedPointer>
#include <QScopedPointer>
#include <QRunnable>
#include <QObject>

//Forward declarations -->
namespace Model {
  class Renderer;
  class RenderTileData;
  class Scene;
}
// <-- Forward declarations

namespace Controller {

  struct RenderParams {
      QSharedPointer <Model::Scene> scene;
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
  class RendererThread: public QObject, public QRunnable {
    Q_OBJECT

    public:

      /**Initializes renderer
       *
       */
      RendererThread (const QSharedPointer <RenderParams> &renderParams);

      /**Needed for QScopedPointer
       *
       */
      ~RendererThread ();

      /**Sets rendering parameters
       *
       * @param renderParams rendering parameters
       */
      void setRenderParams (const RenderParams &renderParams);

      /**Sets tile to render
       *
       * @param newTile description of image part to render
       */
      void setTile (const QSharedPointer <Model::RenderTileData> &newTile){
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
      QSharedPointer <Model::RenderTileData> tile;

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
