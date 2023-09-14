include(mail/imap/imap.cmake)
include(mail/interface/interface.cmake)

file(GLOB MAIL_SRC
        ${INTERFACE_SRC} ${IMAP_SRC}
        ${CMAKE_SOURCE_DIR}/core/mail/mail-session.h
        ${CMAKE_SOURCE_DIR}/core/mail/mail-session.c
)

file(COPY ${INTERFACE_H}    DESTINATION ${CMAKE_BINARY_DIR}/include/mail/)
file(COPY ${IMAP_H}         DESTINATION ${CMAKE_BINARY_DIR}/include/mail/imap/)
