/// @file Main/Controller/GlobalDefines.h
/// @date 01-12-2011
/// @author Mikołaj Milej

#pragma once

//Needed to OS detection
#include <QtGlobal>

// Defines for image
#define BPP 3 //Bytes per pixel: 1 byte for each color: R G B
#define COLOR_COUNT 256
#define COLOR_MAX_VALUE 255

#define IMAGE_MAX_DATA_SIZE 2147483648 //2^31 max of int in QImage
//--------------------------------------------------

// Defines for units
#define KiB 1024.0
#define MiB 1048576.0
#define GiB 1073741824.0
#define KILO 1000
#define MEGA 1000000
#define GIGA 1000000000

#define BYTE_NAME "B"
#define KILOBYTE_NAME "KiB"
#define MEGABYTE_NAME "MiB"
#define GIGABYTE_NAME "GiB"
//--------------------------------------------------

// Type defines
typedef unsigned char uchar;
typedef unsigned int uint;
typedef int imageUnit;
typedef uchar colorType;

#define QSTRING(x) QString::fromUtf8(x)
//--------------------------------------------------

// Other defines
#define TIME_PRECISION 6
#define SCENE_FILE_NAME "scene.xml"

//--------------------------------------------------
