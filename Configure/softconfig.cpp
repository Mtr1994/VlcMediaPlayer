﻿#include "softconfig.h"

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#include <QTextCodec>
#endif

#include <QFile>

SoftConfig::SoftConfig(QObject *parent) : QObject(parent)
{

}

bool SoftConfig::init()
{
    if (!QFile::exists("conf.ini"))
    {
        QFile file("conf.ini");
        if (!file.open(QIODevice::ReadWrite | QIODevice::Text)) return false;
        file.write("[Volume]\nvalue = 36\n\n");
    }

    mSetting = new QSettings("conf.ini", QSettings::IniFormat);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    mSetting->setIniCodec(QTextCodec::codecForName("utf-8"));
#endif
    return true;
}

SoftConfig *SoftConfig::getInstance()
{
    static SoftConfig config;
    return &config;
}

QString SoftConfig::getValue(const QString& entry, const QString& item)
{
    if (nullptr == mSetting) return "";
    QString value = mSetting->value(entry + "/" + item).toString();
    return value;
}

void SoftConfig::setValue(const QString& pEntry, const QString& pItem, const QString& pValue)
{
    mSetting->setValue(pEntry + "/" + pItem, pValue);
    mSetting->sync();
}
