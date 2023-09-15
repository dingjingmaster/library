file(GLOB_RECURSE INTERFACE_SRC *.c *.h)

file(GLOB INTERFACE_H
        ${CMAKE_SOURCE_DIR}/core/mail/interface/mail-error.h
        ${CMAKE_SOURCE_DIR}/core/mail/interface/mail-session.h
)
file(GLOB_RECURSE INTERFACE_PRIVATE_H *-private.h)
