# Unreal Projecte for the course "Modern Game Development"


## Issues/Backlog
**Items** must be doable in less than 2 hours, otherwise the item is too large. 

## Setup
1. Install C/C++ extension pack and C# extension
2. Install Compiler toolset through Visual Studio (The Unreal Package in Visual Studio: https://visualstudio.microsoft.com/downloads/)
3. Right click on .uproject and Generate Project
4. Build Project (Modify Paths): ``"C:\Program Files\Epic Games\UE_5.6\Engine\Build\BatchFiles\Build.bat" SjurvivjalEditor Win64 Development "c:\apps\Sjurvivjal\Sjurvivjal.uproject"`` OR Open .uproject file and click 'Yes' to compile.
5. Add the following to ``.vscode/c_cpp_properties.json`` within the first object of the array. 
```
"includePath": [ 
         "${workspaceFolder}\\Intermediate\\**", 
         "${workspaceFolder}\\Plugins\\**", 
         "${workspaceFolder}\\Source\\**" 
     ]
```
