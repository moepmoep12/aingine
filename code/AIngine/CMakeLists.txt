#
# --Engine
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
		include/public/AIngine/Core.h
		include/public/AIngine/Component.h
		include/public/AIngine/Constants.h
		include/public/AIngine/GameObject.h
		include/public/AIngine/Input.h
		include/public/AIngine/KeyCodes.h
		include/public/AIngine/Macros.h
		include/public/AIngine/Physics.h
		include/public/AIngine/World.h
		include/public/Assets/Assets.h
		include/public/Debug/DebugDraw.h
		include/public/Debug/log.h
		include/public/Events/Event.h
		include/public/Events/ApplicationEvents.h
		include/public/Events/InputEvents.h	
		include/public/Rendering/bitmap.h
		include/public/Rendering/Camera.h
		include/public/Rendering/PhysicsRenderer.h
		include/public/Rendering/Renderer.h
		include/public/Rendering/shader.h
		include/public/Rendering/texture.h
		include/public/Rendering/Viewport.h
		include/public/Structures/Layer.h
		include/public/Structures/LayerStack.h
		include/public/Structures/memory.h
		include/public/Structures/Rectangle.h
		include/public/Structures/SceneGraph.h
		include/public/Structures/Traverser.h
		include/public/UI/ImGuiLayer.h	
		include/private/Editor/Editor.h
		include/private/Editor/Serialization.h
		include/private/Editor/Widgets/EditorWidget.h
		include/private/Editor/Widgets/CameraWidget.h
		include/private/Editor/Widgets/ComponentWidget.h
		include/private/Editor/Widgets/LogWidget.h
		include/private/Editor/Widgets/MenuBarWidget.h
		include/private/Editor/Widgets/SceneGraphWidget.h
		include/private/Editor/Widgets/TextureComponentWidget.h
		include/private/Editor/Widgets/TransformComponentWidget.h
		include/private/gl_error.h
		include/private/readfile.h
	)
	
# source files	
set(AINGINE_SRC
        src/Application.cxx
		src/Window.cxx
		src/AIngine/Component.cxx
		src/AIngine/GameObject.cxx
		src/AIngine/Input.cxx
		src/AIngine/Physics.cxx
		src/AIngine/World.cxx
        src/Assets/Assets.cxx
		src/Debug/gl_error.cxx
		src/Debug/DebugDraw.cxx
		src/Debug/log.cxx
		src/Editor/Editor.cxx
		src/Editor/Serialization.cxx
		src/Editor/Widgets/CameraWidget.cxx
		src/Editor/Widgets/MenuBarWidget.cxx
		src/Editor/Widgets/SceneGraphWidget.cxx
		src/Editor/Widgets/TextureComponentWidget.cxx
		src/Editor/Widgets/TransformComponentWidget.cxx
		src/Rendering/bitmap.cxx
		src/Rendering/Camera.cxx
		src/Rendering/PhysicsRenderer.cxx
		src/Rendering/Renderer.cxx
		src/Rendering/shader.cxx
		src/Rendering/texture.cxx
		src/Rendering/Viewport.cxx
		src/Structures/Layer.cxx
		src/Structures/LayerStack.cxx
		src/Structures/SceneGraph.cxx
		src/UI/ImGuiLayer.cxx
	)
	

#
# --- Configure the main engine target
#

find_package(OpenGL REQUIRED)

add_library(Engine STATIC ${AINGINE_HEADER} ${AINGINE_SRC})

set_target_properties(Engine PROPERTIES
        CXX_STANDARD 17
        CXX_EXTENSIONS OFF)
		
set(THIRD_PARTY_DIR "${CMAKE_SOURCE_DIR}/thirdParty")


target_include_directories(Engine PUBLIC include/public)
target_include_directories(Engine PRIVATE include/private)


target_link_libraries(Engine
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
		cxxopts
		xcsrlib
		freetype
        )

#
# --- Code Editor
#

assign_source_group(${AINGINE_HEADER} ${AINGINE_SRC})

set_target_properties(Engine PROPERTIES FOLDER AIngine)


#
# --- Options
#

option(AINGINE_LOG_TO_FILE "Should the log be written to a file?" ON)
if(AINGINE_LOG_TO_FILE)
	add_definitions(-DLOG_TO_FILE)
endif()
	