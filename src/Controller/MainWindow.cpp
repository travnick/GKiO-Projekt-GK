/// @file Controller/MainWindow.cpp

// QApplication and QDesktopWidget are needed to center window on screen
#include <QApplication>
#include <QDesktopWidget>

#include <QElapsedTimer>
#include <QErrorMessage>
#include <QFileDialog>
#include <QImage>
#include <QImageWriter>
#include <QString>
#include <QThreadPool>
#include <QTimer>

#include "Controller/GlobalDefines.h"
#include "Controller/MainWindow.h"
#include "Controller/RendererThread.h"
#include "Controller/ThreadRunner.h"
#include "Model/RenderTileData.h"
#include "Model/Scene.h"
#include "View/ui_MainWindow.h"

#define TIME_BEFORE_REMOVE_THREADS 1000 //[ms]
#define DEFAULT_REFRESH_TIME 1000 //[ms]
#define WINDOW_MARGIN 0 //
#define IMAGE_SAVE_FORMAT "png"

#define DEFAULT_IMAGE_WIDTH 800
#define DEFAULT_IMAGE_HEIGHT 560
#define DEFAULT_TILE_SIZE 100

namespace Controller
{

  void MainWindow::showWarning (QString message)
  {
    QErrorMessage *errorMessage = new QErrorMessage(this);
    //No memory leaks here
    errorMessage->setAttribute(Qt::WA_DeleteOnClose);
    errorMessage->setWindowModality(Qt::ApplicationModal);

    errorMessage->showMessage(message);
  }

  MainWindow::MainWindow (QMainWindow *myParent)
      : QMainWindow(myParent), image(new Model::RenderTileData), scene(
          new Model::Scene), timeCounter(new QElapsedTimer), renderParams(
          new RenderParams), threadRunner(new ThreadRunner)
  {
    ui.reset(new Ui::MainWindow);
    refreshTimer.reset(new QTimer);

    QThreadPool::globalInstance()->setExpiryTimeout(TIME_BEFORE_REMOVE_THREADS);
    image->imageData = 0;

    renderParams->scene = scene;
    renderParams->randomRender = false;

    threadRunner->setParams(image, renderParams);
    threadRunner->setAutoDelete(false);

    sizeChanged = false;
    addResult = true;
    renderingInProgress = false;

    setUpGUI();
    loadScene();
  }

  MainWindow::~MainWindow ()
  {
    //Send terminate signal to thread runner and wait for end
    ui->terminateRender->click();

    QThreadPool::globalInstance()->waitForDone(TIME_BEFORE_REMOVE_THREADS);
  }

  void MainWindow::runRenderer ()
  {
    renderingInProgress = true;

    deactivateButtons();

    if (!updateCamera())
    {
      return;
    }

    if (!ui->liveCamera->isChecked())
    {
      ui->imageViewer->getImage()->fill(Qt::darkGray);
    }

    renderParams->allowRunning = true;
    renderParams->maxThreadCount = ui->threadCounter->value();
    renderParams->reflectionDeep = ui->maxReflectionDeep->value();
    renderParams->refractionDeep = ui->maxRefractionDeep->value();
    renderParams->shadows = ui->shadows->isChecked();
    renderParams->randomRender = ui->randomRender->isChecked();

    if (renderParams->randomRender)
    {
      threadRunner->randomizeTiles();
    }
    else
    {
      threadRunner->resetTilesOrder();
    }

    refreshTimer->start();
    timeCounter->start();

    QThreadPool::globalInstance()->start(threadRunner.data());
  }

  bool MainWindow::updateCamera ()
  {
    bool tileChanged = image->width != ui->tileSize->value();

    image->width = ui->tileSize->value();
    image->height = image->width;

    if (sizeChanged)
    {
      if (!allocateMemoryForImage())
      {
        return false;
      }

      scene->setImageWidth(image->imageWidth);
      scene->setImageHeight(image->imageHeight);

      ui->imageViewer->setImage(
          new QImage(image->imageData, image->imageWidth, image->imageHeight,
                     imageBytesPerLine, QImage::Format_RGB888),
          true);
    }

    if (sizeChanged || tileChanged)
    {
      threadRunner->createTiles();
    }
    sizeChanged = false;

    scene->getCamera()->setPosition(ui->xPos->value(), ui->yPos->value(),
                                    ui->zPos->value());

    scene->getCamera()->getAngles().set(ui->xAngle->value(),
                                        ui->yAngle->value(),
                                        ui->zAngle->value());

    scene->getCamera()->updateRotation();
    scene->getCamera()->setFOV(ui->fov->value());

    scene->updateCamera();

    return true;
  }

  void MainWindow::terminateRender ()
  {
    addResult = false;

    //stop all threads
    renderParams->allowRunning = false;
  }

  void MainWindow::setRefreshTime (int refreshTime)
  {
    refreshTimer->setInterval(refreshTime);
  }

  void MainWindow::saveImage ()
  {
    const QImage *imageToSave = ui->imageViewer->getImage();
    if (imageToSave != 0)
    {
      QString fileName = QFileDialog::getSaveFileName(
          this, tr("Zapisz obraz"), "RenderedImage.png",
          tr("Image Files (*.png)"), 0, QFileDialog::DontUseNativeDialog);

      if (fileName.isEmpty())
      {
        return;
      }

      QImageWriter imageWriter(fileName);

      imageWriter.setFormat(IMAGE_SAVE_FORMAT);
      imageWriter.setCompression(9);
      imageWriter.write(*imageToSave);
    }
  }

  bool MainWindow::allocateMemoryForImage ()
  {
    void *mem = realloc(image->imageData,
                        image->imageDataSize * sizeof(colorType));

    if (mem == 0)
    {
      showWarning(QSTRING("Nie można przydzielić wymaganej ilości pamięci.<br>"
          "Proszę zmniejszyć obrazek"));

      return false;
    }
    else
    {
      image->imageData = static_cast <colorType*>(mem);
    }
    return true;
  }

  void MainWindow::changeWidth (int newWidth)
  {
    imageUnit matchImageHeight = image->imageHeight;

    while (matchImageHeight >= 1
        && !refreshMemoryRequest(newWidth, matchImageHeight))
    {
      --matchImageHeight;
    }

    ui->imageWidth->setValue(image->imageWidth);
    ui->imageHeight->setValue(image->imageHeight);
  }

  void MainWindow::changeHeight (int newHeight)
  {
    imageUnit matchImageWidth = image->imageWidth;

    while (matchImageWidth >= 1
        && !refreshMemoryRequest(matchImageWidth, newHeight))
    {
      --matchImageWidth;
    }

    ui->imageWidth->setValue(image->imageWidth);
    ui->imageHeight->setValue(image->imageHeight);
  }

  bool MainWindow::refreshMemoryRequest (imageUnit imageWidth,
                                         imageUnit imageHeight)
  {
    QString sufix;
    double memoryForImage;

    int64_t newImageBytesPerLine = imageWidth * BPP;
    int64_t newImageDataSize = newImageBytesPerLine * imageHeight;

    if (newImageDataSize > IMAGE_MAX_DATA_SIZE)
    {
      return false;
    }

    imageBytesPerLine = newImageBytesPerLine;
    image->imageDataSize = newImageDataSize;
    image->imageHeight = imageHeight;
    image->imageWidth = imageWidth;

    if (image->imageDataSize < KiB)
    {
      memoryForImage = image->imageDataSize;
      sufix = QSTRING(BYTE_NAME);
    }
    else
    {
      if (image->imageDataSize < MiB)
      {
        memoryForImage = image->imageDataSize / KiB;
        sufix = QSTRING(KILOBYTE_NAME);
      }
      else
      {
        if (image->imageDataSize < GiB)
        {
          memoryForImage = image->imageDataSize / MiB;
          sufix = QSTRING(MEGABYTE_NAME);
        }
        else
        {
          memoryForImage = image->imageDataSize / GiB;
          sufix = QSTRING(GIGABYTE_NAME);
        }
      }
    }

    ui->memoryRequest->setText(QString::number(memoryForImage, 'f', 3) + sufix);
    sizeChanged = true;

    return true;
  }

  void MainWindow::setUpGUI ()
  {
    //Create gui
    ui->setupUi(this);

    ui->imageViewer->setUpdatesEnabled(true);
    ui->imageViewer->setScrollArea(ui->imageScrollArea);
    ui->refreshTime->setValue(DEFAULT_REFRESH_TIME);

    connectSignals();

    refreshMemoryRequest(ui->imageWidth->value(), ui->imageHeight->value());

    int idealThreadCount = QThread::idealThreadCount();
    if (idealThreadCount < 1)
    {
      //If QThread can not detect number of cores
      idealThreadCount = 1;
    }
    ui->threadCounter->setValue(idealThreadCount);
    ui->threadCounter->setMaximum(idealThreadCount * 15);

    ui->terminateRender->setDisabled(true);

    ui->resultList->horizontalHeader()->setResizeMode(
        QHeaderView::ResizeToContents);
    QErrorMessage::qtHandler()->setMinimumSize(200, 100);

    setRefreshTime(ui->refreshTime->value());
  }

  void MainWindow::connectSignals ()
  {
    connect(ui->render, SIGNAL(pressed()), this, SLOT(runRenderer()));
    connect(ui->imageWidth, SIGNAL(valueChanged(int)), this,
            SLOT(changeWidth(int)));
    connect(ui->imageHeight, SIGNAL(valueChanged(int)), this,
            SLOT(changeHeight(int)));
    connect(ui->loadScene, SIGNAL(pressed()), this, SLOT(loadScene()));
    connect(ui->refreshTime, SIGNAL(valueChanged(int)), this,
            SLOT(setRefreshTime(int)));
    connect(ui->terminateRender, SIGNAL(clicked()), this,
            SLOT(terminateRender()));
    connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(saveImage()));

    //Camera movement
    connect(ui->forward, SIGNAL(clicked()), this, SLOT(moveCamera()));
    connect(ui->backward, SIGNAL(clicked()), this, SLOT(moveCamera()));
    connect(ui->left, SIGNAL(clicked()), this, SLOT(moveCamera()));
    connect(ui->right, SIGNAL(clicked()), this, SLOT(moveCamera()));
    connect(ui->up, SIGNAL(clicked()), this, SLOT(moveCamera()));
    connect(ui->down, SIGNAL(clicked()), this, SLOT(moveCamera()));

    //Camera rotation
    connect(ui->xAngleInc, SIGNAL(clicked()), this, SLOT(rotateCamera()));
    connect(ui->yAngleInc, SIGNAL(clicked()), this, SLOT(rotateCamera()));
    connect(ui->zAngleInc, SIGNAL(clicked()), this, SLOT(rotateCamera()));
    connect(ui->xAngleDec, SIGNAL(clicked()), this, SLOT(rotateCamera()));
    connect(ui->yAngleDec, SIGNAL(clicked()), this, SLOT(rotateCamera()));
    connect(ui->zAngleDec, SIGNAL(clicked()), this, SLOT(rotateCamera()));

    //Rendering signals
    connect(threadRunner.data(), SIGNAL(renderFinished()), this,
            SLOT(renderFinished()));
    connect(refreshTimer.data(), SIGNAL(timeout()), ui->imageViewer,
            SLOT(update()));
  }

  void MainWindow::calibrate ()
  {
    QRect screenGeometry(QApplication::desktop()->screenGeometry(this));
    QRect windowGeometry(this->geometry());
    QRect windowFrameGeometry(this->frameGeometry());
    QPoint topLeft;

    //Shrink window if it's bigger than screen area. Important for Windows users
    if (windowFrameGeometry.width() + WINDOW_MARGIN > screenGeometry.width())
    {
      int frameTotalWidth = this->frameSize().width() - this->size().width();
      windowGeometry.setWidth(
          screenGeometry.width() - frameTotalWidth - WINDOW_MARGIN);
    }
    if (windowFrameGeometry.height() + WINDOW_MARGIN > screenGeometry.height())
    {
      int frameTotalHeight = this->frameSize().height() - this->size().height();
      windowGeometry.setHeight(
          screenGeometry.height() - frameTotalHeight - WINDOW_MARGIN);
    }

    //Place window at the center of the screen
    this->setGeometry(windowGeometry);

    topLeft.setX(
        screenGeometry.center().x() - this->frameGeometry().width() / 2);
    topLeft.setY(
        screenGeometry.center().y() - this->frameGeometry().height() / 2);

    this->move(topLeft);

    //Fill image area
    //    ui->imageWidth->setValue(ui->imageScrollArea->size().width() - 2);
    //    ui->imageHeight->setValue(ui->imageScrollArea->size().height() - 2);
    ui->imageWidth->setValue(DEFAULT_IMAGE_WIDTH);
    ui->imageHeight->setValue(DEFAULT_IMAGE_HEIGHT);
    ui->tileSize->setValue(DEFAULT_TILE_SIZE);

    //Shrink result box
    QList <int> sizes = ui->verticalSplitter->sizes();
    int sizeDiff = 100;
    sizes [0] += sizeDiff;
    sizes [1] -= sizeDiff;
    ui->verticalSplitter->setSizes(sizes);
  }

  void MainWindow::activateButtons ()
  {
    ui->loadScene->setDisabled(false);
    ui->render->setDisabled(false);
    ui->terminateRender->setDisabled(true);

    ui->imageGroup->setDisabled(false);
    ui->cameraGroup->setDisabled(false);

    ui->actionSave->setDisabled(false);
  }

  void MainWindow::deactivateButtons (bool withSceneLoading)
  {
    ui->loadScene->setDisabled(withSceneLoading);
    ui->render->setDisabled(true);
    ui->terminateRender->setDisabled(false);

    ui->imageGroup->setDisabled(true);
    ui->cameraGroup->setDisabled(true);

    //Disable image saving
    ui->actionSave->setDisabled(true);
  }

  void MainWindow::updateImage () const
  {
    ui->imageViewer->update();
  }

  void MainWindow::renderFinished ()
  {
    qint64 elapsedTime = timeCounter->elapsed();
    refreshTimer->stop();

    updateImage();

    activateButtons();

    showRenderTime(elapsedTime);
    renderingInProgress = false;
  }

  void MainWindow::loadScene ()
  {
    bool result = false;
    QString fileName(DEFAULT_SCENE_FILE_NAME);

    if (!QFile::exists(fileName))
    {
      fileName = QFileDialog::getOpenFileName(this,
                                              QSTRING("Otwórz plik sceny"),
                                              QSTRING(DEFAULT_SCENE_FILE_NAME),
                                              tr("XML files (*.xml)"), 0,
                                              QFileDialog::DontUseNativeDialog);
    }

    try
    {
      result = scene->init(fileName, true);
      if (result)
      {
        scene->setImageWidth(image->imageWidth);
        scene->setImageHeight(image->imageHeight);
      }
      else
      {
        showWarning(QSTRING("Nie można otworzyć pliku sceny.<br>"
            "Sprawdź czy plik istnieje.") + fileName);
      }
    }
    catch (std::exception &ex)
    {
      showWarning(
          QSTRING("Błąd parsowania pliku sceny: ") + fileName
              + QSTRING("<br><br>") + QSTRING(ex.what()));
    }

    if (!result)
    {
      //Important because possibility of changing scene file content
      //and trying to reload it
      deactivateButtons(false);
    }
    else
    {
      scene->updateCamera();
      activateButtons();

      //Preserve camera settings
      if (ui->keepCameraSettings->isChecked())
      {
        updateCamera();
      }
      else
      {
        getCameraParameters();
        ui->keepCameraSettings->setChecked(true);
      }
    }
  }

  void MainWindow::showRenderTime (qint64 time)
  {
    ui->lastTime->setValue(time / static_cast <float>(KILO));

    if (!ui->liveCamera->isChecked())
    {
      //Add new row to result list
      if (addResult)
      {
        addTimeToResultList(ui->lastTime->value());
      }
    }
    else if (!ui->refreshTimeGroup->isChecked())
    {
      ui->refreshTime->setValue(time);
    }
    addResult = true;
  }

  void MainWindow::addTimeToResultList (double time)
  {
    int row = ui->resultList->rowCount();
    int col = 0;
    ui->resultList->setSortingEnabled(false);
    ui->resultList->insertRow(row);

    QTableWidgetItem **items =
        new QTableWidgetItem* [ui->resultList->columnCount()];

    for (int i = ui->resultList->columnCount() - 1; i >= 0; --i)
    {
      items [i] = new QTableWidgetItem;
      ui->resultList->setItem(row, i, items [i]);
    }

    items [col++ ]->setData(0, QVariant(row + 1));  //Id
    items [col++ ]->setData(0, QVariant(ui->threadCounter->value())); //Thread count
    items [col++ ]->setData(0, QVariant(ui->tileSize->value()));  //Tile size
    items [col++ ]->setData(0, QVariant(ui->refreshTime->value())); //Refresh time
    items [col++ ]->setData(0, QVariant(time)); //Render time
    items [col++ ]->setData(0, QVariant(image->imageWidth));  //Image width
    items [col++ ]->setData(0, QVariant(image->imageHeight)); //Image height
    delete [] items;

    ui->resultList->sortByColumn(0, Qt::AscendingOrder);
    ui->resultList->setSortingEnabled(true);
  }

  void MainWindow::moveCamera ()
  {
    QPushButton *button = dynamic_cast <QPushButton *>(sender());

    float speed = ui->movementSpeed->value();

    //[1] because [0] == & -> shortcut
    switch (button->text() [1].toAscii())
    {
      case Qt::Key_W:
        scene->getCamera()->move(Model::Z, speed);
        break;
      case Qt::Key_S:
        scene->getCamera()->move(Model::Z, -speed);
        break;
      case Qt::Key_A:
        scene->getCamera()->move(Model::X, -speed);
        break;
      case Qt::Key_D:
        scene->getCamera()->move(Model::X, speed);
        break;
      case Qt::Key_Q:
        scene->getCamera()->move(Model::Y, speed);
        break;
      case Qt::Key_E:
        scene->getCamera()->move(Model::Y, -speed);
        break;
    }

    getCameraParameters();

    if (!renderingInProgress && ui->liveCamera->isChecked())
    {
      runRenderer();
    }
  }

  void MainWindow::rotateCamera ()
  {
    QPushButton *button = dynamic_cast <QPushButton *>(sender());

    float speed = ui->rotateAngle->value();

    //[1] because [0] == & -> shortcut
    switch (button->text() [1].toAscii())
    {
      case 'P':
        scene->getCamera()->rotate(Model::X, speed);
        break;
      case ';':
        scene->getCamera()->rotate(Model::X, -speed);
        break;

      case '[':
        scene->getCamera()->rotate(Model::Y, speed);
        break;
      case '\'':
        scene->getCamera()->rotate(Model::Y, -speed);
        break;

      case ']':
        scene->getCamera()->rotate(Model::Z, speed);
        break;
      case '\\':
        scene->getCamera()->rotate(Model::Z, -speed);
        break;
    }

    getCameraParameters();

    if (!renderingInProgress && ui->liveCamera->isChecked())
    {
      runRenderer();
    }
  }

  void MainWindow::getCameraParameters ()
  {
    //Update data in GUI
    ui->xPos->setValue(scene->getCamera()->getPosition() [Model::X]);
    ui->yPos->setValue(scene->getCamera()->getPosition() [Model::Y]);
    ui->zPos->setValue(scene->getCamera()->getPosition() [Model::Z]);

    ui->xAngle->setValue(scene->getCamera()->getAngles() [Model::X]);
    ui->yAngle->setValue(scene->getCamera()->getAngles() [Model::Y]);
    ui->zAngle->setValue(scene->getCamera()->getAngles() [Model::Z]);

    ui->fov->setValue(scene->getCamera()->getFOV());
  }
}
