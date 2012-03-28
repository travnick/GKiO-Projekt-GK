/// @file Main/Controller/ThreadRunner.h
/// @date 07-01-2012
/// @author Mikołaj Milej

#pragma once

#include <QSharedPointer>
#include <QScopedPointer>

#include <QRunnable>
#include <QObject>

//Forward declarations -->
class QMutex;
class QSemaphore;
class QThreadPool;

namespace Model {
  struct RenderTileData;
  class Scene;
}  // namespace Model
// <-- Forward declarations

namespace Controller {
  //Forward declarations -->
  class MainWindow;
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
       * @param updateEventListener listener for image update events
       */
      void setParams (const QSharedPointer <Model::RenderTileData> &image,
                      const QSharedPointer <Model::Scene> scene,
                      int maxThreadCount,
                      const MainWindow *updateEventListener);

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
      int maxThreadCount;
      /**Stores current count of created threads
       *
       */
      int threadCount;

      /**Thread manager
       *
       */
      QScopedPointer <QThreadPool> threadPool;
      QScopedPointer <QMutex> mutex;

      const MainWindow *updateEventListener;
      QSharedPointer <Model::RenderTileData> image;
      QSharedPointer <Model::Scene> scene;

      /**Creates render thread
       *
       * @param tile image tile to render
       */
      void createThread (const QSharedPointer <Model::RenderTileData> &tile);

      /**Creates image tile
       *
       * @param x tile start x relative to image start
       * @param y tile start y relative to image start
       * @param tileSizeX tile width
       * @param tileSizeY tile height
       * @return image tile
       */
      QSharedPointer <Model::RenderTileData> createTile (const int &x,
                                                         const int &y,
                                                         const int &tileSizeX,
                                                         const int &tileSizeY);

      /**Disables copying of object
       *
       */
      Q_DISABLE_COPY (ThreadRunner)
  };

} /* namespace Controller */
