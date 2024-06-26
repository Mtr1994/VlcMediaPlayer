﻿#ifndef WIDGETSLIDER_H
#define WIDGETSLIDER_H

#include <QSlider>

class WidgetSlider : public QSlider
{
    Q_OBJECT
public:
    explicit WidgetSlider(QWidget *parent = nullptr);

private:
    void init();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

signals:
    void sgl_current_value_changed(int64_t value);

private:
    void setNewValue(QMouseEvent *event);

private:
    bool mMousePressFlag = false;
};

#endif // WIDGETSLIDER_H
