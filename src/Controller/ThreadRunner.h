/// @file Controller/ThreadRunner.h
/// @date 07-01-2012
/// @author Mikołaj Milej

#pragma once

#include <QSharedPointer>
#include <QScopedPointer>

#include <QRunnable>
#include <QObject>
#include <QList>

//Forward declarations -->
class QMutex;
class QThreadPool;

namespace Model {
  struct RenderTileData;
  class Scene;
}  // namespace Model
// <-- Forward declarations

namespace Controller {
  //Forward declarations -->
  class MainWindow;
  class RendererThread;
  // <-- Forward declarations

  /**Creates threads for image rendering
   *
   */
  class ThreadRunner: public QObject, public QRunnable {
    Q_OBJECT

    public:
      ThreadRunner ();
      virtual ~ThreadRunner ();

      /**Sets image parameters
       *
       * @param image image description
       * @param scene 3D scene
       * @param maxThreadCount max thread to run simultaneously
       * @param randomRender determines if render tiles randomly
       */
      void setParams (const QSharedPointer <Model::RenderTileData> &image,
                      const QSharedPointer <Model::Scene> &scene,
                      int maxThreadCount,
                      bool randomRender);

      /**Creates tiles and render threads, each tread renders one tile.
       * There are created as much threads as tiles.
       * Threads above maxThreadCount are added to queue.
       */
      void run ();

    public slots:
      /**Listener for terminate signal
       *
       */
      void terminate ();

    signals:
      /**Sends signal when all threads done their job
       *
       */
      void renderFinished ();

    private:
      bool ableToRunning;
      bool randomRender;
      int maxThreadCount;

      /**Stores current count of created threads
       *
       */
      int finishedThreadCount;

      /**Thread manager
       *
       */
      QScopedPointer <QThreadPool> threadPool;

      QScopedPointer <QMutex> mutex;

      /**Container for tiles that are queued
       *
       */
      QList <QSharedPointer <Model::RenderTileData> > tiles;
      QList <QSharedPointer <Model::RenderTileData> > tilesRandomized;

      QSharedPointer <Model::RenderTileData> image;
      QSharedPointer <Model::Scene> scene;

      /**Creates image tile
       * New tile is added to the end of tile container
       *
       * @param x tile start x relative to image start
       * @param y tile start y relative to image start
       * @param tileSizeX tile width
       * @param tileSizeY tile height
       */
      void createTile (int x, int y, int tileSizeX, int tileSizeY);

      /**Creates image tiles
       *
       */
      void createTiles ();

      /**Randomizes tiles
       *
       */
      void randomizeTiles ();

      /**Disables copying of object
       *
       */
      Q_DISABLE_COPY (ThreadRunner)
  };

} /* namespace Controller */
