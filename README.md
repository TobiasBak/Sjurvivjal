# Unreal Projecte for the course "Modern Game Development"


## Issues/Backlog
**Items** must be doable in less than 2 hours, otherwise the item is too large. 

## Setup
1. Install C/C++ extension pack and C# extension
2. Install Compiler toolset through Visual Studio (The Unreal Package in Visual Studio: https://visualstudio.microsoft.com/downloads/)
3. In Unreal: Edit > Editor Preferences > General > Source Code (Select Visual Studio Code)
4. Right click on .uproject and Generate Project
5. Add the following to .vscode/c_cpp_properties.json
```
"includePath": [ 
         "${workspaceFolder}\\Intermediate\\**", 
         "${workspaceFolder}\\Plugins\\**", 
         "${workspaceFolder}\\Source\\**" 
     ],
```

## Building (Modify paths)
& "C:\Program Files\Epic Games\UE_5.6\Engine\Build\BatchFiles\Build.bat" SjurvivjalEditor Win64 Development "c:\apps\Sjurvivjal\Sjurvivjal.uproject"