//
// Created by dingjing on 23-9-15.
//

#ifndef LIBRARY_MAIL_MESSAGE_H
#define LIBRARY_MAIL_MESSAGE_H

#include <stdbool.h>
#include <glib-object.h>

G_BEGIN_DECLS

struct MailSession;

#define MAIL_TYPE_MESSAGE               (mail_message_get_type())

G_DECLARE_INTERFACE(MailMessage, mail_message, MAIL, MESSAGE, GObject)

/**
 * @brief
 *  这个接口主要规定了用户对邮箱的各种操作，以供 MailSession 使用
 *
 *  用户操作：
 *      1. 登录(包含密码验证)
 *      2. 获取邮箱文件夹
 *      3. 选择邮箱
 *      4. 拉取所有邮件
 *      5. 选中邮件并获取邮件内容
 *      6. 下载邮件附件
 *      7. 注销
 */

struct _MailMessageInterface
{
    GTypeInterface  parent;

    bool    (*login)        (MailMessage* self, struct MailSession* session);           // 用户登录操作
    bool    (*select)       (MailMessage* self, struct MailSession* session);           // 选择邮箱操作
    bool    (*logout)       (MailMessage* self, struct MailSession* session);           // 注销操作
    bool    (*noop)         (MailMessage* self, struct MailSession* session);           // noop 操作
    bool    (*fetch)        (MailMessage* self, struct MailSession* session);           // 拉取邮件
};





G_END_DECLS

#endif //LIBRARY_MAIL_MESSAGE_H
