#
# --- Game
#

# Define the header/sources explicitly, 
# so CMake will be run again if you add a file
# Hint: $> tree

set(GL3_GAME_SRC 
    src/game.cxx
    )

set(GL3_GAME_HEADER
    include/game.h
    )

# Add Executable Target 
add_executable(Game ${GL3_GAME_HEADER} ${GL3_GAME_SRC})

set_target_properties(Game PROPERTIES
    CXX_STANDARD 17
    CXX_EXTENSIONS OFF) 

target_include_directories(Game PRIVATE include)
  
target_link_libraries(Game Engine)

#
# --- Code Editor Configuration
#

if(WIN32)    
    # Configure VS Project
    set_target_properties(Game 
        PROPERTIES 
        VS_DEBUGGER_WORKING_DIRECTORY "$(OutDir)")
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        PROPERTY 
        VS_STARTUP_PROJECT Game)
endif()

assign_source_group(${GL3_GAME_HEADER} ${GL3_GAME_SRC})

# Set as Startup Project 
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Game)

# Move to custom folder
set_target_properties(Game PROPERTIES FOLDER AIngine)
