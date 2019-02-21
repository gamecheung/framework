#ifndef _X_LOG_WIDGET_H
#define _X_LOG_WIDGET_H

#include <QTableWidget>
#include <QHeaderView>
#include <QMenu>
#include <QTime>
#include <QLabel>
#include <QApplication>
#include <QClipboard>
#include <QKeyEvent>
#include <QVariantAnimation>
#include "XLogEx.h"

class XLogWidget : public QTableWidget
{
	Q_OBJECT

signals:
	void linkActivated(const QString& link);
	void itemAdded();
	void itemCleaned();

public:
	XLogWidget(QWidget* parent=nullptr)
		:QTableWidget(parent)
		,m_itemCountLimit(30)
		, m_copyAction(this)
		, m_clearAction(this)
		, m_flashing(false)
		, m_showWarningAndErrorOnly(false)
		, m_haveWarning(false)
		, m_haveError(false)
	{
		setColumnCount(3);
		horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
		horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
		verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
		horizontalHeader()->setVisible(false);
		verticalHeader()->setVisible(false);
		horizontalHeader()->setStretchLastSection(true);
		setEditTriggers(QAbstractItemView::NoEditTriggers);
		setAlternatingRowColors(true);
		setSelectionBehavior(QAbstractItemView::SelectRows);
		setShowGrid(false);

		setContextMenuPolicy(Qt::CustomContextMenu);
		setupMenu();
		
		m_infoIcon = qApp->style()->standardIcon(QStyle::SP_MessageBoxInformation);
		m_warnIcon = qApp->style()->standardIcon(QStyle::SP_MessageBoxWarning);
		m_errIcon = qApp->style()->standardIcon(QStyle::SP_MessageBoxCritical);
		m_infoColor = Qt::black;
		m_warnColor = Qt::darkYellow;
		m_errColor = Qt::red;
	}
	void setItemCountLimit(int limit){ m_itemCountLimit = limit; }
	void setInformationIcon(const QIcon& icon){ m_infoIcon = icon; }
	void setWarningIcon(const QIcon& icon){ m_warnIcon = icon; }
	void setErrorIcon(const QIcon& icon){ m_errIcon = icon; }
	void setInformationColor(const QColor& color){ m_infoColor = color; }
	void setWarningColor(const QColor& color){ m_warnColor = color; }
	void setErrorColor(const QColor& color){ m_errColor = color; }
	void setFlashing(bool flashing){ m_flashing = flashing; }
	void setShowWarningAndErrorOnly(bool yes){ m_showWarningAndErrorOnly = yes; }

	void setLog(XLogEx* log)
	{
		Q_ASSERT(log);
		connect(log, SIGNAL(infoWrited(const QDateTime&, const QString&)), 
			this, SLOT(addInformation(const QDateTime&, const QString&)));
		connect(log, SIGNAL(warnWrited(const QDateTime&, const QString&)),
			this, SLOT(addWarning(const QDateTime&, const QString&)));
		connect(log, SIGNAL(errorWrited(const QDateTime&, const QString&)),
			this, SLOT(addError(const QDateTime&, const QString&)));
	}

	XLog::Level level()const
	{
		if (m_haveError)
			return XLog::Error;
		if (m_haveWarning)
			return XLog::Warn;
		return XLog::Info;
	}

public slots :
	void addInformation(const QString& text) { addItem(m_infoIcon,QDateTime::currentDateTime(), text, m_infoColor); }
	void addWarning(const QString& text) { addItem(m_warnIcon, QDateTime::currentDateTime(), text, m_warnColor); }
	void addError(const QString& text) { addItem(m_errIcon, QDateTime::currentDateTime(), text, m_errColor); }
	void addInformation(const QDateTime& datetime, const QString& text) { addItem(m_infoIcon,datetime,text,m_infoColor); }
	void addWarning(const QDateTime& datetime, const QString& text){ addItem(m_warnIcon, datetime, text, m_warnColor); }
	void addError(const QDateTime& datetime, const QString& text){ addItem(m_errIcon, datetime, text, m_errColor); }

protected:
	void keyPressEvent(QKeyEvent *event)
	{
		if (event->matches(QKeySequence::Copy))
		{
			copy();
			return;
		}
		QTableWidget::keyPressEvent(event);
	}

private slots:
	void copy()
	{
		QStringList strings;
		QStringList cols;
		for (int i = 0, total = rowCount(); i < total; ++i)
		{
			if (!item(i, 0)->isSelected())
				continue;

			cols.clear();
			for (int c = 1; c <= 2; c++)
			{
				if (cellWidget(i, c))
				{
					QLabel* l = dynamic_cast<QLabel*>(cellWidget(i, c));
					if (l)
						cols << l->text();
				}
				else
				{
					cols << item(i, c)->text();
				}
			}
			strings << cols.join(" ");
		}

		QApplication::clipboard()->setText(strings.join("\n"));
	}
	void onCustomContextMenuRequested(const QPoint& pos)
	{
		m_copyAction.setEnabled(selectionModel()->selectedRows().size() > 0);
		m_menu.popup(mapToGlobal(pos));
	}
	void reset()
	{
		for (auto anim : m_itemAnimations)
			anim->stop();

		m_itemAnimations.clear();
		setRowCount(0);
		m_haveWarning = false;
		m_haveError = false;
		emit itemCleaned();
	}

private:
	void addItem(const QIcon& icon, const QDateTime& dateTime, const QString& text, const QColor& color)
	{
		if (m_showWarningAndErrorOnly && (&icon == &m_infoIcon))
			return;

		if (&icon == &m_warnIcon)
			m_haveWarning = true;
		else if (&icon == &m_errIcon)
			m_haveError = true;

		int row = rowCount();
		setRowCount(row + 1);

		if (row > m_itemCountLimit)
		{
			removeRow(0);
			row--;
		}

		QList<QTableWidgetItem*> itemsCreated;
		QTableWidgetItem* item = nullptr;

		item = new QTableWidgetItem();
		item->setIcon(icon);
		setItem(row, 0, item);
		itemsCreated << item;

		QString timeStr = "[" + dateTime.toString("hh:mm:ss") + "]";
		item = new QTableWidgetItem(timeStr);
		item->setForeground(QBrush(color));
		setItem(row, 1, item);
		itemsCreated << item;

		item = new QTableWidgetItem();
		item->setForeground(QBrush(color));
		setItem(row, 2, item);
		itemsCreated << item;

		// While QLabel does detect if the text is rich automatically, we don't want to use qlabel for plain text,
		// because it's not wrapped correctly if the text is longer.
		if (text.contains("<"))
		{
			QLabel* label = new QLabel(text);
			QPalette pa = label->palette();
			pa.setColor(QPalette::Text, Qt::red);
			label->setPalette(pa);
			connect(label, SIGNAL(linkActivated(QString)), this, SIGNAL(linkActivated(QString)));
			setCellWidget(row, 2, label);
		}
		else
		{
			item->setText(text);
		}

		//setVisible(true);
		scrollToBottom();

		if (m_flashing)
			flashItems(itemsCreated, color);

		emit itemAdded();
	}

	void setupMenu()
	{
		m_copyAction.setText(tr("Copy"));
		m_copyAction.setShortcut(QKeySequence::Copy);
		connect(&m_copyAction, SIGNAL(triggered()), this, SLOT(copy()));
		m_menu.addAction(&m_copyAction);

		m_menu.addSeparator();

		m_clearAction.setText(tr("Clear"));
		connect(&m_clearAction, SIGNAL(triggered()), this, SLOT(reset()));
		m_menu.addAction(&m_clearAction);

		connect(this, SIGNAL(customContextMenuRequested(const QPoint &)),
			this, SLOT(onCustomContextMenuRequested(const QPoint&)));
	}

	void flashItems(const QList<QTableWidgetItem*>& items, const QColor& color)
	{
		QColor alphaColor = color;
		alphaColor.setAlpha(0);

		QColor finalColor = color;
		finalColor.setAlpha(150);

		QVariantAnimation* anim = new QVariantAnimation();
		anim->setDuration(500);
		anim->setEasingCurve(QEasingCurve::OutQuad);
		anim->setStartValue(finalColor);
		anim->setEndValue(alphaColor);

		m_itemAnimations << anim;
		connect(anim, &QObject::destroyed, [this, anim]() {m_itemAnimations.removeOne(anim); });

		connect(anim, &QVariantAnimation::valueChanged, [items](const QVariant& value)
		{
			for (QTableWidgetItem* item : items)
				item->setBackground(value.value<QColor>());
		});

		anim->start(QAbstractAnimation::DeleteWhenStopped);
	}

private:
	int		m_itemCountLimit;
	QIcon	m_infoIcon;
	QIcon	m_warnIcon;
	QIcon	m_errIcon;
	QColor	m_infoColor;
	QColor	m_warnColor;
	QColor	m_errColor;
	QMenu	m_menu;
	QAction	m_copyAction;
	QAction	m_clearAction;
	QList<QAbstractAnimation*> m_itemAnimations;
	bool	m_flashing;
	bool	m_showWarningAndErrorOnly;

	bool	m_haveWarning;
	bool	m_haveError;
};

#endif //_X_LOG_WIDGET_H
