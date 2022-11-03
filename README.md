# AIngine

The 2D Game Engine AIngine offers developers an extensive editor and an AI module based on the eXtended Classifier System (XCS).
The game engine was developed as a project at the University of Würzburg in the Games Engineering Bsc. program.

![Poster](documentation/AInginePoster.png)

## Building with CMake

To build with CMake enable the options `GLFW_INSTALL, SPDLOG_INSTALL, JSON_INSTALL`.
Also set the the `CMAKE_INSTALL_PREFIX` to your desired install directory.

## Installing AIngine

Make sure cmake has been added to PATH (User PATH seems not be sufficient)

Either use the installer provided inside the binaries.zip or install manually:
(automatic execution of launcher after installation may fail, bug)

- Inside the AIngine solution build the target `INSTALL`

After installing:

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

## Running example games

- Either run the installer to install the engine or do it manually as described here
- Make sure cmake has been added to PATH (User PATH seems not be sufficient)
- After building the AIngine project with CMake build the target INSTALL. The Engine will be installed into CMAKE_INSTALL_PREFIX
- From the install dir, run launcher/Launcher.exe
- Click "Add existing project" and select a .project.in file ( can be found at "root/code/Projects/CrappyBird")
- The project is now listed in the project launcher, use the "Open" button to open it
- Either build the target and run the executable or simply run in debug mode inside visual studio
