find_package (glfw3 REQUIRED)
find_package (GLEW REQUIRED)
find_package (OpenGL REQUIRED)

file (GLOB KTX2KTX_SOURCES *.c)

include_directories (${ImageMagick_INCLUDE_DIRS} ${GLFW3_INCLUDE_DIR} ${GLEW_INCLUDE_DIR})

add_executable (ktx2ktx ${KTX2KTX_SOURCES})
target_link_libraries (ktx2ktx ktxtables ${GLFW3_LIBRARY} ${OPENGL_LIBRARY} ${GLEW_LIBRARY})

install (TARGETS ktx2ktx RUNTIME DESTINATION bin)
