#define BUILD_sqlite -DNDEBUG
#define SQLITE_OMIT_LOAD_EXTENSION           1
#define SQLITE_DQS                           0 // Disables the double-quoted string literal misfeature.
#define SQLITE_OS_OTHER                      1
#define SQLITE_NO_SYNC                       1 // When SQLITE_NO_SYNC is set, SQLite skips the fsync() step during transaction commit. This can make transaction commits faster, but at the risk of database corruption in the event of a crash or power loss, because changes that have been made but not yet flushed to disk might be lost.
#define SQLITE_TEMP_STORE                    1
#define SQLITE_DISABLE_LFS                   1
#define SQLITE_DISABLE_DIRSYNC               1
#define SQLITE_SECURE_DELETE                 0 // When SQLITE_SECURE_DELETE is enabled, SQLite will overwrite deleted content with zeroes. This makes it much harder (though not necessarily impossible with advanced forensic techniques) to recover the deleted data. However, this comes at a performance cost, since overwriting data is slower than simply marking it as available for reuse.
#define SQLITE_DEFAULT_LOOKASIDE        512,64
#define YYSTACKDEPTH                        20
#define SQLITE_SMALL_STACK                   1 // test disabling this as it only changes one line in with WhereClause
#define SQLITE_SORTER_PMASZ                  4
#define SQLITE_DEFAULT_CACHE_SIZE           -1
#define SQLITE_DEFAULT_MEMSTATUS             0 //  Disables the sqlite3_status() interfaces that track memory usage, making the sqlite3_malloc() routines run much faster and thus making the entire library faster.
#define SQLITE_DEFAULT_MMAP_SIZE             0
#define SQLITE_CORE                          1
#define SQLITE_SYSTEM_MALLOC                 1
#define SQLITE_THREADSAFE                    0 // Omits all of the mutex and thread-safety logic in SQLite, making it run faster and reducing the size of the library. The downside is that SQLite can never be used by more than a single thread at a time.
#define SQLITE_MUTEX_APPDEF                  1
#define SQLITE_OMIT_WAL                      1 // omit the Write-Ahead Logging (WAL) feature from SQLite.
#define SQLITE_DISABLE_FTS3_UNICODE          1
#define SQLITE_DISABLE_FTS4_DEFERRED         1
#define SQLITE_LIKE_DOESNT_MATCH_BLOBS       1 // Causes the LIKE and GLOB operators to always return FALSE if either operand is a BLOB, simplifying the implementation of the LIKE optimization and speeding up queries that use it.
#define SQLITE_DEFAULT_FOREIGN_KEYS          0
#define SQLITE_DEFAULT_LOCKING_MODE          1
#define SQLITE_DEFAULT_PAGE_SIZE           512
#define SQLITE_DEFAULT_PCACHE_INITSZ         8
#define SQLITE_MAX_DEFAULT_PAGE_SIZE     32768
#define SQLITE_POWERSAFE_OVERWRITE           1
#define SQLITE_MAX_EXPR_DEPTH                0 // Disables all checking of the expression parse-tree depth, simplifying the code and reducing memory usage.

//#define SQLITE_OMIT_PROGRESS_CALLBACK        1 // Omits the progress handler callback counter, speeding up SQL statement execution.
//#define SQLITE_OMIT_DECLTYPE                 1 // Omits the ability to return the declared type of columns from the result set of query, reducing memory consumption of prepared statements.
//#define SQLITE_OMIT_DEPRECATED               1 // Omits deprecated interfaces and features, reducing the library footprint.
//#define SQLITE_OMIT_SHARED_CACHE             1 // Omits the possibility of using shared cache, improving performance by eliminating conditionals in performance-critical sections of the code.
//#define SQLITE_OMIT_AUTOINIT                 1 // Omits the automatic initialization of SQLite, speeding up many API calls, but requiring the application to call sqlite3_initialize() manually

/*
#undef SQLITE_OMIT_ALTERTABLE
#undef SQLITE_OMIT_ANALYZE
#undef SQLITE_OMIT_ATTACH
#define SQLITE_OMIT_AUTHORIZATION            1
#undef SQLITE_OMIT_AUTOINCREMENT

#define SQLITE_OMIT_AUTOMATIC_INDEX          1
#define SQLITE_OMIT_AUTORESET                1
#define SQLITE_OMIT_AUTOVACUUM               1
#undef SQLITE_OMIT_BETWEEN_OPTIMIZATION
#define SQLITE_OMIT_BLOB_LITERAL             1
#define SQLITE_OMIT_BTREECOUNT               1
#define SQLITE_OMIT_BUILTIN_TEST             1
#define SQLITE_OMIT_CAST                     1
#define SQLITE_OMIT_CHECK                    1
#define SQLITE_OMIT_COMPILEOPTION_DIAGS      1
#define SQLITE_OMIT_COMPOUND_SELECT          1
#define SQLITE_OMIT_CONFLICT_CLAUSE          1
#undef SQLITE_OMIT_CTE
#define SQLITE_OMIT_DECLTYPE                 1 // Omits the ability to return the declared type of columns from the result set of query, reducing memory consumption of prepared statements.
#define SQLITE_OMIT_DEPRECATED               1 // Omits deprecated interfaces and features, reducing the library footprint.
#undef SQLITE_OMIT_DISKIO
#define SQLITE_OMIT_EXPLAIN                  1
#define SQLITE_OMIT_FLAG_PRAGMAS             1
#define SQLITE_OMIT_FOREIGN_KEY              1
#define SQLITE_OMIT_GET_TABLE                1
#define SQLITE_OMIT_INCRBLOB                 1
#define SQLITE_OMIT_INTEGRITY_CHECK          1
#undef SQLITE_OMIT_LIKE_OPTIMIZATION
#undef SQLITE_OMIT_LOCALTIME
#define SQLITE_OMIT_LOOKASIDE                1
#undef SQLITE_OMIT_MEMORYDB
#undef SQLITE_OMIT_OR_OPTIMIZATION
#undef SQLITE_OMIT_PAGER_PRAGMAS
#define SQLITE_OMIT_PARSER_TRACE             1
#undef SQLITE_OMIT_PRAGMA
#define SQLITE_OMIT_PROGRESS_CALLBACK        1 // Omits the progress handler callback counter, speeding up SQL statement execution.
#define SQLITE_OMIT_QUICKBALANCE             1
#undef SQLITE_OMIT_REINDEX
#define SQLITE_OMIT_SCHEMA_PRAGMAS           1
#define SQLITE_OMIT_SCHEMA_VERSION_PRAGMAS   1
#define SQLITE_OMIT_SHARED_CACHE             1 // Omits the possibility of using shared cache, improving performance by eliminating conditionals in performance-critical sections of the code.
#define SQLITE_OMIT_TCL_VARIABLE             1
#define SQLITE_OMIT_TEMPDB                   1
#define SQLITE_OMIT_TRACE                    1
#undef SQLITE_OMIT_TRIGGER
#define SQLITE_OMIT_TRUNCATE_OPTIMIZATION    1
#define SQLITE_OMIT_UTF16                    1
#undef SQLITE_OMIT_VACUUM
#undef SQLITE_OMIT_VIEW
#undef SQLITE_OMIT_VIRTUALTABLE
#undef SQLITE_OMIT_WSD
#define SQLITE_OMIT_XFER_OPT                 1
#define SQLITE_PERFORMANCE_TRACE             1
//#define SQLITE_OMIT_COMPLETE              1
//#define SQLITE_OMIT_SUBQUERY              1
//#define SQLITE_OMIT_DATETIME_FUNCS        1
//#define SQLITE_OMIT_FLOATING_POINT        1
#define SQLITE_COUNTOFVIEW_OPTIMIZATION      0
*/
