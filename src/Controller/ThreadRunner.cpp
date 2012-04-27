/// @file Main/Controller/ThreadRunner.cpp
/// @date 07-01-2012
/// @author Mikołaj Milej

#include <QMutexLocker>
#include <QSemaphore>
#include <QThreadPool>

#include "Controller/ThreadRunner.h"
#include "Controller/MainWindow.h"
#include "Controller/RendererThread.h"
#include "Controller/GlobalDefines.h"
#include "Model/RenderTileData.h"

#define THREAD_EXPIRE_TIMEOUT 5

namespace Controller {

  void cleanImage (QSharedPointer <Model::RenderTileData> &tile, colorType color =
                       DEFAULT_RENDER_COLOR){
    for (quint64 size = 0; size < tile->imageDataSize; size += BPP)
    {
      tile->imageData [size] = color;
      tile->imageData [size + 1] = color;
      tile->imageData [size + 2] = color;
    }
  }

  ThreadRunner::ThreadRunner (){
    mutex.reset(new QMutex);
  }

  ThreadRunner::~ThreadRunner (){
    QMutexLocker locker(mutex.data());
  }

  void ThreadRunner::setParams (const QSharedPointer <Model::RenderTileData>& newImage,
                                const QSharedPointer <Model::Scene> &newScene,
                                int newMaxThreadCount,
                                bool newRandomRender){
    QMutexLocker locker(mutex.data());
    this->image = newImage;
    this->scene = newScene;
    this->maxThreadCount = newMaxThreadCount;
    cleanImage(this->image);
    randomRender = newRandomRender;
  }

  void ThreadRunner::run (){
    threadPool.reset(new QThreadPool);
    threadPool->setExpiryTimeout(THREAD_EXPIRE_TIMEOUT);
    threadPool->setMaxThreadCount(maxThreadCount);

    finishedThreadCount = 0;
    ableToRunning = true;

    createTiles();

    if (randomRender)
    {
      randomizeTiles();
    }

    int tilesSize = tiles.size();
    for (int i = 0; i < tilesSize; ++i)
    {
      RendererThread *renderWorker = new RendererThread;
      renderWorker->setRenderParams( *scene, ableToRunning);
      renderWorker->setTile(tiles [i]);

      threadPool->start(renderWorker);
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

    for (imageUnit y = 0; y < tileYLimit; y += tileSize)
    {
      for (imageUnit x = 0; x < tileXLimit; x += tileSize)
      {
        createTile(x, y, tileSize, tileSize);
      }
    }

    if (tilesY.rem > 0)
    {
      for (imageUnit x = 0; x < tileXLimit; x += tileSize)
      {
        createTile(x, tileYLimit, tileSize, tilesY.rem);
      }
    }

    if (tilesX.rem > 0)
    {
      for (imageUnit y = 0; y < tileYLimit; y += tileSize)
      {
        createTile(tileXLimit, y, tilesX.rem, tileSize);
      }
    }

    if (tilesX.rem > 0 && tilesY.rem > 0)
    {
      createTile(tileXLimit, tileYLimit, tilesX.rem, tilesY.rem);
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

    tiles.swap(tilesRandomized);
  }

  void ThreadRunner::terminate (){
    QMutexLocker locker(mutex.data());

    ableToRunning = false;
  }

} /* namespace Controller */
