#ifndef SOXWRITER_H
#define SOXWRITER_H
#include <QString>
#include <QMutex>
#include <QThread>

extern "C"
{
	#include <sox.h>
}

class SoxWriter : public QThread
{
public:
	SoxWriter(QString file, sox_signalinfo_t info, const char* type = "alsa");
	~SoxWriter();
	
	void stop();
	void run();
	
	void addBuffer(const sox_sample_t* samples, size_t count);
	void write(sox_sample_t* samples, size_t count);
	
	static sox_bool overwrite_permitted(const char *filename);
	
	void setVolume(float vol) { m_fVolume = vol; }
	float volume() const { return m_fVolume; }
private:
	sox_format_t* m_handle;
	float m_fVolume;
	QMutex m_mutex;
	
	struct Block
	{
		Block() : samples(0), count(0) {}
		Block(sox_sample_t* s, size_t c) : samples(s), count(c) {}
		bool isEmpty() const { return !samples; }
		
		sox_sample_t* samples;
		size_t count;
	};
	
	QList<Block> m_buffers;
	bool m_bAbort;
};

#endif
