
include(mail/mail.cmake)
include(common/common.cmake)

add_library(general-library SHARED ${COMMON_SRC} ${MAIL_SRC})
target_include_directories(general-library PUBLIC
        ${GLIB_INCLUDE_DIRS} ${GIO_INCLUDE_DIRS}
)
target_link_libraries(general-library ${GLIB_LIBRARIES} ${GIO_LIBRARIES})
