#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QDialog>
#include <QTimer>
#include <QSystemTrayIcon>
#include "ui_MainWindow.h"
#include "RadioDevice.h"
#include "Preset.h"

class SoxReader;
class SoxWriter;

class MainWindow : public QDialog, Ui_MainWindow
{
Q_OBJECT
public:
	MainWindow();
	~MainWindow();
	
	static QString loadCSS(QString file);
public slots:
	void volumeChanged(int vol);
	void frequencyChanged(int freq);
	void toggledMute(bool mute);
	void refreshInfo();
	void settings();
	void stationChanged();
	void stationChanged(int index);
	void presetChanged();
	void trayIconActivated(QSystemTrayIcon::ActivationReason reason);
	void showWindow(bool show);
protected:
	void setupUi();
	void connectActions();
	void setupRadio();
	void loadPresets();
	void selectPresetSet(int set);
	
	void openRadio();
	void constructReader();
	void constructPlayer();
	
	void closeEvent(QCloseEvent* event);
private:
	RadioDevice m_device;
	SoxReader* m_reader;
	SoxWriter* m_playback;
	float m_fPreviousVol;
	QTimer m_timer;
	QSystemTrayIcon m_trayIcon;
	
	int m_nPreset;
	QList<PresetSet> m_presets;
	QActionGroup* m_groupPresets;
	QList<QAction*> m_actions;
};

#endif
