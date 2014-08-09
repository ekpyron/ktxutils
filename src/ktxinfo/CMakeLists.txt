file (GLOB KTXINFO_SOURCES *.c)

add_executable (ktxinfo ${KTXINFO_SOURCES})
target_link_libraries (ktxinfo ktxtables)

install (TARGETS ktxinfo RUNTIME DESTINATION bin)
