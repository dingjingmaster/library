//
// Created by dingjing on 23-9-15.
//

#ifndef LIBRARY_MAIL_MESSAGE_TYPE_H
#define LIBRARY_MAIL_MESSAGE_TYPE_H
#include <glib.h>

G_BEGIN_DECLS

typedef enum MailMessageParserType          MailMessageParserType;

enum MailMessageParserType
{
    MAIL_MESSAGE_PARSER_UNKNOWN,
    MAIL_MESSAGE_PARSER_IMAP_V4,
};

G_END_DECLS

#endif //LIBRARY_MAIL_MESSAGE_TYPE_H
