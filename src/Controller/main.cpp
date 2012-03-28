/// @file Main/Controller/main.cpp
/// @date 01-12-2011
/// @author Mikołaj Milej

#include <QApplication>

#include "Controller/MainWindow.h"

#ifndef Q_OS_WIN32
int main (int argc, char *argv [])
#else
#include <windows.h>

    INT WINAPI WinMain (HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT)
#endif
    {
#ifdef Q_WS_WIN
  // Get argc and argv.
  int argc = __argc;
  char** argv = __argv;
#endif

  QApplication app(argc, argv);

  Controller::MainWindow mainWindow;

  mainWindow.show();

  //Place window at center of the screen, and resize it if needed
  mainWindow.calibrate();

  return app.exec();
}
