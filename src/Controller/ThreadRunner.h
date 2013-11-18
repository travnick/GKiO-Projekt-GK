/// @file Controller/ThreadRunner.h

#pragma once

#include <QScopedPointer>

#include <QList>
#include <QObject>
#include <QRunnable>

#include <common.h>

//Forward declarations -->
class QMutex;
class QThreadPool;

namespace Model
{
  struct RenderTileData;
  class Scene;
}  // namespace Model
// <-- Forward declarations

namespace Controller
{
  //Forward declarations -->
  class MainWindow;
  class RendererThread;
  struct RenderParams;
  // <-- Forward declarations

  /**Creates threads for image rendering
   *
   */
  class ThreadRunner: public QObject, public QRunnable
  {
    Q_OBJECT

    public:
      ThreadRunner ();
      virtual ~ThreadRunner ();

      /**Sets image and rendering parameters
       *
       * @param image image description
       * @param newRenderParams rendering parameters
       */
      void setParams (const std::shared_ptr <Model::RenderTileData> &image,
                      const std::shared_ptr <RenderParams> &newRenderParams);

      /**Runs rendering with threads.
       * Threads above maxThreadCount are added to queue.
       */
      virtual void run ();

    public slots:
      /**Listener for terminate signal
       *
       */
      void terminate ();

      /**Creates tiles and render threads, each tread renders one tile.
       * There are created as much threads as tiles.
       *
       */
      void createTiles ();

      /**Randomizes tiles
       *
       */
      void randomizeTiles ();

      /**Sets tile order to default
       *
       */
      void resetTilesOrder ();

    signals:
      /**Sends signal when all threads done their job
       *
       */
      void renderFinished ();

    private:
      bool tilesRandomized;

      /**Stores rendering parameters
       *
       */
      std::shared_ptr <RenderParams> renderParams;

      /**Thread manager
       *
       */
      QScopedPointer <QThreadPool> threadPool;

      QScopedPointer <QMutex> mutex;

      /**Container for tiles that are queued
       *
       */
      QList <std::shared_ptr <Model::RenderTileData> > tiles;

      QList <std::shared_ptr <RendererThread> > renderers;
      QList <std::shared_ptr <RendererThread> > renderersRandomized;

      std::shared_ptr <Model::RenderTileData> image;

      /**Creates image tile
       * New tile is added to the end of tile container
       *
       * @param x tile start x relative to image start
       * @param y tile start y relative to image start
       * @param tileSizeX tile width
       * @param tileSizeY tile height
       */
      void createTile (int x, int y, int tileSizeX, int tileSizeY);

      /**Disables copying of object
       *
       */
      Q_DISABLE_COPY (ThreadRunner)
  };

} /* namespace Controller */
