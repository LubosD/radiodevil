#ifndef CONTEXTTREEWIDGET_H
#define CONTEXTTREEWIDGET_H

#include <QMenu>
#include <QTreeWidget>

class ContextTreeWidget : public QTreeWidget
{
Q_OBJECT
public:
	ContextTreeWidget(QWidget* parent);
	void contextMenuEvent(QContextMenuEvent* event);
public slots:
	void addItem();
	void deleteItem();
private:
	QMenu m_menu;
};

#endif
