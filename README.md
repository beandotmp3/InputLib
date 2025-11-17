# InputLib

**InputLib** is a lightweight library written in C for controlling and emulating user inputs globally. Currently Windows-only.

## Functions

This is a list of functions you can import from the .dll, formatted for C.

<details>
 <summary>Utility</summary>

 `input_init` sets up internal hooks and subsystems. Add to the initialization phase of your code. *(Currently a stub)*.

 ```c
input_init();
 ```

 `input_sleep` pauses execution for a specified amount of time in milliseconds.

 ```c
input_sleep(1000);
 ```

 `input_gle` retrieves the last WinAPI error message (thread-local). `buffer` must have space for `len` characters

 ```c
input_gle(tmp, sizeof(tmp));
 ```

</details>
<details>
 <summary>Keyboard</summary>

 `key_press` simulates a single keypress.

 ```c
key_press("a")
 ```

 `key_pressn` simulates multiple keypresses of the same key.

 ```c
key_pressn("a", 3)
 ```

 `key_pressmod` simulates a keypress with a modifier.

 ```c
key_pressmod("control", "x");
 ```

 `key_pressmod2` simulates a keypress with 2 modifiers

 ```c
key_pressmod2("control", "shift", "escape");
 ```

 `key_type` simulates typing the given string, case-sensitive.

 ```c
key_type("Hello, World!")
 ```

</details>

## License

This project is licensed under the [MIT License](LICENSE). © 2025 beandotmp3
