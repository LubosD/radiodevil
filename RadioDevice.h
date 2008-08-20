#ifndef RADIODEVICE_H
#define RADIODEVICE_H
#include <QObject>
#include <QString>

class RadioDevice : public QObject
{
Q_OBJECT
public:
	RadioDevice();
	~RadioDevice();
	void open(QString dev);
	void close();
	QString name() const { return m_strName; }
	
	//bool muted() const { return m_bMuted; }
	//void setMute(bool bMute);
	
	//void readAttributes();
	//void saveAttributes();
	
	/*enum Attributes
	{
		AttrMutable = 1, AttrVolume = 2, AttrBass = 4, AttrTreble = 8, AttrBalance = 16
	};*/
	
	void unmute();
	
	struct Tuner
	{
		float fqFrom, fqTo;
		QString strName;
	};
	struct SignalInfo
	{
		bool bStereo /*,bRDS, bMBS*/;
		float fSignal; // 0.0f - 1.0f
	};
	
	void readTuners();
	Tuner tuner() const { return m_tuners[0]; }
	
	float frequency() const { return m_fFrequency; }
	void setFrequency(float freq);
	
	//int attributes() const { return m_attr; }
	
	SignalInfo signalInfo();
	static QString capToString(int cap);
private:
	int m_fd;
	QString m_strName;
	
	//int m_attr;
	//bool m_bMuted;
	//int m_nVolume, m_nBass, m_nTreble, m_nBalance;
	int m_nTuners;
	int m_nTuner;
	float m_fFrequency, m_fDelta;
	
	QList<Tuner> m_tuners;
};

#endif
