#ifndef SETTINGS_H
#define SETTINGS_H
#include <QVariant>
#include <QString>

void initSettings();
void exitSettings();

QVariant getSettingsValue(QString id);
void setSettingsValue(QString id, QVariant value);
QVariant getSettingsDefault(QString id);

QString getFilePath(QString file);

#endif
