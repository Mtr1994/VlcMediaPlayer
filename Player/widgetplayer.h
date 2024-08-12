#ifndef WIDGETPLAYER_H
#define WIDGETPLAYER_H

#ifdef _WIN32
#include <basetsd.h>
    typedef SSIZE_T ssize_t;
#endif

#include "vlc/vlc.h"
#include <QOpenGLWidget>

class WidgetPlayer : public QOpenGLWidget
{
    Q_OBJECT
public:
    explicit WidgetPlayer(QWidget *parent = nullptr);
    ~WidgetPlayer();

private:
    void init();

public:
    void play(const QString &path);
    void pause();
    void start();
    void seek(float position);
    void stop();
    void setAudioVolume(float volume);
    void grap();

private:
    void attachEvents();

private:
    QString mMediaPath;

    libvlc_instance_t *inst = nullptr;
    libvlc_media_player_t *mp = nullptr;
    libvlc_media_t *m = nullptr;

    float mMediaVolume = 100.0;
};

#endif // WIDGETPLAYER_H
