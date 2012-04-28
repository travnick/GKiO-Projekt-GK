/// @file Main/Controller/GlobalDefines.h
/// @date 01-12-2011
/// @author Mikołaj Milej

#pragma once
#include <stdint.h>

// Type defines
typedef unsigned char uchar;
typedef unsigned int uint;
typedef int imageUnit;
typedef uchar colorType;
//--------------------------------------------------

// Constants for image
const char BPP = sizeof(uchar) * 3; //Bytes per pixel: 1 byte for each color: R G B
const int COLOR_COUNT = 256;
const int COLOR_MAX_VALUE = 255;

const int64_t IMAGE_MAX_DATA_SIZE = 2147483648LL; //2^31 max of int in QImage
const colorType DEFAULT_RENDER_COLOR = 100;
//--------------------------------------------------

// Constants for units
const double KiB = 1024.0;
const double MiB = 1048576.0;
const double GiB = 1073741824.0;
const int KILO = 1000;
const int MEGA = 1000000;
const int GIGA = 1000000000;

#define BYTE_NAME "B"
#define KILOBYTE_NAME "KiB"
#define MEGABYTE_NAME "MiB"
#define GIGABYTE_NAME "GiB"
//--------------------------------------------------

// Other defines
#define TIME_PRECISION 6
#define DEFAULT_SCENE_FILE_NAME "scene.xml"

#define QSTRING(x) QString::fromUtf8(x)
//--------------------------------------------------
