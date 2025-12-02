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
#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct window_info_t {
 HWND hwnd;
 DWORD pid;
 DWORD tid;
 char title[260];
 char classname[256];
 char procname[260];
 char procpath[520];
 int valid;
} window_info_t;



INPUTLIB_API int INPUTLIB_CALL input_init(void);
INPUTLIB_API void INPUTLIB_CALL input_sleep(int ms);
INPUTLIB_API void INPUTLIB_CALL input_gle(char* buffer, size_t len);

INPUTLIB_API int INPUTLIB_CALL key_press(const char* key);
INPUTLIB_API int INPUTLIB_CALL key_pressa(int vk);
INPUTLIB_API int INPUTLIB_CALL key_pressn(const char* key, int amount);
INPUTLIB_API int INPUTLIB_CALL key_pressm(const char* mod, const char* key);
INPUTLIB_API int INPUTLIB_CALL key_pressmt(const char* mod1, const char* mod2, const char* key);
INPUTLIB_API int INPUTLIB_CALL key_hold(const char* key, int duration_ms);
INPUTLIB_API int INPUTLIB_CALL key_type(const char* text);
INPUTLIB_API int INPUTLIB_CALL key_isdown(const char* key);

INPUTLIB_API int INPUTLIB_CALL cursor_lclick(void);
INPUTLIB_API int INPUTLIB_CALL cursor_rclick(void);
INPUTLIB_API int INPUTLIB_CALL cursor_mclick(void);
INPUTLIB_API int INPUTLIB_CALL cursor_scroll(int amount);
INPUTLIB_API int INPUTLIB_CALL cursor_moveto(int x, int y);
INPUTLIB_API int INPUTLIB_CALL cursor_movetos(int x, int y, int duration_ms);
INPUTLIB_API int INPUTLIB_CALL cursor_movetor(int x, int y);

INPUTLIB_API int INPUTLIB_CALL window_getactive(char* title_out, size_t title_len);
INPUTLIB_API int INPUTLIB_CALL window_setactive(const char* title);
INPUTLIB_API int INPUTLIB_CALL window_getrect(const char* title, int* x, int* y, int* w, int* h);
INPUTLIB_API int INPUTLIB_CALL window_move(const char* title, int x, int y, int w, int h);
INPUTLIB_API int INPUTLIB_CALL window_maximize(const char* title);
INPUTLIB_API int INPUTLIB_CALL window_minimize(const char* title);
INPUTLIB_API int INPUTLIB_CALL window_close(const char* title);
INPUTLIB_API int INPUTLIB_CALL window_info(HWND hwnd, window_info_t* out);
INPUTLIB_API int INPUTLIB_CALL window_list(char*** titles_out, int* count_out);

#ifdef __cplusplus
}
#endif
