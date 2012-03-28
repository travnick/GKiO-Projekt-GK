/// @file Main/Controller/RendererThread.h
/// @date 06-01-2012
/// @author Mikołaj Milej

#pragma once

#include <QSharedPointer>
#include <QScopedPointer>
#include <QRunnable>
#include <QObject>

//Forward declarations -->
namespace Model {
  class Renderer;
  struct RenderTileData;
  class Scene;
}
// <-- Forward declarations

namespace Controller {

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
      RendererThread ();

      /**It's empty now
       *
       */
      virtual ~RendererThread ();

      /**Sets rendering parameters
       * If allowRunning is changed outside the thread to false
       * thread breaks rendering as fast as renderer checks condition
       * "if (allowRunning)"
       *
       * @param tile description of image part to render
       * @param scene 3D scene to render
       * @param allowRunning is thread allower to run or not
       */
      void setRenderParams (const QSharedPointer <Model::RenderTileData> &tile,
                            const Model::Scene &scene,
                            const bool &allowRunning);

      /**Runs rendering
       * It's called by thread manager
       *
       */
      void run ();

    private:
      const bool *allowRunning;

      /**Description of image part to render
       *
       */
      QSharedPointer <Model::RenderTileData> tile;
      QScopedPointer <Model::Renderer> renderer;

      /**Disables copying of object
       *
       */
      Q_DISABLE_COPY (RendererThread)
  };

} /* namespace Controller */
