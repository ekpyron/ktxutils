find_package (GLEW REQUIRED)

file (GLOB LIBKTXTABLES_SOURCES *.c)

include_directories (${GLEW_INCLUDE_DIR})

add_library (ktxtables ${LIBKTXTABLES_SOURCES})
