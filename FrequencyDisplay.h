#ifndef FREQUENCYDISPLAY_H
#define FREQUENCYDISPLAY_H
#include <QWidget>

class FrequencyDisplay : public QWidget
{
public:
	FrequencyDisplay(QWidget* parent);
	
	void setFrequency(float f);
	void setSignal(float s);
	void setStereo(bool s);
protected:
	void paintEvent(QPaintEvent* event);
private:
	float m_fFrequency, m_fSignal;
	bool m_bStereo;
};

#endif
