file (GLOB KTXGENCUBEMAP_SOURCES *.c)
add_executable (ktxgencubemap ${KTXGENCUBEMAP_SOURCES})

install (TARGETS ktxgencubemap RUNTIME DESTINATION bin)
