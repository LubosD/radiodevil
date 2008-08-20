#ifndef ALSAMIXER_H
#define ALSAMIXER_H
#include <QStringList>
#include <alsa/asoundlib.h>

class AlsaMixer
{
public:
	AlsaMixer(int device = 0);
	~AlsaMixer();
	
	struct MixerElem
	{
		snd_mixer_elem_t* elem;
		QString strName;
		bool bHasSwitch;
		
		long nMin, nMax;
	};
	
	void loadElems();
	QList<MixerElem> elems() const { return m_elems; }
	
	void setElemVolume(snd_mixer_elem_t* elem, long volume);
	long elemVolume(snd_mixer_elem_t* elem);
	
	static QStringList devices();
private:
	snd_mixer_t* m_mixer;
	QList<MixerElem> m_elems;
};

#endif
