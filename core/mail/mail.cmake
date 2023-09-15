include(mail/imap/imap.cmake)
include(mail/interface/interface.cmake)

file(GLOB MAIL_SRC
        ${INTERFACE_SRC} ${IMAP_SRC}
        ${CMAKE_SOURCE_DIR}/core/mail/mail-message-parser-manager.c
)
file(GLOB MAIL_H
        ${CMAKE_SOURCE_DIR}/core/mail/mail-error.h
        ${CMAKE_SOURCE_DIR}/core/mail/mail-session.h
        ${CMAKE_SOURCE_DIR}/core/mail/mail-message-parser-manager.h
)
file(GLOB_RECURSE PRIVATE_H *-private.h)

file(COPY ${INTERFACE_H} ${MAIL_H}  DESTINATION ${CMAKE_BINARY_DIR}/include/general-library/mail/)
file(COPY ${IMAP_H}                 DESTINATION ${CMAKE_BINARY_DIR}/include/general-library/mail/imap/)
file(COPY ${PRIVATE_H}              DESTINATION ${CMAKE_BINARY_DIR}/include/general-library/mail/private/)
