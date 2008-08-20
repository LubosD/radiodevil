#ifndef SOXREADER_H
#define SOXREADER_H
#include <QString>
#include "SoxWriter.h"

extern "C"
{
	#include <sox.h>
}

class SoxReader : public QThread
{
public:
	SoxReader(QString file, const char* type = "alsa");
	~SoxReader();
	
	static void globalInit();
	
	sox_signalinfo_t signal() const;
	sox_sample_t* read(size_t* count);
	
	int addWriter(SoxWriter* writer);
	SoxWriter* removeWriter(int which);
	
	void stop();
	void run();
	
	void setVolume(float vol) { m_fVolume = vol; }
	float volume() const { return m_fVolume; }
private:
	sox_format_t* m_handle;
	sox_sample_t* m_buffer;
	float m_fVolume;
	QList<SoxWriter*> m_writers;
	bool m_bAbort;
};

#endif
