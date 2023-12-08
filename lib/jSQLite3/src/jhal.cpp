/* From: https://chromium.googlesource.com/chromium/src.git/+/4.1.249.1050/third_party/sqlite/src/os_symbian.cc
 * https://github.com/spsoft/spmemvfs/tree/master/spmemvfs
 * http://www.sqlite.org/src/doc/trunk/src/test_ESP32vfs.c
 * http://www.sqlite.org/src/doc/trunk/src/test_vfstrace.c
 * http://www.sqlite.org/src/doc/trunk/src/test_onefile.c
 * http://www.sqlite.org/src/doc/trunk/src/test_vfs.c
 * https://github.com/nodemcu/nodemcu-firmware/blob/master/app/sqlite3/esp8266.c
 **/


// jOS
#include <jCommon.h>
#include <jSystem.h>
#include <jFS.h>
#include <jHelp.h>
#include <jWatchdog.h>

#include <jFile_SecBuffered.h>

// SQL Header
#include <sqlite3.h>


#include <jSQLite3_config.h>
#include <config_ext.h>

#include "shox96_0_2.h"


/****************************************************************************/




#if JSQL_DEBUG==1
#define LogX Log
#define LogXD LogD
#else
#define LogX(...)
#define LogXD(...)
#endif

#include <string.h>
#include <stdbool.h>


/*********************************** global object *****************************************/

static jFileSystemClass* jFS_SPIFFS = nullptr;
static jFileSystemClass* jFS_SD = nullptr;


void sqlite3_hal_register_fs(const char* fs_tag, jFileSystemClass* jfs)
{
	if(strcmp(fs_tag, JSQL_FS_TAG_SPIFFS) == 0)
		jFS_SPIFFS = jfs;
	if(strcmp(fs_tag, JSQL_FS_TAG_SD) == 0)
		jFS_SD = jfs;	
}

static String getFS(const char* sql_fname, const char* fs_tag, jFileSystemClass*& _jFS)
{
	String fname = String(sql_fname);
	_jFS = nullptr;

	if(strcmp(fs_tag, JSQL_FS_TAG_SPIFFS) == 0 && jFS_SPIFFS != nullptr) {
		//LogX("sqlite.getFS: fname='%s'", fname.c_str());
		_jFS = jFS_SPIFFS;	
		return fname;
	}
	
	if(strcmp(fs_tag, JSQL_FS_TAG_SD) == 0 && jFS_SD != nullptr) {
		_jFS = jFS_SD;
		return fname;
	}

	LogX("sqlite.err: no fs found for '%s'", fs_tag);

	return "";
}

/************************************** PARAMETER ****************************************/


/*
** Size of the write buffer used by journal files in bytes.
*/
#ifndef SQLITE_VFS_BUFFER_SIZE
	#define SQLITE_VFS_BUFFER_SIZE 8192
#endif

/*
** The maximum pathname length supported by this VFS.
*/
#define MAXPATHNAME 100



/***************************** FILE STRUCT DEFINITION ***********************************/


#define SECTOR_BUFFER_COUNT 25
#define SECTOR_BUFFER_SIZE SQLITE_DEFAULT_PAGE_SIZE
static int sec_buff_counter = 0;
struct Sector_Buffer {
	uint8_t *data = nullptr; // the data of the sector
	sqlite3_int64 address = -1; // Offset in file
	int id = -1; // unique id of this buffer - counts up - older buffer have lower indexs
	bool changed = false; // sector content changed flag
};

/*
** When using this VFS, the sqlite3_file* handles that SQLite uses are
** actually pointers to instances of type jHAL_File.
*/
//typedef struct jHAL_File jHAL_File;
struct jHAL_File {
	sqlite3_file base;              /* Base class. Must be first. */

	jFileSystemClass* jfs; 			// pointer to filesystem
	jFile* fp;                   	// pointer to file
	
	int lock;						// current lock level of file
	int id;							// unique identifer for each file

#if 0
	// write buffer
	char *buffer;                  /* Pointer to malloc'd buffer */
	int buffer_index;                 /* Valid bytes of data in zBuffer */
	sqlite3_int64 buffer_offset;      /* Offset in file of buffer[0] */
#endif 

	// file sector buffer
	Sector_Buffer sec_buffer[SECTOR_BUFFER_COUNT];
};


static void print_sec_buff_status(jHAL_File* p) {
	for(uint8_t sec_buff_i = 0; sec_buff_i < SECTOR_BUFFER_COUNT; sec_buff_i++) {
		LogX("sqlite.id%d.sec_buff.%d: addr=%lld, id=%d, data=%d, changed=%d", p->id, 
			sec_buff_i, 
			p->sec_buffer[sec_buff_i].address, 
			p->sec_buffer[sec_buff_i].id, 
			p->sec_buffer[sec_buff_i].data,
			p->sec_buffer[sec_buff_i].changed
		);

	}
}




/***************************** START OF FILE FUNC IMPLEMENTATIONS ***********************************/


/*
** Write directly to the file passed as the first argument. Even if the
** file has a write-buffer (jHAL_File.buffer), ignore it.
** not used by SQLite directly
*/
static int jhal_write_direct(
	jHAL_File *pFile,				/* File handle */
	const void *zBuf,				/* Buffer containing data to write */
	int iAmt,						/* Size of data to write in bytes */
	sqlite_int64 iOfst				/* File offset to write to */
){
	jHAL_File *p = (jHAL_File*)pFile;
	LogX("sqlite.id%d: state=direct_write, len=%d, offset=%lld", p->id, iAmt, iOfst);

	//LogX("sqlite.id%d.write: state=before, size=%d, posi=%d", p->id, p->fp->size(), p->fp->position());

	if(iOfst > p->fp->size()) {
		//LogXD(">>> WRITE SEEK OUTSIDE OF FILE");
		LogX("sqlite.id%d.write: state=seek pad", p->id);
		
		// write zeros to file until target offset is reached
		uint8_t buff[128] = { 0 };
		memset(buff, 0, sizeof(buff));
		while(p->fp->size() < iOfst)
			p->fp->write(buff, MIN(128, iOfst - p->fp->size()));

		//LogX("sqlite.id%d.write: state=after seek pad, size=%d, posi=%d", p->id, p->fp->size(), p->fp->position());
	}
	
	if(!p->fp->seek(iOfst)) {	
		LogX("sqlite.id%d.direct_write: err=seek failed", p->id);
		return SQLITE_IOERR_WRITE;
	}

	uint32_t nWrite = p->fp->write((const uint8_t*) zBuf, iAmt);
	if( nWrite != iAmt ) {
		LogX("sqlite.id%d.direct_write: err=returned write length incorrect, len_in=%d, len_res=%d", p->id, iAmt, nWrite);
		return SQLITE_IOERR_WRITE;
	}

	// optional flush after write
	//p->fp->flush();

	//LogX("sqlite.id%d.write: state=after, size=%d, posi=%d", p->id, p->fp->size(), p->fp->position());

	return SQLITE_OK;
}


/*
** Flush the contents of the jHAL_File.buffer buffer to disk. This is a
** no-op if this particular file does not have a buffer (i.e. it is not
** a journal file) or if the buffer is currently empty.
*/
static int jhal_buffer_flush(jHAL_File *p) {
	//jHAL_File *p = (jHAL_File*)pFile;
	LogX("sqlite.id%d: state=flush", p->id);

	int rc = SQLITE_OK;
	
#if 0
	if( p->buffer_index ) {
		rc = jhal_write_direct(p, p->buffer, p->buffer_index, p->buffer_offset);
		p->buffer_index = 0;
	}
#endif 

	// flush buffer
	for(uint8_t sec_buff_i = 0; sec_buff_i < SECTOR_BUFFER_COUNT; sec_buff_i++) {
		// if sector content changed
		if(p->sec_buffer[sec_buff_i].changed)
		{
			// write out to file
			int rc = jhal_write_direct(p, 
				p->sec_buffer[sec_buff_i].data,
				SECTOR_BUFFER_SIZE,
				p->sec_buffer[sec_buff_i].address
			);

			// reset changed flag
			p->sec_buffer[sec_buff_i].changed = false;
		}
	}

	//p->fp->flush();

	if(rc != SQLITE_OK)
		LogX("sqlite.id%d.flush: err=flush failed, rc=%d", p->id, rc);

	return rc;
}


/*
** Close a file.
*/
static int jhal_file_close(sqlite3_file *pFile)
{
	jHAL_File *p = (jHAL_File*)pFile;
	LogX("sqlite.id%d: state=close", p->id);
	
	int rc;
	
	// flush
	rc = jhal_buffer_flush(p);
	if(rc != SQLITE_OK)
		LogX("sqlite.id%d.close: err=flush failed, rc=%d", p->id, rc);
	
#if 0
	// free buffer
	sqlite3_free(p->buffer);
#endif 


	// free sec buffer
	for(uint8_t sec_buff_i = 0; sec_buff_i < SECTOR_BUFFER_COUNT; sec_buff_i++) {
		if(p->sec_buffer[sec_buff_i].data != nullptr)
		{
			LogXD("sqlite.id%d.close.sec_buff: i=%d", p->id, sec_buff_i);
			sqlite3_free(p->sec_buffer[sec_buff_i].data);
		}
	}

	// close file
	if(!p->fp->close())
	{
		LogX("sqlite.id%d.close: err=close failed", p->id);
		return SQLITE_IOERR_CLOSE;
	}
	
	return rc;
}


/*
** Read data from a file.
*/
static int jhal_file_read(
	sqlite3_file *pFile, 
	void *zBuf, 			/* Buffer to write data to */
	int iAmt, 				/* Size of data to read in bytes */
	sqlite_int64 iOfst		/* File offset to read from */
){
	jHAL_File *p = (jHAL_File*)pFile;
	LogX("sqlite.id%d: state=read, len=%d, offset=%lld", p->id, iAmt, iOfst);

#if 0
	if( p->buffer != nullptr // check if buffer available
		&& p->buffer_index != 0 // check buffer is not empty
		
		// check if read intersects with buffer content
		// !(offset0        >= offset1 + len1 || offset1 >=         offset0 + len0) 
		&& !(p->buffer_offset >= iOfst + iAmt || iOfst >= p->buffer_offset + p->buffer_index)
	) {
		LogX("sqlite.id%d.read: state=intersect flush, buff_offset=%d, buff_len=%d", p->id, p->buffer_offset, p->buffer_index);
		//LogX("sqlite.id%d.read: state=before flush, size=%d, posi=%d", p->id, p->fp->size(), p->fp->position());

		// TODO read from buffer instead of flushing and then reading from file

		/* Flush any data in the write buffer to disk in case this operation
		** is trying to read data the file-region currently cached in the buffer.
		** It would be possible to detect this case and possibly save an 
		** unnecessary write here, but in practice SQLite will rarely read from
		** a journal file when there is data cached in the write-buffer.
		*/
		int rc = jhal_buffer_flush(p);
		if( rc != SQLITE_OK ) {
			LogX("sqlite.id%d.read: err=flush failed", p->id);
			return rc;
		}

		//LogX("sqlite.id%d.read: state=after flush, size=%d, posi=%d", p->id, p->fp->size(), p->fp->position());

	}
#endif 
	

	// find sector buffer
	for(uint8_t sec_buff_i = 0; sec_buff_i < SECTOR_BUFFER_COUNT; sec_buff_i++) {
		
		// check if buffer is set and is correct address
		if(p->sec_buffer[sec_buff_i].address == iOfst && iAmt == SECTOR_BUFFER_SIZE) {
			LogX("sqlite.id%d.read: mode=read from sec_buffer, sec_buff_i=%d", p->id, sec_buff_i);
			memcpy(zBuf, p->sec_buffer[sec_buff_i].data, iAmt);
			return SQLITE_OK;
		}

		
		// if buffer intersects
		if(rangesIntersect(iOfst, iAmt, p->sec_buffer[sec_buff_i].address, SECTOR_BUFFER_SIZE)) {
			LogX("sqlite.id%d.read: state=intersect flush, sec_buff_i=%d, addr=%lld", p->id, sec_buff_i, p->sec_buffer[sec_buff_i].address);
			int rc = jhal_write_direct(p, 
				p->sec_buffer[sec_buff_i].data,
				SECTOR_BUFFER_SIZE,
				p->sec_buffer[sec_buff_i].address
			);
		}
	
	}


	// seek to position
	if(!p->fp->seek(iOfst)) {
		LogX("sqlite.id%d.read: err=seek failed, offset=%lld, fsize=%d, fpos=%d", p->id, iOfst, p->fp->size(), p->fp->position());
		//return SQLITE_IOERR_READ;
		return SQLITE_IOERR_SHORT_READ;
	}
	
	// read values
	uint32_t rlen = p->fp->read((uint8_t*) zBuf, iAmt);


	if( rlen != iAmt ) { // check for read error
		LogX("sqlite.id%d.read: err=length mismach, len_rqst=%d, len_res=%d", p->id, iAmt, rlen);
		return (rlen >= 0) ? SQLITE_IOERR_SHORT_READ : SQLITE_IOERR_READ;
	}

	
	// update sector buffer
	if(iAmt == SECTOR_BUFFER_SIZE) { // only for 512
		// lock for buffer to write to (empty or oldest)
		int8_t sec_buff_i = -1;
		for(uint8_t i = 0; i < SECTOR_BUFFER_COUNT; i++) {
			// if empty buffer
			if(p->sec_buffer[i].address < 0)
			{
				if(p->sec_buffer[i].data == nullptr) // malloc if not already
					p->sec_buffer[i].data = (uint8_t *)sqlite3_malloc(SECTOR_BUFFER_SIZE);
				

				if(p->sec_buffer[i].data != nullptr) {
					sec_buff_i = i;
				}

				break;
			}

			// if buffer has lower id (is older)
			if(sec_buff_i < 0 || p->sec_buffer[sec_buff_i].id > p->sec_buffer[i].id)
			{
				sec_buff_i = i;
			}
		}

		// write to selected buffer
		if(sec_buff_i >= 0) {
			LogX("sqlite.id%d.read: mode=set sec_buffer, sec_buff_i=%d, addr=%lld", p->id, sec_buff_i, iOfst);
			
			// writeout if previous sector content was changed
			if(p->sec_buffer[sec_buff_i].changed)
			{
				int rc = jhal_write_direct(p, 
					p->sec_buffer[sec_buff_i].data,
					SECTOR_BUFFER_SIZE,
					p->sec_buffer[sec_buff_i].address
				);
			}

			memcpy(p->sec_buffer[sec_buff_i].data, zBuf, iAmt);
			p->sec_buffer[sec_buff_i].address = iOfst;
			p->sec_buffer[sec_buff_i].id = sec_buff_counter++;
			p->sec_buffer[sec_buff_i].changed = false;
		}

		//print_sec_buff_status(p);
	}


	return SQLITE_OK;
}


/*
Write data 
	to a crash-file if buffer is available
	otherwise write to file directly
*/
static int jhal_file_write(
	sqlite3_file *pFile, // file pointer
	const void *zBuf, // the buffer to the data that is to be written
	int iAmt, // length of data to write
	sqlite_int64 iOfst // file offset of data to write
) {
	jHAL_File *p = (jHAL_File*)pFile;
	LogX("sqlite.id%d: state=write, len=%d, offset=%lld", p->id, iAmt, iOfst);	

	// check sector buffers
	for(uint8_t sec_buff_i = 0; sec_buff_i < SECTOR_BUFFER_COUNT; sec_buff_i++) {
		
		// if buffer matches 
		if(p->sec_buffer[sec_buff_i].address == iOfst && SECTOR_BUFFER_SIZE == iAmt) {
			LogXD("sqlite.id%d.write: step=update sec buffer, sec_buff_i=%d", p->id, sec_buff_i);
			
			//memcpy(p->sec_buffer[sec_buff_i].data, zBuf, SECTOR_BUFFER_SIZE);
			
			p->sec_buffer[sec_buff_i].changed = memcpycmp(p->sec_buffer[sec_buff_i].data, zBuf, SECTOR_BUFFER_SIZE);
			return SQLITE_OK;
			/*if(!p->sec_buffer[sec_buff_i].changed) {
				LogXD("sqlite.id%d.write: step=skip write as not different", p->id);
				return SQLITE_OK;
			}*/
			
		}
		// intersects with a buffer -> clear the buffer
		else if(rangesIntersect(iOfst, iAmt, p->sec_buffer[sec_buff_i].address, SECTOR_BUFFER_SIZE)) {
			LogXD("sqlite.id%d.write: step=clear sec buffer, sec_buff_i=%d, addr=%lld", p->id, sec_buff_i, p->sec_buffer[sec_buff_i].address);
			p->sec_buffer[sec_buff_i].address = -1;
			p->sec_buffer[sec_buff_i].id = -1;
		}
		
	}
	

#if 0
	// check if file has buffer
	if( p->buffer ) {
		LogXD("sqlite.id%d.write: mode=to buffer", p->id);
		
		// tracks progress through new data while writing to buffer or file
		char *buff_new = (char *)zBuf;      // pointer to next new data - Pointer to remaining data to write
		int len_new = iAmt;                 // remaining length of new data -  Number of bytes remain in buff_new
		sqlite3_int64 offset_new = iOfst;	// offset of new data - File offset to write to for remaining data

		while( len_new > 0 ) {

			/* If the buffer is full, or if this data is not being written directly
			** following the data already buffered, flush the buffer. Flushing
			** the buffer is a no-op if it is empty.  
			*/
			if( 
				p->buffer_index==SQLITE_VFS_BUFFER_SIZE // buffer full
				|| p->buffer_offset + p->buffer_index != offset_new // buffer end does not match start of new data
			) 
			{
				LogXD("sqlite.id%d.write: mode=buffer writeout", p->id);
				int rc = jhal_buffer_flush(p);
				if( rc != SQLITE_OK ) {
					LogX("sqlite.id%d.write.err: buffer writeout failed", p->id);
					return rc;
				}
			}

			// TODO consider removing this 
			//assert( p->buffer_index==0 || p->buffer_offset+p->buffer_index==offset_new );
			if(!( 
				p->buffer_index == 0 // buffer empty
				|| p->buffer_offset + p->buffer_index == offset_new // buffer end matches new data start
			))
			{
				LogXD("sqlite.id%d.write.err: buffer check failed", p->id);
				return SQLITE_ERROR;
			}
			
			// set new buffer offset to start of new data (needed when buffer is empty and not yet set - otherwise will have no effect)
			p->buffer_offset = offset_new - p->buffer_index;
			
			
			// - - Copy as much data as possible into the buffer - - 

			// Number of bytes to copy into buffer
			int clen = MIN(len_new, SQLITE_VFS_BUFFER_SIZE - p->buffer_index); 
			
			memcpy(&p->buffer[p->buffer_index], buff_new, clen);
			p->buffer_index += clen;

			len_new -= clen;
			offset_new += clen;
			buff_new += clen;
		}
	} 
	else 
#endif 
	{
		LogXD("sqlite.id%d.write: mode=direct", p->id);
		return jhal_write_direct(p, zBuf, iAmt, iOfst);
	}

	return SQLITE_OK;
}


/*
** Truncate a file. This is a no-op for this VFS (see header comments at
** the top of the file).
*/
static int jhal_file_truncate(sqlite3_file *pFile, sqlite_int64 size) 
{
	jHAL_File *p = (jHAL_File*)pFile;
	LogX("sqlite.id%d: state=truncate, size=%lld", p->id, size);
	
	
	if(p->jfs->truncate(p->fp->name(), (uint32_t) size))
	{
		return SQLITE_OK;
	}
	else
	{
		LogX("sqlite.id%d.truncate: err=truncate failed", p->id);
		return SQLITE_IOERR_TRUNCATE;
	}
	
}


/*
** Sync the contents of the file to the persistent media.
*/
static int jhal_file_sync(sqlite3_file *pFile, int flags)
{	
	jHAL_File *p = (jHAL_File*)pFile;
	LogX("sqlite.id%d: state=sync, flags=x%08X", p->id, flags);
	

	int rc = jhal_buffer_flush(p);
	if( rc != SQLITE_OK ) {
		LogX("sqlite.id%d.sync: err=flush failed", p->id);
		return rc;
	}

	// flush file
	p->fp->flush();

	jWatchdog_delay(1);
	
	//LogX("sqlite.id%d.sync, ok=%d", rc == 0);
	
	return SQLITE_OK; // ignore fsync return value // (rc==0 ? SQLITE_OK : SQLITE_IOERR_FSYNC);
}


/*
** Write the size of the file in bytes to *pSize.
*/
static int jhal_file_get_size(sqlite3_file *pFile, sqlite_int64 *pSize) 
{
	jHAL_File *p = (jHAL_File*)pFile;
	LogX("sqlite.id%d: state=filesize", p->id);

	
	int rc;                         /* Return code from fstat() call */

	/* Flush the contents of the buffer to disk. As with the flush in the
	** jhal_file_read() method, it would be possible to avoid this and save a write
	** here and there. But in practice this comes up so infrequently it is
	** not worth the trouble.
	*/
	rc = jhal_buffer_flush(p);
	if( rc != SQLITE_OK ) {
		LogX("sqlite.id%d.fsize: err=flush failed", p->id);
		return rc;
	}

	*pSize = p->fp->size();
	
	LogX("sqlite.id%d.filesize: size=%lld", p->id, *pSize);
	
	return SQLITE_OK;
}

/*
** Locking functions. The xLock() and xUnlock() methods are both no-ops.
** The xCheckReservedLock() always indicates that no other process holds
** a reserved lock on the database file. This ensures that if a hot-journal
** file is found in the file-system it is rolled back.



eLock: (see sqlite3.h)

** CAPI3REF: File Locking Levels
**
** SQLite uses one of these integer values as the second
** argument to calls it makes to the xLock() and xUnlock() methods
** of an [sqlite3_io_methods] object.

#define SQLITE_LOCK_NONE          0
#define SQLITE_LOCK_SHARED        1
#define SQLITE_LOCK_RESERVED      2
#define SQLITE_LOCK_PENDING       3
#define SQLITE_LOCK_EXCLUSIVE     4


eLock: (see sqlite3.c)


** The following values may be passed as the second argument to
** sqlite3OsLock(). The various locks exhibit the following semantics:
**
** SHARED:    Any number of processes may hold a SHARED lock simultaneously.
** RESERVED:  A single process may hold a RESERVED lock on a file at
**            any time. Other processes may hold and obtain new SHARED locks.
** PENDING:   A single process may hold a PENDING lock on a file at
**            any one time. Existing SHARED locks may persist, but no new
**            SHARED locks may be obtained by other processes.
** EXCLUSIVE: An EXCLUSIVE lock precludes all other locks.
**
** PENDING_LOCK may not be passed directly to sqlite3OsLock(). Instead, a
** process that requests an EXCLUSIVE lock may actually obtain a PENDING
** lock. This can be upgraded to an EXCLUSIVE lock by a subsequent call to
** sqlite3OsLock().

#define NO_LOCK         0
#define SHARED_LOCK     1 // is a type of lock that allows multiple transactions to read a database file simultaneously, but prevents any transaction from writing to the file while the lock is held.
#define RESERVED_LOCK   2
#define PENDING_LOCK    3
#define EXCLUSIVE_LOCK  4

*/

static int jhal_lock(sqlite3_file *pFile, int eLock)
{
	jHAL_File *p = (jHAL_File*)pFile;
	LogX("sqlite.id%d: state=lock, elock=%d", p->id, eLock);

	p->lock = eLock;

	return SQLITE_OK;
}

static int jhal_unlock(sqlite3_file *pFile, int eLock)
{
	jHAL_File *p = (jHAL_File*)pFile;
	LogX("sqlite.id%d: state=unlock, elock=%d", p->id, eLock);
	
	p->lock = eLock;
	
	return SQLITE_OK;
}

static int jhal_lock_check_reserved(sqlite3_file *pFile, int *pResOut) 
{
	jHAL_File *p = (jHAL_File*)pFile;
	LogX("sqlite.id%d: state=check_lock", p->id);
	
	*pResOut = p->lock == SQLITE_LOCK_RESERVED;
	
	return SQLITE_OK;
}

/*
** No xFileControl() verbs are implemented by this VFS.
*/
static int jhal_file_control(sqlite3_file *pFile, int op, void *pArg)
{
	jHAL_File *p = (jHAL_File*)pFile;
	LogX("sqlite.id%d: state=file_ctrl, op=%d", p->id, op);
	
	// see sqlite3.h - SQLITE_FCNTL_BUSYHANDLER etc
	// see https://www.sqlite.org/c3ref/c_fcntl_begin_atomic_write.html

	// #define SQLITE_FCNTL_BUSYHANDLER	15 -> register a busy callback 
	// #define SQLITE_FCNTL_PDB         30 -> 
	return SQLITE_OK;
}

/*
** The xSectorSize() and xDeviceCharacteristics() methods. These two
** may return special values allowing SQLite to optimize file-system 
** access to some extent. But it is also safe to simply return 0.
*/
static int jhal_get_sector_size(sqlite3_file *pFile)
{
	jHAL_File *p = (jHAL_File*)pFile;
	LogX("sqlite.id%d: state=sector_size", p->id);
	
	if(p->jfs == jFS_SPIFFS)
		return 4096; // spiffs https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/storage/spiffs.html
	else if(p->jfs == jFS_SD)
		return 512; // sd card

	return 0;
}

static int jhal_get_device_characteristics(sqlite3_file *pFile) 
{
	jHAL_File *p = (jHAL_File*)pFile;
	LogX("sqlite.id%d: state=device_characteristics", p->id);
	return 0;
}

/***************************** END OF FILE FUNC IMPLEMENTATIONS ***********************************/



static const sqlite3_io_methods jhal_io_methods = {
	1,							/* iVersion */
	jhal_file_close,					/* xClose */
	jhal_file_read,					/* xRead */
	jhal_file_write,					/* xWrite */
	jhal_file_truncate,				/* xTruncate */
	jhal_file_sync,					/* xSync */
	jhal_file_get_size,				/* xFileSize */
	jhal_lock,					/* xLock */
	jhal_unlock,				/* xUnlock */
	jhal_lock_check_reserved,		/* xCheckReservedLock */
	jhal_file_control,			/* xFileControl */
	jhal_get_sector_size,			/* xSectorSize */
	jhal_get_device_characteristics	/* xDeviceCharacteristics */
};


/*************************** START OF FILESYSTEM FUNC IMPLEMENTATIONS *************************/


#ifndef F_OK
	#define F_OK 0
#endif
#ifndef R_OK
	#define R_OK 4
#endif
#ifndef W_OK
	#define W_OK 2
#endif

/*
** Query the file-system to see if the named file exists, is readable or
** is both readable and writable.
*/
static int jhal_fs_access(
	sqlite3_vfs *pVfs, 	/* The VFS containing this xAccess method */
	const char *zPath, 	/* Path of the file to examine */
	int flags, 			/* What do we want to learn about the zPath file? */
	int *pResOut 		/* Write result boolean here */
){
	LogX("sqlite.%s: state=access, path='%s', flags=x%08X", pVfs->zName, zPath, flags);

	jFileSystemClass* jFS = nullptr;
	String fname = getFS(zPath, pVfs->zName, jFS);
	if(fname.length() == 0 || jFS == nullptr)
		return SQLITE_IOERR;

	int rc;                         /* access() return code */
	int eAccess = F_OK;             /* Second argument to access() */
	

	assert( flags==SQLITE_ACCESS_EXISTS       	/* access(zPath, F_OK) */
			 || flags==SQLITE_ACCESS_READ       /* access(zPath, R_OK) */
			 || flags==SQLITE_ACCESS_READWRITE  /* access(zPath, R_OK|W_OK) */
	);

	if( flags==SQLITE_ACCESS_READWRITE ) eAccess = R_OK|W_OK;
	if( flags==SQLITE_ACCESS_READ )      eAccess = R_OK;
	
	*pResOut = jFS->exists(fname);
	LogX("sqlite.%s.access: res=%d", pVfs->zName, *pResOut);

	return SQLITE_OK;
}



/*
** Open a file handle.

** ^SQLite guarantees that the zFilename parameter to xOpen
** is either a NULL pointer or string obtained
** from xFullPathname() with an optional suffix added.
** ^If a suffix is added to the zFilename parameter, it will
** consist of a single "-" character followed by no more than
** 11 alphanumeric and/or "-" characters.
** ^SQLite further guarantees that
** the string will be valid and unchanged until xClose() is
** called. Because of the previous sentence,
** the [sqlite3_file] can safely store a pointer to the
** filename if it needs to remember the filename for some reason.
** If the zFilename parameter to xOpen is a NULL pointer then xOpen
** must invent its own temporary name for the file.  ^Whenever the 
** xFilename parameter is NULL it will also be the case that the
** flags parameter will include [SQLITE_OPEN_DELETEONCLOSE].
...
*/
static int jhal_fs_open(
	sqlite3_vfs *pVfs,			/* VFS */
	const char *zPath,			/* File to open, or 0 for a temp file */
	sqlite3_file *pFile,		/* Pointer to jHAL_File struct to populate */
	int flags,					/* Input SQLITE_OPEN_XXX flags */
	int *pOutFlags				/* Output SQLITE_OPEN_XXX flags (or NULL) */
){
	static int open_counter = -1; // remeber to give every open database a unique id
	open_counter++;

	jHAL_File *p = (jHAL_File*)pFile; // convert to extended pointer
	
	// register id
	p->id = open_counter;

	LogX("sqlite.id%d: state=open, fname='%s', flags=x%08X", p->id, zPath == 0 ? "" : zPath, flags);
	
	LogX("sqlite.id%d.open.flags: main_journal=%d", p->id,
		flags & SQLITE_OPEN_MAIN_JOURNAL
	);

	
	

	
	if( zPath==0 ) {
		LogX("sqlite.id%d.open.warn: no filename given", p->id);
		zPath = ".sqlite3.tmp";		
	}

	jFileSystemClass* jFS = nullptr;
	String fname = getFS(zPath, pVfs->zName, jFS);
	if(fname.length() == 0 || jFS == nullptr)
		return SQLITE_IOERR;

	// link filesystem to file struct
	p->jfs = jFS;


	// check if open requested and determine file mode
	jFILE_MODE file_mode = jFILE_MODE::FILE_FLAG_R;
	if( flags & SQLITE_OPEN_CREATE || flags & SQLITE_OPEN_READWRITE || flags & SQLITE_OPEN_MAIN_JOURNAL ) {
		
		bool exists = jFS->exists(fname);
		
		// if not exists: create new
		if (!exists) {
			file_mode = jFILE_MODE::FILE_FLAG_RW_OVERRIDE;

			LogX("sqlite.id%d.open: mode=override", p->id);
		} 
		else 
		{ // if exists: open 
			
			file_mode = jFILE_MODE::FILE_FLAG_RW;

			LogX("sqlite.id%d.open: mode=rw", p->id);
		}
	}
	
	// actually open file	
	LogX("sqlite.id%d.open: filename='%s', fs=%s", p->id, fname.c_str(), jFS->get_tag());
	p->fp = jFS->open(fname, file_mode);
	//p->fp = new jFile_SecBuffered(jFS->open(fname, file_mode));
	
	// check if failed to open
	if( p->fp == nullptr || !p->fp->isOpen()) {
		LogX("sqlite.id%d.open: err=open failed, path='%s'", p->id, fname.c_str());
		return SQLITE_CANTOPEN;
	}

#if 0
	// buffer
	char *aBuf = nullptr;
	if( flags & SQLITE_OPEN_MAIN_JOURNAL ) {
		aBuf = (char *)sqlite3_malloc(SQLITE_VFS_BUFFER_SIZE);
		if( aBuf == nullptr ) {
			LogX("sqlite.id%d.open.err: journal malloc failed", p->id);
			return SQLITE_NOMEM;
		}
	}
	// register buffer
	p->buffer = aBuf;
#endif 

	// clear sec buffer
	for(uint8_t sec_buff_i = 0; sec_buff_i < SECTOR_BUFFER_COUNT; sec_buff_i++) {
		p->sec_buffer[sec_buff_i].data = nullptr;
		p->sec_buffer[sec_buff_i].address = -1;
		p->sec_buffer[sec_buff_i].id = -1;
		p->sec_buffer[sec_buff_i].changed = false;
	}



	// remember flags used to open file
	if( pOutFlags != nullptr )
		*pOutFlags = flags;
	
	// register base function 
	p->base.pMethods = &jhal_io_methods;
	
	
	LogXD("sqlite.id%d.open: res=ok", p->id);
	return SQLITE_OK;
}

/*
** Delete the file identified by argument zPath. If the dirSync parameter
** is non-zero, then ensure the file-system modification to delete the
** file has been synced to disk before returning.
*/
static int jhal_fs_delete(
	sqlite3_vfs *pVfs, 	/* VFS containing this as the xDelete method */
	const char *zPath, 	/* Name of file to be deleted */
	int dirSync			/* If true, fsync() directory after deleting file */
)
{
	LogX("sqlite.%s: state=delete, path=%s, dirSync=%d", pVfs->zName, zPath, dirSync);

	jFileSystemClass* jFS = nullptr;
	String fname = getFS(zPath, pVfs->zName, jFS);
	if(fname.length() == 0 || jFS == nullptr)
		return SQLITE_IOERR;

	bool res = jFS->remove(fname);

	LogX("sqlite.%s.delete, res=%d", pVfs->zName, res);

	return (res ? SQLITE_OK : SQLITE_IOERR_DELETE);
}

/*
** Argument zPath points to a nul-terminated string containing a file path.
** If zPath is an absolute path, then it is copied as is into the output 
** buffer. Otherwise, if it is a relative path, then the equivalent full
** path is written to the output buffer.
**
** This function assumes that paths are UNIX style. Specifically, that:
**
**   1. Path components are separated by a '/'. and 
**   2. Full paths begin with a '/' character.
*/
static int jhal_fs_get_full_path(
	sqlite3_vfs *pVfs,			/* VFS */
	const char *zPath,			/* Input path (possibly a relative path) */
	int nPathOut,				/* Size of output buffer in bytes */
	char *zPathOut				/* Pointer to output buffer */
) {
	LogX("sqlite: state=fullpathname, vfs_name=%s, path='%s', len=%d", pVfs->zName, zPath, nPathOut);

	strncpy( zPathOut, zPath, nPathOut );
	zPathOut[nPathOut-1] = '\0';

	//nPathOut = sprintf(zPathOut, "/%s/%s", pVfs->zName, zPath);

	LogX("sqlite.fullpathname: path_out='%s'", zPathOut);

	return SQLITE_OK;
}

/*
** The following four VFS methods:
**
**   xDlOpen
**   xDlError
**   xDlSym
**   xDlClose
**
** are supposed to implement the functionality needed by SQLite to load
** extensions compiled as shared objects. This simple VFS does not support
** this functionality, so the following functions are no-ops.
*/
static void *jhal_dl_open(sqlite3_vfs *pVfs, const char *zPath) {
	return 0;
}
static void jhal_dl_error(sqlite3_vfs *pVfs, int nByte, char *zErrMsg) {
	sqlite3_snprintf(nByte, zErrMsg, "Loadable extensions are not supported");
	zErrMsg[nByte-1] = '\0';
}
static void (*jhal_dl_sym(sqlite3_vfs *pVfs, void *pH, const char *z))(void) {
	return 0;
}
static void jhal_dl_close(sqlite3_vfs *pVfs, void *pHandle) {
	return;
}

/*
** Parameter zByte points to a buffer nByte bytes in size. Populate this
** buffer with pseudo-random data.
*/
static int jhal_random(sqlite3_vfs *pVfs, int nByte, char *zByte) 
{
	LogX("sqlite: state=random, len=%d", nByte);

	for(int i = 0; i < nByte; i++)
		zByte[i] = (uint8_t) get_random_int(0, 0xFF);

	return SQLITE_OK;
}

/*
** Sleep for at least nMicro microseconds. Return the (approximate) number 
** of microseconds slept for.
*/
static int jhal_sleep(sqlite3_vfs *pVfs, int nMicro) 
{
	LogXD("sqlite: state=sleep, dur_us=%d", nMicro);

	//delay(nMicro / 1000);
	//delayMicroseconds(nMicro % 1000)
	
	delayMicroseconds(nMicro);

	return nMicro;
}

/*
** Set *pTime to the current UTC time expressed as a Julian day. Return
** SQLITE_OK if successful, or an error code otherwise.
**
**   http://en.wikipedia.org/wiki/Julian_day
**
** This implementation is not very good. The current time is rounded to
** an integer number of seconds. Also, assuming time_t is a signed 32-bit 
** value, it will stop working some time in the year 2038 AD (the so-called
** "year 2038" problem that afflicts systems that store time this way). 
*/
static int jhal_get_current_time(sqlite3_vfs *pVfs, double *pTime) 
{
	LogX("sqlite: state=current_time");
	
	// TODO check this 
	//time_t t = time(0);
	uint32_t t = getUNIXTimestamp();
	*pTime = t/86400.0 + 2440587.5; 
	
	return SQLITE_OK;
}


/***************************** END OF VFS FUNC DEFINITIONS **************************/

static sqlite3_vfs jhal_vfs_spiffs = {
	1,							// iVersion
	sizeof(jHAL_File),			// szOsFile
	MAXPATHNAME,				// mxPathname
	0,							// pNext
	JSQL_FS_TAG_SPIFFS,			// zName
	0,							// pAppData
	jhal_fs_open,				// xOpen
	jhal_fs_delete,				// xDelete
	jhal_fs_access,				// xAccess
	jhal_fs_get_full_path,		// xFullPathname
	jhal_dl_open,				// xDlOpen
	jhal_dl_error,				// xDlError
	jhal_dl_sym,				// xDlSym
	jhal_dl_close,				// xDlClose
	jhal_random,				// xRandomness
	jhal_sleep,					// xSleep
	jhal_get_current_time,		// xCurrentTime
};


static sqlite3_vfs jhal_vfs_sd = {
	1,							// iVersion
	sizeof(jHAL_File),			// szOsFile
	MAXPATHNAME,				// mxPathname
	0,							// pNext
	JSQL_FS_TAG_SD,				// zName
	0,							// pAppData
	jhal_fs_open,				// xOpen
	jhal_fs_delete,				// xDelete
	jhal_fs_access,				// xAccess
	jhal_fs_get_full_path,		// xFullPathname
	jhal_dl_open,				// xDlOpen
	jhal_dl_error,				// xDlError
	jhal_dl_sym,				// xDlSym
	jhal_dl_close,				// xDlClose
	jhal_random,				// xRandomness
	jhal_sleep,					// xSleep
	jhal_get_current_time,		// xCurrentTime
};

/****************************** extensions **********************************/

// From https://stackoverflow.com/questions/19758270/read-varint-from-linux-sockets#19760246
// Encode an unsigned 64-bit varint. Returns number of encoded bytes.
// 'buffer' must have room for up to 10 bytes.
static int encode_unsigned_varint(uint8_t *buffer, uint64_t value) {
	int encoded = 0;
	do {
		uint8_t next_byte = value & 0x7F;
		value >>= 7;
		if (value)
			next_byte |= 0x80;
		buffer[encoded++] = next_byte;
	} while (value);
	return encoded;
}

static uint64_t decode_unsigned_varint(const uint8_t *data, int &decoded_bytes) {
	int i = 0;
	uint64_t decoded_value = 0;
	int shift_amount = 0;
	do {
		decoded_value |= (uint64_t)(data[i] & 0x7F) << shift_amount;
		shift_amount += 7;
	} while ((data[i++] & 0x80) != 0);
	decoded_bytes = i;
	return decoded_value;
}


static void shox96_0_2c(sqlite3_context *context, int argc, sqlite3_value **argv) {
	int nIn, nOut;
	long int nOut2;
	const unsigned char *inBuf;
	unsigned char *outBuf;
	unsigned char vInt[9];
	int vIntLen;

	assert( argc==1 );
	nIn = sqlite3_value_bytes(argv[0]);
	inBuf = (unsigned char *) sqlite3_value_blob(argv[0]);
	nOut = 13 + nIn + (nIn+999)/1000;
	vIntLen = encode_unsigned_varint(vInt, (uint64_t) nIn);

	outBuf = (unsigned char *) malloc( nOut+vIntLen );
	memcpy(outBuf, vInt, vIntLen);
	nOut2 = shox96_0_2_compress((const char *) inBuf, nIn, (char *) &outBuf[vIntLen], NULL);
	sqlite3_result_blob(context, outBuf, nOut2+vIntLen, free);
}

static void shox96_0_2d(sqlite3_context *context, int argc, sqlite3_value **argv) {
	unsigned int nIn, nOut, rc;
	const unsigned char *inBuf;
	unsigned char *outBuf;
	long int nOut2;
	uint64_t inBufLen64;
	int vIntLen;

	assert( argc==1 );

	if (sqlite3_value_type(argv[0]) != SQLITE_BLOB)
		return;

	nIn = sqlite3_value_bytes(argv[0]);
	if (nIn < 2){
		return;
	}
	inBuf = (unsigned char *) sqlite3_value_blob(argv[0]);
	inBufLen64 = decode_unsigned_varint(inBuf, vIntLen);
	nOut = (unsigned int) inBufLen64;
	outBuf = (unsigned char *) malloc( nOut );
	//nOut2 = (long int)nOut;
	nOut2 = shox96_0_2_decompress((const char *) (inBuf + vIntLen), nIn - vIntLen, (char *) outBuf, NULL);
	//if( rc!=Z_OK ){
	//  free(outBuf);
	//}else{
		sqlite3_result_blob(context, outBuf, nOut2, free);
	//}
} 

static void unishox1c(sqlite3_context *context, int argc, sqlite3_value **argv) {
	int nIn, nOut;
	long int nOut2;
	const unsigned char *inBuf;
	unsigned char *outBuf;
	unsigned char vInt[9];
	int vIntLen;

	assert( argc==1 );
	nIn = sqlite3_value_bytes(argv[0]);
	inBuf = (unsigned char *) sqlite3_value_blob(argv[0]);
	nOut = 13 + nIn + (nIn+999)/1000;
	vIntLen = encode_unsigned_varint(vInt, (uint64_t) nIn);

	outBuf = (unsigned char *) malloc( nOut+vIntLen );
	memcpy(outBuf, vInt, vIntLen);
	nOut2 = shox96_0_2_compress((const char *) inBuf, nIn, (char *) &outBuf[vIntLen], NULL);
	sqlite3_result_blob(context, outBuf, nOut2+vIntLen, free);
}

static void unishox1d(sqlite3_context *context, int argc, sqlite3_value **argv) {
	unsigned int nIn, nOut, rc;
	const unsigned char *inBuf;
	unsigned char *outBuf;
	long int nOut2;
	uint64_t inBufLen64;
	int vIntLen;

	assert( argc==1 );

	if (sqlite3_value_type(argv[0]) != SQLITE_BLOB)
		return;

	nIn = sqlite3_value_bytes(argv[0]);
	if (nIn < 2){
		return;
	}
	inBuf = (unsigned char *) sqlite3_value_blob(argv[0]);
	inBufLen64 = decode_unsigned_varint(inBuf, vIntLen);
	nOut = (unsigned int) inBufLen64;
	outBuf = (unsigned char *) malloc( nOut );
	//nOut2 = (long int)nOut;
	nOut2 = shox96_0_2_decompress((const char *) (inBuf + vIntLen), nIn - vIntLen, (char *) outBuf, NULL);
	//if( rc!=Z_OK ){
	//  free(outBuf);
	//}else{
		sqlite3_result_blob(context, outBuf, nOut2, free);
	//}
} 

static int registerFunctions(sqlite3 *db, const char **pzErrMsg, const struct sqlite3_api_routines *pThunk) {
	sqlite3_create_function(db, "shox96_0_2c", 1, SQLITE_UTF8 | SQLITE_DETERMINISTIC, 0, shox96_0_2c, 0, 0);
	sqlite3_create_function(db, "shox96_0_2d", 1, SQLITE_UTF8 | SQLITE_DETERMINISTIC, 0, shox96_0_2d, 0, 0);
	sqlite3_create_function(db, "unishox1c", 1, SQLITE_UTF8 | SQLITE_DETERMINISTIC, 0, unishox1c, 0, 0);
	sqlite3_create_function(db, "unishox1d", 1, SQLITE_UTF8 | SQLITE_DETERMINISTIC, 0, unishox1d, 0, 0);
	return SQLITE_OK;
}

/***********************************************************************************************/

static void errorLogCallback(void *pArg, int iErrCode, const char *zMsg) 
{
	LogX("sqlite.err: code=%d, msg='%s'", iErrCode, zMsg);
}


// called in sqlite3_initialize
int sqlite3_os_init(void)
{
	LogX("sqlite: state=init_os");

	// register error log
	sqlite3_config(SQLITE_CONFIG_LOG, errorLogCallback, NULL);

	//TODO use this to get a progress callback for long querys to allow watchdog reset
	//sqlite3_progress_handler()
	
	// register filesystems
	sqlite3_vfs_register(&jhal_vfs_spiffs, 1);
	sqlite3_vfs_register(&jhal_vfs_sd, 0);
	
	// register extensions
	sqlite3_auto_extension((void (*)())registerFunctions);
	

	return SQLITE_OK;
}

int sqlite3_os_end(void) {
	return SQLITE_OK;
}
