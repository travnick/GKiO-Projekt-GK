/// @file Controller/ThreadRunner.h

#pragma once

#include <QScopedPointer>
#include <QSharedPointer>

#include <QList>
#include <QObject>
#include <QRunnable>
#include <QVector>

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
      void setParams (const QSharedPointer <Model::RenderTileData> &image,
                      const QSharedPointer <RenderParams> &newRenderParams);

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

      /**Creates image tiles
       *
       */
      void createTiles ();

    signals:
      /**Sends signal when all threads done their job
       *
       */
      void renderFinished ();

    private:

      /**Stores rendering parameters
       *
       */
      QSharedPointer <RenderParams> renderParams;

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

      QVector <QSharedPointer <RendererThread> > renderers;

      QSharedPointer <Model::RenderTileData> image;

      /**Creates image tile
       * New tile is added to the end of tile container
       *
       * @param x tile start x relative to image start
       * @param y tile start y relative to image start
       * @param tileSizeX tile width
       * @param tileSizeY tile height
       */
      void createTile (int x, int y, int tileSizeX, int tileSizeY);

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
