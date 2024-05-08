#include <stdint.h>

#ifdef _WIN32
#include <basetsd.h>
    typedef SSIZE_T ssize_t;
#endif

#include "vlc/libvlc.h"
#include "vlc/libvlc_media.h"
#include "vlc/libvlc_events.h"

// 日志处理
void libvlc_log_event_handler(void *data, int level, const libvlc_log_t *ctx, const char *fmt, va_list args);

void libvlc_media_event_handler(const libvlc_event_t *event, void *userData);
