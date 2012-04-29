/// @file Main/Controller/MainWindow.cpp
/// @date 01-12-2011
/// @author Mikołaj Milej

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

#include "View/ui_MainWindow.h"
#include "Controller/MainWindow.h"
#include "Controller/GlobalDefines.h"
#include "Controller/ThreadRunner.h"
#include "Model/RenderTileData.h"
#include "Model/Scene.h"

#define TIME_BEFORE_REMOVE_THREADS 60000 //[ms]
#define TIME_BEFORE_REMOVE_THREADS_ON_EXIT 1000 //[ms]
#define DEFAULT_REFRESH_TIME 1000 //[ms]
#define WINDOW_MARGIN 0 //
#define IMAGE_SAVE_FORMAT "png"

#define DEFAULT_IMAGE_WIDTH 800
#define DEFAULT_IMAGE_HEIGHT 560
#define DEFAULT_TILE_SIZE 32

namespace Controller {

  void MainWindow::showWarning (QString message){
    QErrorMessage *errorMessage = new QErrorMessage(this);
    //No memory leaks hear
    errorMessage->setAttribute(Qt::WA_DeleteOnClose);
    errorMessage->setWindowModality(Qt::ApplicationModal);

    errorMessage->showMessage(message);
  }

  MainWindow::MainWindow (QMainWindow *myParent)
      : QMainWindow(myParent){

    image = QSharedPointer <Model::RenderTileData>(new Model::RenderTileData);
    scene = QSharedPointer <Model::Scene>(new Model::Scene);
    ui.reset(new Ui::MainWindow);
    threadPool.reset(new QThreadPool);
    timer.reset(new QTimer);

    threadPool->setExpiryTimeout(TIME_BEFORE_REMOVE_THREADS);
    image->imageData = 0;

    sizeChanged = false;
    addResult = true;

    setUpGUI();
    loadScene();
  }

  MainWindow::~MainWindow (){
    //Send terminate signal to thread runner and wait for end
    ui->terminateRender->click();

    threadPool->waitForDone(TIME_BEFORE_REMOVE_THREADS_ON_EXIT);
  }

  void MainWindow::runRenderer (){

    if (sizeChanged)
    {
      if (allocateMemoryForImage() == false)
      {
        return;
      }

      scene->setImageWidth(image->imageWidth);
      scene->setImageHeight(image->imageHeight);
      scene->updateCamera();

      ui->imageViewer->setImage(
          new QImage(image->imageData, image->imageWidth, image->imageHeight, imageBytesPerLine,
                     QImage::Format_RGB888),
          true);
    }

    deactivateButtons();

    image->width = ui->tileSize->value();
    image->height = image->width;

    ThreadRunner *threadRunner = new ThreadRunner;
    threadRunner->setParams(image, scene, ui->threadCounter->value(),
                            ui->randomRender->isChecked());

    connect(threadRunner, SIGNAL(renderFinished()), this, SLOT(renderFinished()));
    connect(ui->terminateRender, SIGNAL(clicked()), threadRunner, SLOT(terminate()));
    connect(timer.data(), SIGNAL(timeout()), ui->imageViewer, SLOT(update()));

    timeCounter.reset(new QElapsedTimer);

    timer->start();
    timeCounter->start();

    threadPool->start(threadRunner);
  }

  void MainWindow::terminateRender (){
    addResult = false;
  }

  void MainWindow::setRefreshTime (int refreshTime){
    timer->setInterval(refreshTime);
  }

  void MainWindow::saveImage (){
    const QImage *imageToSave = ui->imageViewer->getImage();
    if (imageToSave != 0)
    {
      QString fileName = QFileDialog::getSaveFileName(this, tr("Zapisz obraz"), "RenderedImage.png",
                                                      tr("Image Files (*.png)"), 0,
                                                      QFileDialog::DontUseNativeDialog);

      QImageWriter imageWriter(fileName);

      imageWriter.setFormat(IMAGE_SAVE_FORMAT);
      imageWriter.setCompression(9);
      imageWriter.write( *imageToSave);
    }
  }

  bool MainWindow::allocateMemoryForImage (){
    void *mem = realloc(image->imageData, image->imageDataSize * sizeof(colorType));

    if (mem == 0)
    {
      showWarning(QSTRING("Nie można przydzielić wymaganej ilości pamięci.<br>"
          "Proszę zmniejszyć obrazek"));

      return false;
    }
    else
    {
      image->imageData = static_cast <colorType*>(mem);
      sizeChanged = false;
    }
    return true;
  }

  void MainWindow::changeWidth (int newWidth){
    imageUnit matchImageHeight = image->imageHeight;

    while (matchImageHeight >= 1 && refreshMemoryRequest(newWidth, matchImageHeight) == false)
    {
      --matchImageHeight;
    }

    ui->imageWidth->setValue(image->imageWidth);
    ui->imageHeight->setValue(image->imageHeight);
  }

  void MainWindow::changeHeight (int newHeight){
    imageUnit matchImageWidth = image->imageWidth;

    while (matchImageWidth >= 1 && refreshMemoryRequest(matchImageWidth, newHeight) == false)
    {
      --matchImageWidth;
    }

    ui->imageWidth->setValue(image->imageWidth);
    ui->imageHeight->setValue(image->imageHeight);
  }

  bool MainWindow::refreshMemoryRequest (imageUnit imageWidth, imageUnit imageHeight){
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

  void MainWindow::setUpGUI (){
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

    ui->resultList->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
    QErrorMessage::qtHandler()->setMinimumSize(200, 100);

    setRefreshTime(ui->refreshTime->value());
  }

  void MainWindow::connectSignals (){
    connect(ui->render, SIGNAL(pressed()), this, SLOT(runRenderer()));
    connect(ui->imageWidth, SIGNAL(valueChanged(int)), this, SLOT(changeWidth(int)));
    connect(ui->imageHeight, SIGNAL(valueChanged(int)), this, SLOT(changeHeight(int)));
    connect(ui->loadScene, SIGNAL(pressed()), this, SLOT(loadScene()));
    connect(ui->refreshTime, SIGNAL(valueChanged(int)), this, SLOT(setRefreshTime(int)));
    connect(ui->terminateRender, SIGNAL(clicked()), this, SLOT(terminateRender()));
    connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(saveImage()));
  }

  void MainWindow::calibrate (){
    QRect screenGeometry(QApplication::desktop()->screenGeometry(this));
    QRect windowGeometry(this->geometry());
    QRect windowFrameGeometry(this->frameGeometry());
    QPoint topLeft;

    if (windowFrameGeometry.width() + WINDOW_MARGIN > screenGeometry.width())
    {
      int frameTotalWidth = this->frameSize().width() - this->size().width();
      windowGeometry.setWidth(screenGeometry.width() - frameTotalWidth - WINDOW_MARGIN);
    }

    if (windowFrameGeometry.height() + WINDOW_MARGIN > screenGeometry.height())
    {
      int frameTotalHeight = this->frameSize().height() - this->size().height();
      windowGeometry.setHeight(screenGeometry.height() - frameTotalHeight - WINDOW_MARGIN);
    }

    this->setGeometry(windowGeometry);

    topLeft.setX(screenGeometry.center().x() - this->frameGeometry().width() / 2);
    topLeft.setY(screenGeometry.center().y() - this->frameGeometry().height() / 2);

    this->move(topLeft);

    //Fill image area
    //    ui->imageWidth->setValue(ui->imageScrollArea->size().width() - 2);
    //    ui->imageHeight->setValue(ui->imageScrollArea->size().height() - 2);
    ui->imageWidth->setValue(DEFAULT_IMAGE_WIDTH);
    ui->imageHeight->setValue(DEFAULT_IMAGE_HEIGHT);
    ui->tileSize->setValue(DEFAULT_TILE_SIZE);
  }

  void MainWindow::activateButtons (){
    ui->loadScene->setDisabled(false);
    ui->render->setDisabled(false);
    ui->terminateRender->setDisabled(true);
    ui->imageWidth->setDisabled(false);
    ui->imageHeight->setDisabled(false);

    ui->actionSave->setDisabled(false);
  }

  void MainWindow::deactivateButtons (bool withLibSelect, bool withSceneLoading){
    ui->loadScene->setDisabled(withSceneLoading);
    ui->render->setDisabled(true);
    ui->terminateRender->setDisabled(false | ( !withLibSelect));
    ui->imageWidth->setDisabled(true);
    ui->imageHeight->setDisabled(true);

    //Disable image saving
    ui->actionSave->setDisabled(true);
  }

  void MainWindow::updateImage () const{
    ui->imageViewer->update();
  }

  void MainWindow::renderFinished (){
    qint64 elapsedTime = timeCounter->nsecsElapsed();
    timeCounter.reset();
    timer->stop();

    updateImage();

    activateButtons();

    showRenderTime(elapsedTime);
  }

  void MainWindow::loadScene (){
    bool result = false;
    QString fileName(DEFAULT_SCENE_FILE_NAME);

    if ( !QFile::exists(fileName))
    {
      fileName = QFileDialog::getOpenFileName(this, QSTRING("Otwórz plik sceny"),
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
      showWarning(QSTRING("Błąd parsowania pliku sceny<br>"
          "Sprawdź poprawność pliku: ") + fileName + QSTRING("<br><br>") + QSTRING(ex.what()));
    }

    if ( !result)
    {
      //Important because possibility of changing scene file content and trying to reload it
      deactivateButtons(false, false);
    }
    else
    {
      scene->updateCamera();
      activateButtons();
    }
  }

  void MainWindow::showRenderTime (qint64 time){
    QString timeString(QString::number(time / GIGA));

    qint64 miliSeconds = qRound(static_cast <double>(time % GIGA) / MEGA);
    QString miliSecondsString(QString::number(miliSeconds));

    //Fill with 0 to match 1 second
    while (miliSecondsString.length() < 3)
    {
      miliSecondsString.prepend(QString::number(0));
    }

    timeString.append(QLocale::system().decimalPoint());
    timeString.append(miliSecondsString);

    ui->timePlace->setText(timeString + QSTRING(" s"));

    //Add new row to result list
    if (addResult)
    {
      addTimeToResultList(timeString.toDouble());
    }
    addResult = true;
  }

  void MainWindow::addTimeToResultList (double time){
    int row = ui->resultList->rowCount();
    int col = 0;
    ui->resultList->setSortingEnabled(false);
    ui->resultList->insertRow(row);

    QTableWidgetItem **items = new QTableWidgetItem* [ui->resultList->columnCount()];

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
}
