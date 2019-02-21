#ifndef _X_ALIGNMENT_EDITOR_H
#define _X_ALIGNMENT_EDITOR_H

#include <QAbstractButton>
#include <QButtonGroup>

class XAlignmentEditor:public QObject
{
	Q_OBJECT

signals:
	void alignmentChanged(Qt::Alignment align);

public:
	XAlignmentEditor(QObject* parent = nullptr) :QObject(parent)
	{
		m_btnGroupHorizontal = new QButtonGroup(this);
		m_btnGroupVertical = new QButtonGroup(this);
		connect(m_btnGroupHorizontal, SIGNAL(buttonClicked(int)), this, SLOT(updateData()));
		connect(m_btnGroupVertical, SIGNAL(buttonClicked(int)), this, SLOT(updateData()));
	}
	void setHorizontalWidgets(QAbstractButton* left, QAbstractButton* hcenter,
		QAbstractButton* right, QAbstractButton* justify = nullptr)
	{
		config(left, Qt::AlignLeft, m_btnGroupHorizontal);
		config(hcenter, Qt::AlignHCenter, m_btnGroupHorizontal);
		config(right, Qt::AlignRight, m_btnGroupHorizontal);
		if (justify)
			config(justify, Qt::AlignJustify, m_btnGroupHorizontal);
	}
	void setVerticalWidgets(QAbstractButton* top, QAbstractButton* vcenter, QAbstractButton* bottom)
	{
		config(top, Qt::AlignTop, m_btnGroupVertical);
		config(vcenter, Qt::AlignVCenter, m_btnGroupVertical);
		config(bottom, Qt::AlignBottom, m_btnGroupVertical);
	}

	Qt::Alignment alignment()const
	{
		return (Qt::Alignment)(m_btnGroupHorizontal->checkedId() | m_btnGroupVertical->checkedId());
	}

public slots:
	void setAlignment(Qt::Alignment align)
	{
		setButtonChecked(m_btnGroupHorizontal, align, Qt::AlignLeft);
		setButtonChecked(m_btnGroupHorizontal, align, Qt::AlignHCenter);
		setButtonChecked(m_btnGroupHorizontal, align, Qt::AlignRight);
		setButtonChecked(m_btnGroupHorizontal, align, Qt::AlignJustify);

		setButtonChecked(m_btnGroupVertical, align, Qt::AlignTop);
		setButtonChecked(m_btnGroupVertical, align, Qt::AlignVCenter);
		setButtonChecked(m_btnGroupVertical, align, Qt::AlignBottom);
	}

private slots:
	void updateData()
	{
		emit alignmentChanged(alignment());
	}

private:
	void config(QAbstractButton* btn, Qt::Alignment align, QButtonGroup* btnGroup)
	{
		Q_ASSERT(btn);
		Q_ASSERT(btnGroup);

		btn->setCheckable(true);
		btnGroup->addButton(btn, align);
	}
	void setButtonChecked(QButtonGroup* btnGroup, Qt::Alignment align, Qt::Alignment flag)
	{
		if (align& flag)
			btnGroup->button(flag)->setChecked(true);
	}
	
private:
	QButtonGroup* m_btnGroupHorizontal;
	QButtonGroup* m_btnGroupVertical;
};

#endif //_X_ALIGNMENT_EDITOR_H
