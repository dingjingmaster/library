//
// Created by dingjing on 23-9-14.
//

#ifndef LIBRARY_MAIL_SESSION_H
#define LIBRARY_MAIL_SESSION_H
#include <stdbool.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define MAIL_TYPE_SESSION               (mail_session_get_type())

#define MAIL_SESSION_GET_CLASS(o)       (G_TYPE_INSTANCE_GET_CLASS((o), MAIL_TYPE_SESSION, MailSessionClass))
#define MAIL_SESSION_IS_OBJECT(o)       (G_TYPE_CHECK_INSTANCE_FUNDAMENTAL_TYPE((o), MAIL_TYPE_SESSION))
#define MAIL_SESSION_IS_CLASS(k)        (G_TYPE_CHECK_CLASS_TYPE((k), MAIL_TYPE_SESSION))

typedef struct MailSession              MailSession;
typedef struct MailSessionClass         MailSessionClass;

struct MailSession
{
    GObject         parentInstance;
};

struct MailSessionClass
{
    GObjectClass    parentClass;

    bool (*connectServer)       (MailSession* self, GError** error);
};

MailSession*    mail_session_new                (void);
void            mail_session_set_port           (MailSession* self, guint16 port);
void            mail_session_set_use_SSL        (MailSession* self, bool isUseSSL);
void            mail_session_set_hostname       (MailSession* self, const char* hostname, GError** error);
void            mail_session_set_username       (MailSession* self, const char* username, GError** error);
void            mail_session_set_password       (MailSession* self, const char* password, GError** error);
void            mail_session_connect            (MailSession* self, GError** error);
void            mail_session_login              (MailSession* self, GError** error);

G_END_DECLS

#endif //LIBRARY_MAIL_SESSION_H
