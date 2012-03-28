/// @file Main/Controller/ThreadRunner.cpp
/// @date 07-01-2012
/// @author Mikołaj Milej

#include <QMutexLocker>
#include <QThreadPool>

#include "Controller/ThreadRunner.h"
#include "Controller/MainWindow.h"
#include "Controller/RendererThread.h"
#include "Controller/GlobalDefines.h"
#include "Model/RenderTileData.h"

#define THREAD_EXPIRE_TIMEOUT 5

namespace Controller {

  void cleanImage (QSharedPointer <Model::RenderTileData> tile){
    for (quint64 size = 0; size < tile->imageDataSize; size += BPP)
    {
      tile->imageData [size] = 100;
      tile->imageData [size + 1] = 100;
      tile->imageData [size + 2] = 100;
    }
  }

  ThreadRunner::ThreadRunner (){
    mutex.reset(new QMutex);
  }

  ThreadRunner::~ThreadRunner (){
    //Prevent destroying ThreadRunner if something else is
    QMutexLocker locker(mutex.data());
  }

  void ThreadRunner::setParams (const QSharedPointer <Model::RenderTileData> &newImage,
                                const QSharedPointer <Model::Scene> newScene,
                                int newMaxThreadCount,
                                const MainWindow *eventListener){
    //Prevent destroying ThreadRunner if something is doing here
    QMutexLocker locker(mutex.data());
    this->image = newImage;
    this->scene = newScene;
    this->maxThreadCount = newMaxThreadCount;
    this->updateEventListener = eventListener;

    cleanImage(this->image);
    eventListener->updateImage();
  }

  void ThreadRunner::run (){
    threadPool.reset(new QThreadPool);
    threadPool->setExpiryTimeout(THREAD_EXPIRE_TIMEOUT);
    threadPool->setMaxThreadCount(maxThreadCount);

    threadCount = 0;
    ableToRunning = true;

    imageUnit tileSize = image->width;
    div_t tilesX = div(image->imageWidth, tileSize);
    div_t tilesY = div(image->imageHeight, tileSize);
    imageUnit tileXLimit = image->imageWidth - tilesX.rem;
    imageUnit tileYLimit = image->imageHeight - tilesY.rem;

    for (imageUnit y = 0; y < tileYLimit; y += tileSize)
    {
      for (imageUnit x = 0; x < tileXLimit; x += tileSize)
      {
        createThread(createTile(x, y, tileSize, tileSize));
      }
    }

    if (tilesY.rem > 0)
    {
      for (imageUnit x = 0; x < tileXLimit; x += tileSize)
      {
        createThread(createTile(x, tileYLimit, tileSize, tilesY.rem));
      }
    }

    if (tilesX.rem > 0)
    {
      for (imageUnit y = 0; y < tileYLimit; y += tileSize)
      {
        createThread(createTile(tileXLimit, y, tilesX.rem, tileSize));
      }
    }

    if (tilesX.rem > 0 && tilesY.rem > 0)
    {
      createThread(createTile(tileXLimit, tileYLimit, tilesX.rem, tilesY.rem));
    }

    //It waits for all threads to finish and destroy them
    threadPool->waitForDone();

    QMutexLocker locker(mutex.data());
    emit renderFinished ();
  }

  inline void ThreadRunner::createThread (const QSharedPointer <Model::RenderTileData> &tile){
    if (ableToRunning)
    {
      ++threadCount;

      RendererThread *renderWorker = new RendererThread;
      renderWorker->setRenderParams(tile, *scene, ableToRunning);

      //QThreadPool takes ownership of renderWorker. no memory leaks here
      threadPool->start(renderWorker);
    }
  }

  inline QSharedPointer <Model::RenderTileData> ThreadRunner::createTile (const int &x,
                                                                          const int &y,
                                                                          const int &tileSizeX,
                                                                          const int &tileSizeY){

    QSharedPointer <Model::RenderTileData> tile(new Model::RenderTileData);
    //prevent deleting image data when deleting tile
    tile->deleteImageData = false;
    tile->imageData = image->imageData;
    tile->topLeft.x = x;
    tile->topLeft.y = y;
    tile->bottomRight.x = tile->topLeft.x + tileSizeX;
    tile->bottomRight.y = tile->topLeft.y + tileSizeY;
    tile->width = tileSizeX;
    tile->height = tileSizeY;
    tile->imageWidth = image->imageWidth;
    tile->imageHeight = image->imageHeight;
    tile->imageDataSize = image->imageDataSize;

    return tile;
  }

  void ThreadRunner::terminate (){
    //Prevent destroying ThreadRunner if something is doing here
    QMutexLocker locker(mutex.data());

    ableToRunning = false;
  }

} /* namespace Controller */
