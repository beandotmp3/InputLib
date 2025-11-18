#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "inputlib.h"

static __thread DWORD last_err_code = 0;

int INPUTLIB_CALL input_init(void) {
 last_err_code = 0;
 SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
 return 0;
}
void INPUTLIB_CALL input_sleep(int ms) {
 Sleep(ms);
}
void INPUTLIB_CALL input_gle(char* buffer, size_t len) {
 if(!buffer || len == 0) return;
 DWORD err = GetLastError();
 last_err_code = err;
 if(err == 0) {
  strncpy_s(buffer, len, "No error", _TRUNCATE);
  return;
 }
 LPVOID msg_buf = NULL;
 FormatMessageA(
  FORMAT_MESSAGE_ALLOCATE_BUFFER |
  FORMAT_MESSAGE_FROM_SYSTEM |
  FORMAT_MESSAGE_IGNORE_INSERTS,
  NULL, err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
  (LPSTR)&msg_buf, 0, NULL
 );
 if(msg_buf) {
  strncpy_s(buffer, len, (char*)msg_buf, _TRUNCATE);
  LocalFree(msg_buf);
 } else {
  snprintf(buffer, len, "Unknown error %lu", err);
 }
}
