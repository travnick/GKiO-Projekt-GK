/// @file Controller/ThreadRunner.cpp
/// @date 07-01-2012
/// @author Mikołaj Milej

#include <QMutexLocker>
#include <QThreadPool>
#include <ctime>

#include "Controller/ThreadRunner.h"
#include "Controller/MainWindow.h"
#include "Controller/RendererThread.h"
#include "Controller/GlobalDefines.h"
#include "Model/RenderTileData.h"

#define THREAD_EXPIRE_TIMEOUT 5

namespace Controller {

  ThreadRunner::ThreadRunner ()
      : renderParams(new RenderParams), threadPool(new QThreadPool), mutex(new QMutex){
    threadPool->setExpiryTimeout(THREAD_EXPIRE_TIMEOUT);
  }

  ThreadRunner::~ThreadRunner (){
    QMutexLocker locker(mutex.data());
  }

  void ThreadRunner::setParams (const QSharedPointer <Model::RenderTileData>& newImage,
                                const QSharedPointer <RenderParams> &newRenderParams){
    QMutexLocker locker(mutex.data());
    this->image = newImage;

    renderParams = newRenderParams;
  }

  void ThreadRunner::run (){
    QMutexLocker locker(mutex.data());
    threadPool->setMaxThreadCount(renderParams->maxThreadCount);

    int tilesSize = tiles.size();
    for (int i = 0; i < tilesSize; ++i)
    {
      renderers [i]->setTile(tiles [i]);

      threadPool->start(renderers [i].data());
    }

    threadPool->waitForDone();

    emit renderFinished();
  }

  inline void ThreadRunner::createTile (int x, int y, int tileSizeX, int tileSizeY){
    //Copy common data that are needed to render
    QSharedPointer <Model::RenderTileData> tile(new Model::RenderTileData( *image));

    tile->deleteImageData = false;

    tile->topLeft.x = x;
    tile->topLeft.y = y;
    tile->bottomRight.x = tile->topLeft.x + tileSizeX;
    tile->bottomRight.y = tile->topLeft.y + tileSizeY;

    tile->width = tileSizeX;
    tile->height = tileSizeY;

    tiles.append(tile);
  }

  void ThreadRunner::createTiles (){
    QMutexLocker locker(mutex.data());
    tiles.clear();

    imageUnit tileSize = image->width;
    div_t tilesX = div(image->imageWidth, tileSize);
    div_t tilesY = div(image->imageHeight, tileSize);
    imageUnit tileXLimit = image->imageWidth - tilesX.rem;
    imageUnit tileYLimit = image->imageHeight - tilesY.rem;
    int tilesNumber = tilesX.quot * tilesY.quot;

    tilesNumber += tilesX.rem > 0 ? 1 : 0;
    tilesNumber += tilesY.rem > 0 ? 1 : 0;

    tiles.reserve(tilesNumber);
    tilesRandomized.reserve(tilesNumber);

    //Slice image into whole tiles
    for (imageUnit y = 0; y < tileYLimit; y += tileSize)
    {
      for (imageUnit x = 0; x < tileXLimit; x += tileSize)
      {
        createTile(x, y, tileSize, tileSize);
      }
    }

    //Slice bottom part of image (if any) into part tiles
    if (tilesY.rem > 0)
    {
      for (imageUnit x = 0; x < tileXLimit; x += tileSize)
      {
        createTile(x, tileYLimit, tileSize, tilesY.rem);
      }
    }

    //Slice right part of image (if any) into part tiles
    if (tilesX.rem > 0)
    {
      for (imageUnit y = 0; y < tileYLimit; y += tileSize)
      {
        createTile(tileXLimit, y, tilesX.rem, tileSize);
      }
    }

    //Create tile for right bottom corner if left
    if (tilesX.rem > 0 && tilesY.rem > 0)
    {
      createTile(tileXLimit, tileYLimit, tilesX.rem, tilesY.rem);
    }

    if (renderParams->randomRender)
    {
      randomizeTiles();
    }

    //Create threads if there is more tiles. Don't delete threads if there is less tiles
    if (tiles.size() > renderers.size())
    {
      for (int i = renderers.size(), j = tiles.size(); i < j; ++i)
      {
        QSharedPointer <RendererThread> renderer((new RendererThread(renderParams)));
        renderer->setAutoDelete(false);

        renderers.append(renderer);
      }
    }
  }

  void ThreadRunner::randomizeTiles (){
    qsrand(time(NULL));

    while ( !tiles.empty())
    {
      int rand = qrand() % tiles.size();
      tilesRandomized.append(tiles [rand]);
      tiles.removeAt(rand);
    }

    //Do fast swap of vectors
    tiles.swap(tilesRandomized);
  }

  void ThreadRunner::terminate (){
    QMutexLocker locker(mutex.data());

    renderParams->allowRunning = false;
  }

} /* namespace Controller */
