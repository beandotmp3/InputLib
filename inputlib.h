#pragma once
#ifdef _WIN32
 #ifdef INPUTLIB
  #define INPUTLIB_API __declspec(dllexport)
 #else
  #define INPUTLIB_API __declspec(dllimport)
 #endif 
 #define INPUTLIB_CALL __cdecl
#else
 #define INPUTLIB_API
 #define INPUTLIB_CALL
#endif

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif


INPUTLIB_API int INPUTLIB_CALL input_init(void);
INPUTLIB_API void INPUTLIB_CALL input_sleep(int ms);
INPUTLIB_API void INPUTLIB_CALL input_gle(char* buffer, size_t len);

INPUTLIB_API int INPUTLIB_CALL key_press(const char* key);
INPUTLIB_API int INPUTLIB_CALL key_type(const char* text);

#ifdef __cplusplus
}
#endif
