/// @file View/ImageViewer.h
/// @date 01-12-2011
/// @author Mikołaj Milej

#pragma once

#include <QLabel>

//Forward declarations -->
class QImage;
class QScrollArea;
// <-- Forward declarations

namespace View {

  /**Reimplemented QLabel
   * It's customized to better image displaying.
   * It refresh only part of image visible in QScrollArea.
   *
   */
  class ImageViewer: public QLabel {
    public:
      ImageViewer (QWidget *parent = 0);

      virtual ~ImageViewer ();

      /**Sets scroll area in which ImageViewer is placed
       *
       * @param scrollArea scroll area
       */
      void setScrollArea (QScrollArea *scrollArea);

      /**Sets image to show
       *
       * @param image image to show
       * @param giveOwnership if true ImageViewer takes ownership of image
       */
      void setImage (QImage *image, bool giveOwnership = false);

      /**Returns holden image
       *
       * @return pointer to image
       */
      QImage *getImage ();

    protected:
      /**Repaints image.
       * It repaints only visible part of image because of memory overhead for large images
       *
       * @param event paint event
       */
      virtual void paintEvent (QPaintEvent *event);

    private:
      bool imageOwner;
      QImage *image;
      const QScrollArea *scrollArea;
  };

} /* namespace View */
