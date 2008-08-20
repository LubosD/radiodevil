#include "MainWindow.h"
#include "Settings.h"
#include "GeneralException.h"
#include "SoxReader.h"
#include "SoxWriter.h"
#include "SettingsDlg.h"
#include <cmath>
#include <QFile>
#include <QMessageBox>
#include <QMenu>
#include <QtDebug>

MainWindow::MainWindow()
	: m_reader(0), m_playback(0), m_nPreset(0), m_groupPresets(0)
{
	setupUi();
	connectActions();
	
	float volume = m_fPreviousVol = getSettingsValue("sound/volume").toDouble();
	
	if(volume < 0)
		volume = 0;
	else if(volume > 1)
		volume = 1;
	
	sliderVolume->setValue(volume * 100);
	pushMute->setChecked(!volume);
	
	m_timer.start(getSettingsValue("gui/refresh").toInt());
	
	loadPresets();
	setupRadio();
}

MainWindow::~MainWindow()
{
	delete m_reader;
	delete m_playback;
}

void MainWindow::setupUi()
{
	Ui_MainWindow::setupUi(this);
	
	QString cssSliders = loadCSS("slider.css");
	sliderFrequency->setStyleSheet(cssSliders);
	sliderVolume->setStyleSheet(cssSliders);
	
	m_trayIcon.setIcon(QIcon(":/app/radiodevil-48x48.png"));
	m_trayIcon.setToolTip("RadioDevil");
	m_trayIcon.show();
	
	toolPreset->setMenu(new QMenu(toolPreset));
}

void MainWindow::connectActions()
{
	connect(sliderVolume, SIGNAL(valueChanged(int)), this, SLOT(volumeChanged(int)));
	connect(sliderFrequency, SIGNAL(valueChanged(int)), this, SLOT(frequencyChanged(int)));
	connect(pushMute, SIGNAL(toggled(bool)), this, SLOT(toggledMute(bool)));
	connect(&m_timer, SIGNAL(timeout()), this, SLOT(refreshInfo()));
	connect(pushSettings, SIGNAL(clicked()), this, SLOT(settings()));
	connect(comboPresets, SIGNAL(currentIndexChanged(int)), this, SLOT(stationChanged(int)));
	connect(&m_trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(trayIconActivated(QSystemTrayIcon::ActivationReason)));
	connect(actionQuit, SIGNAL(triggered()), qApp, SLOT(quit()));
	connect(actionDisplay, SIGNAL(toggled(bool)), this, SLOT(showWindow(bool)));
}

void MainWindow::closeEvent(QCloseEvent* event)
{
	if(m_trayIcon.isVisible())
	{
		actionDisplay->setChecked(false);
		hide();
	}
	else
	{
		m_trayIcon.hide();
		QWidget::closeEvent(event);
		qApp->quit();
	}
}

void MainWindow::showWindow(bool show)
{
	if(show)
	{
		QPoint pos = this->pos();
		showNormal();
		move(pos);
	}
	else
		hide();
}

void MainWindow::trayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
	if(reason == QSystemTrayIcon::Trigger)
	{
		if(actionDisplay->isChecked() && !this->isActiveWindow())
		{
			activateWindow();
			raise();
		}
		else
			actionDisplay->toggle();
	}
	else if(reason == QSystemTrayIcon::Context)
	{
		QMenu menu(this);
		
		int sel = comboPresets->currentIndex();
		for(int i=0;i<m_presets[m_nPreset].stations.size();i++)
		{
			QAction* act;
			act = menu.addAction(m_presets[m_nPreset].stations[i].name, this, SLOT(stationChanged()));
			act->setData(i);
			
			if(sel == i)
			{
				act->setCheckable(true);
				act->setChecked(true);
			}
		}
		
		menu.addSeparator();
		menu.addAction(actionDisplay);
		menu.addSeparator();
		menu.addAction(actionQuit);
		
		menu.exec(QCursor::pos());
	}
	else if(reason == QSystemTrayIcon::MiddleClick)
	{
		pushMute->toggle();
	}
}

void MainWindow::setupRadio()
{
	delete m_reader;
	delete m_playback;
	
	try
	{
		openRadio();
		constructReader();
		constructPlayer();
		
		m_playback->setVolume(sliderVolume->value()/100.f);
		refreshInfo();
	}
	catch(const GeneralException& e)
	{
		QMessageBox::critical(this, "RadioDevil", e.error());
		qApp->quit();
	}
}

void MainWindow::loadPresets()
{
	m_presets = ::loadPresets();
	
	if(m_presets.isEmpty())
	{
		PresetSet set;
		set.name = tr("Main");
		m_presets << set;
		savePresets(m_presets);
	}
	
	delete m_groupPresets;
	m_actions.clear();
	
	QMenu* menu = toolPreset->menu();
	m_groupPresets = new QActionGroup(this);
	m_groupPresets->setExclusive(true);
	
	menu->clear();
	
	for(int i=0;i<m_presets.size();i++)
	{
		QAction* act = menu->addAction(m_presets[i].name, this, SLOT(presetChanged()));
		act->setData(i);
		act->setCheckable(true);
		m_groupPresets->addAction(act);
		m_actions << act;
	}
	
	int index = getSettingsValue("preset").toInt();
	
	if(index < 0 || index >= m_presets.size())
		index = 0;
	
	selectPresetSet(index);
}

void MainWindow::presetChanged()
{
	QAction* act = static_cast<QAction*>(sender());
	selectPresetSet(act->data().toInt());
	frequencyChanged(sliderFrequency->value());
}

void MainWindow::selectPresetSet(int set)
{
	m_nPreset = set;
	
	m_actions[set]->setChecked(true);
	
	comboPresets->blockSignals(true);
	comboPresets->clear();
	
	for(int i=0;i<m_presets[set].stations.size();i++)
		comboPresets->addItem(m_presets[set].stations[i].name);
	comboPresets->blockSignals(false);
	comboPresets->setCurrentIndex(-1);
}

void MainWindow::stationChanged()
{
	QAction* act = static_cast<QAction*>(sender());
	stationChanged(act->data().toInt());
}

void MainWindow::stationChanged(int index)
{
	if(index < 0)
		return;
	
	int freq = m_presets[m_nPreset].stations[index].frequency*10;
	//frequencyChanged(freq);
	sliderFrequency->setValue(freq);
	m_trayIcon.setToolTip(QString("RadioDevil - %1").arg(m_presets[m_nPreset].stations[index].name));
}

void MainWindow::openRadio()
{
	m_device.close();
	m_device.open(getSettingsValue("radio/device").toString());
	
	RadioDevice::Tuner tuner = m_device.tuner();
	
	int freq = getSettingsValue("radio/frequency").toInt();
	sliderFrequency->setRange(tuner.fqFrom*10, tuner.fqTo*10);
	sliderFrequency->setValue(freq);
}

void MainWindow::settings()
{
	SettingsDlg dlg(this);
	
	dlg.m_strDeviceName = m_device.name();
	dlg.m_tunerInfo = m_device.tuner();
	
	if(dlg.exec() == QDialog::Accepted)
	{
		setupRadio();
		loadPresets();
		presetChanged();
	}
}

void MainWindow::frequencyChanged(int freq)
{
	float f = freq/10.f;
	setSettingsValue("radio/frequency", freq);
	
	qDebug() << "New frequency:" << f;
	
	widgetDisplay->setFrequency(f);
	
	comboPresets->blockSignals(true);
	
	bool bFound = false;
	for(int i=0;i<m_presets[m_nPreset].stations.size();i++)
	{
		if(fabs(m_presets[m_nPreset].stations[i].frequency-f) < 0.001f)
		{
			comboPresets->setCurrentIndex(i);
			m_trayIcon.setToolTip(QString("RadioDevil - %1").arg(m_presets[m_nPreset].stations[i].name));
			bFound = true;
			
			break;
		}
	}
	
	if(!bFound)
	{
		comboPresets->setCurrentIndex(-1);
		m_trayIcon.setToolTip("RadioDevil");
	}
	
	comboPresets->blockSignals(false);
	
	try
	{
		m_device.setFrequency(f);
	}
	catch(const GeneralException& e)
	{
		QMessageBox::critical(this, "RadioDevil", e.error());
	}
}

void MainWindow::refreshInfo()
{
	try
	{
		RadioDevice::SignalInfo info = m_device.signalInfo();
		widgetDisplay->setSignal(info.fSignal);
		widgetDisplay->setStereo(info.bStereo);
	}
	catch(const GeneralException& e)
	{
		m_timer.stop();
		QMessageBox::critical(this, "RadioDevil", e.error());
		qApp->quit();
	}
}

void MainWindow::constructReader()
{
	QString device = QString("plughw:%1,0").arg(getSettingsValue("sound/source").toInt());
	
	qDebug() << "Input device:" << device;
	
	m_reader = new SoxReader(device);
	m_reader->start();
}

void MainWindow::constructPlayer()
{
	if(!m_reader)
		return;
	
	QString device = getSettingsValue("sound/target").toString();
	
	qDebug() << "Output device:" << device;
	
	m_playback = new SoxWriter(device, m_reader->signal());
	m_reader->addWriter(m_playback);
	m_playback->start();
}

QString MainWindow::loadCSS(QString file)
{
	QFile f ("data/"+file);
	if(!f.open(QIODevice::ReadOnly))
		qDebug() << "Cannot open" << file;
	
	return f.readAll();
}

void MainWindow::volumeChanged(int vol)
{
	float volume = float(vol)/100.f;
	if(m_playback)
		m_playback->setVolume(volume);
	setSettingsValue("sound/volume", volume);
	pushMute->setChecked(!vol);
}

void MainWindow::toggledMute(bool mute)
{
	if(mute)
	{
		if(m_playback)
			m_playback->setVolume(0);
		
		m_fPreviousVol = sliderVolume->value()/100.0f;
		sliderVolume->setValue(0);
	}
	else
	{
		if(!m_fPreviousVol)
			m_fPreviousVol = 1;
		sliderVolume->setValue(m_fPreviousVol*100);
	}
}
