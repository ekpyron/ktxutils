find_package (glfw3 REQUIRED)
find_package (GLEW REQUIRED)
find_package (OpenGL REQUIRED)

file (GLOB KTXVIEWER_SOURCES *.c)

add_executable (ktxviewer ${KTXVIEWER_SOURCES})
target_link_libraries (ktxviewer glfw OpenGL::OpenGL GLEW::GLEW)

install (TARGETS ktxviewer RUNTIME DESTINATION bin)
