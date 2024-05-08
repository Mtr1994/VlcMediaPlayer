#include "playereventcallbacks.h"
#include "Public/appsignal.h"
#include "Configure/softconfig.h"

void libvlc_log_event_handler(void *data, int level, const libvlc_log_t *ctx, const char *fmt, va_list args)
{
    Q_UNUSED(data); Q_UNUSED(level); Q_UNUSED(ctx); Q_UNUSED(fmt); Q_UNUSED(args);
    // 在这里实现您的日志打印逻辑
    //qDebug() << "logs data " << (char*)data << "^^";
}

// 事件处理函数
void libvlc_media_event_handler(const libvlc_event_t *event, void *userData)
{
    Q_UNUSED(userData);
    switch (event->type) {
        case libvlc_MediaPlayerLengthChanged:
        {
            int duration = event->u.media_duration_changed.new_duration;
            emit AppSignal::getInstance()->sgl_init_media_duration(duration);

            float volume = SoftConfig::getInstance()->getValue("Volume", "value").toFloat();
            emit AppSignal::getInstance()->sgl_change_audio_volume(volume);
            break;
        }
        case libvlc_MediaPlayerTimeChanged:
        {
            int nt = event->u.media_player_time_changed.new_time;
            emit AppSignal::getInstance()->sgl_current_video_frame_time(nt);
            break;
        }
        case libvlc_MediaPlayerStopped:
        {
            emit AppSignal::getInstance()->sgl_video_play_finish();
            break;
        }
        default:
            break;
    }
}
