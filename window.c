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