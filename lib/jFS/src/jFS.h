#pragma once

#include "jFS_config.h"

#include "jFile.h"
#include "jFileSystem.h"


#ifdef PLATFORM_ESP32
#include "jFileESP.h"
#include "jFileSystemESP_MMC.h"
#include "jFileSystemESP_SPI.h"
#include "jFileSystemESP_SPIFFS.h"
#endif // PLATFORM_ESP32


#ifdef PLATFORM_RP2040
#include "jFileRP2040.h"
#include "jFileSystemRP2040_LittleFS.h"
#endif // PLATFORM_RP2040


#ifdef PLATFORM_TEENSY
#include "jFileTeensy.h"
#include "jFileSystemTeensy_LittleFS.h"
#endif // PLATFORM_TEENSY


#if JFS_ENABLE_SDFAT == 1
#include "jFileSdFat.h"
#include "jFileSystemSdFat.h"
#endif // JFS_ENABLE_SDFAT

