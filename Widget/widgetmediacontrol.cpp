#include "widgetmediacontrol.h"
#include "ui_widgetmediacontrol.h"
#include "Public/appsignal.h"
#include "Configure/softconfig.h"
#include "Control/Drawer/widgetdrawer.h"
#include "widgetmediapathlist.h"

#include <thread>

/*
    单帧模式可以通过 ffmpeg 的库先解析一遍视频文件，然后获取 pts 列表，然后通过 Apsara 跳转来完成
    单帧模式需要在暂停状态下实现。单帧模式下，禁用播放按钮。可行方案
*/

// test
#include <QDebug>

WidgetMediaControl::WidgetMediaControl(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetMediaControl)
{
    ui->setupUi(this);

    init();
}

WidgetMediaControl::~WidgetMediaControl()
{
    // 记录音量配置
    SoftConfig::getInstance()->setValue("Volume", "value", QString::number(ui->sliderVolume->value()));

    delete ui;
}

void WidgetMediaControl::init()
{
    connect(ui->btnPreviousFrame, &QPushButton::clicked, this, &WidgetMediaControl::slot_btn_play_previous_frame);
    connect(ui->btnPlay, &QPushButton::clicked, this, &WidgetMediaControl::slot_btn_play_click);
    connect(ui->btnNextFrame, &QPushButton::clicked, this, &WidgetMediaControl::slot_btn_play_next_frame);
    connect(ui->btnMediaList, &QPushButton::clicked, this, &WidgetMediaControl::slot_btn_media_list_click);

    // 全屏显示
    connect(ui->btnFullScreen, &QPushButton::clicked, AppSignal::getInstance(), &AppSignal::sgl_media_show_full_screen);

    connect(ui->sliderVolume, &QSlider::valueChanged, this, &WidgetMediaControl::slot_volume_value_change);
    // 读取音量配置
    ui->sliderVolume->setValue(SoftConfig::getInstance()->getValue("Volume", "value").toUInt());

    connect(ui->slider, &WidgetSlider::sliderPressed, this, [this]{ mSliderPressed = true; });
    connect(ui->slider, &WidgetSlider::sliderReleased, this, [this]{ mSliderPressed = false; });

    connect(ui->slider, &WidgetSlider::sgl_current_value_changed, this, [this](int64_t value)
    {
        emit AppSignal::getInstance()->sgl_seek_video_position(value * 1.0 / mMediaBaseDuration);
    });

    connect(AppSignal::getInstance(), &AppSignal::sgl_init_media_duration, this, &WidgetMediaControl::slot_init_media_duration);
    connect(AppSignal::getInstance(), &AppSignal::sgl_start_play_target_media, this, &WidgetMediaControl::slot_start_play_target_media);
    connect(AppSignal::getInstance(), &AppSignal::sgl_current_video_frame_time, this, &WidgetMediaControl::slot_current_video_frame_time, Qt::QueuedConnection);
    connect(AppSignal::getInstance(), &AppSignal::sgl_video_play_start, this, &WidgetMediaControl::slot_video_play_start);
    connect(AppSignal::getInstance(), &AppSignal::sgl_video_play_finish, this, &WidgetMediaControl::slot_video_play_finish);
}

void WidgetMediaControl::changePlayStatus()
{
    ui->btnPlay->setChecked(!ui->btnPlay->isChecked());
    slot_btn_play_click();
}

void WidgetMediaControl::addMediaPath(const QString &path)
{
    if (mListMediaPath.contains(path)) return;
    mListMediaPath.append(path);
}

void WidgetMediaControl::slot_btn_play_previous_frame()
{
    double targetPosition = ui->slider->value() - mMediaBaseDuration * 0.1;
    if (targetPosition <= 0) targetPosition = 0;

    ui->slider->setValue(targetPosition);
    emit AppSignal::getInstance()->sgl_seek_video_position(targetPosition / mMediaBaseDuration);
}

void WidgetMediaControl::slot_btn_play_click()
{
    if (ui->btnPlay->isChecked())
    {
        emit AppSignal::getInstance()->sgl_start_play_video();
    }
    else
    {
        emit AppSignal::getInstance()->sgl_pause_play_video();
    }
}

void WidgetMediaControl::slot_btn_play_next_frame()
{
    double targetPosition = ui->slider->value() + mMediaBaseDuration * 0.1;
    if (targetPosition >= ui->slider->maximum()) targetPosition = ui->slider->maximum();

    ui->slider->setValue(targetPosition);
    emit AppSignal::getInstance()->sgl_seek_video_position(targetPosition / mMediaBaseDuration);
}

void WidgetMediaControl::slot_volume_value_change(int volume)
{
    emit AppSignal::getInstance()->sgl_change_audio_volume(volume);
}

void WidgetMediaControl::slot_init_media_duration(int64_t duration)
{
    if (duration < 0)
    {
        ui->lbDuration->setText("00:00:00");
        ui->lbDurationLeft->setText("00:00:00");
        ui->slider->setValue(ui->slider->maximum());
        return;
    }

    ui->slider->setValue(0);
    ui->slider->setRange(0, duration);

    ui->btnPlay->setChecked(true);
    mMediaBaseDuration = duration;

    QString hour = QString("%1").arg(mMediaBaseDuration / 1000 / 3600, 2, 10, QLatin1Char('0'));
    QString minute = QString("%1").arg(mMediaBaseDuration / 1000 / 60, 2, 10, QLatin1Char('0'));
    QString seconds = QString("%1").arg(mMediaBaseDuration / 1000 % 60, 2, 10, QLatin1Char('0'));

    ui->lbDurationLeft->setText(QString("%1:%2:%3").arg(hour, minute, seconds));
}

void WidgetMediaControl::slot_current_video_frame_time(int64_t pts)
{
    if (mMediaBaseDuration < 0) return;

    if (!mSliderPressed) ui->slider->setValue(pts);

    QString hour = QString("%1").arg(pts / 1000 / 3600, 2, 10, QLatin1Char('0'));
    QString minute = QString("%1").arg(pts / 1000 / 60, 2, 10, QLatin1Char('0'));
    QString seconds = QString("%1").arg(pts / 1000 % 60, 2, 10, QLatin1Char('0'));
    ui->lbDuration->setText(QString("%1:%2:%3").arg(hour, minute, seconds));

    int64_t leftTime = mMediaBaseDuration - pts;
    hour = QString("%1").arg(leftTime / 1000 / 3600, 2, 10, QLatin1Char('0'));
    minute = QString("%1").arg(leftTime / 1000 / 60, 2, 10, QLatin1Char('0'));
    seconds = QString("%1").arg(leftTime / 1000 % 60, 2, 10, QLatin1Char('0'));
    ui->lbDurationLeft->setText(QString("%1:%2:%3").arg(hour, minute, seconds));
}

void WidgetMediaControl::slot_video_play_start()
{
    ui->btnPlay->setChecked(true);
}

void WidgetMediaControl::slot_video_play_finish()
{
    slot_current_video_frame_time(ui->slider->maximum());
    ui->btnPlay->setChecked(false);
}

void WidgetMediaControl::slot_btn_media_list_click()
{
    WidgetDrawer *drawer = new WidgetDrawer(this->parentWidget());
    WidgetMediaPathList *sub = new WidgetMediaPathList(drawer);
    sub->setMediaPaths(mListMediaPath);
    sub->setCurrentMediaPaths(mCurrentMediaPath);
    drawer->setContentWidget(sub);
    drawer->showDrawer();
}

void WidgetMediaControl::slot_start_play_target_media(const QString &path)
{
    mCurrentMediaPath = path;
}
