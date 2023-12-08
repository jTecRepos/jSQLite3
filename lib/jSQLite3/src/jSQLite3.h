#pragma once 

#include <jCommon.h>
#include "sqlite3.h"

#include "jSQLite3_config.h"

/* TODO create jSQLDatabase Class here 
	- holds a database
	- gives generall functions like
 		- list all tables
 		- building of sql queues
		- etc

*/

#include <jFS.h>


extern void sqlite3_hal_register_fs(const char* fs_tag, jFileSystemClass* jfs);