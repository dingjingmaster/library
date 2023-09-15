//
// Created by dingjing on 23-9-14.
//
#include <stdio.h>
#include <log.h>
#include <mail/mail-session.h>

int main (int argc, char* argv[])
{
    g_autoptr(GError) error = NULL;

    struct MailSession* session = mail_session_new();
    if (NULL == session) {
        printf ("new session error\n");
        return -1;
    }

    mail_session_set_port (session, 143);
    mail_session_set_use_SSL (session, false);

    mail_session_set_hostname (session, "imap.163.com", &error);
    if (error) {
        printf ("set hostname error: %s\n", error->message);
        return -1;
    }

    mail_session_set_username(session, "18635038785", &error);
    if (error) {
        printf ("set username error: %s\n", error->message);
        return -1;
    }

    mail_session_set_password(session, "LRZRVSASFWLROZEE", &error);
    if (error) {
        printf ("set password error: %s\n", error->message);
        return -1;
    }

    mail_session_connect (session, &error);
    if (error) {
        printf ("connect error: %s\n", error->message);
        return -1;
    }

    printf ("开始登录\n");
    mail_session_login (session, &error);
    if (error) {
        printf ("login error: %s\n", error->message);
        return -1;
    }
    printf ("登录OK\n");


    return 0;
}