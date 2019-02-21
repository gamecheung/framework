#ifndef _X_TEXT_LENGTH_EDITOR_H
#define _X_TEXT_LENGTH_EDITOR_H

#include <QComboBox>
#include <QSpinBox>
#include <QTextLength>
#include <Qt/XComboBoxHelper.h>

class XTextLengthEditor : public QObject
{
	Q_OBJECT

signals:
	void valueChanged(const QTextLength& value);

public:
	XTextLengthEditor(QComboBox* combo,QSpinBox* spin)
		:QObject(combo),m_combo(combo),m_spin(spin)
	{
		m_combo->addItem(tr("Variable"), QTextLength::VariableLength);
		m_combo->addItem(tr("Fixed"), QTextLength::FixedLength);
		m_combo->addItem(tr("Percent"), QTextLength::PercentageLength);
		connect(m_combo, SIGNAL(currentIndexChanged(int)), this, SLOT(updateSpinBox()));
		connect(m_combo, SIGNAL(currentIndexChanged(int)), this, SLOT(trigger()));
		connect(m_spin, SIGNAL(valueChanged(int)), this, SLOT(trigger()));
	}

	QTextLength::Type type()const{ return (QTextLength::Type)m_combo->currentData().toInt(); }

public slots:
	void setValue(const QTextLength& value)
	{
		if (value != this->value())
		{
			QSignalBlocker blocker(this);
			XCOMBO_BOX_HELPER(m_combo)->setCurrentData(value.type());
			m_spin->setValue(value.rawValue());
			trigger();
		}
	}
	QTextLength value()const
	{
		return QTextLength ((QTextLength::Type)m_combo->currentData().toInt(), m_spin->value());
	}
	void trigger()
	{
		emit valueChanged(value());
	}

private slots:
	void updateSpinBox()
	{
		switch (type())
		{
		case QTextLength::VariableLength:
			m_spin->setEnabled(false);
			break;
		case QTextLength::FixedLength:
			m_spin->setEnabled(true); 
			m_spin->setRange(1,99999);
			m_spin->setSuffix("");
			break;
		case QTextLength::PercentageLength:
			m_spin->setEnabled(true);
			m_spin->setRange(1, 100);
			m_spin->setSuffix("%");
			break;
		}
	}
	
private:
	QComboBox*	m_combo;
	QSpinBox*	m_spin;
};

#endif //_X_TEXT_LENGTH_EDITOR_H
