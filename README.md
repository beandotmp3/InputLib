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
&nbsp;

To use, go into your project's `.csproj` and insert the following lines below the `<PropertyGroup>` declaration:

```xml
<ItemGroup>
 <Reference Include="InputLib">
  <HintPath>(Path to InputLib.dll)</HintPath>
  <Private>True</Private>
 </Reference>
</ItemGroup>
```

Afterward, you can add `using InputLib;` to your `using` statements in your code and everything should be good to go.

## Usage

Any letter, symbol, or key that is a part of the US-English keyboard is a valid input.

Input.Press() is used to send a single key or keybind (up to 3 modifiers):

```csharp
Input.Press("a"); // will print "a" (case-insensitive)
Input.Press("control", "x"); // will perform Ctrl + X
Input.Press("control", "shift", "escape"); // will open task manager
```

Input.Space() is used to simulate pressing space:

```csharp
Input.Space();
```

## License

This project is licensed under the [MIT License](LICENSE)
