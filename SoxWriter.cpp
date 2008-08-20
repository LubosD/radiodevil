#include "SoxWriter.h"
#include "GeneralException.h"
#include <cmath>
#include <QtDebug>

SoxWriter::SoxWriter(QString file, sox_signalinfo_t info, const char* type)
	: m_fVolume(1.0f), m_bAbort(false)
{
	sox_encodinginfo_t encinfo;
	memset(&encinfo, 0, sizeof(encinfo));
	
	encinfo.encoding = SOX_ENCODING_UNKNOWN;
	m_handle = sox_open_write(file.toUtf8().constData(), &info, &encinfo, type, 0, overwrite_permitted);
	
	if(!m_handle)
		throw GeneralException(QObject::tr("Cannot open \"%1\" for writing").arg(file));
}

SoxWriter::~SoxWriter()
{
	stop();
	sox_close(m_handle);
}

void SoxWriter::stop()
{
	if(isRunning())
	{
		m_bAbort = true;
		wait();
	}
}

void SoxWriter::run()
{
	while(!m_bAbort)
	{
		Block block;
		
		m_mutex.lock();
		
		if(!m_buffers.isEmpty())
			block = m_buffers.takeFirst();
		
		m_mutex.unlock();
		
		if(!block.isEmpty())
		{
			write(block.samples, block.count);
			delete [] block.samples;
		}
		//else
		//	qDebug() << "I'm hungry!";
		else
			usleep(10000);
	}
}

void SoxWriter::addBuffer(const sox_sample_t* samples, size_t count)
{
	sox_sample_t* mys = new sox_sample_t[count];
	memcpy(mys, samples, count*sizeof(sox_sample_t));
	
	m_mutex.lock();
	m_buffers << Block(mys, count);
	m_mutex.unlock();
}

void SoxWriter::write(sox_sample_t* samples, size_t count)
{
	size_t written = 0;
	
	if(fabs(m_fVolume-1.0f) > 0.001f)
	{
		for(size_t i=0;i<count;i++)
			samples[i] *= m_fVolume;
	}
	
	while(written < count)
	{
		size_t thistime = sox_write(m_handle, samples + written, count-written);
		if(thistime <= 0)
			throw GeneralException("sox_write() seems to have failed");
		written += thistime;
	}
}

sox_bool SoxWriter::overwrite_permitted(const char*)
{
	return sox_true;
}
