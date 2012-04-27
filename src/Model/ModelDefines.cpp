/// @file ModelDefines.cpp
/// @date 25-04-2012
/// @author Mikołaj Milej

#include <Model/ModelDefines.h>

QString stringifytolowercase (const char* string){
  return QString::fromStdString(string).toLower();
}
