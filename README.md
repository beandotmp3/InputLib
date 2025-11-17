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

</details>

## License

This project is licensed under the [MIT License](LICENSE). © 2025 beandotmp3
