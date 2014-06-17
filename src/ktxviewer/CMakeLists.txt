find_package (glfw REQUIRED)
find_package (GLEW REQUIRED)
find_package (OpenGL REQUIRED)

file (GLOB KTXVIEWER_SOURCES *.c)

include_directories (${GLFW_INCLUDE_DIR} ${GLEW_INCLUDE_DIR})

add_executable (ktxviewer ${KTXVIEWER_SOURCES})
target_link_libraries (ktxviewer ${GLFW_LIBRARY} ${OPENGL_LIBRARY} ${GLEW_LIBRARY})
