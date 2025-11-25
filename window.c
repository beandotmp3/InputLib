#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "inputlib.h"

static HWND find_by_title(const char* title) {
 if(!title) return NULL;
 HWND hwnd = GetTopWindow(NULL);
 char buf[512];
 while(hwnd) {
  if(IsWindowVisible(hwnd)) {
   if(GetWindowTextA(hwnd, buf, sizeof(buf))) {
    if(_stricmp(buf, title) == 0) return hwnd;
   }
  }
  hwnd = GetNextWindow(hwnd, GW_HWNDNEXT);
 }
 return NULL;
}

int INPUTLIB_CALL window_getactive(char* title_out, size_t title_len) {
 if(!title_out || title_len == 0) return 1;
 HWND hwnd = GetForegroundWindow();
 if(!hwnd) return 1;
 if(GetWindowTextA(hwnd, title_out, (int)title_len) == 0) return 1;
 return 0;
}
int INPUTLIB_CALL window_setactive(const char* title) {
 HWND hwnd = find_by_title(title);
 if(!hwnd) return 1;
 if(!SetForegroundWindow(hwnd)) return 1;
 return 0;
}
int INPUTLIB_CALL window_getrect(const char* title, int* x, int* y, int* w, int* h) {
 if(!title || !x || !y || !w || !h) return 1;
 HWND hwnd = find_by_title(title);
 if(!hwnd) return 1;
 RECT r;
 if(!GetWindowRect(hwnd, &r)) return 1;
 *x = r.left;
 *y = r.top;
 *w = r.right - r.left;
 *h = r.bottom - r.top;
 return 0;
}
int INPUTLIB_CALL window_move(const char* title, int x, int y, int w, int h) {
 HWND hwnd = find_by_title(title);
 if(!hwnd) return 1;
 if(!MoveWindow(hwnd, x, y, w, h, TRUE)) return 1;
 return 0;
}

int INPUTLIB_CALL window_maximize(const char* title) {
 HWND hwnd = find_by_title(title);
 if(!hwnd) return 1;
 if(!ShowWindow(hwnd, SW_MAXIMIZE)) return 1;
 return 0;
}
int INPUTLIB_CALL window_minimize(const char* title) {
 HWND hwnd = find_by_title(title);
 if(!hwnd) return 1;
 if(!ShowWindow(hwnd, SW_MINIMIZE)) return 1;
 return 0;
}
int INPUTLIB_CALL window_close(const char* title) {
 HWND hwnd = find_by_title(title);
 if(!hwnd) return 1;
 if(!PostMessageA(hwnd, WM_CLOSE, 0, 0)) return 1;
 return 0;
}

