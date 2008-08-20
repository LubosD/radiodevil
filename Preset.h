#ifndef PRESET_H
#define PRESET_H
#include <QString>
#include <QList>

struct Station
{
	Station(QString n, float f) : name(n), frequency(f) {}
	
	QString name;
	float frequency;
};

struct PresetSet
{
	QString name;
	QList<Station> stations;
};

QList<PresetSet> loadPresets();
void savePresets(const QList<PresetSet>& data);

#endif
