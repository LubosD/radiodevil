#include "SettingsDlg.h"
#include "Settings.h"
#include "AlsaMixer.h"
#include "ListNameDlg.h"
#include <QMessageBox>
#include <QSettings>
#include <QtDebug>

extern QSettings* g_settings;

SettingsDlg::SettingsDlg(QWidget* parent)
	: QDialog(parent), m_nSet(-1)
{
	setupUi(this);
	connect(comboSets, SIGNAL(currentIndexChanged(int)), this, SLOT(changeSet(int)));
	connect(pushListAdd, SIGNAL(clicked()), this, SLOT(listAdd()));
	connect(pushListRemove, SIGNAL(clicked()), this, SLOT(listRemove()));
}

int SettingsDlg::exec()
{
	int rval;
	
	load();
	
	rval = QDialog::exec();
	if(rval == QDialog::Accepted)
		save();
	
	return rval;
}

void SettingsDlg::load()
{
	lineDevice->setText(getSettingsValue("radio/device").toString());
	lineDeviceName->setText(m_strDeviceName);
	lineTunerName->setText(m_tunerInfo.strName);
	labelFrequencyRange->setText(QString("%1 - %2 MHz").arg(m_tunerInfo.fqFrom).arg(m_tunerInfo.fqTo));
	
	comboInputDevice->addItems(AlsaMixer::devices());
	comboInputDevice->setCurrentIndex(getSettingsValue("sound/source").toInt());
	
	lineOutputDevice->setText(getSettingsValue("sound/target").toString());
	
	//treeStations->setHeaderHidden(true);
	treeStations->setHeaderLabels(QStringList() << tr("Station name") << tr("Frequency"));
	treeStations->setColumnWidth(0, 160);
	
	m_presets = loadPresets();
	for(int i=0;i<m_presets.size();i++)
		comboSets->addItem(m_presets[i].name);
	
	changeSet(0);
}

void SettingsDlg::save()
{
	setSettingsValue("radio/device", lineDevice->text());
	setSettingsValue("sound/source", comboInputDevice->currentIndex());
	setSettingsValue("sound/target", lineOutputDevice->text());
	
	changeSet(-1);
	savePresets(m_presets);
}

void SettingsDlg::changeSet(int set)
{
	if(m_nSet != -1)
	{
		m_presets[m_nSet].stations.clear();
		for(int i=0;i<treeStations->topLevelItemCount();i++)
		{
			QTreeWidgetItem* item = treeStations->topLevelItem(i);
			QString name, freq;
			
			name = item->text(0);
			freq = item->text(1);
			
			if(name.isEmpty() || freq.isEmpty() || !freq.toDouble())
				continue;
			
			m_presets[m_nSet].stations << Station(name, freq.toDouble());
		}
	}
	
	m_nSet = set;
	
	if(set != -1)
	{
		treeStations->clear();
		
		for(int i=0;i<m_presets[set].stations.size();i++)
		{
			QTreeWidgetItem* item = new QTreeWidgetItem(treeStations);
			item->setText(0, m_presets[set].stations[i].name);
			item->setIcon(0, QIcon(":/menu/miscellaneous.png"));
			item->setText(1, QString::number(m_presets[set].stations[i].frequency));
			item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsDragEnabled);
			treeStations->addTopLevelItem(item);
		}
	}
}

void SettingsDlg::stationDblClk(QTreeWidgetItem* item, int column)
{
	treeStations->editItem(item, column);
}

void SettingsDlg::listAdd()
{
	ListNameDlg dlg(this);
	if(dlg.exec() == QDialog::Accepted)
	{
		PresetSet set;
		set.name = dlg.m_strName;
		m_presets << set;
		
		comboSets->addItem(set.name);
		comboSets->setCurrentIndex(m_presets.size()-1);
	}
}

void SettingsDlg::listRemove()
{
	if(m_presets.size() <= 1)
	{
		QMessageBox::warning(this, "RadioDevil", tr("You can't remove the last station list!"));
		return;
	}
	
	if(QMessageBox::warning(this, "RadioDevil", tr("Do you really want to remove the selected station list?"),
	   QMessageBox::Yes|QMessageBox::No) == QMessageBox::Yes)
	{
		int i = comboSets->currentIndex();
		comboSets->removeItem(i);
		m_presets.removeAt(i);
	}
}

