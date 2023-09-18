//
// Created by dingjing on 23-9-15.
//

#include "mail-message-manager.h"

// 默认支持的邮件协议
#include "imap/imap-message.h"


#define MESSAGE_MANAGER_PRIVATE(O)              message_manager_get_instance_private(O)

typedef struct MessageManagerPrivate MessageManagerPrivate;

struct MessageManager
{
    GObject             parent;
};

struct MessageManagerClass
{
    GObjectClass        parentClass;
};

struct MessageManagerPrivate
{
    GHashTable*         mHashTable;
};

G_DEFINE_FINAL_TYPE_WITH_CODE(MessageManager, message_manager, G_TYPE_OBJECT, G_ADD_PRIVATE(MessageManager))

static void message_manager_finalize (GObject* obj)
{
    MessageManagerPrivate* priv = MESSAGE_MANAGER_PRIVATE((MessageManager*) obj);

    if (priv->mHashTable)       g_hash_table_unref (priv->mHashTable);
}

static void message_manager_init (MessageManager* self)
{
    MessageManagerPrivate* priv = MESSAGE_MANAGER_PRIVATE((MessageManager*) self);

    priv->mHashTable = g_hash_table_new_full (g_int_hash, g_int_equal, NULL, g_object_unref);

    // 添加默认支持的协议
    mail_message_manager_register (self, MAIL_MESSAGE_PARSER_IMAP_V4, IMAP_TYPE_MESSAGE);
}

static void message_manager_class_init (MessageManagerClass* klass)
{
    GObjectClass* bClass = G_OBJECT_CLASS(klass);

    bClass->finalize = message_manager_finalize;
}

MessageManager* mail_message_manager_instance()
{
    static gsize initialized = 0;
    static MessageManager* instance = NULL;

    if (g_once_init_enter(&initialized)) {
        if (!instance) {
            instance = g_object_new (MESSAGE_TYPE_MANAGER, NULL);
        }
        g_once_init_leave(&initialized, 1);
        g_assert(instance);
    }

    return instance;
}

void mail_message_manager_register(MessageManager* self, MailMessageParserType type, GType gType)
{
    g_return_if_fail(self);

    MessageManagerPrivate* priv = MESSAGE_MANAGER_PRIVATE((MessageManager*) self);

    g_hash_table_insert (priv->mHashTable, (void*) type, g_object_new(gType, NULL));
}
