/*
 * keyboard.c - Keyboard input simulation and key state detection
 * 
 * Implements keyboard input functionality using the Windows keybd_event API.
 * Provides key name to virtual key code mapping and various key press patterns.
 */

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string.h>
#include <ctype.h>
#include "inputlib.h"

/*
 * vKey - Structure mapping key names to virtual key codes
 */
typedef struct {
	const char* name;
	BYTE code;          /* Windows virtual key code */
} vKey;

/*
 * keymap - Maps human-readable key names to Windows virtual key codes
 * 
 * This lookup table allows users to specify keys by name (e.g., "A", "ENTER")
 * instead of using numeric virtual key codes directly.
 */
static const vKey keymap[] = {
	/* Alphabetic keys A-Z (0x41-0x5A) */
	{"A", 0x41}, {"B", 0x42}, {"C", 0x43}, {"D", 0x44}, {"E", 0x45}, {"F", 0x46}, {"G", 0x47},
	{"H", 0x48}, {"I", 0x49}, {"J", 0x4A}, {"K", 0x4B}, {"L", 0x4C}, {"M", 0x4D}, {"N", 0x4E},
	{"O", 0x4F}, {"P", 0x50}, {"Q", 0x51}, {"R", 0x52}, {"S", 0x53}, {"T", 0x54}, {"U", 0x55},
	{"V", 0x56}, {"W", 0x57}, {"X", 0x58}, {"Y", 0x59}, {"Z", 0x5A},

	/* Number keys 0-9 (0x30-0x39) and their shifted symbols */
	{"0", 0x30}, {"1", 0x31}, {"2", 0x32}, {"3", 0x33}, {"4", 0x34}, {"5", 0x35}, {"6", 0x36}, 
	{"7", 0x37}, {"8", 0x38}, {"9", 0x39}, {"!", 0x31}, {"@", 0x32}, {"#", 0x33}, {"$", 0x34}, 
	{"%", 0x35}, {"^", 0x36}, {"&", 0x37}, {"*", 0x38}, {"(", 0x39}, {")", 0x30},

	/* Punctuation and symbol keys */
	{"`", 0xC0}, {"~", 0xC0}, {"-", 0xBD}, {"_", 0xBD}, {"=", 0xBB}, {"+", 0xBB}, {"[", 0xDB}, 
	{"{", 0xDB}, {"]", 0xDD}, {"}", 0xDD}, {"\\", 0xDC}, {"|", 0xDC}, {";", 0xBA}, {":", 0xBA}, 
	{"'", 0xDE}, {"\"", 0xDE}, {",", 0xBC}, {"<", 0xBC}, {".", 0xBE}, {">", 0xBE}, {"/", 0xBF}, 
	{"?", 0xBF},

	/* Special and navigation keys */
	{" ", 0x20}, {"SPACE", 0x20}, {"BACKSPACE", 0x08}, {"DELETE", 0x2E}, {"TAB", 0x09}, 
	{"ENTER", 0x0D}, {"ESCAPE", 0x1B}, {"HOME", 0x24}, {"END", 0x23}, {"INSERT", 0x2D}, 
	{"PAGEUP", 0x21}, {"PAGEDOWN", 0x22}, {"LEFT", 0x25}, {"UP", 0x26}, {"RIGHT", 0x27}, 
	{"DOWN", 0x28}, {"SHIFT", 0x10}, {"CONTROL", 0x11}, {"ALT", 0x12}, {"WIN", 0x5B},

	/* Function keys F1-F12 (0x70-0x7B) */
	{"F1", 0x70}, {"F2", 0x71}, {"F3", 0x72}, {"F4", 0x73}, {"F5", 0x74}, {"F6", 0x75},
	{"F7", 0x76}, {"F8", 0x77}, {"F9", 0x78}, {"F10", 0x79}, {"F11", 0x7A}, {"F12", 0x7B}
};

/* Characters that require the Shift key to be held down */
static const char* shift_req = "~!@#$%^&*()_+{}|:\"<>?";

/*
 * find_vk - Look up virtual key code by key name
 * 
 * @key: Key name to search for (case-insensitive)
 * 
 * Searches the keymap for a matching key name and returns its virtual key code.
 * The search is case-insensitive ("a" and "A" both work).
 * 
 * Returns: Virtual key code (BYTE), or 0 if key not found
 */
static BYTE find_vk(const char* key) {
	if(!key) return 0;  /* Null key name */
	
	/* Linear search through keymap for matching name */
	for(size_t i = 0; i < sizeof(keymap)/sizeof(keymap[0]); ++i) {
		if(_stricmp(keymap[i].name, key) == 0)  /* Case-insensitive comparison */
			return keymap[i].code;
	}
	return 0;  /* Key not found */
}

/*
 * key_press - Press and release a key by name
 * 
 * @key: Name of the key to press (e.g., "A", "ENTER", "F1")
 * 
 * Simulates a single key press and release. This is the most basic key input
 * function - it sends a key down event immediately followed by a key up event.
 * 
 * Returns: 0 on success, 1 if key name not found
 */
int INPUTLIB_CALL key_press(const char* key) {
	BYTE vk = find_vk(key);
	if(!vk) return 1;
	keybd_event(vk, 0, 0, 0);
	keybd_event(vk, 0, KEYEVENTF_KEYUP, 0);
	return 0;
}

/*
 * key_pressa - Press and release a key by virtual key code
 * 
 * @vk: Windows virtual key code (e.g., VK_SPACE = 0x20)
 * 
 * Similar to key_press but accepts a numeric virtual key code instead of
 * a key name. Useful when working with key codes directly.
 * 
 * Returns: 0 on success, 1 if vk is 0
 */
int INPUTLIB_CALL key_pressa(int vk) {
	if(!vk) return 1;
	keybd_event((BYTE)vk, 0, 0, 0);
	keybd_event((BYTE)vk, 0, KEYEVENTF_KEYUP, 0);
	return 0;
}

/*
 * key_pressn - Press a key multiple times
 * 
 * @key: Name of the key to press
 * @amount: Number of times to press the key
 * 
 * Presses and releases the specified key multiple times with a 10ms delay
 * between each press. Useful for repeating actions like arrow key navigation.
 * 
 * Returns: 0 on success, 1 on invalid parameters or key not found
 */
int INPUTLIB_CALL key_pressn(const char* key, int amount) {
	if(!key || amount <= 0) return 1;
	BYTE vk = find_vk(key);
	if(!vk) return 1;
	
	/* Press the key multiple times with delays */
	for(int i = 0; i < amount; ++i) {
		keybd_event(vk, 0, 0, 0);                 /* Key down */
		keybd_event(vk, 0, KEYEVENTF_KEYUP, 0);   /* Key up */
		input_sleep(10);  /* Small delay between presses */
	}
	return 0;
}

/*
 * key_pressm - Press a key with a modifier
 * 
 * @mod: Modifier key name (e.g., "CONTROL", "SHIFT", "ALT")
 * @key: Key to press while modifier is held
 * 
 * Simulates pressing a key combination like CTRL+C or ALT+F4.
 * The modifier is pressed first, then the key, then both are released
 * in reverse order.
 * 
 * Returns: 0 on success, 1 on invalid parameters or keys not found
 */
int INPUTLIB_CALL key_pressm(const char* mod, const char* key) {
	if(!mod || !key) return 1;
	BYTE vk = find_vk(key);
	BYTE vm = find_vk(mod);
	if(!vk || !vm) return 1;
	
	keybd_event(vm, 0, 0, 0);                 /* Press modifier */
	input_sleep(5);                           /* Brief delay for system to register */
	keybd_event(vk, 0, 0, 0);                 /* Press main key */
	keybd_event(vk, 0, KEYEVENTF_KEYUP, 0);   /* Release main key */
	input_sleep(5);
	keybd_event(vm, 0, KEYEVENTF_KEYUP, 0);   /* Release modifier */
	return 0;
}

/*
 * key_pressmt - Press a key with two modifiers
 * 
 * @mod1: First modifier key (e.g., "CONTROL")
 * @mod2: Second modifier key (e.g., "SHIFT")
 * @key: Key to press while both modifiers are held
 * 
 * Simulates three-key combinations like CTRL+SHIFT+ESC or CTRL+ALT+DELETE.
 * Both modifiers are pressed in order, then the key, then all are released
 * in reverse order.
 * 
 * Returns: 0 on success, 1 on invalid parameters or keys not found
 */
int INPUTLIB_CALL key_pressmt(const char* mod1, const char* mod2, const char* key) {
	if(!mod1 || !mod2 || !key) return 1;
	BYTE vk = find_vk(key);
	BYTE vm1 = find_vk(mod1);
	BYTE vm2 = find_vk(mod2);
	if(!vk || !vm1 || !vm2) return 1;
	
	keybd_event(vm1, 0, 0, 0);                /* Press first modifier */
	input_sleep(5);
	keybd_event(vm2, 0, 0, 0);                /* Press second modifier */
	input_sleep(5);
	keybd_event(vk, 0, 0, 0);                 /* Press main key */
	keybd_event(vk, 0, KEYEVENTF_KEYUP, 0);   /* Release main key */
	input_sleep(5);
	keybd_event(vm2, 0, KEYEVENTF_KEYUP, 0);  /* Release second modifier */
	input_sleep(5);
	keybd_event(vm1, 0, KEYEVENTF_KEYUP, 0);  /* Release first modifier */
	return 0;
}

/*
 * key_hold - Hold a key down for a specified duration
 * 
 * @key: Name of the key to hold
 * @duration_ms: How long to hold the key in milliseconds
 * 
 * Presses a key, waits for the specified duration, then releases it.
 * Useful for actions that require sustained key presses.
 * 
 * Returns: 0 on success, 1 on invalid parameters or key not found
 */
int INPUTLIB_CALL key_hold(const char* key, int duration_ms) {
	if(!key || duration_ms < 0) return 1;
	BYTE vk = find_vk(key);
	if(!vk) return 1;
	
	keybd_event(vk, 0, 0, 0);                 /* Press key down */
	if(duration_ms > 0) input_sleep(duration_ms);  /* Hold for duration */
	keybd_event(vk, 0, KEYEVENTF_KEYUP, 0);   /* Release key */
	return 0;
}

/*
 * key_type - Type a text string
 * 
 * @text: String to type
 * 
 * Types each character in the string by automatically handling uppercase
 * letters and special characters that require the Shift key. Unsupported
 * characters are silently skipped. Each character has a 25ms delay after it.
 * 
 * Returns: 0 on success, 1 if text is null
 */
int INPUTLIB_CALL key_type(const char* text) {
	if(!text) return 1;
	size_t len = strlen(text);
	
	/* Process each character in the string */
	for(size_t i = 0; i < len; ++i) {
		char c = text[i];
		
		/* Convert to uppercase for keymap lookup */
		char s[2] = {0};
		s[0] = (char)toupper((unsigned char)c);
		
		BYTE vk = find_vk(s);
		if(!vk) continue;  /* Skip unsupported characters */
		
		/* Determine if Shift is needed (uppercase or shifted symbols) */
		BOOL need_shift = isupper((unsigned char)c) || strchr(shift_req, c) != NULL;
		
		/* Press Shift if needed for this character */
		if(need_shift) keybd_event(VK_SHIFT, 0, 0, 0);
		
		/* Press and release the key */
		keybd_event(vk, 0, 0, 0);
		keybd_event(vk, 0, KEYEVENTF_KEYUP, 0);
		
		/* Release Shift if it was pressed */
		if(need_shift) keybd_event(VK_SHIFT, 0, KEYEVENTF_KEYUP, 0);
		
		input_sleep(25);  /* Delay between characters for natural typing */
	}
	return 0;
}

/*
 * key_isdown - Check if a key is currently pressed
 * 
 * @key: Name of the key to check
 * 
 * Queries the current state of a key to determine if it is being held down.
 * Uses GetAsyncKeyState to check the high-order bit which indicates if the
 * key is currently pressed.
 * 
 * Returns: 1 if key is down, 0 if key is up, -1 on error or key not found
 */
int INPUTLIB_CALL key_isdown(const char* key) {
	if(!key) return -1;
	BYTE vk = find_vk(key);
	if(!vk) return -1;
	
	/* Get asynchronous key state - high-order bit indicates if pressed */
	SHORT state = GetAsyncKeyState(vk);
	if(state & 0x8000) return 1;  /* Key is currently down */
	return 0;  /* Key is up */
}