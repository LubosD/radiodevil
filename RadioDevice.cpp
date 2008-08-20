#include "RadioDevice.h"
#include "GeneralException.h"
#include <fcntl.h>
//#include <linux/videodev.h>
#include <linux/videodev2.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <QtDebug>

RadioDevice::RadioDevice()
	: m_fd(-1), /*m_attr(0), m_bMuted(false),*/ m_fFrequency(0)
{
	//m_nVolume = m_nBass = m_nTreble = m_nBalance = m_nTuners = m_nTuner = 0;
	m_nTuners = 0;
}

RadioDevice::~RadioDevice()
{
	close();
}

void RadioDevice::close()
{
	m_tuners.clear();
	if(m_fd >= 0)
	{
		::close(m_fd);
		m_fd = -1;
		//m_attr = 0;
		
		m_strName.clear();
	}
}

void RadioDevice::open(QString dev)
{
	struct stat buf;
	QByteArray name = dev.toAscii();
	
	close();
	
	try
	{
		if(stat(name.constData(), &buf) < 0)
			throw GeneralException(tr("Unable to stat the device"));
		
		if(!S_ISCHR(buf.st_mode))
			throw GeneralException(tr("The specified file is not a character device"));
		
		m_fd = ::open(name.constData(), O_RDWR | O_NONBLOCK, 0);
		if(m_fd < 0)
			throw GeneralException(tr("Unable to open the specified device"));
		
		v4l2_capability cap2;
		if(ioctl(m_fd, VIDIOC_QUERYCAP, &cap2) < 0)
		{
			perror("open()");
			throw GeneralException(tr("Not a V4L2 device"));
		}
		
		/*video_capability cap;
		if(ioctl(m_fd, VIDIOCGCAP, &cap) < 0)
		{
			perror("open()");
			throw GeneralException(tr("Not a V4L device"));
		}*/
		
		if(!(cap2.capabilities & V4L2_CAP_AUDIO))
			throw GeneralException(tr("Not an audio device: %1").arg(capToString(cap2.capabilities)));
		
		m_strName = (char*) cap2.card;
		
		//readAttributes();
		readTuners();
	}
	catch(...)
	{
		close();
		throw;
	}
}
/*
void RadioDevice::readAttributes()
{
	
	struct video_audio audio;
	if(ioctl(m_fd, VIDIOCGAUDIO, &audio) < 0)
		throw GeneralException(tr("VIDIOCGAUDIO failed"));
	
	if(audio.flags & VIDEO_AUDIO_MUTABLE)
		m_attr |= AttrMutable;
	if(audio.flags & VIDEO_AUDIO_VOLUME)
		m_attr |= AttrVolume;
	if(audio.flags & VIDEO_AUDIO_BASS)
		m_attr |= AttrBass;
	if(audio.flags & VIDEO_AUDIO_TREBLE)
		m_attr |= AttrTreble;
	if(audio.flags & VIDEO_AUDIO_BALANCE)
		m_attr |= AttrBalance;
	
	m_nVolume = audio.volume;
	m_nBass = audio.bass;
	m_nTreble = audio.treble;
	m_nBalance = audio.balance;
	m_bMuted = audio.flags & VIDEO_AUDIO_MUTE;
	
}

void RadioDevice::saveAttributes()
{
	struct video_audio audio;
	audio.audio = 0;
	audio.flags = 0;
	
	audio.flags |= m_bMuted ? VIDEO_AUDIO_MUTE : 0;
	audio.volume = m_nVolume;
	audio.bass = m_nBass;
	audio.treble = m_nTreble;
	audio.balance = m_nBalance;
	
	if(ioctl(m_fd, VIDIOCSAUDIO, &audio) < 0)
		throw GeneralException(tr("VIDIOCSAUDIO failed"));
	
}

void RadioDevice::setMute(bool bMute)
{
	
	m_bMuted = bMute;
	m_nVolume = bMute ? 0 : 65535;
	saveAttributes();
	
}
*/

void RadioDevice::unmute()
{
	v4l2_control ctrl;
	v4l2_queryctrl qctrl;
	
	memset(&ctrl, 0, sizeof(ctrl));
	memset(&qctrl, 0, sizeof(qctrl));
	
	qctrl.id = ctrl.id = V4L2_CID_AUDIO_VOLUME;
	
	if(ioctl(m_fd, VIDIOC_QUERYCTRL, &qctrl) == 0)
	{
		ctrl.value = qctrl.maximum;
		ioctl(m_fd, VIDIOC_S_CTRL, &ctrl);
	}
	
	qctrl.id = ctrl.id = V4L2_CID_AUDIO_MUTE;
	
	if(ioctl(m_fd, VIDIOC_QUERYCTRL, &qctrl) == 0)
	{
		ctrl.value = qctrl.minimum;
		ioctl(m_fd, VIDIOC_S_CTRL, &ctrl);
	}
}

void RadioDevice::readTuners()
{
	m_tuners.clear();
	
	for(m_nTuners = 0; m_nTuners < 1; m_nTuners++)
	{
		v4l2_tuner tuner;
		memset(&tuner, 0, sizeof(tuner));
		
		tuner.index = m_nTuners;
		
		if(ioctl(m_fd, VIDIOC_G_TUNER, &tuner) < 0)
			break;
		
		Tuner t;
		
		m_fDelta = ((tuner.capability & V4L2_TUNER_CAP_LOW) ? (1.0/16000.0) : (1.0/16.0));
		
		t.fqFrom = tuner.rangelow * m_fDelta;
		t.fqTo = tuner.rangehigh * m_fDelta;
		t.strName = (char*) tuner.name;
		
		m_tuners << t;
	}
	
	v4l2_frequency freq;
	memset(&freq, 0, sizeof(freq));
	
	freq.tuner = 0;
	
	if(ioctl(m_fd, VIDIOC_G_FREQUENCY, &freq) < 0)
		throw GeneralException(tr("Failed to read frequency"));
	m_fFrequency = freq.frequency*m_fDelta;
}

void RadioDevice::setFrequency(float fval)
{
	if(m_fd <= 0)
		return;
	
	if(fval < m_tuners[0].fqFrom-0.1 || fval > m_tuners[0].fqTo+0.1)
		throw GeneralException(tr("Frequency is out of range"));
	
	v4l2_frequency freq;
	memset(&freq, 0, sizeof(freq));
	
	freq.frequency = fval / m_fDelta;
	freq.type = V4L2_TUNER_RADIO;
	
	if(ioctl(m_fd, VIDIOC_S_FREQUENCY, &freq) < 0)
		throw GeneralException(tr("Failed to set frequency: %1").arg(strerror(errno)));
	
	m_fFrequency = fval;
}

RadioDevice::SignalInfo RadioDevice::signalInfo()
{
	v4l2_tuner tuner;
	SignalInfo info;
	
	memset(&tuner, 0, sizeof(tuner));
	
	tuner.index = 0;
	
	if(ioctl(m_fd, VIDIOC_G_TUNER, &tuner) < 0)
		throw GeneralException(tr("VIDIOC_G_TUNER failed"));
	
	info.bStereo = tuner.rxsubchans & V4L2_TUNER_SUB_STEREO;
	//info.bRDS = tuner.flags & VIDEO_TUNER_RDS_ON;
	//info.bMBS = tuner.flags & VIDEO_TUNER_MBS_ON;
	info.fSignal = tuner.signal / 65535.0f;
	
	return info;
}

QString RadioDevice::capToString(int cap)
{
#define HANDLE_CAP(x) if(cap & x) result += #x " "
	QString result;
	
	HANDLE_CAP(V4L2_CAP_VIDEO_CAPTURE);
	HANDLE_CAP(V4L2_CAP_VIDEO_OUTPUT);
	HANDLE_CAP(V4L2_CAP_VIDEO_OVERLAY);
	HANDLE_CAP(V4L2_CAP_VBI_CAPTURE);
	HANDLE_CAP(V4L2_CAP_VBI_OUTPUT);
	HANDLE_CAP(V4L2_CAP_SLICED_VBI_CAPTURE);
	HANDLE_CAP(V4L2_CAP_SLICED_VBI_OUTPUT);
	HANDLE_CAP(V4L2_CAP_RDS_CAPTURE);
	HANDLE_CAP(V4L2_CAP_VIDEO_OUTPUT_OVERLAY);
	HANDLE_CAP(V4L2_CAP_TUNER);
	HANDLE_CAP(V4L2_CAP_AUDIO);
	HANDLE_CAP(V4L2_CAP_RADIO);
	HANDLE_CAP(V4L2_CAP_READWRITE);
	HANDLE_CAP(V4L2_CAP_ASYNCIO);
	HANDLE_CAP(V4L2_CAP_STREAMING);
	
	return result;
#undef HANDLE_CAP
}

