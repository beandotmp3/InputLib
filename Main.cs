using System;
using System.Runtime.InteropServices;
namespace InputLib;

public class Input
{
 [DllImport("user32.dll", SetLastError = true)]
 private static extern void keybd_event(byte bVk, byte bScan, uint dwFlags, UIntPtr dwExtra);
 private const int EVENT_KEYDOWN = 0x0000;
 private const int EVENT_KEYUP = 0x0002;

 public static void Type(string input)
 {
  
 }

 public static void Press(string key)
 {
  key = key.ToUpper();
  byte vK = KeyMap.vKey[key];
  keybd_event(vK, 0, EVENT_KEYDOWN, UIntPtr.Zero);
  keybd_event(vK, 0, EVENT_KEYUP, UIntPtr.Zero);
 }
 public static void Press(string modifier, string key)
 {
  modifier = modifier.ToUpper();
  key = key.ToUpper();
  byte vM = KeyMap.vMod[modifier];
  byte vK = KeyMap.vKey[key];
  keybd_event(vM, 0, EVENT_KEYDOWN, UIntPtr.Zero);
  keybd_event(vK, 0, EVENT_KEYDOWN, UIntPtr.Zero);
  keybd_event(vK, 0, EVENT_KEYUP, UIntPtr.Zero);
  keybd_event(vM, 0, EVENT_KEYUP, UIntPtr.Zero);
 }
 public static void Press(string modifier1, string modifier2, string key)
 {
  modifier1 = modifier1.ToUpper();
  modifier2 = modifier2.ToUpper();
  key = key.ToUpper();
  byte vM1 = KeyMap.vMod[modifier1];
  byte vM2 = KeyMap.vMod[modifier2];
  byte vK = KeyMap.vKey[key];
  keybd_event(vM1, 0, EVENT_KEYDOWN, UIntPtr.Zero);
  keybd_event(vM2, 0, EVENT_KEYDOWN, UIntPtr.Zero);
  keybd_event(vK, 0, EVENT_KEYDOWN, UIntPtr.Zero);
  keybd_event(vK, 0, EVENT_KEYUP, UIntPtr.Zero);
  keybd_event(vM2, 0, EVENT_KEYUP, UIntPtr.Zero);
  keybd_event(vM1, 0, EVENT_KEYUP, UIntPtr.Zero);
 }
 public static void Space()
 {
  keybd_event(0x20, 0, EVENT_KEYDOWN, UIntPtr.Zero);
  keybd_event(0x20, 0, EVENT_KEYUP, UIntPtr.Zero);
  Thread.Sleep(15);
 }
}

class KeyMap
{
 public static readonly Dictionary<string, byte> vKey = new() { 
  {"A", 0x41}, {"B", 0x42}, {"C", 0x43}, {"D", 0x44}, {"E", 0x45}, {"F", 0x46}, {"G", 0x47},
  {"H", 0x48}, {"I", 0x49}, {"J", 0x4A}, {"K", 0x4B}, {"L", 0x4C}, {"M", 0x4D}, {"N", 0x4E},
  {"O", 0x4F}, {"P", 0x50}, {"Q", 0x51}, {"R", 0x52}, {"S", 0x53}, {"T", 0x54}, {"U", 0x55},
  {"V", 0x56}, {"W", 0x57}, {"X", 0x58}, {"Y", 0x59}, {"Z", 0x5A},

  {"0", 0x30}, {"1", 0x31}, {"2", 0x32}, {"3", 0x33}, {"4", 0x34}, {"5", 0x35}, {"6", 0x36}, {"7", 0x37}, {"8", 0x38}, {"9", 0x39},
  {";", 0xBA}, {"/", 0xBF},
  
  {"BACKSPACE", 0x08}, {"DELETE", 0x2E}, {"TAB", 0x09}, {"ENTER", 0x0D}, 
  {"ESCAPE", 0x1B}, {"HOME", 0x24}, {"END", 0x23}, {"INSERT", 0x2D}, {"PAGEUP", 0x21}, {"PAGEDOWN", 0x22},
  
  {"F1", 0x70}, {"F2", 0x71}, {"F3", 0x72}, {"F4", 0x73}, {"F5", 0x74}, {"F6", 0x75},
  {"F7", 0x76}, {"F8", 0x77}, {"F9", 0x78}, {"F10", 0x79}, {"F11", 0x7A}, {"F12", 0x7B},
 };
 public static readonly Dictionary<string, byte> vMod = new() {
  {"SHIFT", 0x10}, {"CONTROL", 0x11}, {"ALT", 0x12}, {"WIN", 0x5B}
 };
}