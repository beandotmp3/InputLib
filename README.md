# InputLib

**InputLib** is a lightweight library written in C for controlling and emulating user inputs globally. Currently Windows-only.

## Functions

This is a list of functions you can import from the .dll, formatted for C.

<details>
 <summary>Utility</summary>

 `input_init` sets up internal hooks and subsystems. Add to the initialization phase of your code.

 ```c
input_init();
 ```

 `input_sleep` pauses execution for a specified amount of time in milliseconds.

 ```c
input_sleep(1000);
 ```

 `input_gle` retrieves the last WinAPI error message (thread-local). `buffer` must have space for `len` characters

 ```c
input_gle(buf, sizeof(buf));
 ```

</details>

<details>
 <summary>Keyboard</summary>

 `key_press` simulates a single keypress.

 ```c
key_press("a");
 ```

 `key_pressn` simulates multiple keypresses of the same key.

 ```c
key_pressn("a", 3);
 ```

 `key_pressm` simulates a keypress with a modifier.

 ```c
key_pressm("control", "x");
 ```

 `key_pressmt` simulates a keypress with 2 modifiers

 ```c
key_pressmt("control", "shift", "escape");
 ```

 `key_hold` simulates holding the given key for a certain amount of milliseconds.

 ```c
key_hold("a", 1500);
 ```

 `key_type` simulates typing the given string, case-sensitive.

 ```c
key_type("Hello, World!");
 ```

 `key_isdown` checks if the given key is currently down. 1 indicates it is down, 0 inicates it is up, -1 indicates there was an error.

 ```c
key_isdown("a");
 ```

</details>

<details>
 <summary>Cursor</summary>

 `cursor_lclick`, `cursor_rclick`, and `cursor_mclick` simulates the left, right, and middle click on the mouse;

 `cursor_scroll` simulates scrolling the mouse wheel. Positive is up, negative is down, with each unit being 1 wheel delta.

 ```c
cursor_scroll(-120);
 ```

 `cursor_moveto` moves the cursor from the current location to the x and y coordinates given. X0 Y0 is considered the top-left corner of the primary monitor.

 ```c
cursor_moveto(960, 540);
 ```

 `cursor_movetos` moves the cursor smoothly from the current location to the x and y coordinates given.

 ```c
cursor_movetos(960, 540, 1000);
 ```

 `cursor_movetor` moves the cursor from the current location to the x and y relative to the starting position.

 ```c
cursor_movetor(-200, 400);
 ```

</details>

<details>
 <summary>Window</summary>

 `window_getactive` retrieves the title of the current top window.

 ```c
window_getactive(buf, sizeof(buf));
 ```

 `window_setactive` sets the window with the given title as the active window.

 ```c
window_setactive("title");
 ```

 `window_getrect` retrieves the dimensions and location of the window with the given title.

 ```c
int x, y, w, h;
window_getrect("title", &x, &y, &w, &h);
 ```

 `window_maximize`, `window_minimize`, and `window_close` maximizes, minimizes, and closes the window with the given title.

</details>

## License

This project is licensed under the [MIT License](LICENSE). © 2025 beandotmp3
