#ifndef GENERALEXCEPTION_H
#define GENERALEXCEPTION_H
#include <QObject>
#include <QString>
#include <string>

class GeneralException : public QObject
{
public:
	GeneralException(QString error) : m_strError(error)
	{
	}
	GeneralException(const GeneralException& that) : QObject(), m_strError(that.m_strError)
	{
	}
	
	QString error() const { return m_strError; }
	std::string cError() const { return m_strError.toStdString(); }
private:
	QString m_strError;
};

#endif
