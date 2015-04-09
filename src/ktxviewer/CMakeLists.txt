find_package (glfw3 REQUIRED)
find_package (GLEW REQUIRED)
find_package (OpenGL REQUIRED)

file (GLOB KTXVIEWER_SOURCES *.c)

include_directories (${GLFW3_INCLUDE_DIR} ${GLEW_INCLUDE_DIR})

add_executable (ktxviewer ${KTXVIEWER_SOURCES})
target_link_libraries (ktxviewer ${GLFW3_LIBRARY} ${OPENGL_LIBRARY} ${GLEW_LIBRARY})

install (TARGETS ktxviewer RUNTIME DESTINATION bin)
