//
// Created by dingjing on 23-9-15.
//

#ifndef LIBRARY_MAIL_MESSAGE_H
#define LIBRARY_MAIL_MESSAGE_H

#include <stdbool.h>
#include <glib-object.h>

#define MAIL_PROTO_UNKNOWN              0x00
#define MAIL_PROTO_IMAP_V4              0x01

G_BEGIN_DECLS

typedef enum MailProtoType          MailProtoType;

typedef struct MailMessage          MailMessage;
typedef struct MailMessageClass     MailMessageClass;


struct MailMessage
{
    GObject             parentInstance;
};

struct MailMessageClass
{
    GObjectClass        parentClass;
};



G_END_DECLS

#endif //LIBRARY_MAIL_MESSAGE_H
