#ifndef _X_BUTTON_GROUP_H
#define _X_BUTTON_GROUP_H

#include <QAbstractButton>
#include <QGroupBox>
#include <QList>
#include <QHash>

class XButtonGroup : QObject
{
	Q_OBJECT
signals:
	void buttonClicked(QWidget* button);
	void buttonClicked(int id);
	void buttonToggled(QWidget* button, bool checked);
	void buttonToggled(int id, bool checked);
	void checkedButtonChanged(QWidget* button);
	void checkedButtonChanged(int id);

public:
	XButtonGroup(QObject* parent = nullptr) :QObject(parent),m_checkedButton(nullptr){}

	void addButton(QWidget* button, int id = -1)
	{
		if (m_buttons.contains(button))
			return;

		m_buttons.append(button);
		m_mapping[button] = id;
		connect(button, SIGNAL(clicked(bool)), this, SLOT(onButtonClicked(bool)));
		connect(button, SIGNAL(toggled(bool)), this, SLOT(onButtonToggled(bool)));
	}
	void removeButton(QWidget* button)
	{
		disconnect(button, 0, this, 0);
		m_buttons.removeOne(button);
	}

	QList<QWidget*> buttons()const{ return m_buttons; }

	QWidget* checkedButton()const{ return m_checkedButton; }
	int checkedId()const{ return m_mapping.value(m_checkedButton, -1); }

	QWidget* button(int id)const
	{
		for (auto btn: m_buttons)
		{
			if (this->id(btn) == id)
				return btn;
		}
		return nullptr;
	}
	void setId(QWidget* button, int id){ m_mapping[button] = id; }
	int id(QWidget* button)const{ return m_mapping.value(button, -1); }

private slots:
	void onButtonClicked(bool checked)
	{
		QWidget* w = qobject_cast<QWidget*>(sender());
		emit buttonClicked(w);
		emit buttonClicked(m_mapping.value(w,-1));
	}

	void onButtonToggled(bool on)
	{
		QWidget* w = qobject_cast<QWidget*>(sender());

		if (on && m_checkedButton!=w)
		{
			m_checkedButton = w;
		}

		for (auto& button : m_buttons)
		{
			setButtonChecked(button, button == m_checkedButton);
		}

		emit buttonToggled(w,on);
		emit buttonToggled(m_mapping.value(w, -1),on);
	}

private:
	void setButtonChecked(QWidget* button, bool checked)
	{
		QAbstractButton* b1 = qobject_cast<QAbstractButton*>(button);
		if (b1)
		{
			if (b1->isChecked()!=checked)
				b1->setChecked(checked);
		}
		else
		{
			QGroupBox* b2 = qobject_cast<QGroupBox*>(button);
			if (b2)
			{
				if (b2->isChecked()!=checked)
					b2->setChecked(checked);
			}
			else
			{
				Q_ASSERT(false);
			}
		}
	}

private:
	QList<QWidget*>	m_buttons;
	QWidget*		m_checkedButton;
	QHash<QWidget*, int> m_mapping;
};

#endif //_X_BUTTON_GROUP_H
