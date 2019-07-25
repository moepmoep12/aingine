#
# -- AIngine
#

# Define the header/sources explicitly, 
# so CMake will be run again if you add a file
# Hint: $> tree

include (assign_source_group)


# header files
set(AINGINE_HEADER
		include/public/Application.h
		include/public/Entrypoint.h
		include/public/Window.h
	)

set(AINGINE_HEADER_AINGINE
		include/public/AIngine/Agent.h
		include/public/AIngine/Core.h
		include/public/AIngine/Component.h
		include/public/AIngine/Constants.h
		include/public/AIngine/GameObject.h
		include/public/AIngine/Graphics.h
		include/public/AIngine/Input.h
		include/public/AIngine/KeyCodes.h
		include/public/AIngine/Macros.h
		include/public/AIngine/Math.h
		include/public/AIngine/ParticleEmitter.h
		include/public/AIngine/Physics.h
		include/public/AIngine/Script.h
		include/public/AIngine/SoundComponent.h
		include/public/AIngine/Sounds.h
		include/public/AIngine/Sprite.h
		include/public/AIngine/World.h
		include/public/AIngine/XCSAgents.h
	)

set(AIngine_HEADER_ASSETS
		include/public/Assets/Assets.h
)

set(AINGINE_HEADER_DEBUG
		include/public/Debug/log.h
)

set(AINGINE_HEADER_EDITOR
		include/public/Editor/Widgets/ComponentWidget.h
)

set(AINGINE_HEADER_EVENTS
		include/public/Events/Event.h
		include/public/Events/ApplicationEvents.h
		include/public/Events/InputEvents.h	
)

set(AINGINE_HEADER_RENDERING
		include/public/Rendering/bitmap.h
		include/public/Rendering/Camera.h
		include/public/Rendering/Font.h
		include/public/Rendering/shader.h
		include/public/Rendering/texture.h
		include/public/Rendering/Viewport.h
)

set(AINGINE_HEADER_STRUCTURES
		include/public/Structures/Layer.h
		include/public/Structures/LayerStack.h
		include/public/Structures/memory.h
		include/public/Structures/Rectangle.h
		include/public/Structures/Traverser.h
)

set(AINGINE_HEADER_UTIL
		include/public/Util/FileSystem.h
		include/public/Util/Project.h
		include/public/Util/Random.h
		include/public/Util/SystemCommand.h
)

set(AINGINE_HEADER_UI
		include/public/UI/Button.h	
		include/public/UI/CheckBox.h	
		include/public/UI/Image.h	
		include/public/UI/ImGuiLayer.h	
		include/public/UI/UIElement.h	
		include/public/UI/Slider.h	
		include/public/UI/Text.h	
)

set(AIngine_HEADER_PRIVATE
		include/private/Editor/Editor.h
		include/private/Editor/ScriptingAPI.h
		include/private/Editor/Serialization.h
		include/private/Editor/Widgets/AddComponentWidget.h
		include/private/Editor/Widgets/AgentComponentWidget.h
		include/private/Editor/Widgets/ButtonComponentWidget.h
		include/private/Editor/Widgets/CanvasComponentWidget.h
		include/private/Editor/Widgets/CheckBoxComponentWidget.h
		include/private/Editor/Widgets/Common.h
		include/private/Editor/Widgets/EditorWidget.h
		include/private/Editor/Widgets/CameraWidget.h
		include/private/Editor/Widgets/ImageComponentWidget.h
		include/private/Editor/Widgets/LogWidget.h
		include/private/Editor/Widgets/MenuBarWidget.h
		include/private/Editor/Widgets/ParticleEmitterWidget.h
		include/private/Editor/Widgets/PhysicsComponentWidget.h
		include/private/Editor/Widgets/PopUps.h
		include/private/Editor/Widgets/SceneGraphWidget.h
		include/private/Editor/Widgets/SliderComponentWidget.h
		include/private/Editor/Widgets/SoundComponentWidget.h
		include/private/Editor/Widgets/SpriteComponentWidget.h
		include/private/Editor/Widgets/ToolbarWidget.h
		include/private/Editor/Widgets/TransformComponentWidget.h
		include/private/Editor/Widgets/UIElementComponentWidget.h
		include/private/Editor/Widgets/UITextComponentWidget.h
		include/private/Rendering/ParticleRenderer.h
		include/private/Rendering/PhysicsRenderer.h
		include/private/Rendering/Renderer.h
		include/private/Rendering/UIRenderer.h
		include/private/Structures/SceneGraph.h
		include/private/gl_error.h
		include/private/readfile.h
	)
	
# source files	
set(AINGINE_SRC
        src/Application.cxx
		src/Window.cxx
		src/AIngine/Agent.cxx
		src/AIngine/GameObject.cxx
		src/AIngine/Graphics.cxx
		src/AIngine/Input.cxx
		src/AIngine/ParticleEmitter.cxx
		src/AIngine/Physics.cxx
		src/AIngine/SoundComponent.cxx
		src/AIngine/Sounds.cxx
		src/AIngine/Sprite.cxx
		src/AIngine/World.cxx
		src/AIngine/XCSAgents.cxx
        src/Assets/Assets.cxx
		src/Debug/gl_error.cxx
		src/Debug/log.cxx
		src/Editor/Editor.cxx
		src/Editor/ScriptingAPI.cxx
		src/Editor/Serialization.cxx
		src/Editor/Widgets/AgentComponentWidget.cxx
		src/Editor/Widgets/AddComponentWidget.cxx
		src/Editor/Widgets/ButtonComponentWidget.cxx
		src/Editor/Widgets/CanvasComponentWidget.cxx
		src/Editor/Widgets/CheckBoxComponentWidget.cxx
		src/Editor/Widgets/CameraWidget.cxx
		src/Editor/Widgets/Common.cxx
		src/Editor/Widgets/ImageComponentWidget.cxx
		src/Editor/Widgets/MenuBarWidget.cxx
		src/Editor/Widgets/ParticleEmitterWidget.cxx
		src/Editor/Widgets/PhysicsComponentWidget.cxx
		src/Editor/Widgets/PopUps.cxx
		src/Editor/Widgets/SceneGraphWidget.cxx
		src/Editor/Widgets/SliderComponentWidget.cxx
		src/Editor/Widgets/SoundComponentWidget.cxx
		src/Editor/Widgets/SpriteComponentWidget.cxx
		src/Editor/Widgets/ToolbarWidget.cxx
		src/Editor/Widgets/TransformComponentWidget.cxx
		src/Editor/Widgets/UIElementComponentWidget.cxx
		src/Editor/Widgets/UITextComponentWidget.cxx
		src/Rendering/bitmap.cxx
		src/Rendering/Camera.cxx
		src/Rendering/Font.cxx
		src/Rendering/ParticleRenderer.cxx
		src/Rendering/PhysicsRenderer.cxx
		src/Rendering/Renderer.cxx
		src/Rendering/UIRenderer.cxx
		src/Rendering/shader.cxx
		src/Rendering/texture.cxx
		src/Rendering/Viewport.cxx
		src/Structures/Layer.cxx
		src/Structures/LayerStack.cxx
		src/Structures/SceneGraph.cxx
		src/Util/FileSystem.cxx
		src/Util/Project.cxx
		src/Util/Random.cxx
		src/Util/SystemCommand.cxx
		src/UI/CheckBox.cxx
		src/UI/Image.cxx
		src/UI/ImGuiLayer.cxx
		src/UI/Button.cxx
		src/UI/UIElement.cxx
		src/UI/Slider.cxx
		src/UI/Text.cxx
	)
	

#
# --- Configure the main engine target
#

find_package(OpenGL REQUIRED)

add_library(AIngineEditor STATIC ${AINGINE_HEADER} ${AINGINE_HEADER_AINGINE} ${AIngine_HEADER_ASSETS} ${AIngine_HEADER_EDITOR} ${AINGINE_HEADER_DEBUG} ${AINGINE_HEADER_EVENTS} ${AINGINE_HEADER_RENDERING} ${AINGINE_HEADER_STRUCTURES}  ${AINGINE_HEADER_UTIL} ${AINGINE_HEADER_UI} ${AIngine_HEADER_PRIVATE} ${AINGINE_SRC})
add_library(AIngine STATIC ${AINGINE_HEADER} ${AINGINE_HEADER_AINGINE} ${AIngine_HEADER_ASSETS} ${AIngine_HEADER_EDITOR} ${AINGINE_HEADER_DEBUG} ${AINGINE_HEADER_EVENTS} ${AINGINE_HEADER_RENDERING} ${AINGINE_HEADER_STRUCTURES}  ${AINGINE_HEADER_UTIL} ${AINGINE_HEADER_UI} ${AIngine_HEADER_PRIVATE} ${AINGINE_SRC})

set_target_properties(AIngineEditor PROPERTIES
        CXX_STANDARD 17
        CXX_EXTENSIONS OFF)
set_target_properties(AIngine PROPERTIES
        CXX_STANDARD 17
        CXX_EXTENSIONS OFF)	

set(THIRD_PARTY_DIR "${CMAKE_SOURCE_DIR}/thirdParty")

target_include_directories(AIngineEditor PUBLIC 
						$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/public>
						$<INSTALL_INTERFACE:include/AIngine>)
target_include_directories(AIngine PUBLIC 
						$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/public>
						$<INSTALL_INTERFACE:include/AIngine>)
#target_include_directories(Engine PUBLIC include/public)
include_directories(AIngineEditor PRIVATE include/private)
include_directories(AIngine PRIVATE include/private)


target_link_libraries(AIngineEditor
        glad
        glfw
        OpenGL::GL
        glm
        tinyobjloader
        stb
		imgui
		spdlog
		Box2D-cmake
		nlohmann_json
		nativefiledialog
		xcsrlib
		freetype
		cute_sound
        )
target_link_libraries(AIngine
        glad
        glfw
        OpenGL::GL
        glm
        tinyobjloader
        stb
		imgui
		spdlog
		Box2D-cmake
		nlohmann_json
		nativefiledialog
		xcsrlib
		freetype
		cute_sound
        )

#
# --- Code Editor
#

assign_source_group( ${AINGINE_HEADER} ${AINGINE_HEADER_AINGINE} ${AIngine_HEADER_ASSETS} ${AIngine_HEADER_EDITOR} ${AINGINE_HEADER_DEBUG} ${AINGINE_HEADER_EVENTS} ${AINGINE_HEADER_RENDERING} ${AINGINE_HEADER_STRUCTURES} ${AINGINE_HEADER_UTIL} ${AINGINE_HEADER_UI} ${AIngine_HEADER_PRIVATE} ${AINGINE_SRC})

set_target_properties(AIngineEditor PROPERTIES FOLDER AIngine)
set_target_properties(AIngine PROPERTIES FOLDER AIngine)


#
# --- Options
#

option(AINGINE_LOG_TO_FILE "Should the log be written to a file?" ON)
if(AINGINE_LOG_TO_FILE)
	add_definitions(-DLOG_TO_FILE)
endif()

target_compile_definitions(AIngineEditor PUBLIC -DEDITOR)

#
# --- Install
#
install(TARGETS AIngineEditor freetype EXPORT AIngineEditor DESTINATION "lib/AIngineEditor" COMPONENT editor)
install(TARGETS AIngine freetype EXPORT AIngine DESTINATION "lib/AIngine" COMPONENT aingine)
install(EXPORT AIngineEditor DESTINATION "lib/AIngineEditor"
									FILE aingineeditor-config.cmake)
install(EXPORT AIngine DESTINATION "lib/AIngine"
									FILE aingine-config.cmake)

# Header Files
install(FILES ${AINGINE_HEADER}  DESTINATION "include/AIngine")
install(FILES ${AINGINE_HEADER_AINGINE} DESTINATION "include/AIngine/AIngine")
install(DIRECTORY include/public/Assets DESTINATION "include/AIngine")
install(DIRECTORY include/public/Editor/Widgets DESTINATION "include/AIngine/Editor/Widgets")
install(FILES ${AINGINE_HEADER_DEBUG} DESTINATION "include/AIngine/Debug")
install(FILES ${AINGINE_HEADER_EVENTS} DESTINATION "include/AIngine/Events")
install(FILES ${AINGINE_HEADER_RENDERING} DESTINATION "include/AIngine/Rendering")
install(FILES ${AINGINE_HEADER_STRUCTURES} DESTINATION "include/AIngine/Structures")
install(FILES ${AINGINE_HEADER_UTIL} DESTINATION "include/AIngine/Util")
install(FILES ${AINGINE_HEADER_UI} DESTINATION "include/AIngine/UI")
install(FILES ${CMAKE_SOURCE_DIR}/Resources/AIngine/imgui.ini DESTINATION "Resources/AIngine")

# Resource Folder
install(DIRECTORY ${CMAKE_SOURCE_DIR}/Resources/AIngine DESTINATION Resources COMPONENT resources)



#
# CPACK for creating an Installer
#
set (CPACK_PACKAGE_NAME "AIngine")
set (CPACK_PACKAGE_VENDOR "Andreas Schmidt")
set (CPACK_PACKAGE_DESCRIPTION_SUMMARY "AIngine - a 2D GameEngine")
set (CPACK_PACKAGE_VERSION "1.0.0")
set (CPACK_PACKAGE_VERSION_MAJOR "1")
set (CPACK_PACKAGE_VERSION_MINOR "0")
set (CPACK_PACKAGE_VERSION_PATCH "0")
set (CPACK_PACKAGE_INSTALL_DIRECTORY "AIngine")

set (CPACK_COMPONENTS_ALL editor aingine launcher resources)
set (CPACK_COMPONENT_EDITOR_DISPLAY_NAME "AIngine Editor")
set (CPACK_COMPONENT_EDITOR_REQUIRED "TRUE")
set (CPACK_COMPONENT_AINGINE_DISPLAY_NAME "AIngine Library")
set (CPACK_COMPONENT_AINGINE_REQUIRED "TRUE")
set (CPACK_COMPONENT_LAUNCHER_DISPLAY_NAME "AIngine project launcher")
set (CPACK_COMPONENT_LAUNCHER_REQUIRED "TRUE")
set (CPACK_COMPONENT_RESOURCES_REQUIRED "TRUE")

set (CPACK_COMPONENT_EDITOR_DESCRIPTION
		"The AIngine Editor")
set (CPACK_COMPONENT_AINGINE_DESCRIPTION
		"The static C++ Library")

set (CPACK_COMPONENT_EDITOR_DEPENDS aingine)
set (CPACK_COMPONENT_LAUNCHER_DEPENDS aingine)
#set(CPACK_NSIS_EXECUTABLES_DIRECTORY "launcher")
#set (CPACK_NSIS_MUI_FINISHPAGE_RUN "Launcher.exe")
 #set (CPACK_PACKAGE_ICON "${CMake_SOURCE_DIR}/Resources/AIngine/textures\\\\icon.bmp")
 
# Must be after the last CPACK macros
include(CPack)