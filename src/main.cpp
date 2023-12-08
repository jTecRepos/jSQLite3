#include <jOS.h>


/********************* PARAM ************************/

#define WAIT_4_SERIAL

#define ENABLE_DEBUG

#define ENABLE_STORAGE_SD 1
#define JFS_USE_SDFAT // disable this to use the SD Library on ESP32
#define ENABLE_STORAGE_MODE_SPI
#define PIN_SPI_CS_SD 10

#define ENABLE_STORAGE_SPIFFS 0


#define DATABASE_DATA_FILENAME "data.db"

#define jFS_DBData jFS_SD
#define JSQL_FS_TAG_DB_DATA JSQL_FS_TAG_SD



#ifdef ENABLE_DEBUG
#define LogX Log
#define LogXD LogD
#else
#define LogX(...)
#define LogXD(...)
#endif



/**********************************************/


#ifndef SerialObj0
	#define SerialObj0 Serial
#endif

jSerialHard jSerial0(SerialObj0);

static void _Log(const String& msg)
{
	jSerial0.print("[");
	jSerial0.print(millis());
	jSerial0.print("] ");
	jSerial0.print(msg);
	if(!msg.endsWith("\n"))
		jSerial0.print("\n");
}

void Log(const String& msg)
{
	_Log(msg);
}
void Log(const char* format, ...) 
{
	va_list arg;
	va_start(arg, format);
	String s = s_vsnprintf(format, arg);
	va_end(arg);

	_Log(s);
}
void LogD(const String& msg)
{
	_Log(msg);
}
void LogD(const char* format, ...) 
{
	va_list arg;
	va_start(arg, format);
	String s = s_vsnprintf(format, arg);
	va_end(arg);

	_Log(s);
}

uint64_t millis_epoch()
{
	uint64_t epoch_time = 0;
	
	if(epoch_time == 0)
		epoch_time = (UNIXTIME_2018_START * FACTOR_s_2_ms) + millis();

	return epoch_time;
}


void wait_4_serial() {
	jSerial0.begin(SERIAL_BAUD_115200);
#ifdef WAIT_4_SERIAL
	while (!jSerial0.available()) 
	{
		jSerial0.print(".");
		delay(1000);
	}
	while(jSerial0.available())
		jSerial0.read();
	jSerial0.println();
#endif // WAIT_4_SERIAL
}

/**********************************************/




#pragma region FILESYSTEMS 

#include <jFS.h>


#if ENABLE_STORAGE_SD==1

#ifndef PIN_SPI_CS_SD
	#define PIN_SPI_CS_SD -1
#endif // PIN_SPI_CS_SD


#ifdef JFS_USE_SDFAT
#ifdef PLATFORM_TEENSY

// Teensy SDIO SdFat
jFileSystemSdFatTeensySDIOClass jFS_SdFat;

#else // Regular SPI SdFat

#if SPI_DRIVER_SELECT == 0	|| SPI_DRIVER_SELECT == 1
jFileSystemSdFatSPIClass jFS_SdFat(PIN_SPI_CS_SD);
#elif SPI_DRIVER_SELECT == 3
#include "Modules/SERCOM_Module.h"
jSPI SPI_SD(&SPI0, PIN_SPI_CS_SD);
jFileSystemSdFatSPIClass jFS_SdFat(PIN_SPI_CS_SD, &SPI_SD);
#else
#error "no driver select"
#endif // 

#endif

#define jFS_SD jFS_SdFat
#define jFileSD jFileSdFat

#else


#if defined(ENABLE_STORAGE_MODE_SPI)
jFileSystemESPClass_SPI jFS_ESP(PIN_SPI_CS_SD);

#elif defined(ENABLE_STORAGE_MODE_MMC_1BIT)
#ifdef VARIANTE_ESP32S3
jFileSystemESPClass_MMC jFS_ESP(PIN_SD_CLK, PIN_SD_CMD, PIN_SD_DAT0);
#else
jFileSystemESPClass_MMC jFS_ESP(true);
#endif
#elif defined(ENABLE_STORAGE_MODE_MMC_4BIT)
#ifdef VARIANTE_ESP32S3
jFileSystemESPClass_MMC jFS_ESP(PIN_SD_CLK, PIN_SD_CMD, PIN_SD_DAT0, PIN_SD_DAT1, PIN_SD_DAT2, PIN_SD_DAT3);
#else
jFileSystemESPClass_MMC jFS_ESP(false);
#endif
#else
#error "no sd mode defined"
#endif 

#define jFS_SD jFS_ESP
#define jFileSD jFileESP

#endif // JFS_USE_SDFAT

#endif // ENABLE_STORAGE_SD


#if ENABLE_STORAGE_SPIFFS==1

#ifdef PLATFORM_ESP32
jFileSystemESPClass_SPIFFS jFS_SPIFFS;
#endif // PLATFORM_ESP32

#ifdef PLATFORM_RP2040
jFileSystemRP2040Class_LittleFS jFS_SPIFFS;
#endif // PLATFORM_RP2040

#ifdef PLATFORM_TEENSY
jFileSystemTeensyClass_LittleFS jFS_SPIFFS;
#endif // PLATFORM_TEENSY

#endif // ENABLE_STORAGE_SPIFFS


#pragma endregion


#pragma region DATABASE

#include <jSQLite3.h>


static sqlite3 *db_data;
static int rc;
static bool db_is_open = false;


// callback for db_exec
static int callback(void *rqst_code, int col_count, char **col_vals, char **col_names) 
{
	
	String txt = format("sql.res.%s: ", (const char*)rqst_code);
	for (int i = 0; i < col_count; i++)
		txt += format(i + 1 == col_count ? "%s=%s": "%s=%s, ", col_names[i], col_vals[i] ? col_vals[i] : "NULL");
	
	Log(txt);

	return 0;
}



static int db_open(sqlite3 **db, const char *filename, const char* fs_tag) {
	//int rc = sqlite3_open(filename, db);
	int rc = sqlite3_open_v2(filename, db, (SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE), fs_tag);
	if (rc != SQLITE_OK) {
		Log("sql: err=failed to open database, sql_err='%s'", sqlite3_errmsg(*db));
		//LogX("sql.open: rc=%d, err=%s", rc, sqlite3_errmsg(db_data));
		return rc;
	} else {
		LogX("sql.msg: opened database successfully");
	}
	return rc;
}


static char *zErrMsg = 0;
static bool db_exec(sqlite3 *db, const char *sql, const char *rqst_code) {
	LogXD("sql.exec: queue=\"%s\"", sql);
	
	TRACK_DT_MILLIS_START0

	int rc = sqlite3_exec(db, sql, callback, (void*)rqst_code, &zErrMsg);
	if (rc != SQLITE_OK) {
		LogX("sql.exec: rc=%d, err=%s", rc, zErrMsg);
		sqlite3_free(zErrMsg);
	} else {
		LogXD("sql.exec: rc=%d, result=ok");
	}
	
	TRACK_DT_MILLIS_END0(dt_exec);

	LogX("sql.exec: dt=%d, rc=%d", dt_exec, rc);

	return rc == SQLITE_OK;
}


#pragma endregion



void setup() {
	
	Log("____________ START ________________");

	wait_4_serial();
	
	Log("____________ INIT FILSYSTEMS ________________");

#if ENABLE_STORAGE_SD==1
	
	if (!jFS_SD.begin())
	{
		LogD("storage.state: sd mount failed");
		while(true) delay(100);
	}
	jFS_SD.listDir("/");
	
#endif // ENABLE_STORAGE_SD

#if ENABLE_STORAGE_SPIFFS==1
	
	if(!jFS_SPIFFS.begin())
	{
		LogD("jFS_SPIFFS: open failed");
		return;
	}
	
#endif // ENABLE_STORAGE_SPIFFS

	
	Log("____________ INIT DATABASE ________________");


#if ENABLE_STORAGE_SPIFFS==1
	sqlite3_hal_register_fs(JSQL_FS_TAG_SPIFFS, &jFS_SPIFFS);
#endif // ENABLE_STORAGE_SPIFFS

#if ENABLE_STORAGE_SD==1
	sqlite3_hal_register_fs(JSQL_FS_TAG_SD, &jFS_SD);
#endif // ENABLE_STORAGE_SD

	
	sqlite3_initialize();
	
	
	Log("____________ OPEN DATABASE ________________");



	if(!jFS_SD.isMounted())
	{
		Log("sql.init.err: fs is not mounted");
		return;
	}
	
	
	if (db_open(&db_data, DATABASE_DATA_FILENAME, JSQL_FS_TAG_DB_DATA) != SQLITE_OK)
	{
		Log("sql.init.err: failed to open database");
		db_is_open = false;
		return;
	}
	db_is_open = true;

	
	Log("____________ TEST CODE ________________");


	// list all tables
	db_exec(db_data, "SELECT name FROM sqlite_master WHERE type = 'table';", "list_table");



	Log("____________ DONE ________________");

}



void loop() {



}

