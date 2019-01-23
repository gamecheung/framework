#ifndef _XCUSTOM_WINDOW_HELPER_H
#define _XCUSTOM_WINDOW_HELPER_H

#include <QWidget>
#include <QHash>
#include <QVBoxLayout>
#include <QPushButton>
#include <QMainWindow>
#include <QStyleOption>
#include <QPainter>
#include <QMouseEvent>
#include <QDebug>
#include <QToolButton>
#include <QLabel>
#include <QWindow>
#include <QMessageBox>
#include <QInputDialog>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QApplication>
#include <QToolBar>
#include <QPlainTextEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>

class XCusrosPosCalculator
{
public:
	XCusrosPosCalculator(){ reset(); }
	void reset()
	{
		onEdges=false;
		onLeftEdge=false;
		onTopEdge=false;
		onRightEdge=false;
		onBottomEdge=false;
		onTopLeftEdge=false;
		onTopRightEdge=false;
		onBottomLeftEdge=false;
		onBottomRightEdge=false;
	}
	void calculate(const QPoint& pos, const QRect& rect)
	{
		static int border = 5;

		onLeftEdge = inBorder(pos.x(), rect.x(),border);
		onTopEdge = inBorder(pos.y(), rect.y(),border);
		onRightEdge = inBorder(pos.x(), rect.x() + rect.width() - border, border);
		onBottomEdge = inBorder(pos.y(), rect.y() + rect.height() - border, border);
		
		onTopLeftEdge = onTopEdge&&onLeftEdge;
		onTopRightEdge = onTopEdge&&onRightEdge;
		onBottomLeftEdge = onBottomEdge&&onLeftEdge;
		onBottomRightEdge = onBottomEdge&&onRightEdge;

		onEdges = onLeftEdge || onTopEdge || onRightEdge || onBottomEdge;
	}

private:
	bool inBorder(int pos, int minPos, int border)
	{
		return (minPos <= pos && pos <= minPos + border);
	}

public:
	bool	onEdges;
	bool	onLeftEdge;
	bool	onTopEdge;
	bool	onRightEdge;
	bool	onBottomEdge;
	bool	onTopLeftEdge;
	bool	onTopRightEdge;
	bool	onBottomLeftEdge;
	bool	onBottomRightEdge;
};

class XCustomTitleBar :public QWidget
{
	Q_OBJECT
	signals :

public:
	XCustomTitleBar(QWidget* parent)
		:QWidget(parent), m_parentWidget(parent)
	{
		Q_ASSERT(m_parentWidget);

		m_icon = new QLabel(this);
		m_icon->setMargin(5);
		m_icon->setScaledContents(true);
		m_icon->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
		m_title = new QLabel(this);
		m_title->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

		connect(parent, SIGNAL(windowIconChanged(const QIcon&)), this, SLOT(setIcon(const QIcon&)));
		connect(parent, SIGNAL(windowTitleChanged(const QString&)), this, SLOT(setTitle(const QString&)));

		m_btnMin = createButton(style()->standardIcon(QStyle::SP_TitleBarMinButton), SLOT(showMinimized()));
		m_btnMax = createButton(style()->standardIcon(QStyle::SP_TitleBarMaxButton), SLOT(showMaximized()));
		m_btnNormal = createButton(style()->standardIcon(QStyle::SP_TitleBarNormalButton), SLOT(showNormal()));
		m_btnClose = createButton(style()->standardIcon(QStyle::SP_TitleBarCloseButton), SLOT(close()));

		m_layout = new QHBoxLayout();
		m_layout->setSpacing(0);
		m_layout->setMargin(0);
		m_layout->addWidget(m_icon);
		m_layout->addWidget(m_title);

		m_layoutCenter = new QHBoxLayout();
		m_layoutCenter->setMargin(0);
		m_layout->addLayout(m_layoutCenter);

		m_layout->addWidget(m_btnMin, 0, Qt::AlignTop);
		m_layout->addWidget(m_btnMax, 0, Qt::AlignTop);
		m_layout->addWidget(m_btnNormal, 0, Qt::AlignTop);
		m_layout->addWidget(m_btnClose, 0, Qt::AlignTop);

		setLayout(m_layout);

		m_parentWidget->installEventFilter(this);
		updateTitleBarButtonStatus();
		setIcon(parent->windowIcon());
		setTitle(parent->windowTitle());
	}

	QString title()const{ return m_title->text(); }
	const QPixmap* icon()const{ return m_icon->pixmap(); }
	QToolButton* minimizedButton()const{ return m_btnMin; }
	QToolButton* maximizedButton()const{ return m_btnMax; }
	QToolButton* normalButton()const{ return m_btnNormal; }
	QToolButton* closeButton()const{ return m_btnClose; }

	void addWidget(QWidget* w){ m_layoutCenter->addWidget(w); }
	QSize buttonIconSize()const{ return m_btnClose->iconSize(); }

public slots:
	void setTitle(const QString& title){ m_title->setText(title); }
	void setIcon(const QIcon& icon){ 
		m_icon->setPixmap(icon.pixmap(height()-(m_icon->margin()+1)*2)); 
	}
	void setButtonIconSize(const QSize& sz)
	{
		for (auto& btn : m_buttons)
			btn->setIconSize(sz);
	}

private:
	QToolButton* createButton(const QIcon& icon, const char *parentSlot)
	{
		QToolButton* btn = new QToolButton(this);
		btn->setAutoRaise(true);
		btn->setIcon(icon);
		connect(btn, SIGNAL(clicked()), parent(), parentSlot);
		m_buttons.append(btn);
		return btn;
	}

protected:
	bool eventFilter(QObject *obj, QEvent *event)
	{
		if (obj == m_parentWidget && event->type() == QEvent::WindowStateChange)
		{
			updateTitleBarButtonStatus();
		}
		return false;
	}
	void updateTitleBarButtonStatus()
	{
		switch (m_parentWidget->windowState())
		{
		case Qt::WindowNoState:
			m_btnNormal->setVisible(false);
			m_btnMax->setVisible(true);
			break;
		case Qt::WindowMaximized:
			m_btnNormal->setVisible(true);
			m_btnMax->setVisible(false);
			break;
		}
	}
	void mouseDoubleClickEvent(QMouseEvent *event)
	{
		if (m_parentWidget->isMaximized())
			m_parentWidget->showNormal();
		else
			m_parentWidget->showMaximized();
	}
	void paintEvent(QPaintEvent *)
	{
		QStyleOption opt;
		opt.init(this);
		QPainter p(this);
		style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
	}

private:
	void setParent(QWidget*){}

private:
	QHBoxLayout* m_layout;
	QHBoxLayout* m_layoutCenter;
	QLabel*	m_icon;
	QLabel* m_title;
	QToolButton* m_btnMin;
	QToolButton* m_btnMax;
	QToolButton* m_btnNormal;
	QToolButton* m_btnClose;
	QList<QToolButton*> m_buttons;
	QWidget*	m_parentWidget;
};

class XCustomWindowHelper : public QObject{
	Q_OBJECT
public:
	~XCustomWindowHelper(){instances()->remove(m_obj);}
	static XCustomWindowHelper* getInstance(QWidget* obj){
		if (!instances()->contains(obj)){
			(*instances())[obj] = new XCustomWindowHelper(obj);
		}
		return (*instances())[obj];
	}
	void setTitleBar(XCustomTitleBar* titleBar)
	{
		Q_ASSERT(m_titleBar == nullptr);
		m_titleBar = titleBar;
	}
	XCustomTitleBar* titleBar()const{ return m_titleBar; }

protected:
	bool eventFilter(QObject* obj, QEvent* event)
	{
		switch (event->type())
		{
		case QEvent::MouseButtonPress:handleMousePressEvent(static_cast<QMouseEvent*>(event)); break;
		case QEvent::MouseButtonRelease:handleMouseReleaseEvent(static_cast<QMouseEvent*>(event)); break;
		case QEvent::MouseMove:handleMouseMoveEvent(static_cast<QMouseEvent*>(event)); break;
		//case QEvent::Resize:handleResizeEvent(static_cast<QResizeEvent*>(event));	break;
		case QEvent::HoverMove:handleHoverMoveEvent(static_cast<QHoverEvent*>(event)); break;
		}
		return QObject::eventFilter(obj, event);
	}
private:
	XCustomWindowHelper(QWidget* obj) 
		:QObject(obj), m_obj(obj),m_mouseLeftButtonPressed(false)
		, m_cursorShapeChanged(false), m_mouseLeftButtonPressedOnTitleBar(false), m_titleBar(new XCustomTitleBar(m_obj))
	{
        Q_ASSERT(m_obj);
		Q_ASSERT(m_obj->isWindow());

		QMainWindow* mw = dynamic_cast<QMainWindow*>(m_obj);
		if (mw)
		{
			auto titleBar = mw->addToolBar(tr("Title Bar"));
			titleBar->setMovable(false);
			titleBar->addWidget(this->titleBar());
			mw->addToolBarBreak();
			m_layout = mw->layout();
		}
		else
		{
			QLayout* objLayout = m_obj->layout();
			QVBoxLayout* layout = new QVBoxLayout();
			layout->setSpacing(0);
			layout->addWidget(titleBar(),0,Qt::AlignTop);
			if (objLayout)
			{
				QWidget* w = new QWidget(m_obj);
				w->setLayout(objLayout);
				layout->addWidget(w);
			}
			m_obj->setLayout(layout);
			layout->setMargin(0);
			m_layout = layout;
		}
		m_obj->setWindowFlags(m_obj->windowFlags() | Qt::FramelessWindowHint);
		m_obj->setAttribute(Qt::WA_Hover, true);
		m_obj->installEventFilter(this);
    }
	static QHash<QWidget*, XCustomWindowHelper*>* instances(){
		static QHash<QWidget*, XCustomWindowHelper*> s_instances;
		return &s_instances;
	}
	void handleMousePressEvent(QMouseEvent* event)
	{
		if (event->button() == Qt::LeftButton)
		{
			m_mouseLeftButtonPressed = true;
			m_pressedMousePos.calculate(event->globalPos(), m_obj->geometry());
			if (m_pressedMousePos.onEdges)
			{
				resizeWidget(event->globalPos());
			}
			else
			{
				if (m_titleBar)
					m_mouseLeftButtonPressedOnTitleBar = m_titleBar->rect().contains(event->pos());
				else
					m_mouseLeftButtonPressedOnTitleBar = false;
			}
			m_prePos = event->globalPos() - m_obj->geometry().topLeft();
		}
	}
	void handleMouseReleaseEvent(QMouseEvent* event)
	{
		m_mouseLeftButtonPressed = false;
		m_mouseLeftButtonPressedOnTitleBar = false;
		m_pressedMousePos.reset();
	}
	void handleMouseMoveEvent(QMouseEvent* event)
	{
		if (m_mouseLeftButtonPressed)
		{
			if (m_pressedMousePos.onEdges)
			{
				resizeWidget(event->globalPos());
			}
			else if (m_mouseLeftButtonPressedOnTitleBar)
			{
				m_obj->move(event->globalPos() - m_prePos);
			}
		}
		else
		{
			updateCursorShape(event->globalPos());
		}
	}
	void handleHoverMoveEvent(QHoverEvent* event)
	{
		updateCursorShape(m_obj->mapToGlobal(event->pos()));
	}


	void resizeWidget(const QPoint& pos)
	{
		QRect rect = m_obj->geometry();
		if (m_pressedMousePos.onLeftEdge)	rect.setLeft(pos.x());
		if (m_pressedMousePos.onTopEdge)	rect.setTop(pos.y());
		if (m_pressedMousePos.onRightEdge)	rect.setRight(pos.x());
		if (m_pressedMousePos.onBottomEdge)	rect.setBottom(pos.y());
		m_obj->setGeometry(rect);
	}

	void updateCursorShape(const QPoint& globalPos)
	{
		if (m_obj->isFullScreen() || m_obj->isMaximized())
		{
			if (m_cursorShapeChanged)
			{
				m_obj->unsetCursor();
			}
			return;
		}
		m_moveMousePos.calculate(globalPos, m_obj->geometry());

		if (m_moveMousePos.onTopLeftEdge || m_moveMousePos.onBottomRightEdge)
		{
			m_obj->setCursor(Qt::SizeFDiagCursor);
			m_cursorShapeChanged = true;
		}
		else if (m_moveMousePos.onTopRightEdge || m_moveMousePos.onBottomLeftEdge)
		{
			m_obj->setCursor(Qt::SizeBDiagCursor);
			m_cursorShapeChanged = true;
		}
		else if (m_moveMousePos.onLeftEdge || m_moveMousePos.onRightEdge)
		{
			m_obj->setCursor(Qt::SizeHorCursor);
			m_cursorShapeChanged = true;
		}
		else if (m_moveMousePos.onTopEdge || m_moveMousePos.onBottomEdge)
		{
			m_obj->setCursor(Qt::SizeVerCursor);
			m_cursorShapeChanged = true;
		}
		else
		{
			if (m_cursorShapeChanged)
			{
				m_obj->unsetCursor();
				m_cursorShapeChanged = false;
			}
		}
	}
private:
	QWidget*			m_obj;
	QLayout*			m_layout;
	XCustomTitleBar*	m_titleBar;
	bool				m_mouseLeftButtonPressed;
	bool				m_mouseLeftButtonPressedOnTitleBar;
	XCusrosPosCalculator	m_pressedMousePos;
	XCusrosPosCalculator	m_moveMousePos;
	bool					m_cursorShapeChanged;
	QPoint				m_prePos;
};

#define XCUSTOM_WINDOW_HELPER(obj)  XCustomWindowHelper::getInstance(obj)

class XCustomMessageBox : public QDialog
{
	Q_OBJECT
public:
	XCustomMessageBox(QWidget* parent = nullptr, const QPixmap& icon = QPixmap() , const QString& title = QString(),
		const QString& text = QString(), QDialogButtonBox::StandardButtons buttons = QDialogButtonBox::NoButton,
		QDialogButtonBox::StandardButton defaultButton = QDialogButtonBox::NoButton) :QDialog(parent)
	{
		m_icon = new QLabel(this);
		m_icon->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
		m_text = new QLabel(this);
		m_text->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
		m_layout = new QGridLayout();
		m_buttonBox = new QDialogButtonBox(this);
		m_layout->addWidget(m_icon, 0, 0, Qt::AlignTop);
		m_layout->addWidget(m_text, 0, 1);
		m_layout->addWidget(m_buttonBox, 1, 0, 1, 2);
		setLayout(m_layout);

		connect(m_buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(onButtonClicked(QAbstractButton*)));

		XCUSTOM_WINDOW_HELPER(this);
		setIcon(icon);
		setWindowTitle(title);
		setText(text);
		setStandardButtons(buttons);
		setDefaultButton(defaultButton);
	}

	void setIcon(const QPixmap& icon){ m_icon->setPixmap(icon); }
	void setText(const QString& text){ m_text->setText(text); }
	void setStandardButtons(QDialogButtonBox::StandardButtons buttons){ m_buttonBox->setStandardButtons(buttons); }
	void setDefaultButton(QPushButton* button)
	{
		if (m_buttonBox->buttons().contains(button))
		{
			button->setDefault(true);
			button->setFocus();
		}
	}
	void setDefaultButton(QDialogButtonBox::StandardButton button)
	{
		setDefaultButton(m_buttonBox->button(button));
	}
	QAbstractButton* clickedButton()const{ return m_clickedButton; }

	static QDialogButtonBox::StandardButton question(QWidget *parent, const QString &title, const QString &text,
		QDialogButtonBox::StandardButtons buttons = QDialogButtonBox::StandardButtons(QDialogButtonBox::Yes | QDialogButtonBox::No),
		QDialogButtonBox::StandardButton defaultButton = QDialogButtonBox::NoButton)
	{
		return showNewMessageBox(parent, defaultQuestionIcon(), title, text, buttons, defaultButton);
	}
	static QDialogButtonBox::StandardButton information(QWidget *parent, const QString &title, const QString &text,
		QDialogButtonBox::StandardButtons buttons = QDialogButtonBox::StandardButtons(QDialogButtonBox::Ok),
		QDialogButtonBox::StandardButton defaultButton = QDialogButtonBox::NoButton)
	{
		return showNewMessageBox(parent, defaultInformationIcon(), title, text, buttons, defaultButton);
	}
	static QDialogButtonBox::StandardButton warning(QWidget *parent, const QString &title, const QString &text,
		QDialogButtonBox::StandardButtons buttons = QDialogButtonBox::StandardButtons( QDialogButtonBox::Ok),
		QDialogButtonBox::StandardButton defaultButton = QDialogButtonBox::NoButton)
	{
		return showNewMessageBox(parent, defaultWarningIcon(), title, text, buttons, defaultButton);
	}
	static QDialogButtonBox::StandardButton critical(QWidget *parent, const QString &title, const QString &text,
		QDialogButtonBox::StandardButtons buttons = QDialogButtonBox::StandardButtons(QDialogButtonBox::Ok),
		QDialogButtonBox::StandardButton defaultButton = QDialogButtonBox::NoButton)
	{
		return showNewMessageBox(parent, defaultCriticalIcon(), title, text, buttons, defaultButton);
	}
	static QPixmap& defaultQuestionIcon(){ static QPixmap o = qApp->style()->standardPixmap(QStyle::SP_MessageBoxQuestion); return o; }
	static QPixmap& defaultInformationIcon(){ static QPixmap o = qApp->style()->standardPixmap(QStyle::SP_MessageBoxInformation); return o; }
	static QPixmap& defaultWarningIcon(){ static QPixmap o = qApp->style()->standardPixmap(QStyle::SP_MessageBoxWarning); return o; }
	static QPixmap& defaultCriticalIcon(){ static QPixmap o = qApp->style()->standardPixmap(QStyle::SP_MessageBoxCritical); return o; }

private slots:
	void onButtonClicked(QAbstractButton* button)
	{
		m_clickedButton = button;
		done(m_buttonBox->standardButton(button));
	}

private:
	static QDialogButtonBox::StandardButton showNewMessageBox(QWidget *parent, const QPixmap& icon, const QString& title,
		const QString& text, QDialogButtonBox::StandardButtons buttons, QDialogButtonBox::StandardButton defaultButton)
	{
		XCustomMessageBox msgBox(parent,icon, title, text, buttons,defaultButton);
		msgBox.setWindowTitle(title);
		msgBox.setWindowIcon(parent->windowIcon());
		if (msgBox.exec() == -1)
			return QDialogButtonBox::Cancel;
		return msgBox.m_buttonBox->standardButton(msgBox.clickedButton());
	}

private:
	QGridLayout*		m_layout;
	QLabel*				m_icon;
	QLabel*				m_text;
	QDialogButtonBox*	m_buttonBox;
	QAbstractButton*	m_clickedButton;
};

class XCustomInputDialog :public QDialog
{
	Q_OBJECT
public:
	XCustomInputDialog(QWidget *parent = Q_NULLPTR, Qt::WindowFlags flags = Qt::WindowFlags())
		:QDialog(parent, flags), m_layout(new QVBoxLayout()), m_label(new QLabel(this))
		, m_buttonBox(new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,this))
	{
		setLayout(m_layout);
		m_layout->addWidget(m_label);
		m_layout->addWidget(m_buttonBox);

		connect(m_buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
		connect(m_buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

		XCUSTOM_WINDOW_HELPER(this);
	}

	void setInputItem(QWidget* w){ Q_ASSERT(w); m_layout->insertWidget(1, w); }
	void setInputItem(QLayout* layout){ Q_ASSERT(layout); m_layout->insertLayout(1, layout); }

	void setLabelText(const QString& text){ m_label->setText(text); }

	static QString getText(QWidget *parent, const QString &title, const QString &label,
		QLineEdit::EchoMode echo = QLineEdit::Normal,
		const QString &text = QString(), bool *ok = Q_NULLPTR)
	{
		XCustomInputDialog dlg(parent);
		dlg.setWindowTitle(title);
		dlg.setLabelText(label);

		QLineEdit* lineEdit = new QLineEdit(&dlg);
		dlg.setInputItem(lineEdit);
		lineEdit->setEchoMode(echo);
		lineEdit->setText(text);

		int ret = dlg.exec();
		if (ok)
			*ok = (ret == QDialog::Accepted);

		if (ret)
			return lineEdit->text();
		else
			return QString();
	}
	static QString getMultiLineText(QWidget *parent, const QString &title, const QString &label,
		const QString &text = QString(), bool *ok = Q_NULLPTR)
	{

		XCustomInputDialog dlg(parent);
		dlg.setWindowTitle(title);
		dlg.setLabelText(label);

		QPlainTextEdit* editor = new QPlainTextEdit(&dlg);
		dlg.setInputItem(editor);
		editor->setPlainText(text);

		int ret = dlg.exec();
		if (ok)
			*ok = (ret == QDialog::Accepted);

		if (ret)
			return editor->toPlainText();
		else
			return QString();
	}
	static QString getItem(QWidget *parent, const QString &title, const QString &label,
		const QStringList &items, int current = 0, bool editable = true,
		bool *ok = Q_NULLPTR)
	{
		XCustomInputDialog dlg(parent);
		dlg.setWindowTitle(title);
		dlg.setLabelText(label);

		QComboBox* editor = new QComboBox(&dlg);
		dlg.setInputItem(editor);
		editor->addItems(items);
		editor->setCurrentIndex(current);
		editor->setEditable(editable);

		int ret = dlg.exec();
		if (ok)
			*ok = (ret == QDialog::Accepted);

		if (ret)
			return editor->currentText();
		else
			return QString();
	}

	static int getInt(QWidget *parent, const QString &title, const QString &label, int value = 0,
		int minValue = -2147483647, int maxValue = 2147483647,
		int step = 1, bool *ok = Q_NULLPTR)
	{
		XCustomInputDialog dlg(parent);
		dlg.setWindowTitle(title);
		dlg.setLabelText(label);

		QSpinBox* editor = new QSpinBox(&dlg);
		dlg.setInputItem(editor);
		editor->setRange(minValue, maxValue);
		editor->setSingleStep(step);
		editor->setValue(value);

		int ret = dlg.exec();
		if (ok)
			*ok = (ret == QDialog::Accepted);

		if (ret)
			return editor->value();
		else
			return 0;
	}
	static double getDouble(QWidget *parent, const QString &title, const QString &label, double value = 0,
		double minValue = -2147483647, double maxValue = 2147483647,
		int decimals = 1, bool *ok = Q_NULLPTR)
	{
		XCustomInputDialog dlg(parent);
		dlg.setWindowTitle(title);
		dlg.setLabelText(label);

		QDoubleSpinBox* editor = new QDoubleSpinBox(&dlg);
		dlg.setInputItem(editor);
		editor->setRange(minValue, maxValue);
		editor->setDecimals(decimals);
		editor->setValue(value);

		int ret = dlg.exec();
		if (ok)
			*ok = (ret == QDialog::Accepted);

		if (ret)
			return editor->value();
		else
			return 0;
	}

private:
	QVBoxLayout* m_layout;
	QLabel* m_label;
	QDialogButtonBox* m_buttonBox;
};

#endif // _XCUSTOM_WINDOW_HELPER
