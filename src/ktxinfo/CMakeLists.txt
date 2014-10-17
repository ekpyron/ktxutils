find_package (GLEW REQUIRED)

include_directories (${GLEW_INCLUDE_DIR})

file (GLOB KTXINFO_SOURCES *.c)

add_executable (ktxinfo ${KTXINFO_SOURCES})
target_link_libraries (ktxinfo ktxtables)

install (TARGETS ktxinfo RUNTIME DESTINATION bin)
