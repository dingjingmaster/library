//
// Created by dingjing on 23-9-14.
//

#include "mail-session.h"

#include <gio/gio.h>

#include "mail-error.h"
#include "../../common/log.h"


typedef struct MailSessionPrivate  MailSessionPrivate;

struct MailSessionPrivate
{
    bool                mIsUseSSL;
    guint16             mPort;
    GResolver*          mResolver;
    GSocket*            mSocket;

    gchar*              mHostName;
    gchar*              mUserName;
    gchar*              mPassword;
};

G_DEFINE_TYPE_WITH_PRIVATE(MailSession, mail_session, G_TYPE_OBJECT)

#define MAIL_SESSION_PRIVATE(o)         ((MailSessionPrivate*) mail_session_get_instance_private(o))





static void mail_session_dispose (GObject* self)
{
}

static void mail_session_finalize (GObject* self)
{
    MailSessionPrivate* priv = MAIL_SESSION_PRIVATE((MailSession*) self);

    g_return_if_fail(priv);

    if (priv->mHostName)    g_free (priv->mHostName);
    if (priv->mUserName)    g_free (priv->mUserName);
    if (priv->mPassword)    g_free (priv->mPassword);
    if (priv->mResolver)    g_object_unref (priv->mResolver);
}

static void mail_session_init (MailSession* self)
{
    MailSessionPrivate* priv = MAIL_SESSION_PRIVATE(self);

    priv->mIsUseSSL = false;
    priv->mHostName = NULL;
    priv->mUserName = NULL;
    priv->mPassword = NULL;
    priv->mPort = 0;
    priv->mResolver = g_resolver_get_default();
}

static void mail_session_class_init (MailSessionClass* klass)
{
    GObjectClass* bClass = G_OBJECT_CLASS(klass);

    bClass->dispose = mail_session_dispose;
    bClass->finalize = mail_session_finalize;

    g_log_set_writer_func(log_handler, NULL, NULL);

    klass->connectServer = NULL;
}

MailSession* mail_session_new(void)
{
    return g_object_new (MAIL_TYPE_SESSION, NULL);
}


void mail_session_set_hostname(MailSession* self, const char *hostname, GError **error)
{
    g_return_if_fail(self && hostname);

    MailSessionPrivate* priv = MAIL_SESSION_PRIVATE(self);

    if (priv->mHostName) {
        g_free (priv->mHostName);
    }

    priv->mHostName = g_strdup(hostname);
    if (!priv->mHostName) {
        mail_error_get_error_info (MAIL_ERROR_OUT_OF_MEMORY, error);
    }
}

void mail_session_set_port(MailSession *self, guint16 port)
{
    g_return_if_fail(self);

    MailSessionPrivate* priv = MAIL_SESSION_PRIVATE(self);

    priv->mPort = port;
}

void mail_session_set_use_SSL(MailSession *self, bool isUseSSL)
{
    g_return_if_fail(self);

    MailSessionPrivate* priv = MAIL_SESSION_PRIVATE(self);

    priv->mIsUseSSL = isUseSSL;
}

void mail_session_set_username(MailSession *self, const char *username, GError **error)
{
    g_return_if_fail(self && username);

    MailSessionPrivate* priv = MAIL_SESSION_PRIVATE(self);

    if (priv->mUserName) {
        g_free (priv->mUserName);
    }

    priv->mUserName = g_strdup(username);
    if (!priv->mUserName) {
        mail_error_get_error_info (MAIL_ERROR_OUT_OF_MEMORY, error);
    }
}

void mail_session_set_password(MailSession *self, const char *password, GError **error)
{
    g_return_if_fail(self && password);

    MailSessionPrivate* priv = MAIL_SESSION_PRIVATE(self);

    if (priv->mPassword) {
        g_free (priv->mPassword);
    }

    priv->mPassword = g_strdup(password);
    if (!priv->mPassword) {
        mail_error_get_error_info (MAIL_ERROR_OUT_OF_MEMORY, error);
    }
}

void mail_session_connect(MailSession *self, GError **error)
{
    g_return_if_fail(self);

    MailSessionClass* klass = MAIL_SESSION_GET_CLASS(self);

    g_return_if_fail(MAIL_SESSION_IS_CLASS (klass));

    if (klass->connectServer) {
        klass->connectServer(self, error);
        return;
    }

    g_autoptr(GError) errorT = NULL;
    MailSessionPrivate* priv = MAIL_SESSION_PRIVATE(self);

    if (priv->mPort) {
        priv->mPort = 143;
    }

    // get ip
    GList* ipList = g_resolver_lookup_by_name (priv->mResolver, priv->mHostName, NULL, &errorT);
    if (errorT) {
        LOG_ERROR("ip lookup error: %s", errorT->message);
        mail_error_get_error_info (MAIL_ERROR_DOMAIN_FAIL, error);
        return;
    }

    g_autoptr(GInetAddress) inetTcp = NULL;
    for (GList* l = ipList; l; l = l->next) {
        inetTcp = g_object_ref(l->data);
        if (inetTcp) {
            LOG_DEBUG("");
            break;
        }
    }
    // TODO:// check ip is available
    g_resolver_free_addresses (ipList);

    // get socket
    if (priv->mSocket) {
        g_object_unref (priv->mSocket);
    }

    priv->mSocket = g_socket_new (G_SOCKET_FAMILY_IPV4, G_SOCKET_TYPE_STREAM, G_SOCKET_PROTOCOL_DEFAULT, &errorT);
    if (NULL == priv->mSocket) {
        if (errorT) {
            LOG_ERROR("g_socket_new error: %s", errorT->message);
        }
        mail_error_get_error_info (MAIL_ERROR_OUT_OF_MEMORY, error);
        return;
    }

    g_autoptr(GSocketAddress) address = g_inet_socket_address_new (inetTcp, priv->mPort);
    if (NULL == address) {
        mail_error_get_error_info (MAIL_ERROR_DOMAIN_FAIL, error);
        return;
    }

//    g_socket_set_ttl (priv->mSocket, priv->mIsUseSSL);

    if (!g_socket_connect (priv->mSocket, G_SOCKET_ADDRESS(address), NULL, &errorT)) {
        if (errorT) {
            LOG_ERROR("g_socket_connect error: %s", errorT->message);
        }
        mail_error_get_error_info (MAIL_ERROR_CONNECT_FAIL, error);
        return;
    }

    if (!g_socket_check_connect_result (priv->mSocket, &errorT)) {
        LOG_ERROR("g_socket_check_connect_result error: %s", errorT->message);
        mail_error_get_error_info (MAIL_ERROR_CONNECT_FAIL, error);
        return;
    }
    else {
        LOG_DEBUG("connect to server OK!");
    }
}

void mail_session_login(MailSession *self, GError **error)
{
    g_return_if_fail(self);

    MailSessionPrivate* priv = MAIL_SESSION_PRIVATE(self);

    g_autoptr(GError) errorT = NULL;
    GByteArray* readBuf = g_byte_array_new();

    gsize rLen = 0;
    gchar buf[1024] = {0};
    while (true) {
        rLen = g_socket_receive_with_blocking (priv->mSocket, buf, sizeof (buf) - 1, true, NULL, &errorT);
        if (rLen > 0) {
            readBuf = g_byte_array_append (readBuf, buf, rLen);
        }

        if (rLen < sizeof (buf)) {
            break;
        }
    }
    g_autofree gchar* readData = (gchar*) g_byte_array_free (readBuf, false);

    LOG_DEBUG("%s", readData);
}
