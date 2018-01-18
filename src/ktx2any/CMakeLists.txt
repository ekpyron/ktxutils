find_package (glfw3 REQUIRED)
find_package (GLEW REQUIRED)
find_package (OpenGL REQUIRED)
find_package (ImageMagick COMPONENTS MagickCore MagickWand REQUIRED)

file (GLOB KTX2ANY_SOURCES main.c)

include_directories (${ImageMagick_INCLUDE_DIRS})

add_executable (ktx2any ${KTX2ANY_SOURCES})
target_link_libraries (ktx2any glfw OpenGL::OpenGL GLEW::GLEW ${ImageMagick_LIBRARIES})

install (TARGETS ktx2any RUNTIME DESTINATION bin)
