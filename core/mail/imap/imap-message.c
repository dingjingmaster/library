//
// Created by dingjing on 23-9-14.
//

#include "imap-message.h"

#include "../../common/log.h"

bool imap_message_login(IMAPMessage* self, struct MailSession* session);

void imap_message_interface_init(MailMessageInterface *iFace)
{
    iFace->login = (void*) imap_message_login;
}


static void imap_message_init (IMAPMessage* self)
{

}

static void imap_message_class_init (IMAPMessageClass* klass)
{

}

G_DEFINE_TYPE_WITH_CODE(IMAPMessage, imap_message, MAIL_TYPE_MESSAGE, G_IMPLEMENT_INTERFACE (MAIL_TYPE_MESSAGE, imap_message_interface_init))


bool imap_message_login(IMAPMessage* self, struct MailSession* session)
{
    LOG_DEBUG("imap login!");

    return false;
}
