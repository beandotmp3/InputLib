/*
 * window.c - Window management and enumeration
 * 
 * Provides functions for finding, manipulating, and querying information about
 * Windows desktop windows. Includes support for window activation, positioning,
 * state changes, and process information retrieval.
 */

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <psapi.h>
#pragma comment(lib, "Psapi.lib")
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "inputlib.h"

/*
 * find_by_title - Find a window by its exact title
 * 
 * @title: Window title to search for (case-insensitive)
 * 
 * Searches through all top-level windows to find one whose title matches
 * the provided string. Only visible windows are considered.
 * 
 * Returns: Window handle (HWND) if found, NULL otherwise
 */
static HWND find_by_title(const char* title) {
	if(!title) return NULL;
	
	/* Start with the first top-level window */
	HWND hwnd = GetTopWindow(NULL);
	char buf[512];
	
	/* Iterate through all top-level windows */
	while(hwnd) {
		/* Only check visible windows */
		if(IsWindowVisible(hwnd)) {
			if(GetWindowTextA(hwnd, buf, sizeof(buf))) {
				/* Case-insensitive title comparison */
				if(_stricmp(buf, title) == 0) return hwnd;
			}
		}
		/* Move to next window in Z-order */
		hwnd = GetNextWindow(hwnd, GW_HWNDNEXT);
	}
	return NULL;  /* Window not found */
}

/*
 * window_getactive - Get the title of the active foreground window
 * 
 * @title_out: Buffer to store the window title
 * @title_len: Size of the buffer
 * 
 * Retrieves the title text of the currently active (foreground) window.
 * 
 * Returns: 0 on success, 1 on failure
 */
int INPUTLIB_CALL window_getactive(char* title_out, size_t title_len) {
	if(!title_out || title_len == 0) return 1;
	HWND hwnd = GetForegroundWindow();
	if(!hwnd) return 1;
	if(GetWindowTextA(hwnd, title_out, (int)title_len) == 0) return 1;
	return 0;
}

/*
 * window_setactive - Activate a window by its title
 * 
 * @title: Title of the window to activate
 * 
 * Brings the specified window to the foreground, making it the active window.
 * The window must be visible and have an exact title match.
 * 
 * Returns: 0 on success, 1 if window not found or activation failed
 */
int INPUTLIB_CALL window_setactive(const char* title) {
	HWND hwnd = find_by_title(title);
	if(!hwnd) return 1;
	if(!SetForegroundWindow(hwnd)) return 1;
	return 0;
}

/*
 * window_getrect - Get window position and size
 * 
 * @title: Title of the window to query
 * @x: Pointer to store X position (screen coordinates)
 * @y: Pointer to store Y position (screen coordinates)
 * @w: Pointer to store width in pixels
 * @h: Pointer to store height in pixels
 * 
 * Retrieves the bounding rectangle of a window in screen coordinates.
 * 
 * Returns: 0 on success, 1 on failure or invalid parameters
 */
int INPUTLIB_CALL window_getrect(const char* title, int* x, int* y, int* w, int* h) {
	if(!title || !x || !y || !w || !h) return 1;
	HWND hwnd = find_by_title(title);
	if(!hwnd) return 1;
	
	RECT r;
	if(!GetWindowRect(hwnd, &r)) return 1;
	
	/* Extract position and dimensions from RECT */
	*x = r.left;
	*y = r.top;
	*w = r.right - r.left;   /* Width */
	*h = r.bottom - r.top;   /* Height */
	return 0;
}

/*
 * window_move - Move and resize a window
 * 
 * @title: Title of the window to move
 * @x: New X position (screen coordinates)
 * @y: New Y position (screen coordinates)
 * @w: New width in pixels
 * @h: New height in pixels
 * 
 * Changes both the position and size of a window. The window is redrawn
 * after the move.
 * 
 * Returns: 0 on success, 1 if window not found or move failed
 */
int INPUTLIB_CALL window_move(const char* title, int x, int y, int w, int h) {
	HWND hwnd = find_by_title(title);
	if(!hwnd) return 1;
	if(!MoveWindow(hwnd, x, y, w, h, TRUE)) return 1;
	return 0;
}

/*
 * window_maximize - Maximize a window
 * 
 * @title: Title of the window to maximize
 * 
 * Expands the window to fill the entire screen (or working area).
 * 
 * Returns: 0 on success, 1 if window not found or operation failed
 */
int INPUTLIB_CALL window_maximize(const char* title) {
	HWND hwnd = find_by_title(title);
	if(!hwnd) return 1;
	if(!ShowWindow(hwnd, SW_MAXIMIZE)) return 1;
	return 0;
}

/*
 * window_minimize - Minimize a window
 * 
 * @title: Title of the window to minimize
 * 
 * Minimizes the window to the taskbar.
 * 
 * Returns: 0 on success, 1 if window not found or operation failed
 */
int INPUTLIB_CALL window_minimize(const char* title) {
	HWND hwnd = find_by_title(title);
	if(!hwnd) return 1;
	if(!ShowWindow(hwnd, SW_MINIMIZE)) return 1;
	return 0;
}

/*
 * window_close - Close a window
 * 
 * @title: Title of the window to close
 * 
 * Sends a WM_CLOSE message to the window, requesting it to close.
 * The application can still choose to ignore or cancel the close request.
 * 
 * Returns: 0 on success, 1 if window not found or message failed to send
 */
int INPUTLIB_CALL window_close(const char* title) {
	HWND hwnd = find_by_title(title);
	if(!hwnd) return 1;
	if(!PostMessageA(hwnd, WM_CLOSE, 0, 0)) return 1;  /* Request close */
	return 0;
}

/*
 * window_info - Get detailed information about a window
 * 
 * @hwnd: Window handle to query
 * @out: Pointer to window_info_t structure to fill
 * 
 * Retrieves comprehensive information about a window including its handle,
 * process ID, thread ID, title, class name, and the process executable
 * name and path. Requires PROCESS_QUERY_INFORMATION permission.
 * 
 * Returns: 0 on success, 1 if handle is invalid or output pointer is NULL
 */
int INPUTLIB_CALL window_info(HWND hwnd, window_info_t* out) {
	if(!out) return 1;
	
	/* Initialize structure to zeros */
	memset(out, 0, sizeof(window_info_t));
	out->hwnd = hwnd;
	
	/* Validate window handle */
	if(!hwnd || !IsWindow(hwnd)) {
		out->valid = 0;
		return 1;
	}
	
	/* Get process and thread IDs */
	DWORD pid = 0;
	DWORD tid = GetWindowThreadProcessId(hwnd, &pid);
	out->pid = pid;
	out->tid = tid;
	
	/* Get window title and class name */
	if(!GetWindowTextA(hwnd, out->title, sizeof(out->title))) out->title[0] = '\0';
	if(!GetClassNameA(hwnd, out->classname, sizeof(out->classname))) out->classname[0] = '\0';
	
	/* Open process to get executable information */
	HANDLE hProc = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
	if(hProc) {
		/* Get full path to executable */
		if(!GetModuleFileNameExA(hProc, NULL, out->procpath, sizeof(out->procpath))) out->procpath[0] = '\0';
		
		/* Extract just the filename from the full path */
		if(out->procpath[0]) {
			const char* p = strrchr(out->procpath, '\\');  /* Find last backslash */
			if(p) {
				strncpy(out->procname, p + 1, sizeof(out->procname));  /* Copy filename */
			} else {
				strncpy(out->procname, out->procpath, sizeof(out->procname));  /* No path separator */
			}
		} else out->procname[0] = '\0';
		
		CloseHandle(hProc);
	} else {
		/* Unable to open process - may lack permissions */
		out->procpath[0] = '\0';
		out->procname[0] = '\0';
	}
	
	out->valid = 1;
	return 0;
}

/*
 * window_list_builder - Helper structure for building window list
 * 
 * Used by window enumeration to dynamically build an array of window titles.
 */
typedef struct {
	char** arr;
	int capacity;
	int count;          /* Current number of windows */
} window_list_builder;

/*
 * enum_windows_proc - Callback for EnumWindows to collect window titles
 * 
 * @hwnd: Handle of the window being enumerated
 * @lparam: User data (pointer to window_list_builder)
 * 
 * Called once for each top-level window. Collects titles of visible windows
 * into a dynamically growing array. The array capacity doubles when full.
 * 
 * Returns: TRUE to continue enumeration, FALSE on error
 */
static BOOL CALLBACK enum_windows_proc(HWND hwnd, LPARAM lparam) {
	/* Skip invisible windows */
	if(!IsWindowVisible(hwnd)) return TRUE;
	
	char title[512];
	if(GetWindowTextA(hwnd, title, sizeof(title)) == 0) return TRUE;
	if(title[0] == '\0') return TRUE;  /* Skip windows with no title */
	
	window_list_builder* b = (window_list_builder*)lparam;
	
	/* Grow array if needed (doubling strategy) */
	if(b->count >= b->capacity) {
		int newcap = b->capacity * 2;
		if(newcap < 8) newcap = 8;  /* Initial capacity */
		char** newarr = (char**)realloc(b->arr, newcap * sizeof(char*));
		if(!newarr) return FALSE;  /* Allocation failed */
		b->arr = newarr;
		b->capacity = newcap;
	}
	
	/* Allocate and copy the title string */
	size_t len = strlen(title);
	char* t = (char*)malloc(len + 1);
	if(!t) return FALSE;  /* Allocation failed */
	strcpy(t, title);
	b->arr[b->count++] = t;
	
	return TRUE;  /* Continue enumeration */
}

/*
 * window_list - Get list of all visible window titles
 * 
 * @titles_out: Pointer to receive array of title strings
 * @count_out: Pointer to receive number of windows
 * 
 * Enumerates all top-level visible windows and returns their titles as an
 * array of strings. The caller is responsible for freeing each string and
 * the array itself.
 * 
 * Returns: 0 on success, 1 on failure
 */
int INPUTLIB_CALL window_list(char*** titles_out, int* count_out) {
	if(!titles_out || !count_out) return 1;
	
	/* Initialize builder structure */
	window_list_builder b = {0};
	b.capacity = 0;
	b.count = 0;
	b.arr = NULL;
	
	/* Enumerate all windows, collecting titles */
	if(!EnumWindows(enum_windows_proc, (LPARAM)&b)) {
		/* Enumeration failed - clean up partial results */
		for(int i = 0; i < b.count; i++) free(b.arr[i]);
		free(b.arr);
		return 1;
	}
	
	/* Return the collected window titles */
	*titles_out = b.arr;
	*count_out = b.count;
	return 0;
}