#include "AlsaMixer.h"
#include "GeneralException.h"
#include <QtDebug>

AlsaMixer::AlsaMixer(int device) : m_mixer(0)
{
	int err;
	QString name;
	
	if((err = snd_mixer_open(&m_mixer, 0)) < 0)
		throw GeneralException(QObject::tr("Unable to open the ALSA mixer: ") + snd_strerror(err));
	
	name = QString("hw:%1").arg(device);
	if((err = snd_mixer_attach(m_mixer, name.toLatin1())) < 0)
		throw GeneralException(QObject::tr("Unable to open the ALSA mixer device: ") + snd_strerror(err));
	
	if((err = snd_mixer_load(m_mixer)) < 0)
		throw GeneralException(QObject::tr("Unable to load the mixer data: ") + snd_strerror(err));
	
	if((err = snd_mixer_selem_register(m_mixer, NULL, NULL)) < 0)
		throw GeneralException(QObject::tr("snd_mixer_selem_register failed: ") + snd_strerror(err));
	
	loadElems();
}

AlsaMixer::~AlsaMixer()
{
	if(m_mixer != 0)
	{
		snd_mixer_free(m_mixer);
		snd_mixer_close(m_mixer);
	}
}

QStringList AlsaMixer::devices()
{
	QStringList names;
	
	for(int i=0;;i++)
	{
		char* name;
		
		if(snd_card_get_name(i, &name) < 0)
			break;
		
		names << name;
	}
	
	return names;
}

void AlsaMixer::loadElems()
{
	QList<MixerElem> list;
	
	m_elems.clear();
	
	for(snd_mixer_elem_t* e = snd_mixer_first_elem(m_mixer); e; e = snd_mixer_elem_next(e))
	{
		if(!snd_mixer_selem_has_playback_volume(e))
			continue;
		
		MixerElem elem;
		elem.elem = e;
		elem.strName = snd_mixer_selem_get_name(e);
		elem.bHasSwitch = snd_mixer_selem_has_playback_switch(e);
		snd_mixer_selem_get_playback_volume_range(e, &elem.nMin, &elem.nMax);
		
		m_elems << elem;
	}
}

void AlsaMixer::setElemVolume(snd_mixer_elem_t* elem, long volume)
{
	if(snd_mixer_selem_has_playback_switch(elem))
		snd_mixer_selem_set_playback_switch_all(elem, volume ? 1 : 0);
	snd_mixer_selem_set_playback_volume_all(elem, volume);
}

long AlsaMixer::elemVolume(snd_mixer_elem_t* elem)
{
	long val;
	snd_mixer_selem_get_playback_volume(elem, SND_MIXER_SCHN_MONO, &val);
	return val;
}

