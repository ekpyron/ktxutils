find_package (glfw3 REQUIRED)
find_package (GLEW REQUIRED)
find_package (OpenGL REQUIRED)
find_package (ImageMagick COMPONENTS MagickCore MagickWand REQUIRED)

file (GLOB ANY2KTX_SOURCES *.c)

include_directories (${ImageMagick_INCLUDE_DIRS} ${GLFW3_INCLUDE_DIR} ${GLEW_INCLUDE_DIR})

add_executable (any2ktx ${ANY2KTX_SOURCES})
target_link_libraries (any2ktx ktxtables ${GLFW3_LIBRARY} ${OPENGL_LIBRARY} ${GLEW_LIBRARY} ${ImageMagick_LIBRARIES})

install (TARGETS any2ktx RUNTIME DESTINATION bin)
