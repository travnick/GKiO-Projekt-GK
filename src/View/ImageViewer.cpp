/// @file Main/View/ImageViewer.cpp
/// @date 01-12-2011
/// @author Mikołaj Milej

#include <QPainter>
#include <QScrollBar>
#include <QScrollArea>
#include <QPaintEvent>
#include <QApplication>
#include "ImageViewer.h"

namespace View {

  ImageViewer::ImageViewer (QWidget *myParent)
      : QLabel(myParent){
    image = 0;
    imageOwner = false;
    scrollArea = 0;
  }

  ImageViewer::~ImageViewer (){
    if (imageOwner)
    {
      delete image;
    }
  }

  void ImageViewer::setScrollArea (QScrollArea *newScrollArea){
    if (newScrollArea != 0)
    {
      this->scrollArea = newScrollArea;
    }
  }

  void ImageViewer::setImage (QImage *newImage, bool giveOwnership){
    if (imageOwner)
    {
      delete this->image;
    }

    this->image = newImage;
    imageOwner = giveOwnership;

    if (this->image != 0)
    {
      resize(this->image->size());
      parentWidget()->resize(size());

      if (scrollArea != 0)
      {
        this->repaint(scrollArea->rect());
      }
    }
  }

  const QImage *ImageViewer::getImage (){
    return image;
  }

  void ImageViewer::paintEvent (QPaintEvent *){
    if (image != 0 && scrollArea != 0)
    {
      QPainter painter(this);
      QRect rectangle(scrollArea->rect());
      rectangle.moveLeft(scrollArea->horizontalScrollBar()->value());
      rectangle.moveTop(scrollArea->verticalScrollBar()->value());

      QPixmap imagePixmap = QPixmap::fromImage(image->copy(rectangle));

      painter.drawPixmap(rectangle, imagePixmap, imagePixmap.rect());
    }
  }

} /* namespace View */
