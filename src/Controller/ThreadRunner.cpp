/// @file Controller/ThreadRunner.cpp

#include <ctime>
#include <QMutexLocker>
#include <QThreadPool>

#include "Controller/GlobalDefines.h"
#include "Controller/MainWindow.h"
#include "Controller/RendererThread.h"
#include "Controller/ThreadRunner.h"
#include "Model/RenderTileData.h"

#define THREAD_EXPIRE_TIMEOUT 5

namespace Controller
{

  ThreadRunner::ThreadRunner ()
      : renderParams(new RenderParams), threadPool(new QThreadPool), mutex(
          new QMutex)
  {
    threadPool->setExpiryTimeout(THREAD_EXPIRE_TIMEOUT);
    tilesRandomized = false;
  }

  ThreadRunner::~ThreadRunner ()
  {
    QMutexLocker locker(mutex.data());
  }

  void ThreadRunner::setParams (const std::shared_ptr <Model::RenderTileData>& newImage,
                                const std::shared_ptr <RenderParams> &newRenderParams)
  {
    QMutexLocker locker(mutex.data());
    this->image = newImage;

    renderParams = newRenderParams;
  }

  void ThreadRunner::run ()
  {
    QMutexLocker locker(mutex.data());
    threadPool->setMaxThreadCount(renderParams->maxThreadCount);

    int tileCount = tiles.size();
    for (int i = 0; i < tileCount; ++i)
    {
      threadPool->start(renderers [i].get());
    }

    threadPool->waitForDone();

    emit renderFinished();
  }

  inline void ThreadRunner::createTile (int x,
                                        int y,
                                        int tileSizeX,
                                        int tileSizeY)
  {
    //Copy common data that are needed to render
    std::shared_ptr <Model::RenderTileData> tile(
        new Model::RenderTileData(*image));

    tile->deleteImageData = false;

    tile->topLeft.x = x;
    tile->topLeft.y = y;
    tile->bottomRight.x = tile->topLeft.x + tileSizeX;
    tile->bottomRight.y = tile->topLeft.y + tileSizeY;

    tile->width = tileSizeX;
    tile->height = tileSizeY;

    tiles.append(tile);
  }

  void ThreadRunner::createTiles ()
  {
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

    renderers.reserve(tiles.size());
    renderersRandomized.reserve(tiles.size());

    int i = renderers.size() - tiles.size();

    if (i < 0)
    { //Create threads if there are more tiles
      do
      {
        std::shared_ptr <RendererThread> renderer(
            (new RendererThread(renderParams)));
        renderer->setAutoDelete(false);

        renderers.append(renderer);
      }
      while ( ++i < 0);
    }
    else
    { //Delete threads, if there are more than tiles
      while (i-- > 0)
      {
        renderers.removeLast();
      }
    }
    //force assigning tiles to threads
    tilesRandomized = true;
    resetTilesOrder();
  }

  void ThreadRunner::randomizeTiles ()
  {
    qsrand(time(NULL));
    int rand;
    while (!renderers.empty())
    {
      rand = qrand() % renderers.size();
      renderersRandomized.append(renderers [rand]);
      renderers.removeAt(rand);
    }
    //Do fast swap of vectors
    renderers.swap(renderersRandomized);
    tilesRandomized = true;
  }

  void ThreadRunner::resetTilesOrder ()
  {
    if (!tilesRandomized)
    {
      return;
    }

    int tileCount = tiles.size();
    for (int i = 0; i < tileCount; ++i)
    {
      renderers [i]->setTile(tiles [i]);
    }
    tilesRandomized = false;
  }

  void ThreadRunner::terminate ()
  {
    QMutexLocker locker(mutex.data());

    renderParams->allowRunning = false;
  }

} /* namespace Controller */
