#ifndef SETTINGSDLG_H
#define SETTINGSDLG_H
#include <QDialog>
#include "ui_SettingsDlg.h"
#include "RadioDevice.h"
#include "Preset.h"

class SettingsDlg : public QDialog, Ui_SettingsDlg
{
Q_OBJECT
public:
	SettingsDlg(QWidget* parent);
	int exec();
	
	void load();
	void save();
	
	RadioDevice::Tuner m_tunerInfo;
	QString m_strDeviceName;
protected slots:
	void changeSet(int set);
	void stationDblClk(QTreeWidgetItem* item, int column);
	void listAdd();
	void listRemove();
private:
	QList<PresetSet> m_presets;
	int m_nSet;
};

#endif
