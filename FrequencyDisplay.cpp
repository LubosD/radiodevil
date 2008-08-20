#include "FrequencyDisplay.h"
#include <QPaintEvent>
#include <QPainter>

FrequencyDisplay::FrequencyDisplay(QWidget* parent)
	: QWidget(parent), m_fFrequency(100), m_fSignal(0), m_bStereo(false)
{
}

void FrequencyDisplay::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);
	const int height = this->height(), width = this->width();
	
	painter.setClipRegion(event->region());
	painter.fillRect(event->rect(), QBrush(Qt::black));
	painter.setRenderHint(QPainter::TextAntialiasing);
	painter.setRenderHint(QPainter::Antialiasing);
	
	painter.setPen(Qt::green);
	
	QFont defaultFont, font = painter.font();
	defaultFont = font;
	
	font.setBold(true);
	painter.setFont(font);
	
	if(m_bStereo)
	{
		//painter.drawText(0, 0, width, height, Qt::AlignRight | Qt::AlignTop, "STEREO ");
		painter.drawEllipse(width-25, 5, 14, 14);
		painter.drawEllipse(width-25-7, 5, 14, 14);
	}
	painter.drawText(width-34, height-19, 25, 19, Qt::AlignCenter, "MHz");
	
	font.setPixelSize(height*0.75);
	painter.setFont(font);
	painter.drawText(20, 0, width-40, height, Qt::AlignCenter, QString("%1").arg(m_fFrequency, 0, 'f', 1));
	
	
	float barlen = (height-10-10)*m_fSignal;
	painter.fillRect(5, height - 15 - barlen, 10, barlen, Qt::blue);
	painter.setPen(Qt::white);
	painter.drawLine(10, height-12, 10, height-2);
	painter.drawLine(10, height-5, 5, height-12);
	painter.drawLine(10, height-5, 15, height-12);
}

void FrequencyDisplay::setFrequency(float f)
{
	if(f != m_fFrequency)
	{
		m_fFrequency = f;
		update();
	}
}

void FrequencyDisplay::setSignal(float s)
{
	if(s != m_fSignal)
	{
		m_fSignal = s;
		update();
	}
}

void FrequencyDisplay::setStereo(bool s)
{
	if(s != m_bStereo)
	{
		m_bStereo = s;
		update();
	}
}
