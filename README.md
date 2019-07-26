# GameLab Project Repository

|  General Info  | |
| ---|---|
| Working Title | AIngine |
| Final Title | AIngine |
| Participant 1 | Schmidt Andreas, andreas.schmidt1@uni-wuerzburg.de, s341981 |
|Start-Date| 15.10.2018 |
|Study Program | Games Engineering B.Sc.|

### Abstract

Die 2D Game Engine AIngine bietet Entwicklern einen umfangreichen Editor und ein AI Modul um lernende Agenten für ihre Spiele zu entwickeln.

## Building with CMake

To build with CMake enable the options `GLFW_INSTALL, SPDLOG_INSTALL, JSON_INSTALL`.
Also set the the `CMAKE_INSTALL_PREFIX` to your desired install directory.

## Installing AIngine

The windows installer is currently not working, instead follow these steps:
- Make sure cmake has been added to PATH (User PATH seems not be sufficient)
- Inside the AIngine solution build the target `INSTALL`
- Execute launcher/Launcher.exe inside your install dir
- Click "Add existing project" inside the project launcher
- Select a .project.in file (one can be found at /code/Projects/CrappyBird)
- Open the project by selecting it and clicking "Open" inside the project launcher
- Either build the target and execute the executable or simply run in debug mode inside Visual Studio
- Load a scene via the menubar or toolbar


## Building a project with the Editor

To build an exectable from your project inside the Editor:
- Make sure at least one scene will be build (you can add scenes under the menuitem Build/BuildScenes)
- Execute the menuitem Build/Build and select your output folder
- The Visual Studio solution needs to be rebuild after building

## Scripting

Scripts can be added inside the editor via the menuitem Scripts. After adding/removing a script the editor needs to be restarted manually.

