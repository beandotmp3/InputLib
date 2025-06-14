# InputLib

**InputLib** is a lightweight library for simulating keyboard inputs in .NET applications. I created it because I was tired of the lack of simple solutions when it came to having an app type outside of the console window. All the answers I came across were either long and cluttery, or old and nonfunctional. Currently Windows-only.

## Installation

To install, you can either compile it yourself or download a pre-compiled binary from the Releases tab.

If you want to compile yourself:

1. Ensure you have the .NET 9.0 SDK installed
2. Download the source code and unzip the files into an easily accessible directory.
3. Open a command prompt window and navigate to whatever directory you unzipped the files to.
4. In the prompt, type `dotnet build -c Release`
5. Retrieve the `InputLib.dll` from the `\bin\Release\net9.0\` directory.

To use, go into your project's `.csproj` and insert the following lines below the `<PropertyGroup>` declaration:

```xml
<ItemGroup>
 <Reference Include="InputLib">
  <HintPath>C:\Path\To\InputLib.dll</HintPath>
  <Private>True</Private>
 </Reference>
</ItemGroup>
```

Afterward, you can add `using InputLib;` to your `using` statements in your code and everything should be good to go.

## Usage

Any letter, symbol, or key that is a part of the US-English keyboard is a valid input.

`Input.Type()` is used to type a string of characters. It will output *exactly* what is provided, so it is case-sensitive:

```csharp
Input.Type("This is an example sentence.") 
```

`Input.Press()` is used to send a keybind (up to 2 modifiers) or repeatedly press a key (such as to navigate with Tab):

```csharp
Input.Press("a", 3) // will type "aaa", case-insensitive
Input.Press("control", "x"); // will perform Ctrl + X
Input.Press("control", "shift", "escape"); // will open task manager
```

`Input.Cursor.GoTo()` is used to move the mouse to specific coordinates on the screen. X0 Y0 is considered the top-left corner of the primary monitor:

```csharp
Input.Cursor.GoTo(0, 0); // moves the cursor to the top left of the screen
Input.Cursor.GoTo(1920, 1080, true); // then to the bottom right of the screen in a fluid motion
```

`Input.Cursor.LClick`, `Input.Cursor.RClick`, and `Input.Cursor.MClick` will perform left-, right-, and middle-click respectivly:

```csharp
Input.Cursor.GoTo(1890, 20); // moves the cursor to the close window button
Input.Cursor.LClick(); // clicks the button
```

If you plan to have a series of commands, such as to do a long list of things using just keybinds, I would suggest to create a new class in your project to hold the logic for that. If you also plan to interact with other applications, I would implement `Thread.Sleep(500)` every other line so any loading doesn't mess up your inputs.

## License

This project is licensed under the [MIT License](LICENSE)
