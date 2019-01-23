#ifndef _X_ITEM_EDITOR_H
#define _X_ITEM_EDITOR_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFocusEvent>
#include <QSpinBox>
#include <QDebug>
#include <QPushButton>
#include <QColorDialog>
#include <QFontDialog>
#include <QCheckBox>
#include <QComboBox>
#include <QApplication>
#include <QButtonGroup>
#include <QToolButton>
#include "XQt.h"

class XItemEditor : public QWidget{
	Q_OBJECT
	signals :
	void textChanged(const QString&);
	void valueChanged();
	void valueEdited();

public:
	XItemEditor(QWidget* parent=nullptr)
		:_editor(nullptr),_autoCloseEditor(false){
		_label.setParent(this);
	}
	virtual ~XItemEditor(){}
	void setText(const QString& text){ 
		bool isTextChanged = (_label.text() != text);
		_label.setText(text);
		if (isTextChanged)
			emit textChanged(text);
	}
	QString text()const{ return _label.text(); }
protected:
	virtual QWidget* createEditor() = 0;
	virtual void setEditorData(QWidget* editor) = 0;
	virtual QString itemData(QWidget* editor) = 0;
	void mousePressEvent(QMouseEvent *event){ edit(); }
	bool eventFilter(QObject *obj, QEvent *event){
		if (obj == _editor && event->type() == QEvent::FocusOut){
			if (_autoCloseEditor){
				closeEditor();
				return true;
			}
		}
		return QWidget::eventFilter(obj, event);
	}
	void resizeEvent(QResizeEvent *event){
		QWidget::resizeEvent(event);
		_label.setFixedSize(event->size());
		editor()->setFixedSize(event->size());
	}

	QWidget* editor(){ 
		if (_editor == nullptr){
			_editor = createEditor();
			Q_ASSERT(_editor);
			_editor->setVisible(false);
			_editor->setParent(this);
			_editor->installEventFilter(this);
		}
		return _editor; 
	}
	QLabel* label(){ return &_label; }
	void setAutoCloseEditor(bool autoClose){ _autoCloseEditor = autoClose; }

protected slots:
	void edit(){
		_label.setVisible(false);
		editor(); // if editor is null, create it
		setEditorData(_editor);
		_editor->setVisible(true);
		_editor->setFocus();
	}
	void closeEditor(){
		Q_ASSERT(_editor);
		setText(itemData(_editor));
		_editor->clearFocus();
		_editor->setVisible(false);
		_label.setVisible(true);
		emit valueChanged();
	}

private:
	QLabel		_label;
	QWidget*	_editor;
	bool		_autoCloseEditor;
};

class XTextItemEditor :public XItemEditor{
	Q_OBJECT
public:
	XTextItemEditor(const QString& text = QString(), QWidget* parent = nullptr)
		:XItemEditor(parent){
		setText(text);
	}

	void setInputMask(const QString& mask){ m_inputMask = mask; }

protected:
	QWidget* createEditor(){
		QLineEdit* editor = new QLineEdit(this);
		editor->setInputMask(m_inputMask);
		connect(editor, SIGNAL(editingFinished()), this, SLOT(closeEditor()));
		return editor;
	}
	void setEditorData(QWidget* editor){
		QLineEdit* lineEdit = static_cast<QLineEdit*>(editor);
		lineEdit->setText(text());
	}
	QString itemData(QWidget* editor){
		QLineEdit* lineEdit = static_cast<QLineEdit*>(editor);
		return lineEdit->text();
	}

private:
	QString m_inputMask;
};

class XSpinBoxItemEditor :public XItemEditor{
	Q_OBJECT
signals:
	void valueChanged(int);

public:
	XSpinBoxItemEditor(QWidget* parent = nullptr)
		:XItemEditor(parent){
		setAutoCloseEditor(true);
	}

	int value()const{ return m_editor->value(); }

public slots:
	void setValue(int val){
		setText(QString::number(val));
	}

protected:
	QWidget* createEditor(){
		m_editor = new QSpinBox(this);
		connect(m_editor, SIGNAL(valueChanged(int)), this, SIGNAL(valueChanged(int)));
		m_editor->setRange(0, 1000);
		return m_editor;
	}
	void setEditorData(QWidget* editor){
		QSpinBox* spinBox = static_cast<QSpinBox*>(editor);
		spinBox->setValue(text().toInt());
	}
	QString itemData(QWidget* editor){
		QSpinBox* spinBox = static_cast<QSpinBox*>(editor);
		return QString::number(spinBox->value());
	}

private:
	QSpinBox* m_editor;
};

class XColorItemEditor :public XItemEditor{
	Q_OBJECT
signals:
	void colorChanged(const QColor&);
public:
	XColorItemEditor(QWidget* parent = nullptr)
		:XItemEditor(parent){
	}
	void setColor(const QColor& color){ 
		if (_color != color){
			_color = color;
			updateColor();
		}
	}
	const QColor& color()const{ return _color; }

protected:
	QWidget* createEditor(){
		QPushButton* editor = new QPushButton(this);
		connect(editor, SIGNAL(clicked()), this, SLOT(selectColor()));
		return editor;
	}
	void setEditorData(QWidget* editor){
		//QPushButton* pushButton = static_cast<QPushButton*>(editor);
		//spinBox->setValue(text().toInt());
	}
	QString itemData(QWidget* editor){
		return "";
	}

private slots:
	void selectColor(){
		QColor color = QColorDialog::getColor();
		if (color.isValid()){
			_color = color;
			updateColor();
			emit valueChanged();
			emit colorChanged(color);
		}
	}
private:
	void updateColor(){
		SET_WIDGET_BG_COLOR(this, _color);
		SET_WIDGET_BG_COLOR(editor(), _color);
	}

private:
	QColor	_color;
};

class XFontItemEditor :public XItemEditor{
	Q_OBJECT
signals :
	void fontChanged(const QFont&);
public:
	XFontItemEditor(QWidget* parent = nullptr)
		:XItemEditor(parent){
		label()->setAlignment(Qt::AlignCenter);
		edit();
	}
	void setFont(const QFont& font){
		if (this->font() != font){
			updateFont(font);
		}
	}
	const QFont& font(){ return editor()->font(); }

protected:
	QWidget* createEditor(){
		QPushButton* editor = new QPushButton(this);
		connect(editor, SIGNAL(clicked()), this, SLOT(selectFont()));
		return editor;
	}
	void setEditorData(QWidget* editor){
	}
	QString itemData(QWidget* editor){
		return "";
	}

private slots:
	void selectFont(){
		bool ok;
		QFont font = QFontDialog::getFont(&ok, this->font(), this);
		if (ok){
			updateFont(font);
			emit fontChanged(font);
		}
	}
private:
	void updateFont(const QFont& f){
		QFont font = f;
		QPushButton* pushButton = static_cast<QPushButton*>(editor());
		pushButton->setText(QString("%1,%2").arg(font.family()).arg(font.pointSize()));
		font.setPointSize(qApp->font().pointSize());
		pushButton->setFont(font);
		label()->setText(pushButton->text());
		label()->setFont(font);
	}

private:
};

class XAlignItemEditor :public XItemEditor{
	Q_OBJECT
	signals :
	void alignChanged(Qt::Alignment);
public:
	XAlignItemEditor(QWidget* parent = nullptr)
		:XItemEditor(parent){
		edit();
	}
	void setAlign(Qt::Alignment align){
		if (this->align() != align){
			_toolButtonLeft.setChecked(align & Qt::AlignLeft);
			_toolButtonHCenter.setChecked(align & Qt::AlignHCenter);
			_toolButtonRight.setChecked(align & Qt::AlignRight);
			_toolButtonTop.setChecked(align & Qt::AlignTop);
			_toolButtonVCenter.setChecked(align & Qt::AlignVCenter);
			_toolButtonBottom.setChecked(align & Qt::AlignBottom);
			emit alignChanged(align);
		}
	}
	Qt::Alignment align()const{
		Qt::Alignment retval = 0;
		if (_toolButtonLeft.isChecked())	retval |= Qt::AlignLeft;
		if (_toolButtonHCenter.isChecked())	retval |= Qt::AlignHCenter;
		if (_toolButtonRight.isChecked())	retval |= Qt::AlignRight;
		if (_toolButtonTop.isChecked())	retval |= Qt::AlignTop;
		if (_toolButtonVCenter.isChecked())	retval |= Qt::AlignVCenter;
		if (_toolButtonBottom.isChecked())	retval |= Qt::AlignBottom;
		return retval;
	}

protected:
	QWidget* createEditor(){
		QWidget* w = new QWidget(this);
		QHBoxLayout* layout1 = new QHBoxLayout();
		layout1->setSpacing(0);
		layout1->addWidget(&_toolButtonLeft);
		layout1->addWidget(&_toolButtonHCenter);
		layout1->addWidget(&_toolButtonRight);
		QHBoxLayout* layout2 = new QHBoxLayout();
		layout2->setSpacing(0);
		layout2->addWidget(&_toolButtonTop);
		layout2->addWidget(&_toolButtonVCenter);
		layout2->addWidget(&_toolButtonBottom);
		QHBoxLayout* layout = new QHBoxLayout();
		layout->addLayout(layout1);
		layout->addLayout(layout2);
		layout->addStretch();
		layout->setMargin(0);
		w->setLayout(layout);
		connect(&_toolButtonLeft, SIGNAL(clicked()), this, SLOT(updateAlign()));
		connect(&_toolButtonHCenter, SIGNAL(clicked()), this, SLOT(updateAlign()));
		connect(&_toolButtonRight, SIGNAL(clicked()), this, SLOT(updateAlign()));
		connect(&_toolButtonTop, SIGNAL(clicked()), this, SLOT(updateAlign()));
		connect(&_toolButtonVCenter, SIGNAL(clicked()), this, SLOT(updateAlign()));
		connect(&_toolButtonBottom, SIGNAL(clicked()), this, SLOT(updateAlign()));

		_toolButtonTop.setCheckable(true);
		_toolButtonVCenter.setCheckable(true);
		_toolButtonBottom.setCheckable(true);
		_toolButtonLeft.setCheckable(true);
		_toolButtonHCenter.setCheckable(true);
		_toolButtonRight.setCheckable(true);


		QButtonGroup* buttonGroup1 = new QButtonGroup(this);
		buttonGroup1->addButton(&_toolButtonLeft);
		buttonGroup1->addButton(&_toolButtonHCenter);
		buttonGroup1->addButton(&_toolButtonRight);
		QButtonGroup* buttonGroup2 = new QButtonGroup(this);
		buttonGroup2->addButton(&_toolButtonTop);
		buttonGroup2->addButton(&_toolButtonVCenter);
		buttonGroup2->addButton(&_toolButtonBottom);


		return w;
	}
	void setEditorData(QWidget* editor){
	}
	QString itemData(QWidget* editor){
		return "";
	}


private slots :
	void updateAlign(){
		emit alignChanged(align());
	}

private:
	QToolButton	_toolButtonTop;
	QToolButton	_toolButtonVCenter;
	QToolButton	_toolButtonBottom;
	QToolButton	_toolButtonLeft;
	QToolButton	_toolButtonHCenter;
	QToolButton	_toolButtonRight;
};

class XCheckBoxItemEditor :public XItemEditor{
	Q_OBJECT
public:
	XCheckBoxItemEditor(QWidget* parent = nullptr)
		:XItemEditor(parent)
		,_trueValue(tr("Y")),_falseValue(tr("N")){
		setAutoCloseEditor(true);
	}

	void setTrueFalseValue(const QString& trueValue, const QString& falseValue){
		_trueValue = trueValue;
		_falseValue = falseValue;
	}

protected:
	QWidget* createEditor(){
		QCheckBox* editor = new QCheckBox(this);
		return editor;
	}
	void setEditorData(QWidget* editor){
		QCheckBox* checkBox = static_cast<QCheckBox*>(editor);
		checkBox->setChecked(text() == _trueValue);
	}
	QString itemData(QWidget* editor){
		QCheckBox* checkBox = static_cast<QCheckBox*>(editor);
		return checkBox->isChecked() ? _trueValue : _falseValue;
	}

private:
	QString _trueValue;
	QString _falseValue;
};

class XComboBoxItemEditor :public XItemEditor{
	Q_OBJECT
public:
	XComboBoxItemEditor(QWidget* parent = nullptr)
		:XItemEditor(parent){
	}

	QComboBox* comboBox(){ return  static_cast<QComboBox*>(editor()); }

protected:
	QWidget* createEditor(){
		QComboBox* editor = new QComboBox(this);
		//connect(editor, SIGNAL(activated(int)), this, SLOT(closeEditor()));
		return editor;
	}
	void setEditorData(QWidget* editor){
		QComboBox* comboBox = static_cast<QComboBox*>(editor);
		comboBox->setCurrentText(text());
	}
	QString itemData(QWidget* editor){
		QComboBox* comboBox = static_cast<QComboBox*>(editor);
		return comboBox->currentText();
	}

};

#endif //_X_ITEM_EDITOR_H
