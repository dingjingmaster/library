//
// Created by dingjing on 23-9-14.
//

#ifndef LIBRARY_MAIL_ERROR_H
#define LIBRARY_MAIL_ERROR_H
#include <glib.h>

enum
{
    MAIL_ERROR_NONE,
    MAIL_ERROR_DOMAIN_FAIL,
    MAIL_ERROR_OUT_OF_MEMORY,
    MAIL_ERROR_CONNECT_FAIL,
    MAIL_ERROR_RECEIVE_DATA,
    MAIL_ERROR_PARSE_DATA,

    MAIL_ERROR_MAX,
};

void mail_error_get_error_info (gint32 errorCode, GError** error /* out */);


#endif //LIBRARY_MAIL_ERROR_H
