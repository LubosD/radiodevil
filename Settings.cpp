#include "Settings.h"
#include <QSettings>
#include <QApplication>

QSettings* g_settings = 0;
static QSettings* m_settingsDefaults = 0;

void initSettings()
{
	g_settings = new QSettings;
	m_settingsDefaults = new QSettings(getFilePath("defaults.conf"), QSettings::IniFormat, qApp);
}

void exitSettings()
{
	delete g_settings;
	delete m_settingsDefaults;
}

QVariant getSettingsValue(QString id)
{
	return g_settings->value(id, getSettingsDefault(id));
}

void setSettingsValue(QString id, QVariant value)
{
	g_settings->setValue(id, value);
}

QVariant getSettingsDefault(QString id)
{
	return m_settingsDefaults->value(id);
}

QString getFilePath(QString file)
{
	return "data/" + file;
}
