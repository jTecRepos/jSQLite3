#pragma once

#include "jCommon.h"

#ifdef ENABLE_MULTITASK

#ifdef ENABLE_QUADCORE
//#pragma message "QUADCORE MODE"
#elif defined(ENABLE_DUALCORE)
//#pragma message "DUALCORE MODE"
#else
#define ENABLE_DUALCORE
//#pragma message "DUALCORE MODE"
#endif

#else
#define ENABLE_SINGLECORE
//#pragma message "SINGLECORE MODE"
#endif


/********************************************************
 *                         MUTEX                        *
 * ******************************************************/

// Not working
// Mutex code for ESP32 (from HardwareSerial.cpp)
//static xSemaphoreHandle lock;
//#define MUTEX_PRINT_TRY_LOCK()	(xSemaphoreTake(lock, portMAX_DELAY) == pdPASS)
//#define MUTEX_PRINT_LOCK()	(do {} while (xSemaphoreTake(lock, portMAX_DELAY) != pdPASS))
//#define MUTEX_PRINT_UNLOCK()  (xSemaphoreGive(lock))


//std::atomic<bool> flag {false};
//flag.load() // Read
//flag.store(true); // Set

//#include <atomic>
//std::atomic_flag atomicFlag{};
//std::atomic<bool> mutex_print_flag {false};



#if defined(__cplusplus) && defined(ENABLE_MULTITASK) && defined(PLATFORM_ESP32) // ESP32

#include <mutex>
#define MUTEX_CREATE(VAR_NAME) std::mutex VAR_NAME;
#define MUTEX_CREATE_LOCAL(VAR_NAME) static std::mutex VAR_NAME;
#define MUTEX_EXTERN(VAR_NAME) extern std::mutex VAR_NAME;
#define MUTEX_TRY_LOCK(VAR_NAME) VAR_NAME.try_lock()
#define MUTEX_LOCK(VAR_NAME) VAR_NAME.lock()
#define MUTEX_UNLOCK(VAR_NAME) VAR_NAME.unlock()

#elif defined(ENABLE_MULTITASK) && defined(PLATFORM_TEENSY) // Teensy

#define MUTEX_CREATE(VAR_NAME) Threads::Mutex VAR_NAME;
#define MUTEX_CREATE_LOCAL(VAR_NAME) static Threads::Mutex VAR_NAME;
#define MUTEX_EXTERN(VAR_NAME) extern Threads::Mutex VAR_NAME;
#define MUTEX_TRY_LOCK(VAR_NAME) (VAR_NAME.try_lock())
#define MUTEX_LOCK(VAR_NAME) (VAR_NAME.lock())
#define MUTEX_UNLOCK(VAR_NAME) (VAR_NAME.unlock())

#elif defined(ENABLE_MULTITASK) && defined(PLATFORM_RP2040) // RP2040

//#include <atomic>
//#define MUTEX_CREATE(VAR_NAME) std::atomic<bool> VAR_NAME {false};
//#define MUTEX_TRY_LOCK(VAR_NAME) while (VAR_NAME.load()); VAR_NAME.store(true);
//#define MUTEX_LOCK(VAR_NAME) while (VAR_NAME.load()); VAR_NAME.store(true);
//#define MUTEX_UNLOCK(VAR_NAME) VAR_NAME.store(false)


#include "pico/mutex.h"
#include <atomic>

class jMutex {
private:
	mutex_t* _mutex;

public:

	
	jMutex(mutex_t *mutex) {
		mutex_init(mutex);
		_mutex = mutex;
	}
	
	jMutex() {
		
		_mutex = new mutex_t();
		//mutex_init(_mutex);
	}
	

	bool try_lock()
	{
		if(!mutex_is_initialized(_mutex))
			mutex_init(_mutex);
		uint32_t owner;
		return mutex_try_enter(_mutex, &owner);
	}

	void lock() {
		if(!mutex_is_initialized(_mutex))
			mutex_init(_mutex);
		mutex_enter_blocking(_mutex);
	}

	void unlock() {
		mutex_exit(_mutex);
	}

};


//#define MUTEX_CREATE(VAR_NAME) std::atomic<jMutex> VAR_NAME(new mutex_t);
#define MUTEX_CREATE(VAR_NAME) std::atomic<jMutex> VAR_NAME;
#define MUTEX_CREATE_LOCAL(VAR_NAME) static std::atomic<jMutex> VAR_NAME;
#define MUTEX_EXTERN(VAR_NAME) extern std::atomic<jMutex> VAR_NAME;
#define MUTEX_TRY_LOCK(VAR_NAME) VAR_NAME.load().try_lock()
#define MUTEX_LOCK(VAR_NAME) VAR_NAME.load().lock()
#define MUTEX_UNLOCK(VAR_NAME) VAR_NAME.load().unlock()


#else

#define MUTEX_CREATE(VAR_NAME) 
#define MUTEX_CREATE_LOCAL(VAR_NAME) 
#define MUTEX_EXTERN(VAR_NAME) 
#define MUTEX_TRY_LOCK(VAR_NAME) (true)
#define MUTEX_LOCK(VAR_NAME)	(0)
#define MUTEX_UNLOCK(VAR_NAME)  (0)

#endif


