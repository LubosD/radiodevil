#include "Preset.h"
#include <QSettings>
#include <QStringList>

extern QSettings* g_settings;

QList<PresetSet> loadPresets()
{
	QList<PresetSet> result;
	int num = g_settings->beginReadArray("presets");
	
	for(int i=0;i<num;i++)
	{
		PresetSet set;
		QStringList stations;
		
		g_settings->setArrayIndex(i);
		
		set.name = g_settings->value("name").toString();
		stations = g_settings->value("stations").toString().split('\n', QString::SkipEmptyParts);
		
		foreach(QString station, stations)
		{
			QStringList sp = station.split('\t', QString::SkipEmptyParts);
			set.stations << Station(sp[0], sp[1].toInt()/10.f);
		}
		
		result << set;
	}
	g_settings->endArray();
	
	return result;
}

void savePresets(const QList<PresetSet>& data)
{
	g_settings->beginWriteArray("presets", data.size());
	
	for(int i=0;i<data.size();i++)
	{
		g_settings->setArrayIndex(i);
		g_settings->setValue("name", data[i].name);
		
		QString stations;
		for(int j=0;j<data[i].stations.size();j++)
			stations += QString("%1\t%2\n").arg(data[i].stations[j].name).arg(int(data[i].stations[j].frequency*10));
		
		g_settings->setValue("stations", stations);
	}
	
	g_settings->endArray();
}
