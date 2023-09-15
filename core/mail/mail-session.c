//
// Created by dingjing on 23-9-14.
//

#include "mail-session.h"

#include <stdio.h>
#include <gio/gio.h>

#include "mail-error.h"
#include "../common/log.h"
#include "mail-session-private.h"
#include "interface/mail-message.h"
#include "mail-message-parser-manager.h"


typedef struct MailSessionPrivate  MailSessionPrivate;

struct MailSessionPrivate
{
    bool                mIsUseSSL;
    guint16             mPort;
    GResolver*          mResolver;
    GSocket*            mReceiveSocket;

    gchar*              mHostName;
    gchar*              mUserName;
    gchar*              mPassword;

    MailMessageParser   mProtoType;

    GBytes*             mReadBuffer;
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

    if (priv->mHostName)        g_free (priv->mHostName);
    if (priv->mUserName)        g_free (priv->mUserName);
    if (priv->mPassword)        g_free (priv->mPassword);
    if (priv->mResolver)        g_object_unref (priv->mResolver);
    if (priv->mReadBuffer)      g_object_unref (priv->mReadBuffer);
    if (priv->mReceiveSocket)   g_object_unref (priv->mReceiveSocket);
}

static void mail_session_init (MailSession* self)
{
    MailSessionPrivate* priv = MAIL_SESSION_PRIVATE(self);

    priv->mPort = 0;
    priv->mIsUseSSL = false;

    priv->mHostName = NULL;
    priv->mUserName = NULL;
    priv->mPassword = NULL;
    priv->mReadBuffer = NULL;
    priv->mReceiveSocket = NULL;
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

    if (!mail_session_private_connect (self)) {
        mail_error_get_error_info (MAIL_ERROR_CONNECT_FAIL, error);
        return;
    }
}

void mail_session_login(MailSession *self, GError **error)
{
    g_return_if_fail(self);

    MailSessionPrivate* priv = MAIL_SESSION_PRIVATE(self);

    // 1. 如果服务器没有连接成功，则重新连接服务器
    if (!mail_session_private_is_connected (self)) {
        mail_session_connect (self, error);
        if (error) {
            return;
        }
    }

    // 2. 获取服务器支持的协议
    if (!mail_session_private_read_data_to_buffer (self)) {
        mail_error_get_error_info (MAIL_ERROR_RECEIVE_DATA, error);
        return;
    }

    // 3. 解析服务器支持的协议
    if (!mail_session_private_before_login_parse_proto_type (self)) {
        mail_error_get_error_info (MAIL_ERROR_PARSE_DATA, error);
        return;
    }

}

/**
 * @NOTE:// private
 */
bool mail_session_private_before_login_parse_proto_type(MailSession* self)
{
    g_return_val_if_fail(self, false);

    MailSessionPrivate* priv = MAIL_SESSION_PRIVATE(self);

    g_return_val_if_fail(priv->mReadBuffer, false);

    const char* data = mail_session_private_get_read_string (self);

    char protoBuf[16] = {0};

    int ret = sscanf (data, "* OK %s ready\r\n", protoBuf);
    LOG_DEBUG("proto: %s", protoBuf);

    if (0 == g_ascii_strcasecmp (protoBuf, "IMAP4")) {
        priv->mProtoType = MAIL_MESSAGE_PARSER_IMAP_V4;
    }
    else {
        priv->mProtoType = MAIL_MESSAGE_PARSER_UNKNOWN;
    }

    return (ret >= 0);
}

bool mail_session_private_is_connected(MailSession* self)
{
    g_return_val_if_fail(self, false);

    MailSessionPrivate* priv = MAIL_SESSION_PRIVATE(self);

    return g_socket_check_connect_result (priv->mReceiveSocket, NULL);
}

bool mail_session_private_read_data_to_buffer(MailSession* self)
{
    g_return_val_if_fail(self, false);

    MailSessionPrivate* priv = MAIL_SESSION_PRIVATE(self);

    if (priv->mReadBuffer) {
        g_object_unref (priv->mReadBuffer);
        priv->mReadBuffer = NULL;
    }

    g_autoptr(GError) errorT = NULL;

    GByteArray* readBuf = g_byte_array_new();

    gsize rLen = 0;
    guchar buf[1024] = {0};

    while (true) {
        rLen = g_socket_receive_with_blocking (priv->mReceiveSocket, (gchar*) buf, sizeof (buf) - 1, true, NULL, &errorT);
        if (rLen > 0) {
            readBuf = g_byte_array_append (readBuf, buf, rLen);
        }

        if (rLen < sizeof (buf)) {
            break;
        }
    }

    priv->mReadBuffer = g_byte_array_free_to_bytes (readBuf);

    return (g_bytes_get_size (priv->mReadBuffer) > 0);
}

const char* mail_session_private_get_read_data (MailSession* self, gsize* dataLen/* out, must */)
{
    g_return_val_if_fail(self && dataLen, NULL);

    MailSessionPrivate* priv = MAIL_SESSION_PRIVATE(self);

    return g_bytes_get_data (priv->mReadBuffer, dataLen);
}

const char* mail_session_private_get_read_string (MailSession* self)
{
    gsize size = 0;

    return mail_session_private_get_read_data (self, &size);
}

const char* mail_session_private_read_data(MailSession* self, gsize* dataLen/* out, must */)
{
    g_return_val_if_fail(self && dataLen, NULL);

    MailSessionPrivate* priv = MAIL_SESSION_PRIVATE(self);

    if (!mail_session_private_read_data_to_buffer (self)) {
        return NULL;
    }

    return g_bytes_get_data (priv->mReadBuffer, dataLen);
}

bool mail_session_private_connect(MailSession* self)
{
    g_return_val_if_fail(self, false);

    g_autoptr(GError) errorT = NULL;

    MailSessionPrivate* priv = MAIL_SESSION_PRIVATE(self);

    if (0 == priv->mPort) {
        priv->mPort = 143;
    }

    // get ip
    GList* ipList = g_resolver_lookup_by_name (priv->mResolver, priv->mHostName, NULL, &errorT);
    if (errorT) {
        LOG_ERROR("ip lookup error: %s", errorT->message);
        goto out;
    }

    // get socket
    if (priv->mReceiveSocket) {
        g_object_unref (priv->mReceiveSocket);
        priv->mReceiveSocket = NULL;
    }

    for (GList* l = ipList; l; l = l->next) {
        g_autoptr(GInetAddress) inetTcp = g_object_ref(l->data);

        priv->mReceiveSocket = g_socket_new (G_SOCKET_FAMILY_IPV4, G_SOCKET_TYPE_STREAM, G_SOCKET_PROTOCOL_DEFAULT, &errorT);
        if (errorT) {
            LOG_ERROR("g_socket_new error: %s", errorT->message);
            continue;
        }

        g_autoptr(GSocketAddress) address = g_inet_socket_address_new (inetTcp, priv->mPort);
        if (NULL == address) {
            LOG_WARNING("g_inet_socket_address_new error!");
            continue;
        }

        g_socket_connect (priv->mReceiveSocket, G_SOCKET_ADDRESS(address), NULL, &errorT);
        if (errorT) {
            LOG_ERROR("g_socket_connect error: %s", errorT->message);
            continue;
        }

        if (g_socket_check_connect_result (priv->mReceiveSocket, &errorT)) {
            break;
        }
    }

out:
    if (ipList) {
        g_resolver_free_addresses (ipList);
    }

    if (G_IS_SOCKET(priv->mReceiveSocket) && g_socket_check_connect_result (priv->mReceiveSocket, &errorT)) {
        LOG_DEBUG("connect to server OK!");
        return true;
    }

    return false;
}

