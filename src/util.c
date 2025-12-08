/*
 * util.c - Utility functions for initialization, timing, and error handling
 * 
 * Provides core utility functions needed by other parts of the library,
 * including DPI awareness setup, sleep functionality, and Windows error
 * message formatting.
 */

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "inputlib.h"

/* Thread-local storage for the last Windows error code */
static __thread DWORD last_err_code = 0;

/*
 * input_init - Initialize the input library
 * 
 * Performs one-time initialization required by the library, primarily
 * setting up DPI awareness so that coordinate systems work correctly
 * on high-DPI displays.
 * 
 * Should be called once before using any other library functions.
 * 
 * Returns: 0 on success
 */
int INPUTLIB_CALL input_init(void) {
	last_err_code = 0;
	
	/*
	 * Set DPI awareness to per-monitor V2 mode
	 * This ensures cursor coordinates and window rectangles are correct
	 * on systems with multiple monitors at different DPI settings
	 */
	SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
	return 0;
}

/*
 * input_sleep - Sleep for a specified duration
 * 
 * @ms: Number of milliseconds to sleep
 * 
 * Pauses execution for the specified duration. Used to add delays between
 * input events to make automation appear more natural and to ensure the
 * system has time to process events.
 */
void INPUTLIB_CALL input_sleep(int ms) {
	Sleep(ms);
}

/*
 * input_gle - Get Last Error as formatted string
 * 
 * @buffer: Buffer to store the error message
 * @len: Size of the buffer
 * 
 * Retrieves the last Windows error code (from GetLastError) and formats
 * it as a human-readable error message. If there is no error, returns
 * "No error". The error code is also stored in the thread-local variable.
 * 
 * This is useful for debugging when Windows API calls fail.
 */
void INPUTLIB_CALL input_gle(char* buffer, size_t len) {
	if(!buffer || len == 0) return;
	
	/* Get the last Windows error code */
	DWORD err = GetLastError();
	last_err_code = err;
	
	/* Handle the "no error" case */
	if(err == 0) {
		strncpy_s(buffer, len, "No error", _TRUNCATE);
		return;
	}
	
	/* Format the error code into a human-readable message */
	LPVOID msg_buf = NULL;
	FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |   /* Allocate buffer for message */
		FORMAT_MESSAGE_FROM_SYSTEM |       /* Get message from system */
		FORMAT_MESSAGE_IGNORE_INSERTS,     /* Don't process insertion parameters */
		NULL,                              /* No message source */
		err,                               /* Error code to format */
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),  /* Default language */
		(LPSTR)&msg_buf,                   /* Output buffer */
		0,                                 /* Minimum size */
		NULL                               /* No arguments */
	);
	
	if(msg_buf) {
		/* Successfully formatted - copy to output buffer */
		strncpy_s(buffer, len, (char*)msg_buf, _TRUNCATE);
		LocalFree(msg_buf);  /* Free the allocated message buffer */
	} else {
		/* FormatMessage failed - just show the error code */
		snprintf(buffer, len, "Unknown error %lu", err);
	}
}
