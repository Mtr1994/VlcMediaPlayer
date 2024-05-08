#include "widgetplayer.h"
#include "Public/appsignal.h"
#include "Configure/softconfig.h"
#include "playereventcallbacks.h"

// https://www.cnblogs.com/vichang/p/8043627.html
// https://videolan.videolan.me/vlc/index.html

// test
#include <QDebug>

WidgetPlayer::WidgetPlayer(QWidget *parent) : QOpenGLWidget(parent)
{
    init();
}

WidgetPlayer::~WidgetPlayer()
{
    if (nullptr != mp)
    {
        libvlc_media_player_stop(mp);
        libvlc_media_player_release(mp);
    }
    if (nullptr != inst) libvlc_release(inst);
}

void WidgetPlayer::init()
{
    // 不解析音频 "--no-audio"
    const char *argv[] = {"--no-osd", "--no-snapshot-preview", "--no-sub-autodetect-file", "--ignore-config", "--no-advanced", "--no-stats"};
    inst = libvlc_new(sizeof(argv) / sizeof(argv[0]), argv);
    libvlc_set_log_verbosity(inst, LIBVLC_ERROR);

    libvlc_log_set(inst, libvlc_log_event_handler, nullptr);
}

void WidgetPlayer::play(const QString &path)
{
    if (nullptr == inst) return;
    if (nullptr != mp)
    {
        libvlc_media_player_stop(mp);
        libvlc_media_player_release(mp);
    }
    mMediaPath = path;

    m = libvlc_media_new_location(inst, path.toStdString().data());
    mp = libvlc_media_player_new_from_media(m);

    attachEvents();

    libvlc_video_set_mouse_input(mp, true);

    // 倍速播放
    //libvlc_media_player_set_rate(mp, 2.0);

    libvlc_media_player_set_hwnd(mp, (void*)this->winId());
    libvlc_media_release(m);

    // 获取视频时常
    int length = libvlc_media_player_get_length(mp);
    emit AppSignal::getInstance()->sgl_init_media_duration(length);

    libvlc_media_player_play(mp);
}

void WidgetPlayer::pause()
{
    if (nullptr == mp) return;
    libvlc_media_player_pause(mp);
}

void WidgetPlayer::start()
{
    if (nullptr == mp) return;
    libvlc_media_player_play(mp);
}

void WidgetPlayer::seek(float position)
{
    if (nullptr == mp) return;
    //libvlc_media_player_next_frame(mp);
    //libvlc_media_player_set_time(mp, position);
    libvlc_media_player_set_position(mp, position);
}

void WidgetPlayer::stop()
{
    if (nullptr == mp) return;
    libvlc_media_player_stop(mp);
    libvlc_media_player_release(mp);
}

void WidgetPlayer::setAudioVolume(float volume)
{
    if (nullptr == inst) return;
    libvlc_audio_set_volume(mp, volume * 1.5);
}

void WidgetPlayer::grap()
{
    if (nullptr == mp) return;

    libvlc_video_take_snapshot(mp, 0, "./demo1.png" ,0, 0);
    emit AppSignal::getInstance()->sgl_save_capture_status(true, "");
}

void WidgetPlayer::attachEvents()
{
    libvlc_event_manager_t* eventManager = libvlc_media_player_event_manager(mp);

    // 订阅事件
    libvlc_event_attach(eventManager, libvlc_MediaPlayerLengthChanged, &libvlc_media_event_handler, this);
    libvlc_event_attach(eventManager, libvlc_MediaPlayerTimeChanged, &libvlc_media_event_handler, this);
    libvlc_event_attach(eventManager, libvlc_MediaPlayerStopped, &libvlc_media_event_handler, this);
}


