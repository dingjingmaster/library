#include "log.h"

#include <time.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>
#include <sys/uio.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <execinfo.h>

#define FG_BLACK                    30
#define FG_RED                      31
#define FG_GREEN                    32
#define FG_YELLOW                   33
#define FG_BLUE                     34
#define FG_MAGENTA                  35
#define FG_CYAN                     36
#define FG_WHITE                    37
#define BG_BLACK                    40
#define BG_RED                      41
#define BG_GREEN                    42
#define BG_YELLOW                   43
#define BG_BLUE                     44
#define BG_MAGENTA                  45
#define BG_CYAN                     46
#define BG_WHITE                    47
#define B_RED(str)                  "\033[1;31m" str "\033[0m"
#define B_GREEN(str)                "\033[1;32m" str "\033[0m"
#define B_YELLOW(str)               "\033[1;33m" str "\033[0m"
#define B_BLUE(str)                 "\033[1;34m" str "\033[0m"
#define B_MAGENTA(str)              "\033[1;35m" str "\033[0m"
#define B_CYAN(str)                 "\033[1;36m" str "\033[0m"
#define B_WHITE(str)                "\033[1;37m" str "\033[0m"
#define RED(str)                    "\033[31m" str "\033[0m"
#define GREEN(str)                  "\033[32m" str "\033[0m"
#define YELLOW(str)                 "\033[33m" str "\033[0m"
#define BLUE(str)                   "\033[34m" str "\033[0m"
#define MAGENTA(str)                "\033[35m" str "\033[0m"
#define CYAN(str)                   "\033[36m" str "\033[0m"
#define WHITE(str)                  "\033[37m" str "\033[0m"

#define LOG_TIME_MAX                (64)
#define LOG_IOVEC_MAX               (32)
#define LOG_MAX_SIZE                (1024 * 1024 * 500)

static gsize                        gInit = 0;
int                                 gLogFd = -1;
pthread_mutex_t                     gLogLock = PTHREAD_MUTEX_INITIALIZER;
char*                               gLogPath = "/tmp/library.log";

static int _open_file ();
static ssize_t _log_write (struct iovec *vec, int n);
static void _log_get_time(char *str);

GLogWriterOutput log_handler (GLogLevelFlags level, const GLogField* fields, gsize nFields, gpointer udata)
{
    (void) udata;

    const char *logLevel = NULL;

    g_autofree char *msg = NULL;
    g_autofree char *line = NULL;
    const char *file = NULL;
    const char *func = NULL;
    struct iovec vec[LOG_IOVEC_MAX];
    char s_time[LOG_TIME_MAX] = {0};
    _log_get_time (s_time);

    switch (level) {
        case G_LOG_LEVEL_DEBUG: {
            logLevel = " DEBUG  ";
            break;
        }
        case G_LOG_LEVEL_MESSAGE:
        case G_LOG_LEVEL_INFO: {
            logLevel = "  INFO  ";
            break;
        }
        case G_LOG_LEVEL_WARNING: {
            logLevel = " WARNING";
            break;
        }
        case G_LOG_LEVEL_CRITICAL: {
            logLevel = "CRITICAL";
            break;
        }
        case G_LOG_LEVEL_ERROR: {
            logLevel = "  ERROR ";
            break;
        }
        default: {
            logLevel = " UNKNOWN";
        }
    }
    for (int i = 0; i < nFields; ++i) {
        if (0 == g_ascii_strcasecmp ("file", fields[i].key)) {
            file = fields[i].value;
        } else if (0 == g_ascii_strcasecmp ("func", fields[i].key)) {
            func = fields[i].value;
        } else if (0 == g_ascii_strcasecmp ("line", fields[i].key)) {
            line = g_strdup_printf ("%d", *((int*)&(fields[i].value)));
        } else if (0 == g_ascii_strcasecmp ("message", fields[i].key)) {
            msg = g_strdup_printf ("%s", (const char*) ((fields[i].value) ? fields[i].value : "<null>"));
        }
#if 0
        else {
            write (2, fields[i].key, strlen (fields[i].key));
            write (2, "\n", 1);
        }
#endif
    }

    int i = -1;
    {
        // time
        vec[++i].iov_base = (void*)s_time;
        vec[i].iov_len = strlen(s_time);
        vec[++i].iov_base = " ";
        vec[i].iov_len = 1;
    }

    vec[++i].iov_base = "[";
    vec[i].iov_len = 1;

    if (file) {
        vec[++i].iov_base = (void*)file;
        vec[i].iov_len = strlen(file);
        vec[++i].iov_base = (void*)(":");
        vec[i].iov_len = 1;
    }

    if (line) {
        vec[++i].iov_base = (void*)line;
        vec[i].iov_len = strlen(line);
    }

    if (func) {
        vec[++i].iov_base = (void*)(" ");
        vec[i].iov_len = 1;
        vec[++i].iov_base = (void*)func;
        vec[i].iov_len = strlen(func);
    }

    vec[++i].iov_base = "] ";
    vec[i].iov_len = 2;

    if (msg) {
        vec[++i].iov_base = (void*)msg;
        vec[i].iov_len = strlen(msg);
        vec[++i].iov_base = "\n";
        vec[i].iov_len = 1;
    }

    if (g_once_init_enter (&gInit)) {
        if (0 != _open_file ()) {
            fprintf (stderr, "open %s failed, error: %s\n", gLogPath, strerror(errno));
        }
        g_once_init_leave (&gInit, 1);
    }

    while (pthread_mutex_lock (&gLogLock)) usleep (1000);

    {
        write (gLogFd, APP_NAME, strlen (APP_NAME));
        write (gLogFd, " ", 1);
    }

    {
        write (gLogFd, "[", 1);
        write (gLogFd, logLevel, strlen (logLevel));
        write (gLogFd, "] ", 2);
    }

    _log_write (vec, i);

    write (gLogFd, "\n", 1);

    while (pthread_mutex_unlock (&gLogLock)) usleep (1000);

    return G_LOG_WRITER_HANDLED;
}

static unsigned long long get_file_size ()
{
    struct stat buf;
    if (stat (gLogPath, &buf) < 0) {
        return 0;
    }
    return (unsigned long long) buf.st_size;
}

static const char* get_dir (const char* path)
{
    char* p = (char*) path + strlen (path);
    for (; p != path; p--) {
        if ('/' == *p) {
            *(p + 1) = '\0';
            break;
        }
    }
    return path;
}

static int check_dir (const char* path)
{
    char* pathTmp = NULL;
    const char* dir = NULL;
    pathTmp = strdup (path);
    if (NULL != strstr (path, "/")) {
        dir = get_dir (pathTmp);
        if (-1 == access (dir, F_OK | W_OK | R_OK)) {
            fprintf (stderr, "dir '%s' not exists\n", pathTmp);
            goto RET_ERR;
        }
    }

    free (pathTmp);

    return 0;

RET_ERR:
    free (pathTmp);

    return -1;
}

static int _open_file ()
{
    if (!gLogPath)  return -1;

    if (0 != check_dir (gLogPath)) {
        fprintf(stderr, "check_dir error, '%s' not exists\n", gLogPath);
        return -1;
    }

    gLogFd = open (gLogPath, O_CREAT | O_RDWR | O_APPEND, 0664);
    if (-1 == gLogFd) {
        fprintf (stderr, "open %s error: %s, log_init failed\n", gLogPath, strerror(errno));
        gLogFd = STDERR_FILENO;
        return -1;
    }

    return 0;
}

static int _log_open_rewrite ()
{
    check_dir (gLogPath);
    gLogFd = open (gLogPath, O_RDWR | O_CREAT | O_TRUNC, 0644);
    if (gLogFd == -1) {
        fprintf (stderr, "open %s failed: %s\n", gLogPath, strerror(errno));
        fprintf (stderr, "use STDERR_FILEIO as output\n");
        gLogFd = STDERR_FILENO;
    }

    return 0;
}

static ssize_t _log_write (struct iovec *vec, int n)
{
    unsigned long long tmpSize = get_file_size();
    if (tmpSize > LOG_MAX_SIZE) {
        if (-1 == close (gLogFd)) {
            fprintf(stderr, "close file errno: %d", errno);
        }
        _log_open_rewrite();
    }

    return writev (gLogFd, vec, n);
}

static void _log_get_time(char *str)
{
    char date_fmt[20] = {0};
    char date_ms[4] = {0};
    struct timeval tv;
    struct tm now_tm;
    guint now_ms;
    time_t now_sec;
    gettimeofday (&tv, NULL);
    now_sec = tv.tv_sec;
    now_ms = tv.tv_usec/1000;
    localtime_r (&now_sec, &now_tm);

    strftime (date_fmt, 20, "%Y-%m-%d %H:%M:%S", &now_tm);
    snprintf (date_ms, sizeof (date_ms), "%03u", now_ms);
    snprintf (str, (unsigned long) 64, "[%s.%s]", date_fmt, date_ms);
}

#if 0
void signal_handler (int sig, siginfo_t* siginfo, void* context)
{
    void*   bt[256] = {0};
    char**  btSymbols = NULL;

    int bufSize = 0;
    char buf[256000] = {0};

    int n = backtrace (bt, sizeof(bt) / sizeof(bt[0]));
    if (n <= 0) {
        LOG_ERROR("backtrace error, '%s'", g_strerror (errno));
        return;
    }
    btSymbols = backtrace_symbols(bt, n);
    for (int i = 1; i < n; ++i) {
        char lineBuf[10240] = {0};
        char binPath[1024] = {0};
        char p[32] = {0};

        sscanf(btSymbols[i], "%*[^(]%[^)]s", lineBuf);
        const char* sc = strstr (btSymbols[i], "(");
        if (NULL != sc) {
            strncpy(binPath, btSymbols[i], sc - btSymbols[i]);
        }
        sscanf(lineBuf, "(+%s", p);
        memset(lineBuf, 0, sizeof(lineBuf));
        if (strlen(p) > 0 && strlen (binPath) > 0) {
            snprintf(lineBuf, sizeof(lineBuf) - 1, "addr2line %s -e %s", p, binPath);
            FILE* fr = popen(lineBuf, "r");
            memset(lineBuf, 0, sizeof(lineBuf));
            fread(lineBuf, sizeof(lineBuf) - 1, 1, fr);
            fclose(fr);
            for (int j = 0; j < sizeof(lineBuf) - 1; ++j) {
                if ('\n' == lineBuf[j] || 0 == lineBuf[j]) {
                    lineBuf[j] = 0;
                    break;
                }
            }
        }

        int fl = (int) strlen (lineBuf);
        int l = (int) strlen(btSymbols[i]);
        if (bufSize + l + fl + 2 >= sizeof(lineBuf) - 1) {
            break;
        }

        strncat(buf + bufSize, btSymbols[i], sizeof(buf) - bufSize - 1);
        bufSize += l;
        strncat(buf + bufSize, " ", sizeof(buf) - bufSize - 1);
        bufSize += 1;
        strncat(buf + bufSize, lineBuf, sizeof(buf) - bufSize - 1);
        bufSize += fl;
        strncat(buf + bufSize, "\n", sizeof(buf) - bufSize - 1);
        bufSize += 1;
    }

    LOG_ERROR ("\n\n%s\n\n", buf);

    free(btSymbols);
}
#endif
