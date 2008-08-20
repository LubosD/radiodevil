#include "SoxReader.h"
#include "GeneralException.h"
#include <QtDebug>
#include <cstdlib>
#include <cmath>

const int SAMPLES_PER_CYCLE = 5000;

SoxReader::SoxReader(QString file, const char* type)
	: m_buffer(0), m_fVolume(1.0f), m_bAbort(false)
{
	sox_signalinfo_t info;
	memset(&info, 0, sizeof(info));
	
	info.rate = 44100;
	//info.encoding = SOX_ENCODING_UNKNOWN;
	info.channels = 2;
	
	sox_encodinginfo_t encinfo;
	memset(&encinfo, 0, sizeof(encinfo));
	
	encinfo.encoding = SOX_ENCODING_UNKNOWN;
	
	m_handle = sox_open_read(file.toUtf8().constData(), &info, &encinfo, type);
	
	if(!m_handle)
		throw GeneralException(QObject::tr("Cannot open \"%1\" for reading").arg(file));
}

SoxReader::~SoxReader()
{
	stop();
	sox_close(m_handle);
	free(m_buffer);
}

void SoxReader::stop()
{
	if(isRunning())
	{
		m_bAbort = true;
		wait();
	}
}

void SoxReader::globalInit()
{
	sox_format_init();
}

sox_signalinfo_t SoxReader::signal() const
{
	return m_handle->signal;
}

void SoxReader::run()
{
	try
	{
		while(!m_bAbort)
		{
			size_t numSamples = SAMPLES_PER_CYCLE;
			sox_sample_t* samples = read(&numSamples);
			
			foreach(SoxWriter* writer, m_writers)
				writer->addBuffer(samples, numSamples);
		}
	}
	catch(...)
	{
	}
}

int SoxReader::addWriter(SoxWriter* writer)
{
	m_writers << writer;
	return m_writers.size() - 1;
}

SoxWriter* SoxReader::removeWriter(int which)
{
	return m_writers.takeAt(which);
}

sox_sample_t* SoxReader::read(size_t* count)
{
	m_buffer = (sox_sample_t*) realloc(m_buffer, *count*sizeof(sox_sample_t));
	
	*count = sox_read(m_handle, m_buffer, *count);
	
	if(*count <= 0)
		throw GeneralException("Nothing read!");
	
	if(fabs(m_fVolume-1.0f) > 0.001f)
	{
		for(size_t i=0;i<*count;i++)
			m_buffer[i] *= m_fVolume;
	}
	
	return m_buffer;
}
