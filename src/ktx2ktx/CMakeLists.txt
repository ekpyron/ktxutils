find_package (glfw3 REQUIRED)
find_package (GLEW REQUIRED)
find_package (OpenGL REQUIRED)

file (GLOB KTX2KTX_SOURCES *.c)

include_directories (${ImageMagick_INCLUDE_DIRS})

add_executable (ktx2ktx ${KTX2KTX_SOURCES})
target_link_libraries (ktx2ktx ktxtables glfw OpenGL::OpenGL GLEW::GLEW)

install (TARGETS ktx2ktx RUNTIME DESTINATION bin)
