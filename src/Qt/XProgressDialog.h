#ifndef _X_PROGRESS_DIALOG
#define _X_PROGRESS_DIALOG

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QProgressBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStackedLayout>
#include <QApplication>
#include <QThread>
#include <QPainter>
#include <QStyleOption>

class XProgressDialog : public QDialog
{
	Q_OBJECT
public:
	XProgressDialog(QWidget *parent = Q_NULLPTR) :QDialog(parent)
	{
		setWindowModality(Qt::WindowModal);

		m_text = new QLabel(this);
		m_progressBar = new QProgressBar(this);
		m_progressBar->setRange(0, 0);
		m_widget = new QWidget(this);
		QVBoxLayout* widgetLayout = new QVBoxLayout();
		m_widget->setLayout(widgetLayout);
		widgetLayout->addWidget(m_text);
		widgetLayout->addWidget(m_progressBar);
		widgetLayout->addStretch();

		m_okButton = new QPushButton(tr("OK"), this);
		connect(m_okButton, SIGNAL(clicked()), this, SLOT(setOkFlag()));
		m_cancelButton = new QPushButton(tr("Cancel"), this);
		connect(m_cancelButton, SIGNAL(clicked()), this, SLOT(setCancelFlag()));

		m_label = new QLabel(this);
		QFont font = m_label->font();
		font.setBold(true);
		font.setPointSize(font.pointSize()*1.2);
		m_label->setFont(font);
		m_stackedLayout = new QStackedLayout();
		m_stackedLayout->addWidget(m_widget);
		m_buttonsLayout = new QHBoxLayout();
		m_buttonsLayout->addStretch();
		m_buttonsLayout->addWidget(m_okButton);
		m_buttonsLayout->addWidget(m_cancelButton);
		
		m_layout = new QVBoxLayout();
		m_layout->addWidget(m_label);
		m_layout->addLayout(m_stackedLayout);
		m_layout->addLayout(m_buttonsLayout);
		setLayout(m_layout);

		m_okFlag = false;
		m_cancelFlag = false;

		m_autoHideButton = false;
	}

public slots :
	void setLabel(const QString& label)
	{ 
		m_label->setText(label); 
	}
	void setText(const QString& text)
	{
		m_text->setText(text);
	}
	void setOkFlag(){ m_okFlag = true; }
	void setCancelFlag(){ m_cancelFlag = true; }
	void setButtonsEnabled(bool ok, bool cancel)
	{
		m_okButton->setEnabled(ok);
		m_cancelButton->setEnabled(cancel);

		if (m_autoHideButton)
		{
			maybeHideButton(m_okButton);
			maybeHideButton(m_cancelButton);
		}
	}
	void setText(const QString& text, bool ok, bool cancel)
	{
		setText(text);
		setButtonsEnabled(ok, cancel);
	}

	void setAutoHideButton(bool yes){ m_autoHideButton = yes; }

public:
	bool waitOkButtonClicked()
	{
		while (!m_okFlag && !m_cancelFlag)
		{
			QThread::msleep(10);
		}
		bool rs = m_okFlag;
		m_okFlag = m_cancelFlag = false;
		return rs;
	}
	bool waitCancelButtonCliced()
	{
		while (!m_okFlag && !m_cancelFlag)
		{
			QThread::msleep(10);
		}
		bool rs = m_cancelFlag;
		m_okFlag = m_cancelFlag = false;
		return rs;
	}

private:
	void maybeHideButton(QPushButton* button)
	{
		button->setVisible(button->isEnabled());
	}


private:
	QVBoxLayout*	m_layout;
	QStackedLayout* m_stackedLayout;
	QHBoxLayout*	m_buttonsLayout;
	QLabel*			m_label;
	QLabel*			m_text;
	QProgressBar*	m_progressBar;
	QWidget*		m_widget;
	QPushButton*	m_okButton;
	QPushButton*	m_cancelButton;

	bool m_okFlag;
	bool m_cancelFlag;

	bool m_autoHideButton;
};


class XProgress : public QThread
{
	Q_OBJECT
signals:
	void wantSetLabel(const QString& label);
	void wantSetText(const QString& text);
	void wantSetText(const QString& text,bool ok,bool cancel);

public:
	XProgress(XProgressDialog* dlg) 
		:m_dlg(dlg), QThread(dlg)
	{
		connect(this, SIGNAL(started()), m_dlg, SLOT(show()));
		connect(this, SIGNAL(finished()), m_dlg, SLOT(close()));
		connect(m_dlg, SIGNAL(destroyed()), this, SLOT(terminate()));

		connect(this, SIGNAL(wantSetLabel(const QString&)), m_dlg, SLOT(setLabel(const QString&)));
		connect(this, SIGNAL(wantSetText(const QString&)), m_dlg, SLOT(setText(const QString&)));
		connect(this, SIGNAL(wantSetText(const QString&,bool,bool)), m_dlg, SLOT(setText(const QString&,bool,bool)));
	}

	XProgressDialog* dialog(){ return m_dlg; }
	void setLabel(const QString& label){ emit wantSetLabel(label); }
	void setText(const QString& text){ emit wantSetText(text); }
	void setText(const QString& text, bool ok, bool cancel){ emit wantSetText(text, ok, cancel); }
	bool waitOkButtonClicked(){ return m_dlg->waitOkButtonClicked(); }
	bool waitCancelButtonCliced(){ return m_dlg->waitCancelButtonCliced(); }

private:
	XProgressDialog* m_dlg;
};

#endif //_X_PROGRESS_DIALOG
