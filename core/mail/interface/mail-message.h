//
// Created by dingjing on 23-9-15.
//

#ifndef LIBRARY_MAIL_MESSAGE_H
#define LIBRARY_MAIL_MESSAGE_H

#include <stdbool.h>
#include <glib-object.h>

G_BEGIN_DECLS

typedef struct MailMessage              MailMessage;
typedef struct MailMessageClass         MailMessageClass;


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
