#ifndef LISTNAMEDLG_H
#define LISTNAMEDLG_H
#include <QDialog>
#include "ui_ListNameDlg.h"

class ListNameDlg : public QDialog, Ui_ListNameDlg
{
public:
	ListNameDlg(QWidget* parent)
	: QDialog(parent)
	{
		setupUi(this);
	}
	int exec()
	{
		int rval;
		lineName->setText(m_strName);
		rval = QDialog::exec();
		
		if(rval == QDialog::Accepted)
			m_strName = lineName->text();
		
		return rval;
	}
	QString m_strName;
};

#endif
