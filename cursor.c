#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "inputlib.h"

int INPUTLIB_CALL cursor_lclick(void) {
 mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
 mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
 return 0;
}
int INPUTLIB_CALL cursor_rclick(void) {
 mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, 0);
 mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);
 return 0;
}
int INPUTLIB_CALL cursor_mclick(void) {
 mouse_event(MOUSEEVENTF_MIDDLEDOWN, 0, 0, 0, 0);
 mouse_event(MOUSEEVENTF_MIDDLEUP, 0, 0, 0, 0);
 return 0;
}
int INPUTLIB_CALL cursor_moveto(int x, int y) {
 if(!SetCursorPos(x, y)) return 1;
 return 0;
}
int INPUTLIB_CALL cursor_scroll(int amount) {
 mouse_event(MOUSEEVENTF_WHEEL, 0, 0, amount * WHEEL_DELTA, 0);
 return 0;
}
int INPUTLIB_CALL cursor_movetos(int x, int y, int duration_ms) {
 POINT p;
 if(!GetCursorPos(&p)) return 1;
 int sx = p.x;
 int sy = p.y;
 if(sx == x && sy == y) return 0;
 int steps = duration_ms / 10;
 if(steps <= 0) steps = 1;
 int delay = duration_ms / steps;
 double dx = (double)(x - sx);
 double dy = (double)(y - sy);
 for(int i = 1; i <= steps; i++) {
  double t = (double)i / (double)steps;
  int nx = (int)(sx + dx * t);
  int ny = (int)(sy + dy * t);
  SetCursorPos(nx, ny);
  if(delay > 0) input_sleep(delay);
 }
 return 0;
}
int INPUTLIB_CALL cursor_movetor(int x, int y) {
 POINT p;
 if(!GetCursorPos(&p)) return 1;
 return cursor_moveto(p.x + x, p.y + y);
}