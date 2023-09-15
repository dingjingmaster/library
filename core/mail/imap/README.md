# IMAP 

IMAP(Internet Message Access Protocol)，互联网消息访问协议允许客户端访问和操作服务器上的电子邮件消息。

IMAP 是基于可靠的数据流（例如：TCP），端口默认 143。


|IMAP版本|协议信息|
|----|----|
|IMAP v4|`https://datatracker.ietf.org/doc/rfc3501/`|

## IMAP 4rev1

IMAP 4rev1 包括以下操作： 创建、删除和重命名邮箱、检查新消息、永久删除消息、设置和清除标志、RFC 2822和RFC 2045解析、搜索和选择性获取消息属性、文本及其部分。

IMAP4rev1中的消息是通过使用数字访问的。这些数字要么是消息序列号，要么是唯一标识符。

IMAP 4rev1支持单个服务器。RFC 2244中讨论了访问配置信息以支持多个IMAP 4rev1服务器的机制。

IMAP4rev1没有指定邮寄邮件的方式;此功能由邮件传输协议(如RFC 2821)处理。

## IMAP 状态与流程图

```

                   +----------------------+
                   |connection established|
                   +----------------------+
                              ||
                              \/
            +--------------------------------------+
            |          server greeting             |
            +--------------------------------------+
                      || (1)       || (2)        || (3)
                      \/           ||            ||
            +-----------------+    ||            ||
            |Not Authenticated|    ||            ||
            +-----------------+    ||            ||
             || (7)   || (4)       ||            ||
             ||       \/           \/            ||
             ||     +----------------+           ||
             ||     | Authenticated  |<=++       ||
             ||     +----------------+  ||       ||
             ||       || (7)   || (5)   || (6)   ||
             ||       ||       \/       ||       ||
             ||       ||    +--------+  ||       ||
             ||       ||    |Selected|==++       ||
             ||       ||    +--------+           ||
             ||       ||       || (7)            ||
             \/       \/       \/                \/
            +--------------------------------------+
            |               Logout                 |
            +--------------------------------------+
                              ||
                              \/
                +-------------------------------+
                |both sides close the connection|
                +-------------------------------+
```

### Not Authenticated 状态

未验证状态是客户端启动时候进入的状态，没有提供身份验证，因此没有办法执行大多数命令。

### Authenticated 状态

已验证状态，客户端已经经过身份验证，并且必须在允许影响消息命令之前选择要访问的邮箱。

当启动预身份验证连接、提供了可接受的身份验证凭证、选择邮箱出错或者成功执行 CLOSE 命令之后进入此状态。

### Selected 状态

选择状态，已选择要访问的邮箱，当成功选择邮箱时候进入此状态。

### Logout 状态

注销状态，连接将被终止。此状态可作为客户机请求的结果进入，也可通过客户机或服务器的单方面操作进入。

如果客户端请求注销状态，服务器必须在服务器关闭连接之前向注销命令发送一个未标记的 BYE 响应和一个标记的OK响应；在客户端关闭连接之前，客户端必须读取对 LOGOUT 命令的标记 OK 响应。

服务端绝不能在没有发送未标记 BYE 的情况下单方面关闭连接，该响应包含这样做的原因。客户端不应该单方面关闭连接，而应该发出 LOGOUT 命令，如果服务器检测到客户端单方面关闭了连接，服务器可能会忽略标记的 BYE 响应，并简单关闭其连接。


## IMAP 协议规范

### 说明

- "C:" 和 "S:"：分别表示客户端和服务端发送的内容
- "Connection"：表示从最初建立网络连接到终止网络连接的整个C/S交互
- "Session"：表示从一个邮箱被选中(SELECT或inspect命令)到选择结束、连接终止，C/S交互顺序
- 字符串默认使用 7-bit US-ASCII，除非指定使用特殊字符集，这个有需要的时候后续会讲到
- 一个完整的交互过程由三部分组成：客户端的请求命令 + 服务端的返回数据 + 服务端的结果响应
- 客户端和服务端之间的传输交互都是行形式，最终都以 CRLF 字符串结尾
- 客户端命令通常都以一个简短的字符串作为前缀，称为“标签”（例如：A0001，A0002）。
- 在两种情况下，来自客户端的一行不代表完整的命令。1. 命令参数用八位字节计数引号括起来（参见String中的文字描述）；2. 命令需要服务器反馈。
- 如果服务器准备好接收命令的其余部分，则发送命令延续请求响应。此响应以标记 "+" 作为前缀。
- 如果服务器检测到命令中存在错误则发送 "BAD" 标志
- 服务器向客户端传输的数据和不指示命令完成的状态以 "*" 为前缀，标为未标记响应。
- 服务器完成结果响应指示操作的成功或失败，此标记与客户端发给服务端标记保持一致。最终结果有三种状态：OK(成功)，NO(失败)，BAD(协议错误)
- 客户端从服务器读取响应行，然后它根据响应的第一个标志对响应采取操作，该标记可以是：tag、"*"、"+"

|命令前缀|说明|
|----|----|
|`*`| 未标记响应，无须客户端回复/交互 |
|`+`| 延续请求响应，服务端只发一个 `+` 后续客户端可以继续发送数据 |
|tag| 请求的标签 |


> 注意：
>   客户端需要严格遵循规范中的语法，发送多余或缺失的空格、参数都会导致语法错误。

### 消息属性

除了消息文本外，每个消息还有几个与之关联的属性。这些属性可以单独检索，也可以与其它属性或消息文本一起检索。

#### 消息编号

IMAP4 rev1 中通过两个数字之一进行访问消息，一个是消息的序列号一个是消息的唯一标识符(Unique Identifer 即：UID)。

每条消息分配一个32位值，当与UID一起使用就形成了一个唯一的 64 位值。UID在邮箱中以严格升序分配，但是不一定连续，而消息序列号是连续的。

消息的UID在会话期间和会话之间是不能改变的。UID的任何改变都必须使用 UIDAVALIDITY 机制检测到。

消息的序列号在会话期间可以重新分配。例如：当从邮箱中永久删除消息时候，所有后续消息的序列号将递减。类似的，给新消息分配的序列号也可以是被其它消息持有过的。

#### 消息标志

所有的标志都是预定义且以"\"开头的，具体如下：
|标志|说明|
|----|----|
|`\Seen`|消息已被阅读|
|`\Answered`|消息已被回复|
|`\Flagged`|消息已被标记为紧急或者特别紧急|
|`\Deleted`|消息已被删除|
|`\Draft`|消息被标记为“草稿”|
|`\Recent`|消息是最近到达邮箱的。此会话是收到此消息通知的第一个会话|

服务器允许客户端在邮箱中定义新的关键字（更多信息参考 PERMANENTFLAGS 响应代码的描述）

#### 消息的日期属性

服务器上消息的时间反映消息接收日期和时间，是由 SMTP 定义的消息传递日期和时间

#### 消息的大小属性

RFC-2822

#### 消息结构属性

#### 消息的正文结构属性

[MIME-IMB]

### 消息文本

[RFC-2822]

初次 IMAP4 rev1 还允许获取完整消息文本的一部分（[RFC-2822]消息头、[RFC-2822]消息体、[MIME-IMB]消息体或[MIME-IMB]消息头）

## 数据格式

IMAP 4rev1 使用文本命令和响应，其数据可以采用以下几种形式：
- Atom
- 数字
- 字符串
- 列表
- NIL

### Atom

由一个或多个非特殊字符组成

### 数字

表示一个数值

### String

字符串由两种形式：字面值或带引号的字符串。

字面形式是字符串的一般形式，带引号的字符串形式是一种替代方法，它避免了以可能使用的字符串限制为代价来处理字面量的开销。

字面量是一个由0个或多个八位元组（包括CR和LF）组成的序列，前缀加上一个八位元组计算，形式为：`左大括号("{")、八位元组数、右大括号("}") 和 CRLF`。

对于从服务器传输到客户机的字面值，CRLF 后面紧跟着八位字节数据。在字面量从客户端传输到服务器的情况下，客户端必须在发送octet数据(和命令的其它部分)之前等待接收命令延续请求。

引号字符串是由0个或多个七位字符组成的序列，不包含 CR和LF，每个字符串的末尾都有双引号(")字符。

空字符串要么表示为""(双引号之间包含0个字符的引号字符串)，要么表示为{0}后根CRLF

#### 二进制字符

通过使用[MIME-IMB]内容传输编码支持 8 位文本和二进制邮件。

IMAP4 rev1 设置了[CHARSET]且被识别的情况下，可以实现在字面量中传输8位或多8位字符。

虽然定义了BINARY主体编码，但是不允许使用未编码的二进制字符串。"二进制字符串"是任务带有NUL字符的字符串。实现必须在传输数据之前将二进制数据编码成文本形式，例如BASE64。具有多个 CTL 字符的字符串也可以被认为是二进制的。

#### 列表

数据结构表示为“圆括号列表”另一种数据项序列，用空格分割，两端用圆括号隔开。一个圆括号可以包含其它圆括号列表，使用多级圆括号表示嵌套。

空列表表示为()

#### NIL

特殊形式 NIL 表示不存在以字符串或圆括号列表表示的特定数据项，与空字符串""或空列表() 不同。

## 操作中的注意事项

邮箱名一定是 7-bit 的字符串，服务端应该禁止 8-bit 的邮箱名，否则应该当成 UTF-8 处理

INBOX(不区分大小写)，用于表示“此用户在此服务器上的主邮箱”，所有其它名称的解释都依赖于此。

### 创建邮箱时候要考虑的事项

...

## 命令

### CAPABILITY

请求服务器支持的功能列表

```
Arguments:  none

Responses:  REQUIRED untagged response: CAPABILITY

Result:     OK - capability completed
            BAD - command unknown or arguments invalid
```

例子：

```
C: abcd CAPABILITY
S: * CAPABILITY IMAP4rev1 STARTTLS AUTH=GSSAPI
   LOGINDISABLED
S: abcd OK CAPABILITY completed
C: efgh STARTTLS
S: efgh OK STARTLS completed
   <TLS negotiation, further commands are under [TLS] layer>
C: ijkl CAPABILITY
S: * CAPABILITY IMAP4rev1 AUTH=GSSAPI AUTH=PLAIN
S: ijkl OK CAPABILITY completed
```

### NOOP 

空请求，相当于心跳机制、定期轮询新消息或状态更新

```
Arguments:  none

Responses:  no specific responses for this command (but see below)

Result:     OK - noop completed
            BAD - command unknown or arguments invalid
```

例子：
```
C: a002 NOOP
S: a002 OK NOOP completed
   . . .
C: a047 NOOP
S: * 22 EXPUNGE
S: * 23 EXISTS
S: * 3 RECENT
S: * 14 FETCH (FLAGS (\Seen \Deleted))
S: a047 OK NOOP completed
```

### LOGOUT 

注销

```
Arguments:  none

Responses:  REQUIRED untagged response: BYE

Result:     OK - logout completed
            BAD - command unknown or arguments invalid
```

例子：
```
C: A023 LOGOUT
S: * BYE IMAP4rev1 Server logging out
               S: A023 OK LOGOUT completed
               (Server and client then close the connection)
```

## 客户端命令 —— Not Authenticated State

在此未认证状态下，可以使用 AUTHENTICATE 或者 LOGIN 命令建立身份验证并进入身份验证状态。

AUTHENTICATE 命令为各种身份验证技术、隐私保护和完整性检查提供了通用机制；

LOGIN 命令使用传统的用户名和明文密码对，没有办法建立隐私保护或完整性检查。

STARTTLS 命令是建立会话隐私保护和完整性检查的另一种形式，但不建立身份验证或进入身份验证状态。

服务器实现可能允许访问某些邮箱而不建立身份验证。这可以通过[ANONYMOUS]中描述的匿名[SASL]身份验证器来完成。旧的约定是使用用户id“anonymous”的LOGIN命令;在这种情况下，需要密码，尽管服务器可以选择接受任何密码。对匿名用户的限制取决于具体实现。

一旦通过身份验证(包括匿名)，就不可能重新进入未经过身份验证的状态。

除了通用命令(CAPABILITY、NOOP和LOGOUT)之外，以下命令在未经过身份验证的状态下也有效:STARTTLS、AUTHENTICATE和LOGIN。

### 命令 STARTTLS 

[TLS]协商在CRLF之后立即开始，在服务器标记的OK响应结束时开始。一旦客户端发出STARTTLS命令，在看到服务器响应和[TLS]协商完成之前，它绝对不能发出进一步的命令。

服务器保持在未经身份验证的状态，即使在[TLS]协商期间提供了客户端凭证。这并不排除像EXTERNAL(在[SASL]中定义)这样的身份验证机制使用由[TLS]协商确定的客户端身份。

一旦[TLS]启动，客户端必须丢弃关于服务器能力的缓存信息，并且应该重新发出CAPABILITY命令。这对于防止中间人攻击是必要的，中间人攻击会在STARTTLS之前更改功能列表。服务器可以在STARTTLS之后发布不同的功能。

```
Arguments:  none

Responses:  no specific response for this command

Result:     OK - starttls completed, begin TLS negotiation
            BAD - command unknown or arguments invalid
```

例子：
```
C: a001 CAPABILITY
S: * CAPABILITY IMAP4rev1 STARTTLS LOGINDISABLED
S: a001 OK CAPABILITY completed
C: a002 STARTTLS
S: a002 OK Begin TLS negotiation now
   <TLS negotiation, further commands are under [TLS] layer>
C: a003 CAPABILITY
S: * CAPABILITY IMAP4rev1 AUTH=PLAIN
S: a003 OK CAPABILITY completed
C: a004 LOGIN joe password
S: a004 OK LOGIN completed
```

### 命令 AUTHENTICATE 

AUTHENTICATE命令表示对服务器采用[SASL]认证机制。如果服务器支持所请求的身份验证机制，它将执行身份验证协议交换以对客户机进行身份验证和标识。它也可以为后续协议交互协商一个可选的安全层。如果请求的认证机制不被支持，服务器应该通过发送一个带标签的NO响应来拒绝AUTHENTICATE命令。

身份验证协议交换由一系列特定于身份验证机制的服务器请求和客户端响应组成。服务器质询由命令延续请求响应组成，该响应带有“+”令牌，后跟一个BASE64编码的字符串。客户端响应由一行BASE64编码字符串组成。如果客户端希望取消身份验证交换，它会发出由单个“*”组成的一行。如果服务器收到这样的响应，它必须通过发送一个带标签的BAD响应来拒绝AUTHENTICATE命令。

如果安全层是通过[SASL]身份验证交换协商的，则在客户端结束身份验证交换的CRLF和服务器标记OK响应的CRLF之后立即生效。

虽然客户端和服务器实现必须实现AUTHENTICATE命令本身，但它不需要实现除[IMAP-TLS]中描述的PLAIN机制之外的任何身份验证机制。此外，不需要身份验证机制来支持任何安全层。

服务器和客户端可以支持多种身份验证机制。服务器应该在对CAPABILITY命令的响应中列出其支持的身份验证机制，以便客户端知道使用哪种身份验证机制。

服务器可以在成功的AUTHENTICATE命令的标记OK响应中包含CAPABILITY响应代码，以便自动发送功能。如果客户机能够识别这些自动功能，则无需发送单独的CAPABILITY命令。只有在AUTHENTICATE命令没有协商安全层时才应该这样做，因为作为AUTHENTICATE命令一部分的标记OK响应不受加密/完整性检查的保护。在这种情况下，[SASL]要求客户端重新发出CAPABILITY命令。

如果一个AUTHENTICATE命令失败并返回NO响应，客户端可以通过发出另一个AUTHENTICATE命令来尝试另一个身份验证机制。它也可以尝试使用LOGIN命令进行身份验证。换句话说，客户端可以按优先级递减的顺序请求身份验证类型，而LOGIN命令是最后的手段。

在身份验证交换期间从客户端传递到服务器的授权标识被服务器解释为客户端请求其特权的用户名。

```
Arguments:  authentication mechanism name

Responses:  continuation data can be requested

Result:     OK - authenticate completed, now in authenticated state
            NO - authenticate failure: unsupported authentication
                 mechanism, credentials rejected
            BAD - command unknown or arguments invalid,
                  authentication exchange cancelled
```
例子：
```
S: * OK IMAP4rev1 Server
C: A001 AUTHENTICATE GSSAPI
S: +
C: YIIB+wYJKoZIhvcSAQICAQBuggHqMIIB5qADAgEFoQMCAQ6iBw
   MFACAAAACjggEmYYIBIjCCAR6gAwIBBaESGxB1Lndhc2hpbmd0
   b24uZWR1oi0wK6ADAgEDoSQwIhsEaW1hcBsac2hpdmFtcy5jYW
   Mud2FzaGluZ3Rvbi5lZHWjgdMwgdCgAwIBAaEDAgEDooHDBIHA
   cS1GSa5b+fXnPZNmXB9SjL8Ollj2SKyb+3S0iXMljen/jNkpJX
   AleKTz6BQPzj8duz8EtoOuNfKgweViyn/9B9bccy1uuAE2HI0y
   C/PHXNNU9ZrBziJ8Lm0tTNc98kUpjXnHZhsMcz5Mx2GR6dGknb
   I0iaGcRerMUsWOuBmKKKRmVMMdR9T3EZdpqsBd7jZCNMWotjhi
   vd5zovQlFqQ2Wjc2+y46vKP/iXxWIuQJuDiisyXF0Y8+5GTpAL
   pHDc1/pIGmMIGjoAMCAQGigZsEgZg2on5mSuxoDHEA1w9bcW9n
   FdFxDKpdrQhVGVRDIzcCMCTzvUboqb5KjY1NJKJsfjRQiBYBdE
   NKfzK+g5DlV8nrw81uOcP8NOQCLR5XkoMHC0Dr/80ziQzbNqhx
   O6652Npft0LQwJvenwDI13YxpwOdMXzkWZN/XrEqOWp6GCgXTB
   vCyLWLlWnbaUkZdEYbKHBPjd8t/1x5Yg==
S: + YGgGCSqGSIb3EgECAgIAb1kwV6ADAgEFoQMCAQ+iSzBJoAMC
   AQGiQgRAtHTEuOP2BXb9sBYFR4SJlDZxmg39IxmRBOhXRKdDA0
   uHTCOT9Bq3OsUTXUlk0CsFLoa8j+gvGDlgHuqzWHPSQg==
C:
S: + YDMGCSqGSIb3EgECAgIBAAD/////6jcyG4GE3KkTzBeBiVHe
   ceP2CWY0SR0fAQAgAAQEBAQ=
C: YDMGCSqGSIb3EgECAgIBAAD/////3LQBHXTpFfZgrejpLlLImP
   wkhbfa2QteAQAgAG1yYwE=
S: A001 OK GSSAPI authentication successful
```

> 注意:
> 服务器和客户端响应中的换行符是为了编辑清晰，而不是在真实的验证环节中。

### 命令 LOGIN

LOGIN命令将客户端标识到服务器，并携带验证该用户的明文密码。

服务器可以在对成功的LOGIN命令的标记OK响应中包含CAPABILITY响应代码，以便自动发送功能。如果客户机能够识别这些自动功能，则无需发送单独的CAPABILITY命令。

```
Arguments:  user name
            password

Responses:  no specific responses for this command

Result:     OK - login completed, now in authenticated state
            NO - login failure: user name or password rejected
            BAD - command unknown or arguments invalid
```

例子：

```
Example:    C: a001 LOGIN SMITH SESAME
            S: a001 OK LOGIN completed
```
