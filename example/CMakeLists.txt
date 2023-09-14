add_executable(imap-example.run imap-example.c)

target_link_libraries(imap-example.run
        -Wl,-rpath=${CMAKE_BINARY_DIR}
        -L${CMAKE_BINARY_DIR} -lgeneral-library
        ${GIO_LIBRARIES}
)
target_include_directories(imap-example.run PUBLIC
        ${CMAKE_BINARY_DIR}/include
        ${GIO_INCLUDE_DIRS}
)
add_dependencies(imap-example.run general-library)
