#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string.h>
#include <ctype.h>
#include "inputlib.h"

typedef struct {
 const char* name;
 BYTE code;
} vKey;

static const vKey keymap[] = {
 {"A", 0x41}, {"B", 0x42}, {"C", 0x43}, {"D", 0x44}, {"E", 0x45}, {"F", 0x46}, {"G", 0x47},
 {"H", 0x48}, {"I", 0x49}, {"J", 0x4A}, {"K", 0x4B}, {"L", 0x4C}, {"M", 0x4D}, {"N", 0x4E},
 {"O", 0x4F}, {"P", 0x50}, {"Q", 0x51}, {"R", 0x52}, {"S", 0x53}, {"T", 0x54}, {"U", 0x55},
 {"V", 0x56}, {"W", 0x57}, {"X", 0x58}, {"Y", 0x59}, {"Z", 0x5A},

 {"0", 0x30}, {"1", 0x31}, {"2", 0x32}, {"3", 0x33}, {"4", 0x34}, {"5", 0x35}, {"6", 0x36}, {"7", 0x37}, {"8", 0x38}, {"9", 0x39},
 {"!", 0x31}, {"@", 0x32}, {"#", 0x33}, {"$", 0x34}, {"%", 0x35}, {"^", 0x36}, {"&", 0x37}, {"*", 0x38}, {"(", 0x39}, {")", 0x30}, 

 {"`", 0xC0}, {"~", 0xC0}, {"-", 0xBD}, {"_", 0xBD}, {"=", 0xBB}, {"+", 0xBB}, {"[", 0xDB}, {"{", 0xDB}, {"]", 0xDD}, {"}", 0xDD}, {"\\", 0xDC},
 {"|", 0xDC}, {";", 0xBA}, {":", 0xBA}, {"'", 0xDE}, {"\"", 0xDE}, {",", 0xBC}, {"<", 0xBC}, {".", 0xBE}, {">", 0xBE}, {"/", 0xBF}, {"?", 0xBF},

 {" ", 0x20}, {"SPACE", 0x20}, {"BACKSPACE", 0x08}, {"DELETE", 0x2E}, {"TAB", 0x09}, {"ENTER", 0x0D},
 {"ESCAPE", 0x1B}, {"HOME", 0x24}, {"END", 0x23}, {"INSERT", 0x2D}, {"PAGEUP", 0x21}, {"PAGEDOWN", 0x22},
 {"LEFT", 0x25}, {"UP", 0x26}, {"RIGHT", 0x27}, {"DOWN", 0x28}, {"SHIFT", 0x10}, {"CONTROL", 0x11}, {"ALT", 0x12}, {"WIN", 0x5B},
 
 {"F1", 0x70}, {"F2", 0x71}, {"F3", 0x72}, {"F4", 0x73}, {"F5", 0x74}, {"F6", 0x75},
 {"F7", 0x76}, {"F8", 0x77}, {"F9", 0x78}, {"F10", 0x79}, {"F11", 0x7A}, {"F12", 0x7B}
};
static const char* shift_req = "~!@#$%^&*()_+{}|:\"<>?";
static BYTE find_vk(const char* key) {
 if(!key) return 0;
 for(size_t i = 0; i < sizeof(keymap)/sizeof(keymap[0]); ++i) {
  if(_stricmp(keymap[i].name, key) == 0)
   return keymap[i].code;
 }
 return 0;
}

int INPUTLIB_CALL key_press(const char* key) {
 BYTE vk = find_vk(key);
 if(!vk) {
  SetLastError(ERROR_INVALID_PARAMETER);
  return 1;
 }
 keybd_event(vk, 0, 0, 0);
 keybd_event(vk, 0, KEYEVENTF_KEYUP, 0);
 return 0;
}
int INPUTLIB_CALL key_type(const char* text) {
 if(!text) return 1;
 size_t len = strlen(text);
 for(size_t i = 0; i < len; ++i) {
  char c = text[i];
  char s[2] = {0};
  s[0] = (char)toupper((unsigned char)c);
  BYTE vk = find_vk(s);
  if(!vk) continue;
  BOOL need_shift = isupper((unsigned char)c) || strchr(shift_req, c) != NULL;
  if(need_shift) keybd_event(VK_SHIFT, 0, 0, 0);
  keybd_event(vk, 0, 0, 0);
  keybd_event(vk, 0, KEYEVENTF_KEYUP, 0);
  if(need_shift) keybd_event(VK_SHIFT, 0, KEYEVENTF_KEYUP, 0);
  input_sleep(25);
 }
 return 0;
}