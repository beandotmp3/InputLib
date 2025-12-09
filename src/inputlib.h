/*
 * InputLib - Windows Input Automation Library
 * 
 * A library for automating keyboard input, mouse cursor control, and window management.
 * Provides a simple C API for simulating user input and managing windows programmatically.
 */

#pragma once

/* Platform-specific API export/import macros */
#ifdef _WIN32
	#ifdef INPUTLIB
		/* Export symbols when building the library */
		#define INPUTLIB_API __declspec(dllexport)
	#else
		/* Import symbols when using the library */
		#define INPUTLIB_API __declspec(dllimport)
	#endif
	/* Use __cdecl calling convention for Windows */
	#define INPUTLIB_CALL __cdecl
#else
	/* No special attributes needed for non-Windows platforms */
	#define INPUTLIB_API
	#define INPUTLIB_CALL
#endif

#include <stddef.h>
#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Structure containing detailed information about a keyboard event
 */
typedef struct Event {
 int vk;                   /* Virtual key code */
 int scan;                 /* Raw hardware scan code */
 int pressed;              /* 1 if pressed, 0 if released */
 int injected;             /* 1 if input was injected, 0 otherwise */
 int modifiers;            /* Bitmask of active modifiers */
 unsigned long time;       /* Milliseconds since library init */
 unsigned long delta;      /* Milliseconds since last event */
 unsigned long held;       /* Milliseconds key was held, valid on release */
} Event;

/*
 * Structure containing detailed information about a window
 */
typedef struct window_info_t {
	HWND hwnd;             /* Window handle */
	DWORD pid;             /* Process ID that owns this window */
	DWORD tid;             /* Thread ID that created this window */
	char title[260];       /* Window title text */
	char classname[256];   /* Window class name */
	char procname[260];    /* Process executable name (e.g., "notepad.exe") */
	char procpath[520];    /* Full path to process executable */
	int valid;             /* 1 if data is valid, 0 otherwise */
} window_info_t;



/* ========== Utility Functions ========== */

/* Initialize the input library - sets up DPI awareness */
INPUTLIB_API int INPUTLIB_CALL input_init(void);

/* Sleep for the specified number of milliseconds */
INPUTLIB_API void INPUTLIB_CALL input_sleep(int ms);

/* Get the last Windows error as a formatted string */
INPUTLIB_API void INPUTLIB_CALL input_gle(char* buffer, size_t len);

/* ========== Keyboard Functions ========== */

/* Press and release a key by name (e.g., "A", "ENTER", "F1") */
INPUTLIB_API int INPUTLIB_CALL key_press(const char* key);

/* Press and release a key by virtual key code */
INPUTLIB_API int INPUTLIB_CALL key_pressa(int vk);

/* Press a key multiple times with delays between presses */
INPUTLIB_API int INPUTLIB_CALL key_pressn(const char* key, int amount);

/* Press a key while holding a modifier (e.g., CTRL+C) */
INPUTLIB_API int INPUTLIB_CALL key_pressm(const char* mod, const char* key);

/* Press a key while holding two modifiers (e.g., CTRL+SHIFT+ESC) */
INPUTLIB_API int INPUTLIB_CALL key_pressmt(const char* mod1, const char* mod2, const char* key);

/* Hold a key down for a specified duration in milliseconds */
INPUTLIB_API int INPUTLIB_CALL key_hold(const char* key, int duration_ms);

/* Type a text string, handling uppercase and special characters automatically */
INPUTLIB_API int INPUTLIB_CALL key_type(const char* text);

/* Check if a key is currently pressed down - returns 1 if down, 0 if up, -1 on error */
INPUTLIB_API int INPUTLIB_CALL key_isdown(const char* key);

/* ========== Cursor Functions ========== */

/* Perform a left mouse button click at the current cursor position */
INPUTLIB_API int INPUTLIB_CALL cursor_lclick(void);

/* Perform a right mouse button click at the current cursor position */
INPUTLIB_API int INPUTLIB_CALL cursor_rclick(void);

/* Perform a middle mouse button click at the current cursor position */
INPUTLIB_API int INPUTLIB_CALL cursor_mclick(void);

/* Scroll the mouse wheel - positive values scroll up, negative scroll down */
INPUTLIB_API int INPUTLIB_CALL cursor_scroll(int amount);

/* Move cursor instantly to absolute screen coordinates (x, y) */
INPUTLIB_API int INPUTLIB_CALL cursor_moveto(int x, int y);

/* Move cursor smoothly to absolute coordinates over the specified duration */
INPUTLIB_API int INPUTLIB_CALL cursor_movetos(int x, int y, int duration_ms);

/* Move cursor relative to current position by (x, y) pixels */
INPUTLIB_API int INPUTLIB_CALL cursor_movetor(int x, int y);

/* ========== Listener Functions ========== */

void listener_init(void);

/* Start listener */
INPUTLIB_API int INPUTLIB_CALL listener_start(void);

/* Stop listener */
INPUTLIB_API int INPUTLIB_CALL listener_stop(void);

/* Flush listener-related variables */
INPUTLIB_API int INPUTLIB_CALL listener_flush(void);

/* Subscribe to keyboard event callbacks */
INPUTLIB_API int INPUTLIB_CALL listener_cbsub(void (*cb)(Event* ev));

/* Unsubscribe from keyboard event callbacks */
INPUTLIB_API int INPUTLIB_CALL listener_ucbsub(void);

/* Enable or disable polling mode */
INPUTLIB_API int INPUTLIB_CALL listener_cbpollmode(int enabled);

/* Poll next event in queue */
INPUTLIB_API int INPUTLIB_CALL listener_cbpoll(Event* out);

/* Dump poll queue to buffer */
INPUTLIB_API int INPUTLIB_CALL listener_cbdumppoll(char* buffer, size_t len);

/* Flush callback-related variables */
INPUTLIB_API int INPUTLIB_CALL listener_cbflush(void);

/* Block a key by name */
INPUTLIB_API int INPUTLIB_CALL listener_block(const char* key);

/* Unblock a key by name */
INPUTLIB_API int INPUTLIB_CALL listener_ublock(const char* key);

/* Block a combo with 1 modifier */
INPUTLIB_API int INPUTLIB_CALL listener_blockc(const char* mod, const char* key);

/* Unblock a combo with 1 modifier */
INPUTLIB_API int INPUTLIB_CALL listener_ublockc(const char* mod, const char* key);

/* Check if a key is currently blocked */
INPUTLIB_API int INPUTLIB_CALL listener_isblocked(const char* key);

/* Check the current state of a key */
INPUTLIB_API int INPUTLIB_CALL listener_keystate(const char* key);

/* Check the current state of the modifier bitmask */
INPUTLIB_API int INPUTLIB_CALL listener_modstate(int* out_mask);

/* ========== Window Management Functions ========== */

/* Get the title of the currently active (foreground) window */
INPUTLIB_API int INPUTLIB_CALL window_getactive(char* title_out, size_t title_len);

/* Set (activate) a window by its title, bringing it to the foreground */
INPUTLIB_API int INPUTLIB_CALL window_setactive(const char* title);

/* Get the position and size of a window by its title */
INPUTLIB_API int INPUTLIB_CALL window_getrect(const char* title, int* x, int* y, int* w, int* h);

/* Move and resize a window by its title */
INPUTLIB_API int INPUTLIB_CALL window_move(const char* title, int x, int y, int w, int h);

/* Maximize a window by its title */
INPUTLIB_API int INPUTLIB_CALL window_maximize(const char* title);

/* Minimize a window by its title */
INPUTLIB_API int INPUTLIB_CALL window_minimize(const char* title);

/* Close a window by its title (sends WM_CLOSE message) */
INPUTLIB_API int INPUTLIB_CALL window_close(const char* title);

/* Get detailed information about a window from its handle */
INPUTLIB_API int INPUTLIB_CALL window_info(HWND hwnd, window_info_t* out);

/* Get a list of all visible window titles - caller must free the returned array */
INPUTLIB_API int INPUTLIB_CALL window_list(char*** titles_out, int* count_out);

#ifdef __cplusplus
}
#endif
