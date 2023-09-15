FILE(GLOB COMMON_SRC
        ${CMAKE_SOURCE_DIR}/core/common/log.h
        ${CMAKE_SOURCE_DIR}/core/common/log.c)

file(GLOB COMMON_H
        ${CMAKE_SOURCE_DIR}/core/common/log.h
)
file(COPY ${COMMON_H}       DESTINATION ${CMAKE_BINARY_DIR}/include/general-library/)
