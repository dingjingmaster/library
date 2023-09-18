//
// Created by dingjing on 23-9-14.
//

#ifndef LIBRARY_IMAP_MESSAGE_H
#define LIBRARY_IMAP_MESSAGE_H
#include <mail/mail-message.h>


G_BEGIN_DECLS

#define IMAP_TYPE_MESSAGE       (imap_message_get_type())

void imap_message_interface_init(MailMessageInterface* iFace);

typedef struct IMAPMessage          IMAPMessage;
typedef struct IMAPMessageClass     IMAPMessageClass;

struct IMAPMessage
{
    GObject         parent;
};

struct IMAPMessageClass
{
    GObjectClass    parentClass;
};

GType imap_message_get_type();

G_END_DECLS

#endif //LIBRARY_IMAP_MESSAGE_H
