//
// Created by dingjing on 23-9-15.
//

#ifndef LIBRARY_MAIL_SESSION_PRIVATE_H
#define LIBRARY_MAIL_SESSION_PRIVATE_H
#include "mail-session.h"


/**
 * @brief 检测是否与服务器连接成功
 *
 * @param self      ..
 *
 * @return
 */
bool mail_session_private_is_connected(MailSession* self);

/**
 * @brief
 *  连接到服务器之后，登录之前调用，检测支持的协议类型
 *
 * @param self      ..
 *
 * @note 客户端第一次与服务端连接时候使用
 *
 * @return
 *  解析成功返回 true
 *  解析失败返回 false
 */
bool mail_session_private_before_login_parse_proto_type(MailSession* self);

/**
 * @brief
 *  连接到服务器
 *
 * @return
 *  成功返回 true
 *  失败返回 false
 */
bool mail_session_private_connect(MailSession* self);

/**
 * @brief
 *  从服务器读取数据到缓存区，读取的数据不可以释放
 *
 * @param self      ..
 * @param dataLen   返回读取数据的长度，必须传入，否则返回空字符串
 *
 * @return
 *  返回读取数据的首地址
 */
const char* mail_session_private_read_data(MailSession* self, gsize* dataLen/* out, must */);

/**
 * @brief
 *  从服务端读取数据到缓存区
 *
 * @param self      ..
 *
 * @return
 *  成功: 返回true
 *  失败: 返回false
 */
bool mail_session_private_read_data_to_buffer(MailSession* self);

/**
 * @brief
 *  从读缓存区获取数据
 *
 * @note 并不会从服务器读取数据
 *
 * @param self
 * @param dataLen   返回数据的长度
 *
 * @return 返回数据首地址
 */
const char* mail_session_private_get_read_data (MailSession* self, gsize* dataLen/* out, must */);

/**
 * @brief 从缓存区读取数据
 *
 * @note 并不会从服务器读取数据，另外一定注意，必须是可见字符串才能用此函数，如果是二进制则会出错
 *
 * @param self
 *
 * @return 返回数据首地址
 */
const char* mail_session_private_get_read_string (MailSession* self);



#endif //LIBRARY_MAIL_SESSION_PRIVATE_H
