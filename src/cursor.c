/*
 * cursor.c - Mouse cursor control and click operations
 * 
 * Implements cursor movement and mouse button click functionality
 * using the Windows API mouse_event and cursor positioning functions.
 */

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "inputlib.h"

/*
 * cursor_lclick - Perform a left mouse button click
 * 
 * Simulates pressing and releasing the left mouse button at the current
 * cursor position. Uses deprecated mouse_event API for compatibility.
 * 
 * Returns: 0 on success
 */
int INPUTLIB_CALL cursor_lclick(void) {
	mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
	mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
	return 0;
}

/*
 * cursor_rclick - Perform a right mouse button click
 * 
 * Simulates pressing and releasing the right mouse button at the current
 * cursor position. Typically opens context menus.
 * 
 * Returns: 0 on success
 */
int INPUTLIB_CALL cursor_rclick(void) {
	mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, 0);
	mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);
	return 0;
}

/*
 * cursor_mclick - Perform a middle mouse button click
 * 
 * Simulates pressing and releasing the middle mouse button (scroll wheel click)
 * at the current cursor position.
 * 
 * Returns: 0 on success
 */
int INPUTLIB_CALL cursor_mclick(void) {
	mouse_event(MOUSEEVENTF_MIDDLEDOWN, 0, 0, 0, 0);
	mouse_event(MOUSEEVENTF_MIDDLEUP, 0, 0, 0, 0);
	return 0;
}

/*
 * cursor_moveto - Move cursor instantly to absolute coordinates
 * 
 * @x: Target X coordinate in screen pixels
 * @y: Target Y coordinate in screen pixels
 * 
 * Moves the cursor immediately to the specified screen position.
 * Coordinates are absolute screen coordinates where (0,0) is top-left.
 * 
 * Returns: 0 on success, 1 on failure
 */
int INPUTLIB_CALL cursor_moveto(int x, int y) {
	if(!SetCursorPos(x, y)) return 1;
	return 0;
}

/*
 * cursor_scroll - Scroll the mouse wheel
 * 
 * @amount: Number of scroll increments - positive scrolls up, negative scrolls down
 * 
 * Simulates mouse wheel scrolling. Each increment is multiplied by WHEEL_DELTA (120)
 * which is the standard Windows scroll unit.
 * 
 * Returns: 0 on success
 */
int INPUTLIB_CALL cursor_scroll(int amount) {
	mouse_event(MOUSEEVENTF_WHEEL, 0, 0, amount * WHEEL_DELTA, 0);
	return 0;
}

/*
 * cursor_movetos - Move cursor smoothly to absolute coordinates
 * 
 * @x: Target X coordinate in screen pixels
 * @y: Target Y coordinate in screen pixels
 * @duration_ms: Time in milliseconds to complete the movement
 * 
 * Moves the cursor from its current position to the target position in a smooth,
 * linear motion over the specified duration. The movement is broken into steps
 * with approximately 10ms intervals between updates.
 * 
 * Returns: 0 on success, 1 if unable to get current cursor position
 */
int INPUTLIB_CALL cursor_movetos(int x, int y, int duration_ms) {
	POINT p;
	if(!GetCursorPos(&p)) return 1;  /* Get current cursor position */
	
	int sx = p.x;  /* Starting X coordinate */
	int sy = p.y;  /* Starting Y coordinate */
	
	/* Already at target position, no movement needed */
	if(sx == x && sy == y) return 0;
	
	/* Calculate number of steps (aim for ~10ms per step) */
	int steps = duration_ms / 10;
	if(steps <= 0) steps = 1;
	
	int delay = duration_ms / steps;  /* Actual delay per step */
	
	/* Calculate total distance to travel */
	double dx = (double)(x - sx);
	double dy = (double)(y - sy);
	
	/* Perform linear interpolation for smooth movement */
	for(int i = 1; i <= steps; i++) {
		double t = (double)i / (double)steps;  /* Progress ratio (0.0 to 1.0) */
		int nx = (int)(sx + dx * t);  /* Interpolated X position */
		int ny = (int)(sy + dy * t);  /* Interpolated Y position */
		SetCursorPos(nx, ny);
		if(delay > 0) input_sleep(delay);
	}
	return 0;
}

/*
 * cursor_movetor - Move cursor relative to current position
 * 
 * @x: Number of pixels to move horizontally (positive = right, negative = left)
 * @y: Number of pixels to move vertically (positive = down, negative = up)
 * 
 * Moves the cursor by the specified offset from its current position.
 * This is useful for relative positioning without knowing absolute coordinates.
 * 
 * Returns: 0 on success, 1 if unable to get current cursor position
 */
int INPUTLIB_CALL cursor_movetor(int x, int y) {
	POINT p;
	if(!GetCursorPos(&p)) return 1;  /* Get current position */
	/* Move to current position + offset */
	return cursor_moveto(p.x + x, p.y + y);
}