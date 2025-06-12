using System;
using System.Runtime.InteropServices;
namespace InputLib;

public class Input
{
 [DllImport("user32.dll", SetLastError = true)]
 private static extern void keybd_event(byte bVk, byte bScan, uint dwFlags, UIntPtr dwExtra);
 private const int EVENT_KEYDOWN = 0x0000;
 private const int EVENT_KEYUP = 0x0002;

 public static void PressKey(string key)
 {
  key = key.ToUpper();
  byte vCode = KeyMap.vKeys[key];
  keybd_event(vCode, 0, EVENT_KEYDOWN, UIntPtr.Zero);
  keybd_event(vCode, 0, EVENT_KEYUP, UIntPtr.Zero);
 }

 public static void Space()
 {
  keybd_event(0x20, 0, EVENT_KEYDOWN, UIntPtr.Zero);
  keybd_event(0x20, 0, EVENT_KEYUP, UIntPtr.Zero);
  Thread.Sleep(10);
 }
}

class KeyMap
{
 public static readonly Dictionary<string, byte> vKeys = new() { 
  {"A", 0x41}, {"B", 0x42}, {"C", 0x43}, {"D", 0x44}, {"E", 0x45}, {"F", 0x46}, {"G", 0x47},
  {"H", 0x48}, {"I", 0x49}, {"J", 0x4A}, {"K", 0x4B}, {"L", 0x4C}, {"M", 0x4D}, {"N", 0x4E},
  {"O", 0x4F}, {"P", 0x50}, {"Q", 0x51}, {"R", 0x52}, {"S", 0x53}, {"T", 0x54}, {"U", 0x55},
  {"V", 0x56}, {"W", 0x57}, {"X", 0x58}, {"Y", 0x59}, {"Z", 0x5A},

  {"0", 0x30}, {"1", 0x31}, {"2", 0x32}, {"3", 0x33}, {"4", 0x34}, {"5", 0x35}, {"6", 0x36}, {"7", 0x37}, {"8", 0x38}, {"9", 0x39}
 };
 public static readonly Dictionary<string, byte> vMods = new() {
  {"SHIFT", 0x10}, {"CONTROL", 0x11}, {"ALT", 0x12}
 };
}