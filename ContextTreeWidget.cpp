#include "ContextTreeWidget.h"
#include <QContextMenuEvent>

ContextTreeWidget::ContextTreeWidget(QWidget* parent)
	: QTreeWidget(parent)
{
	QAction* action;
	action = m_menu.addAction(tr("Add"));
	connect(action, SIGNAL(triggered()), this, SLOT(addItem()));
	
	action = m_menu.addAction(tr("Delete"));
	connect(action, SIGNAL(triggered()), this, SLOT(deleteItem()));
}

void ContextTreeWidget::contextMenuEvent(QContextMenuEvent* event)
{
	m_menu.exec(mapToGlobal(event->pos()));
}

void ContextTreeWidget::addItem()
{
	QTreeWidgetItem* item = new QTreeWidgetItem(this);
	item->setIcon(0, QIcon(":/menu/miscellaneous.png"));
	addTopLevelItem(item);
	item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsDragEnabled);
	editItem(item, 0);
}

void ContextTreeWidget::deleteItem()
{
	QTreeWidgetItem* i = currentItem();
	if(i != 0)
		delete takeTopLevelItem(indexOfTopLevelItem(i));
}
