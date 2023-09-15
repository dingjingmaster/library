//
// Created by dingjing on 23-9-14.
//
#include "mail-error.h"

#include <glib/gi18n.h>


typedef struct MailError    MailError;

struct MailError
{
    gint32              errorCode;
    const char*         desc;
};

const MailError gMailError[] =
    {
        {MAIL_ERROR_NONE,                   NULL},
        {MAIL_ERROR_DOMAIN_FAIL,            N_("Domain name resolution failure!")},
        {MAIL_ERROR_OUT_OF_MEMORY,          N_("Out of memory!")},
        {MAIL_ERROR_CONNECT_FAIL,           N_("Failed to connect to the server!")},
        {MAIL_ERROR_RECEIVE_DATA,           N_("Failed to read data from the server!")},
        {MAIL_ERROR_PARSE_DATA,             N_("Parsing server data failed!")},
    };

void mail_error_get_error_info(gint32 errorCode, GError **error)
{
    if (NULL == error) { return; }

    if (errorCode >= MAIL_ERROR_MAX || errorCode <= MAIL_ERROR_NONE) {
        return;
    }

    if (*error) { g_error_free (*error); }

    *error = g_error_new (1, errorCode, gMailError[errorCode].desc, NULL);
}
