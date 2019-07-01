cmake_minimum_required(VERSION 3.12)

project(GLAD C)

set(GLAD_HEADER
    include/glad/glad.h
    include/KHR/khrplatform.h)

set(GLAD_SRC
    src/glad.c)

add_library(glad STATIC
            ${GLAD_HEADER}
            ${GLAD_SRC}
            )

set_property(TARGET glad PROPERTY C_STANDARD 11)

target_include_directories(glad PUBLIC 
						$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
						"$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/glad>")
						
install(TARGETS glad EXPORT AIngineEditor DESTINATION "${CMAKE_INSTALL_LIBDIR}")
install(TARGETS glad EXPORT AIngine DESTINATION "${CMAKE_INSTALL_LIBDIR}")
# install(EXPORT GLAD DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/glad"
								# FILE glad-config.cmake)
install(FILES include/glad/glad.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/glad)
install(FILES include/KHR/khrplatform.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/glad/KHR)