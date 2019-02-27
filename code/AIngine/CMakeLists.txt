#
# --Engine
#

# Define the header/sources explicitly, 
# so CMake will be run again if you add a file
# Hint: $> tree

include (assign_source_group)


# header files
set(AINGINE_HEADER
		include/public/Core.h
		include/public/Application.h
		include/public/Entrypoint.h
		include/public/memory.h
		include/public/log.h
		include/public/Window.h
		include/public/Rendering/shader.h
		include/public/Rendering/mvp.h
		include/public/Rendering/bitmap.h
		include/public/Rendering/mesh.h
		include/public/Rendering/meshloader.h
		include/public/Rendering/texture.h
		include/public/Events/Event.h
		include/public/Events/ApplicationEvents.h
		include/public/Events/InputEvents.h	
		include/private/gl_error.h
		include/private/readfile.h
	)
	
# source files	
set(AINGINE_SRC
        src/Application.cxx
		src/gl_error.cxx
		src/Window.cxx
		src/Rendering/shader.cxx
		src/Rendering/mvp.cxx
		src/Rendering/bitmap.cxx
		src/Rendering/mesh.cxx
		src/Rendering/meshloader.cxx
		src/Rendering/texture.cxx
		src/log.cxx
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


target_include_directories(Engine PUBLIC include/public ${THIRD_PARTY_DIR}/spdlog/include/)
target_include_directories(Engine PRIVATE include/private)


target_link_libraries(Engine
        glad
        glfw
        OpenGL::GL
        glm
        tinyobjloader
        stb
        )

#
# --- Code Editor
#

assign_source_group(${AINGINE_HEADER} ${AINGINE_SRC})

set_target_properties(Engine PROPERTIES FOLDER AIngine)
	