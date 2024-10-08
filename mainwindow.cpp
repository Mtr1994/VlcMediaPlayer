﻿#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Public/appsignal.h"
#include "Configure/softconfig.h"
#include "Dialog/dialogversion.h"
#include "Control/Message/messagewidget.h"

#include <QKeyEvent>
#include <QFileDialog>
#include <QStandardPaths>
#include <QScreen>

// test
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    init();

    setWindowTitle("VlcPlayer");
}

MainWindow::~MainWindow()
{
    SoftConfig::getInstance()->setValue("Media", "path", "");
    delete ui;
}

void MainWindow::init()
{
    float w = screen()->logicalDotsPerInch() * 13 * (screen()->physicalDotsPerInch() / 96.0);
    resize(w / devicePixelRatio(), w * 0.618 / devicePixelRatio());

    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::slot_open_video_file);
    connect(ui->actionClose, &QAction::triggered, this, &MainWindow::slot_close_video_file);
    connect(ui->actionQuit, &QAction::triggered, this, [this]{ this->close(); });

    connect(ui->actionSetting, &QAction::triggered, this, []{ /* 还没写，哈哈 */});
    connect(ui->actionGrapImage, &QAction::triggered, this, &MainWindow::slot_grap_video_frame);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::slot_show_about_developer);

    connect(AppSignal::getInstance(), &AppSignal::sgl_start_play_video, this, &MainWindow::slot_start_play_video);
    connect(AppSignal::getInstance(), &AppSignal::sgl_pause_play_video, this, &MainWindow::slot_pause_play_video);
    connect(AppSignal::getInstance(), &AppSignal::sgl_change_audio_volume, this, &MainWindow::slot_change_audio_volume);
    connect(AppSignal::getInstance(), &AppSignal::sgl_seek_video_position, this, &MainWindow::slot_seek_video_position);
    connect(AppSignal::getInstance(), &AppSignal::sgl_media_show_full_screen, this, &MainWindow::slot_media_show_full_screen);
    connect(AppSignal::getInstance(), &AppSignal::sgl_system_output_message, this, &MainWindow::slot_system_output_message);
    connect(AppSignal::getInstance(), &AppSignal::sgl_start_play_target_media, this, &MainWindow::slot_start_play_target_media);
    connect(AppSignal::getInstance(), &AppSignal::sgl_save_capture_status, this, &MainWindow::slot_save_capture_status);

    ui->menuFile->setWindowFlag(Qt::NoDropShadowWindowHint);
    ui->menuTool->setWindowFlag(Qt::NoDropShadowWindowHint);
    ui->menuAbout->setWindowFlag(Qt::NoDropShadowWindowHint);
    ui->menuSetting->setWindowFlag(Qt::NoDropShadowWindowHint);
    ui->menuView->setWindowFlag(Qt::NoDropShadowWindowHint);

    // 默认测试播放
    //emit AppSignal::getInstance()->sgl_start_play_target_media("http://101.34.253.220/image/upload/video/Demo.mp4");
    //emit AppSignal::getInstance()->sgl_start_play_target_media("screen://");
}

void MainWindow::slot_start_play_video()
{
    ui->widgetOpenGLPlayer->start();
}

void MainWindow::slot_pause_play_video()
{
    ui->widgetOpenGLPlayer->pause();
}

void MainWindow::slot_change_audio_volume(int volume)
{
    ui->widgetOpenGLPlayer->setAudioVolume(volume);
}

void MainWindow::slot_seek_video_position(float position)
{
    ui->widgetOpenGLPlayer->seek(position);
}

void MainWindow::slot_show_about_developer()
{
    DialogVersion dialog(this);
    dialog.exec();
}

void MainWindow::slot_open_video_file()
{
    QStringList fileNameList = QFileDialog::getOpenFileNames(nullptr, tr("选择视频文件"), QStandardPaths::writableLocation(QStandardPaths::DesktopLocation), tr("视频文件 (*.mp4 *.mkv *.avi *.mov *.flv *.wmv *.mpg *.m4v *.ts)"));
    if (fileNameList.isEmpty()) return;

    // 添加到列表
    for (auto &path : fileNameList)
    {
        std::replace(path.begin(), path.end(), QChar('/'), QChar('\\'));
        ui->widgetMediaControl->addMediaPath(path);
    }

    emit AppSignal::getInstance()->sgl_start_play_target_media(fileNameList.first());
}

void MainWindow::slot_close_video_file()
{
    setWindowTitle("Mtr1994");
    ui->widgetOpenGLPlayer->stop();
}

void MainWindow::slot_grap_video_frame()
{
    ui->widgetOpenGLPlayer->grap();
}

void MainWindow::slot_media_show_full_screen()
{
    ui->widgetMediaControl->setVisible(false);
    ui->menubar->setVisible(false);
    showFullScreen();
}

void MainWindow::slot_save_capture_status(bool status, const QString &path)
{
    Q_UNUSED(path);
    int type = status ? MessageWidget::M_Success : MessageWidget::M_Error;
    MessageWidget *widget = new MessageWidget(type, MessageWidget::P_Top_Center, this);
    widget->showMessage(status ? "截图已保存到桌面" : "无法保存空的图片");
}

void MainWindow::slot_system_output_message(const QString &message)
{
    MessageWidget *widget = new MessageWidget(MessageWidget::M_Info, MessageWidget::P_Top_Center, this);
    widget->showMessage(message);
}

void MainWindow::slot_start_play_target_media(const QString &path)
{
    // 标题栏现实视频的路径信息
    setWindowTitle(QString("VlcPlayer  -  %1").arg(path));
    ui->widgetOpenGLPlayer->play(path);
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if(event->modifiers() == Qt::Modifier::CTRL)
    {
        if (event->key() == Qt::Key_O)
        {
            emit ui->actionOpen->triggered(true);
        }
        else if (event->key() == Qt::Key_P)
        {
            emit ui->actionGrapImage->triggered(true);
        }
    }
    else if (event->key() == Qt::Key_Escape)
    {
        showNormal();
        ui->widgetMediaControl->setVisible(true);
        ui->menubar->setVisible(true);
    }
    else if (event->key() == Qt::Key_Space)
    {
        ui->widgetMediaControl->changePlayStatus();
    }
}

