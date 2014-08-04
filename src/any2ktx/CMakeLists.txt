find_package (glfw REQUIRED)
find_package (GLEW REQUIRED)
find_package (OpenGL REQUIRED)
find_package (ImageMagick COMPONENTS MagickCore MagickWand REQUIRED)

file (GLOB ANY2KTX_SOURCES *.c)

include_directories (${ImageMagick_INCLUDE_DIRS} ${GLFW_INCLUDE_DIR} ${GLEW_INCLUDE_DIR})

add_executable (any2ktx ${ANY2KTX_SOURCES})
target_link_libraries (any2ktx ${GLFW_LIBRARY} ${OPENGL_LIBRARY} ${GLEW_LIBRARY} ${ImageMagick_LIBRARIES})

install (TARGETS any2ktx RUNTIME DESTINATION bin)
