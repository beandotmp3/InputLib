#pragma once
#ifdef INPUTLIB
#define INPUTLIB_API __declspec(dllexport)
#else
#define INPUTLIB_API __declspec(dllimport)
#endif

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// Initializes internal subsystems (currently stub)
INPUTLIB_API int input_init(void);
// Sleeps for given milliseconds
INPUTLIB_API void input_sleep(int ms);
// Retrieves the last error message (thread-local, buffer must have space for len characters)
INPUTLIB_API void input_gle(char* buffer, size_t len);

// Press specified key
INPUTLIB_API int key_press(const char* key);

#ifdef __cplusplus
}
#endif
