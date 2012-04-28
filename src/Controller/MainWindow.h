/// @file Main/Controller/MainWindow.h
/// @date 01-12-2011
/// @author Mikołaj Milej

#pragma once

#include <QSharedPointer>
#include <QScopedPointer>

#include <QMainWindow>

#include "Controller/GlobalDefines.h"

//Forward declarations -->
class QElapsedTimer;
class QThreadPool;

namespace Ui {
  class MainWindow;
}

namespace Model {
  struct RenderTileData;
  class Scene;
}
// <-- Forward declarations

namespace Controller {
  /**
   * Main window controller
   */
  class MainWindow: public QMainWindow {
    Q_OBJECT

    public:
      /**Initializes all necessary fields and creates GUI
       *
       * @param parent parent of window
       */
      MainWindow (QMainWindow *parent = 0);
      ~MainWindow ();

      /**Allocates memory for image.
       * It shows warning dialog if there isn't enough memory
       *
       */
      bool allocateMemoryForImage ();

      /**Adjusts window size if needed and moves it to center of the screen
       *
       */
      void calibrate ();

    public slots:

      /**Listens to load scene event
       *
       */
      void loadScene ();

      /**Listener for width changes
       *
       * @param newValue new value of width
       */
      void changeWidth (int newValue);

      /**Listener for height changes
       *
       * @param newValue new value of height
       */
      void changeHeight (int newValue);

      /**Listener for rendering finished signal
       *
       */
      void renderFinished ();

      /**Listener for "render" button
       * It starts new thread which starts proper amount render threads
       *
       */
      void runRenderer ();

      /**Saves image on disk
       *
       */
      void saveImage ();

      /**Listener for "terminate render"
       * It emits terminate signal to thread runner
       *
       */
      void terminateRender ();

      /**Listener for refresh time changes
       * It sets new refresh time immediately.
       * Refresh timer is responsible for image updating during rendering
       *
       * @param refreshTime new refrest time value
       */
      void setRefreshTime (int refreshTime);

      /**Listener for update image events
       * It updates image in the main window
       *
       */
      void updateImage () const;

      /**Shows warnings
       *
       * @param message message to show
       */
      void showWarning (QString message);

    private:
      /**Image data structure
       * It handles image data and image properties
       */
      QSharedPointer <Model::RenderTileData> image;

      /**Stores byte count per line in image
       *
       */
      quint64 imageBytesPerLine;

      /**Stores information is size was changed or not
       *
       */
      bool sizeChanged;

      /**Determines if controller is able to add rendering result or not.
       * Controller can't add result if rendering was terminated
       */
      bool addResult;

      /**Stores 3D scene
       *
       */
      QSharedPointer <Model::Scene> scene;

      /**Stores thread management object
       *
       */
      QScopedPointer <QThreadPool> threadPool;

      /**Stores timer for rendering time measuring
       *
       */
      QScopedPointer <QElapsedTimer> timeCounter;

      /**Stores timer for image updating events
       *
       */
      QScopedPointer <QTimer> timer;

      /**Stores GUI object
       *
       */
      QScopedPointer <Ui::MainWindow> ui;

      /**Activates buttons in GUI.
       * Sets state to rendering ready.
       * It enables:
       *  - library changing
       *  - rendering starting
       *  - scene loading
       *  - size changing
       * It disables:
       *  - rendering terminating
       */
      void activateButtons ();

      /**Deactivates buttons in GUI.
       * Sets state to rendering in progress.
       * It enables:
       *  - render terminating
       * It disables:
       *  - library changing // if withLibSelect == true
       *  - rendering starting
       *  - scene loading // if withSceneLoading == true
       *  - size changing
       *
       * @param withLibSelect disable library changing ?
       * @param withSceneLoading disable scene loading ?
       */
      void deactivateButtons (bool withLibSelect = true, bool withSceneLoading = true);

      /**Calculates memory requirement for image size width x height
       *
       * @param width width of image
       * @param height height of image
       * @return image could be created or not
       */
      bool refreshMemoryRequest (imageUnit width, imageUnit height);

      /**Sets up GUI
       *
       */
      void setUpGUI ();

      /**Connects signals and slots in GUI
       *
       */
      void connectSignals ();

      /**Calculates rendering time and shows it
       *
       * @param time elapsed time in nanoseconds
       */
      void showRenderTime (qint64 time);

      /**Adds rendering time and other properties to result list
       *
       * @param time time in seconds
       */
      void addTimeToResultList (double time);
  };
}

