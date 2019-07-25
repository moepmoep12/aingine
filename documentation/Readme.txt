Building with CMake:

- Enable options GLW_INSTALL, SPDLOG_INSTALL, JSON_INSTALL. Turn off testing options (optional)
- set CMAKE_INSTALL_PREFIX to desired install directory

Running CrappyBird or Pong with Editor:
- Make sure cmake has been added to PATH (User PATH seems not be sufficient)
- After building the AIngine project with CMake build the target INSTALL. The Engine will be installed into CMAKE_INSTALL_PREFIX
- From the install dir, run launcher/Launcher.exe
- Click "Add existing project" and select a .project.in file ( can be found at "root/code/Projects/CrappyBird")
- The project is now listed in the project launcher, use the "Open" button to open it
- Either build the target and run the executable or simply run in debug mode inside visual studio