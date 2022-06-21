#ifndef MAIN_H
#define MAIN_H
#include <stdint.h>

/**
 * Static has too many uses, narrow it down.
 */
#define internal 	static
#define lpersist 	static
#define global 		static

/**
 * Alias primitive types.
 */
typedef uint8_t 	 u8;
typedef uint16_t 	u16;
typedef uint32_t 	u32;
typedef uint64_t 	u64;
typedef int8_t 		 i8;
typedef int16_t 	i16;
typedef int32_t 	i32;
typedef int64_t 	i64;
typedef i32			b32;
typedef i64			b64;

/**
 * A structure that contains the various elements needed to handle the application
 * state and its required components.
 */
struct app_state
{
	b32 isRunning;
};


#endif