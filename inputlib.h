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
INPUTLIB_API int INPUTLIB_CALL key_pressn(const char* key, int amount);
INPUTLIB_API int INPUTLIB_CALL key_pressmod(const char* mod, const char* key);
INPUTLIB_API int INPUTLIB_CALL key_pressmod2(const char* mod1, const char* mod2, const char* key);
INPUTLIB_API int INPUTLIB_CALL key_hold(const char* key, int duration_ms);
INPUTLIB_API int INPUTLIB_CALL key_type(const char* text);

INPUTLIB_API int INPUTLIB_CALL cursor_lclick(void);
INPUTLIB_API int INPUTLIB_CALL cursor_rclick(void);
INPUTLIB_API int INPUTLIB_CALL cursor_mclick(void);
INPUTLIB_API int INPUTLIB_CALL cursor_scroll(int amount);
INPUTLIB_API int INPUTLIB_CALL cursor_moveto(int x, int y);
INPUTLIB_API int INPUTLIB_CALL cursor_movetos(int x, int y, int duration_ms);
INPUTLIB_API int INPUTLIB_CALL cursor_movetor(int x, int y);

INPUTLIB_API int INPUTLIB_CALL window_getactive(char* title_out, size_t title_len);
INPUTLIB_API int INPUTLIB_CALL window_setactive(const char* title);
INPUTLIB_API int INPUTLIB_CALL window_maximize(const char* title);
INPUTLIB_API int INPUTLIB_CALL window_minimize(const char* title);
INPUTLIB_API int INPUTLIB_CALL window_close(const char* title);

#ifdef __cplusplus
}
#endif
