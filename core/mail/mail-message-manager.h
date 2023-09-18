//
// Created by dingjing on 23-9-15.
//

#ifndef LIBRARY_MAIL_MESSAGE_MANAGER_H
#define LIBRARY_MAIL_MESSAGE_MANAGER_H
#include <glib-object.h>

#include <mail/mail-message.h>
#include <mail/mail-message-type.h>

G_BEGIN_DECLS

typedef struct MessageManager           MessageManager;
typedef struct MessageManagerClass      MessageManagerClass;

#define MESSAGE_TYPE_MANAGER            (message_manager_get_type())


MessageManager* mail_message_manager_instance();
void            mail_message_manager_register(MessageManager* self, MailMessageParserType type, GType gType);

G_END_DECLS

#endif //LIBRARY_MAIL_MESSAGE_MANAGER_H
