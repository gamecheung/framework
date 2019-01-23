#ifndef _X_COLOR_WIDGET_HELPER_H
#define _X_COLOR_WIDGET_HELPER_H

#include <QToolButton>
#include <QPushButton>
#include <QHash>
#include <QWidgetAction>
#include <QGridLayout>
#include <QMenu>
#include <QColorDialog>
#include <QPainter>
#include <QBitmap>

class _XColorPanelButton : public QToolButton
{
	Q_OBJECT
	signals :
	void clicked(const QColor& color);

public:
	_XColorPanelButton(const QColor& color, QWidget* parent = nullptr)
		:QToolButton(parent)
	{
		setAutoRaise(true);
		connect(this, SIGNAL(clicked()), this, SLOT(onClicked()));
		setColor(color);
	}

	const QColor& color()const{ return m_color; }

	public slots:
	void setColor(const QColor& color)
	{
		if (m_color != color)
		{
			m_color = color;
			QPixmap pix(iconSize());
			pix.fill(color);
			setIcon(pix);
		}
	}

	private slots:
	void onClicked()
	{
		emit clicked(m_color);
	}

private:
	QColor m_color;
};

class _XColorPanel :public QWidget
{
	Q_OBJECT
	signals :
	void colorChanged(const QColor& color);

public:
	_XColorPanel(QWidget* parent = nullptr) :QWidget(parent)
		, m_columnCount(5), m_rowCount(1)
	{
		m_layout = new QGridLayout();
		m_layout->setMargin(0);
		m_layout->setSpacing(0);
		setLayout(m_layout);

		appendColor(Qt::black);
		appendColor(Qt::red);
		appendColor(Qt::green);
		appendColor(Qt::blue);
		appendColor(Qt::yellow);
	}

	void setColumnCount(int count){ m_columnCount = count; }
	void setRowCount(int count){ m_rowCount = count; }

	public slots:
	void setColor(const QColor& color){ appendColor(color); }

private:
	void appendColor(const QColor& color)
	{
		if (m_btns.count() > 0 && m_btns.at(0)->color() == color)
			return;

		int count = m_btns.count();
		for (int i = 0; i < count; ++i)
		{
			if (m_btns.at(i)->color() == color)
			{
				m_btns.removeAt(i);
				break;
			}
		}

		_XColorPanelButton* btn = new _XColorPanelButton(color, this);
		connect(btn, SIGNAL(clicked(const QColor&)), this, SIGNAL(colorChanged(const QColor&)));
		m_btns.prepend(btn);
		count = m_btns.count();
		if (count > m_rowCount*m_columnCount)
		{
			delete m_btns.at(count - 1);
			m_btns.removeAt(count - 1);
			--count;
		}

		for (int i = 0; i < count; ++i)
		{
			m_layout->addWidget(m_btns.at(i), i / m_columnCount, i%m_columnCount);
		}
	}

private:
	QGridLayout* m_layout;
	int m_columnCount;
	int m_rowCount;
	QList<QColor>		m_colors;
	QList<_XColorPanelButton*> m_btns;
};

class XColorWidgetHelper : public QObject{
	Q_OBJECT

signals :
	void colorChanged(const QColor& color);
	void colorActived(const QColor& color);


public:
	~XColorWidgetHelper(){instances()->remove(m_obj);}
	static XColorWidgetHelper* getInstance(QToolButton* obj){
		if (!instances()->contains(obj)){
			(*instances())[obj] = new XColorWidgetHelper(obj);
		}
		return (*instances())[obj];
	}
	static XColorWidgetHelper* getInstance(QPushButton* obj){
		if (!instances()->contains(obj)){
			(*instances())[obj] = new XColorWidgetHelper(obj);
		}
		return (*instances())[obj];
	}

	void init(const QPixmap& pixmap, const QRect& roi)
	{
		m_pixmap = pixmap;
		m_roi = roi;
		update();
	}
	void setPixmap(const QPixmap& pixmap){ m_pixmap = pixmap; update(); }
	void setRoi(const QRect& roi){ m_roi = roi; update(); }
	void setNoColorVisible(bool b){ m_actNoColor->setVisible(b); }
	const QPixmap& pixmap()const{ return m_pixmap; }
	const QRect& roi()const{ return m_roi; }

	const QColor& color()const{ return m_color; }
	QWidget* widget()const{ return m_obj; }
	QToolButton* toolButton()const{ return m_toolButton; }
	QPushButton* pushButton()const{ return m_pushButton; }

public slots:
	void setColor(const QColor& color, bool signal=true)
	{
		if (m_color != color)
		{
			m_color = color;
			update();

			if (signal)
				emit colorChanged(m_color);
		}
		if (signal)
			emit colorActived(m_color);
		m_menu->close();
	}
	void clearColor(){ setColor(QColor()); }
	void setColorWithNoSignal(const QColor& color){ setColor(color, false); }

private slots:
	void showColorDialog()
	{
		QColor color = QColorDialog::getColor(m_color, m_obj,QString(),QColorDialog::ShowAlphaChannel);
		if (!color.isValid())
			return;

		setColor(color);
		m_colorPanel->setColor(color);
	}

private:
    XColorWidgetHelper(QToolButton* obj) :QObject(obj), m_obj(obj){ 
        Q_ASSERT(m_obj);
		m_widgetType = ToolButtonType;
		init();
    }
	XColorWidgetHelper(QPushButton* obj) :QObject(obj), m_obj(obj){
		Q_ASSERT(m_obj);
		m_widgetType = PushButtonType;
		init();
	}
	static QHash<QWidget*, XColorWidgetHelper*>* instances(){
		static QHash<QWidget*, XColorWidgetHelper*> s_instances;
		return &s_instances;
	}

	void init()
	{
		m_menu = new QMenu(m_obj);
		m_colorPanel = new _XColorPanel(m_obj);
		connect(m_colorPanel, SIGNAL(colorChanged(const QColor&)), this, SLOT(setColor(const QColor&)));
		QWidgetAction* wAct = new QWidgetAction(m_obj);
		wAct->setDefaultWidget(m_colorPanel);
		m_menu->addAction(wAct);

		m_actNoColor = m_menu->addAction(tr("No Color"), this, SLOT(clearColor()));
		m_menu->addAction(tr("Other Color..."), this, SLOT(showColorDialog()));

		m_toolButton = nullptr;
		m_pushButton = nullptr;
		if (m_widgetType == ToolButtonType)
		{
			m_toolButton = qobject_cast<QToolButton*>(m_obj);
			m_toolButton->setPopupMode(QToolButton::MenuButtonPopup);
			m_toolButton->setMenu(m_menu);
		}
		else if (m_widgetType == PushButtonType)
		{
			m_pushButton = qobject_cast<QPushButton*>(m_obj);
			m_pushButton->setMenu(m_menu);
		}
		update();
	}
	QSize iconSize()const
	{
		if (m_widgetType == ToolButtonType)
			return m_toolButton->iconSize();
		else if (m_widgetType == PushButtonType)
			return m_pushButton->iconSize();
		Q_ASSERT(false);
		return QSize();
	}
	QPixmap genPixmap()const
	{
		QPixmap pixmap;
		if (!m_pixmap.isNull())
		{
			pixmap = m_pixmap.copy();
			if (m_roi.isValid())
			{
				if (m_color.isValid())
				{
					QPainter painter(&pixmap);
					painter.fillRect(m_roi, m_color);
				}
			}
			else
			{
				if (m_color.isValid())
				{
					pixmap.fill(m_color);
					pixmap.setMask(m_pixmap.mask());
				}
			}
		}
		else
		{
			if (m_color.isValid())
			{
				pixmap = QPixmap(iconSize());
				pixmap.fill(m_color);
			}
		}
		return pixmap;
	}
	void update()
	{
		if (m_widgetType == ToolButtonType)
		{
			m_toolButton->setIcon(genPixmap());
		}
		else if (m_widgetType == PushButtonType)
		{
			m_pushButton->setIcon(genPixmap());
			m_pushButton->setText(m_color.isValid() ? "" : tr("No Color"));
		}
	}

private:
	enum WidgetType{ToolButtonType,PushButtonType};

	WidgetType	m_widgetType;

	QWidget*		m_obj;
	QToolButton*	m_toolButton;
	QPushButton*	m_pushButton;

	QPixmap		m_pixmap;
	QRect		m_roi;

	QMenu*		m_menu;
	QAction*	m_actNoColor;
	_XColorPanel* m_colorPanel;

	QColor		m_color;
};

#define X_COLOR_WIDGET_HELPER(obj)  XColorWidgetHelper::getInstance(obj)

#endif // _X_COLOR_WIDGET_HELPER_H
