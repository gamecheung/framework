#ifndef _X_PAINTER_H
#define _X_PAINTER_H


#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsTextItem>
#include <QWheelEvent>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsSceneDragDropEvent>
#include <QDebug>
#include "XXml.hpp"
#include "Qt/XQt.h"

#define ITEM_TYPE_TEXT	"text"
#define ITEM_TYPE_LINE	"line"
#define ITEM_TYPE_IMAGE	"image"
#define ITEM_TYPE_TABLE	"table"

#define XML_ROOT	"painter"
#define XML_ITEM	"item"
#define XML_TYPE	"type"
#define XML_ID		"id"
#define XML_SIZE	"size"
#define XML_POS		"pos"

class XPainterItem :public QGraphicsItem{
public:
	XPainterItem()
		:_backgroundColor(255,255,255)
		,_foregroundColor(0,0,0){
		setFlags(ItemIsSelectable | ItemIsMovable);
		setVisible(true);
		setAcceptHoverEvents(true);

		setSize(QSize(50, 20));
	}
	virtual ~XPainterItem(){}

	void setId(const QString& id){ _id = id; }
	const QString& id()const{ return _id; }
	void setSize(const QSize& size){ _size = size; update(); }
	const QSize& size()const { return _size; }
	void setBackgroundColor(const QColor& color){ _backgroundColor = color; update(); }
	const QColor& backgroundColor()const{ return _backgroundColor; }
	void setForegroundColor(const QColor& color){ _foregroundColor = color; update(); }
	const QColor& foregroundColor()const{ return _foregroundColor; }

	QRectF boundingRect() const Q_DECL_OVERRIDE{
		return QRectF(0, 0, _size.width(), _size.height());
	}
	
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) Q_DECL_OVERRIDE{
		draw(painter, option);
	}

//protected:
	virtual void draw(QPainter* painter,const QStyleOptionGraphicsItem *option=nullptr)const{
		QColor backgroundColor = _backgroundColor;
		if (option){
			if (option->state&QStyle::State_Selected || option->state&QStyle::State_MouseOver){
			if (backgroundColor.lightness() > 127)
				backgroundColor = backgroundColor.darker();
			else
				backgroundColor = backgroundColor.lighter();
			}
		}
		//painter->setBrush(QBrush(backgroundColor));
		painter->setPen(_foregroundColor);
		painter->fillRect(QRect(0, 0, _size.width(), _size.height()), QBrush(backgroundColor));
		painter->drawRect(0, 0, _size.width(), _size.height());
	}

protected:
	QString _id;
	QSize	_size;
	QColor	_backgroundColor;
	QColor	_foregroundColor;
};

class XTextPainterItem :public XPainterItem{
public:
	XTextPainterItem(){}
	void setText(const QString& text){ _text = text;  }
	const QString& text()const{ return _text; }

	void draw(QPainter *painter, const QStyleOptionGraphicsItem *item = nullptr)const{
		XPainterItem::draw(painter, item);
		painter->drawText(QRectF(0, 0, _size.width(), _size.height()), _text);
	}

private:
	QString	_text;
};

class XLinePainterItem :public XPainterItem{
};
class XImagePainterItem :public XPainterItem{
};
class XTablePainterItem :public XPainterItem{
};

class XPainterScene : public QGraphicsScene{
	Q_OBJECT
public:
	XPainterScene(QObject* parent = nullptr) :QGraphicsScene(parent){}
protected:
	void dragMoveEvent(QGraphicsSceneDragDropEvent *event){ event->accept(); }
};

class XPainter : public QGraphicsView{
	Q_OBJECT
signals:
	void selectionChanged();
	void scaled(double d);
	void contentsChanged();
public:
	XPainter(QWidget* parent = nullptr) 
		:QGraphicsView(parent)
		,_scale(1),_modified(false){
		_scene = new XPainterScene(this);
		connect(_scene, SIGNAL(selectionChanged()), this, SIGNAL(selectionChanged()));
		this->setScene(_scene);
		this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		this->setDragMode(QGraphicsView::RubberBandDrag);
		
		//// TODO
		setSize(500, 400);
		//for (int i = 11; i < 13; ++i){
		//	XPainterItem* item = new XPainterItem();
		//	_scene->addItem(item);
		//	item->moveBy(i * 10, i * 20);
		//}

		//XTextPainterItem* textItem = new XTextPainterItem();
		//_scene->addItem(textItem);
		//textItem->setText("text");
		//textItem->setBackgroundColor(QColor(100, 0, 0));
		//textItem->moveBy(150, 300);
	}

	bool load(const QString& fileName){ 
		XXml xml;
		if (!xml.load(fileName.toStdWString())||xml.key().toStdString()!=XML_ROOT)
			return false;
		for (auto& node : xml.children()){
			XPainterItem* item = nullptr;
			item = new XPainterItem();
			assert(item);
			item->setId(QString::fromStdWString(node.data().attr(XML_ID)));
			item->setSize(QtHelper::covertStringToSize(QString::fromStdWString(node.data().attr(XML_SIZE))));
			_scene->addItem(item);
			QPoint pos = QtHelper::covertStringToPoint(QString::fromStdWString(node.data().attr(XML_POS)));
			item->moveBy(pos.x(), pos.y());
		}
		return true;
	}
	bool save(const QString& fileName){ 
		auto items = _scene->items();

		XXml xml;
		xml.setKey(XML_ROOT);

		for (auto& var : items){
			XXmlNode node;
			node.setKey(XML_ITEM);
			XPainterItem* item = dynamic_cast<XPainterItem*>(var);
			if (item){
				node.setAttr(XML_ID, item->id().toStdWString());
				node.setAttr(XML_SIZE, QtHelper::covertSizeToString(item->size()).toStdWString());
				node.setAttr(XML_POS, QtHelper::covertPointToString(item->pos().toPoint()).toStdWString());
			}
			else{
				continue;
			}
			xml.addChild(node);
		}

		return xml.save(fileName.toStdWString());
	}

	void setSize(int w, int h){
		_scene->setSceneRect(0, 0, w, h);
		updateSize();
	}
	void scale(double d){
		_scale = d;
		updateSize();
		emit scaled(_scale);
	}

	QGraphicsScene* scene()const{ return _scene; }
	QList<QGraphicsItem *> selectedItems()const{ return _scene->selectedItems(); }

	void clean(){ _scene->clear(); }
	bool isModified()const{ return _modified; }
	void setModified(bool b){ _modified = b; }

protected:
	void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE{
		if (event->modifiers() & Qt::ControlModifier) {
			if (event->delta() > 0)
				_scale *= 0.9;
			else
				_scale *= 1.1;
			if (_scale < 0.1)_scale = 0.1;
			if (_scale > 2)_scale = 2;
			scale(_scale);
			event->accept();
		}
		else {
			QGraphicsView::wheelEvent(event);
		}
	}

private:
	void updateSize(){
		QMatrix matrix;
		matrix.scale(_scale, _scale);
		setMatrix(matrix);
		setFixedSize((_scene->sceneRect().size()*_scale).toSize()+QSize(2,2));
	}

private:
	XPainterScene* _scene;
	double	_scale;
	bool	_modified;
};

#endif //_X_PAINTER_H
