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
#include "XPainterTableItemEditor.h"

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
#define XML_COLOR	"color"
#define XML_FG_COLOR	"fg_color"
#define XML_BG_COLOR	"bg_color"
#define XML_FONT		"font"
#define XML_ALIGN		"align"
#define XML_LINE_TYPE	"line_type"
#define XML_FILE		"file"
#define XML_COL_WIDTHS		"col_widths"
#define XML_ROW_HEIGHTS		"row_heights"
#define XML_TEXT		"text"
#define XML_ROW			"row"
#define XML_COL			"col"

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
	void setWidth(int w){ _size.setWidth(w); update(); }
	void seeHeight(int h){ _size.setHeight(h); update(); }
	const QSize& size()const { return _size; }
	int width()const{ return _size.width(); }
	int height()const{ return _size.height(); }
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
	void setText(const QString& text){ _text = text;  update(); }
	const QString& text()const{ return _text; }
	void setFont(const QFont& font){ _font = font; update(); }
	const QFont& font()const{ return _font; }
	void setAlignment(const Qt::Alignment alignment){ 
		_alignment = alignment; 
		update(); 
	}
	Qt::Alignment alignment()const{ return _alignment; }

	void draw(QPainter *painter, const QStyleOptionGraphicsItem *item = nullptr)const{
		XPainterItem::draw(painter, item);
		painter->setFont(_font);
		painter->drawText(QRectF(0, 0, _size.width(), _size.height()), _text,QTextOption(_alignment));
	}

private:
	QString	_text;
	QFont	_font;
	Qt::Alignment _alignment;
};

class XLinePainterItem :public XPainterItem{
public:
	XLinePainterItem() :_type(HLine){}
	enum LineType{HLine,VLine,LLine,RLine};
	void setLineType(int type){ _type = (LineType)type; update(); }
	LineType lineType()const{ return _type; }
	void draw(QPainter *painter, const QStyleOptionGraphicsItem *item = nullptr)const{
		XPainterItem::draw(painter, item);
		QPoint p1, p2;
		switch (_type)
		{
		case XLinePainterItem::HLine:
			p1 = QPoint(0, height() / 2);
			p2 = QPoint(width(), height() / 2);
			break;
		case XLinePainterItem::VLine:
			p1 = QPoint(width() / 2, 0);
			p2 = QPoint(width() / 2, height());
			break;
		case XLinePainterItem::LLine:
			p1 = QPoint(width(), 0);
			p2 = QPoint(0, height());
			break;
		case XLinePainterItem::RLine:
			p1 = QPoint(0, 0);
			p2 = QPoint(width(), height());
			break;
		default:
			break;
		}
		painter->drawLine(p1, p2);
	}
private:
	LineType _type;
};
class XImagePainterItem :public XPainterItem{
public:
	void setImage(const QString& pathfile){
		_image.load(pathfile);
	}
	const QString& pathfile()const{ return _pathfile; }
	void draw(QPainter *painter, const QStyleOptionGraphicsItem *item = nullptr)const{
		XPainterItem::draw(painter, item);
		painter->drawImage(QRect(0, 0, width(), height()), _image);
	}
private:
	QString _pathfile;
	QImage	_image;
};

class XTablePainterItem :public XPainterItem{
public:
	typedef struct{
		int row;
		int col;
		QString text;
		QFont	font;
		Qt::Alignment align;
		QColor	bgColor;
		QColor	fgColor;
	}Item;
	void setColWidths(const QList<int>& widths, bool update=true){
		_colWidths = widths;
		updateColLinesPoss();

		if (update) this->update();
	}
	void setColWidths(const QString& str,bool update=true){
		setColWidths(QtHelper::covertStringsToInts(str.split(',')),update);
	}
	const QList<int>& colWidths()const{ return _colWidths; }
	QString colWidthsStr()const{
		return QtHelper::covertIntsToStrings(_colWidths).join(',');
	}
	void setRowHeights(QList<int>& heights, bool update=true){
		_rowHeights = heights;
		updateRowLinesPoss();

		if (update) this->update();
	}
	const QList<int>& rowHeights()const{ return _rowHeights; }
	void setRowHeights(const QString& heights,bool update=true){
		setRowHeights(QtHelper::covertStringsToInts(heights.split(',')),update);
	}
	QString rowHeightsStr()const{
		return QtHelper::covertIntsToStrings(_rowHeights).join(',');
	}
	void setItem(const Item& item, bool update=true){
		auto pItem = this->item(item.row, item.col);
		if (pItem)
			*pItem = item;
		else
			_items.append(item);

		if (update) this->update();
	}
	Item* item(int row, int col){
		for (auto& var : _items){
			if (var.row == row&&var.col == col){
				return &var;
			}
		}
		if (row < rowCount() && col < colCount()){
			Item item;
			item.row = row;
			item.col = col;
			_items.append(item);
			return this->item(row, col);
		}
		return nullptr;
	}
	const QList<Item>& items()const{ return _items; }
	void cleanItems(){ _items.clear(); }
	void draw(QPainter *painter, const QStyleOptionGraphicsItem *item = nullptr)const{
		XPainterItem::draw(painter, item);
		auto pen = painter->pen();
		for (auto& var : _items){
			QRect rect = itemRect(var.row, var.col);
			painter->fillRect(rect, var.bgColor);
			painter->setPen(var.fgColor);
			painter->setFont(var.font);
			painter->drawText(rect.marginsRemoved(QMargins(6,0,6,0)), var.text, QTextOption(var.align));
		}

		painter->setPen(pen);
		for (auto& x : _colLinesPoss){
			painter->drawLine(x, 0, x, height());
		}
		for (auto& y : _rowLinesPoss){
			painter->drawLine(0, y, width(), y);
		}
	}
	//void setSize(const QSize& size){
	//	XPainterItem::setSize(size);
	//	updateColLinesPoss();
	//	updateRowLinesPoss();
	//	update();
	//}

	int rowCount()const{ return _rowHeights.count(); }
	int colCount()const{ return _colWidths.count(); }
	int rowHeight(int row)const{ return _rowHeights.at(row); }
	int colWidth(int col)const{ return _colWidths.at(col); }
	

protected:
	//void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event){
	//	XPainterItem::mouseDoubleClickEvent(event);
	//}
	void updateColLinesPoss(){
		_colLinesPoss.clear();
		int x = 0;
		_colLinesPoss.append(x);
		for (auto& var : _colWidths){
			x += var;
			_colLinesPoss.append(x);
		}
		setWidth(x);
	}
	void updateRowLinesPoss(){
		_rowLinesPoss.clear();
		int y = 0;
		_rowLinesPoss.append(y);
		for (auto& var : _rowHeights){
			y += var;
			_rowLinesPoss.append(y);
		}
		seeHeight(y);
	}

private:
	QRect itemRect(int row, int col)const{
		return QRect(_colLinesPoss.at(col), _rowLinesPoss.at(row), colWidth(col), rowHeight(row));
	}

private:
	QList<int> _colWidths;
	QList<int> _rowHeights;
	QList<int> _colLinesPoss;
	QList<int> _rowLinesPoss;
	QList<Item>	_items;
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
	void pageSizeChanged(const QSize& size);
	void pageColorChanged(const QColor& color);
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
		
		//setSize(500, 400);
	}

	bool load(const QString& fileName){ 
		XXml xml;
		if (!xml.load(fileName.toStdWString())||xml.key().toStdString()!=XML_ROOT)
			return false;
		setPageSize(QtHelper::covertStringToSize(QString::fromStdWString(xml.attr(XML_SIZE))));
		setPageColor(QtHelper::covertStringToColor(QString::fromStdWString(xml.attr(XML_COLOR))));

		for (auto& node : xml.children()){
			XPainterItem* item = nullptr;
			QString type = QString::fromStdWString(node.data().attr(XML_TYPE));
			QString value = QString::fromStdWString(node.data().val);
			if (type == ITEM_TYPE_TEXT){
				XTextPainterItem* textItem = new XTextPainterItem();
				item = textItem;
				textItem->setText(value);
				QFont font;
				font.fromString(QString::fromStdWString(node.data().attr(XML_FONT)));
				textItem->setFont(font);
				textItem->setAlignment(Qt::Alignment(node.data().attr(XML_ALIGN).toInt()));
			}
			if (type == ITEM_TYPE_LINE){
				XLinePainterItem* lineItem = new XLinePainterItem();
				item = lineItem;
				lineItem->setLineType(node.data().attr(XML_LINE_TYPE).toInt());
			}
			if (type == ITEM_TYPE_IMAGE){
				XImagePainterItem* imageItem = new XImagePainterItem();
				item = imageItem;
				imageItem->setImage(QString::fromStdWString(node.data().attr(XML_FILE)));
			}
			if (type == ITEM_TYPE_TABLE){
				XTablePainterItem* tableItem = new XTablePainterItem();
				item = tableItem;
				tableItem->setColWidths(QString::fromStdWString(node.data().attr(XML_COL_WIDTHS)));
				tableItem->setRowHeights(QString::fromStdWString(node.data().attr(XML_ROW_HEIGHTS)));
				for (auto itemnode : node.children()){
					XTablePainterItem::Item titem;
					titem.row = itemnode.data().attr(XML_ROW).toInt();
					titem.col = itemnode.data().attr(XML_COL).toInt();
					titem.text = QString::fromStdWString(itemnode.data().attr(XML_TEXT));
					titem.align = (Qt::Alignment)itemnode.data().attr(XML_ALIGN).toInt();
					titem.font.fromString(QString::fromStdWString(itemnode.data().attr(XML_FONT)));
					titem.bgColor = QtHelper::covertStringToColor(QString::fromStdWString(itemnode.data().attr(XML_BG_COLOR)));
					titem.fgColor = QtHelper::covertStringToColor(QString::fromStdWString(itemnode.data().attr(XML_FG_COLOR)));
					tableItem->setItem(titem);
				}
			}

			assert(item);
			if (item){
				item->setId(QString::fromStdWString(node.data().attr(XML_ID)));
				item->setSize(QtHelper::covertStringToSize(QString::fromStdWString(node.data().attr(XML_SIZE))));
				QPoint pos = QtHelper::covertStringToPoint(QString::fromStdWString(node.data().attr(XML_POS)));
				item->setBackgroundColor(QtHelper::covertStringToColor(QString::fromStdWString(node.data().attr(XML_BG_COLOR))));
				item->setForegroundColor(QtHelper::covertStringToColor(QString::fromStdWString(node.data().attr(XML_FG_COLOR))));
				_scene->addItem(item);
				item->setPos(pos);
			}
		}
		return true;
	}
	bool save(const QString& fileName){ 
		auto items = _scene->items();

		XXml xml;
		xml.setKey(XML_ROOT);
		xml.setAttr(XML_SIZE, QtHelper::covertSizeToString(_scene->sceneRect().size().toSize()).toStdWString());
		xml.setAttr(XML_COLOR, QtHelper::covertColorToString(_scene->backgroundBrush().color()).toStdWString());

		for (auto& var : items){
			XXmlNode node;
			node.setKey(XML_ITEM);
			XPainterItem* item = dynamic_cast<XPainterItem*>(var);
			XTextPainterItem* textItem = dynamic_cast<XTextPainterItem*>(var);
			XLinePainterItem* lineItem = dynamic_cast<XLinePainterItem*>(var);
			XImagePainterItem* imageItem = dynamic_cast<XImagePainterItem*>(var);
			XTablePainterItem* tableItem = dynamic_cast<XTablePainterItem*>(var);

#ifdef _DEBUG
			int this_types = 0;
			if (item)		++this_types;
			if (textItem)	++this_types;
			if (lineItem)	++this_types;
			if (imageItem)	++this_types;
			if (tableItem)	++this_types;
			assert(this_types == 2);
#endif 

			if (item){
				node.setAttr(XML_ID, item->id().toStdWString());
				node.setAttr(XML_SIZE, QtHelper::covertSizeToString(item->size()).toStdWString());
				node.setAttr(XML_POS, QtHelper::covertPointToString(item->pos().toPoint()).toStdWString());
				node.setAttr(XML_BG_COLOR, QtHelper::covertColorToString(item->backgroundColor()).toStdWString());
				node.setAttr(XML_FG_COLOR, QtHelper::covertColorToString(item->foregroundColor()).toStdWString());
			}
			if (textItem){
				node.setAttr(XML_TYPE, ITEM_TYPE_TEXT);
				node.setValue(textItem->text().toStdWString());
				node.setAttr(XML_FONT, textItem->font().toString().toStdWString());
				node.setAttr(XML_ALIGN, XString::number(textItem->alignment()));
			}
			if (lineItem){
				node.setAttr(XML_TYPE, ITEM_TYPE_LINE);
				node.setAttr(XML_LINE_TYPE, XString::number(lineItem->lineType()));
			}
			if (imageItem){
				node.setAttr(XML_TYPE, ITEM_TYPE_IMAGE);
				node.setAttr(XML_FILE, imageItem->pathfile().toStdWString());
			}
			if (tableItem){
				node.setAttr(XML_TYPE, ITEM_TYPE_TABLE);
				node.setAttr(XML_COL_WIDTHS, tableItem->colWidthsStr().toStdWString());
				node.setAttr(XML_ROW_HEIGHTS, tableItem->rowHeightsStr().toStdWString());

				auto tableItems = tableItem->items();
				for (auto& tableItem : tableItems){
					XXmlNode tableNode;
					tableNode.setKey(XML_ITEM);
					tableNode.setAttr(XML_ROW, XString::number(tableItem.row));
					tableNode.setAttr(XML_COL, XString::number(tableItem.col));
					tableNode.setAttr(XML_TEXT, tableItem.text.toStdWString());
					tableNode.setAttr(XML_FONT, tableItem.font.toString().toStdWString());
					tableNode.setAttr(XML_ALIGN, XString::number(tableItem.align));
					tableNode.setAttr(XML_BG_COLOR, QtHelper::covertColorToString(tableItem.bgColor).toStdWString());
					tableNode.setAttr(XML_FG_COLOR, QtHelper::covertColorToString(tableItem.fgColor).toStdWString());
					node.addChild(tableNode);
				}
			}
			xml.addChild(node);
		}

		return xml.save(fileName.toStdWString());
	}
	QSize pageSize()const{ return _scene->sceneRect().size().toSize(); }
	QColor pageColor()const{ return _scene->backgroundBrush().color(); }
	
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
public slots:
	void setPageSize(const QSize& size){
		if (size != pageSize()){
			_scene->setSceneRect(0, 0, size.width(), size.height());
			updateSize();
			emit pageSizeChanged(size);
		}
	}
	void setPageColor(const QColor& color){
		if (color != pageColor()){
			_scene->setBackgroundBrush(QBrush(color));
			emit pageColorChanged(color);
		}
	}
	void setItemsId(const QString& id){
		auto items = selectedItems();
		for (auto& var : items){
			XPainterItem* item = dynamic_cast<XPainterItem*>(var);
			if (item){
				item->setId(id);
			}
		}
	}
	void setItemsPosX(int x){
		auto items = selectedItems();
		for (auto& var : items){
			XPainterItem* item = dynamic_cast<XPainterItem*>(var);
			if (item){
				auto pos= item->pos();
				pos.setX(x);
				item->setPos(pos);
			}
		}
	}
	void setItemsPosY(int y){
		auto items = selectedItems();
		for (auto& var : items){
			XPainterItem* item = dynamic_cast<XPainterItem*>(var);
			if (item){
				auto pos = item->pos();
				pos.setY(y);
				item->setPos(pos);
			}
		}
	}
	void setItemsWidth(int w){
		auto items = selectedItems();
		for (auto& var : items){
			XPainterItem* item = dynamic_cast<XPainterItem*>(var);
			if (item){
				QSize size = item->size();
				size.setWidth(w);
				item->setSize(size);
			}
		}
	}
	void setItemsHeight(int h){
		auto items = selectedItems();
		for (auto& var : items){
			XPainterItem* item = dynamic_cast<XPainterItem*>(var);
			if (item){
				QSize size = item->size();
				size.setHeight(h);
				item->setSize(size);
			}
		}
	}
	void setItemsBgColor(const QColor& color){
		auto items = selectedItems();
		for (auto& var : items){
			XPainterItem* item = dynamic_cast<XPainterItem*>(var);
			if (item){
				item->setBackgroundColor(color);
			}
		}
	}
	void setItemsFgColor(const QColor& color){
		auto items = selectedItems();
		for (auto& var : items){
			XPainterItem* item = dynamic_cast<XPainterItem*>(var);
			if (item){
				item->setForegroundColor(color);
			}
		}
	}
	void setTextItemsText(const QString& text){
		auto items = selectedItems();
		for (auto& var : items){
			XTextPainterItem* item = dynamic_cast<XTextPainterItem*>(var);
			if (item){
				item->setText(text);
			}
		}
	}
	void setTextItemsFont(const QFont& font){
		auto items = selectedItems();
		for (auto& var : items){
			XTextPainterItem* item = dynamic_cast<XTextPainterItem*>(var);
			if (item){
				item->setFont(font);
			}
		}
	}
	void setTextItemsAlign(Qt::Alignment align){
		auto items = selectedItems();
		for (auto& var : items){
			XTextPainterItem* item = dynamic_cast<XTextPainterItem*>(var);
			if (item){
				item->setAlignment(align);
			}
		}
	}
	void deleteSelectedItems(){
		auto items = selectedItems();
		for (auto& var : items){
			delete var;
		}
	}

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

	void mouseDoubleClickEvent(QMouseEvent *event){
		QGraphicsView::mouseDoubleClickEvent(event);
		auto items = selectedItems();
		if (items.isEmpty())
			return;

		auto item = items.at(0);
		XTextPainterItem* textItem = dynamic_cast< XTextPainterItem*>(item);
		XLinePainterItem* lineItem = dynamic_cast< XLinePainterItem*>(item);
		XImagePainterItem* imageItem = dynamic_cast< XImagePainterItem*>(item);
		XTablePainterItem* tableItem = dynamic_cast< XTablePainterItem*>(item);
		if (tableItem){
			_tableEditor.setRowCount(tableItem->rowCount());
			_tableEditor.setColCount(tableItem->colCount());
			for (int col = 0; col < tableItem->colCount(); ++col)
				_tableEditor.setColWidth(col, tableItem->colWidth(col));
			for (int row = 0; row < tableItem->rowCount(); ++row)
				_tableEditor.setRowHeight(row, tableItem->rowHeight(row));
			for (auto& item : tableItem->items()){
				_tableEditor.setItemBgColor(item.row, item.col, item.bgColor);
				_tableEditor.setItemFgColor(item.row, item.col, item.fgColor);
				_tableEditor.setItemFont(item.row, item.col, item.font);
				_tableEditor.setItemText(item.row, item.col, item.text);
				_tableEditor.setItemAlign(item.row, item.col, item.align);
			}
			if (_tableEditor.exec()){

				int rowCount = _tableEditor.rowCount();
				QList<int> rowHeights;
				for (int row = 0; row < rowCount; ++row)
					rowHeights.append(_tableEditor.rowHeight(row));
				tableItem->setRowHeights(rowHeights, false);

				int colCount = _tableEditor.colCount();
				QList<int> colWidths;
				for (int col = 0; col < colCount; ++col)
					colWidths.append(_tableEditor.colWidth(col));
				tableItem->setColWidths(colWidths, false);
				tableItem->cleanItems();
				for (int row = 0; row < rowCount; ++row){
					for (int col = 0; col < colCount; ++col){
						XTablePainterItem::Item it;
						it.row = row; it.col = col;
						it.bgColor = _tableEditor.itemBgColor(row, col);
						it.fgColor = _tableEditor.itemFgColor(row, col);
						it.font = _tableEditor.itemFont(row, col);
						it.text = _tableEditor.itemText(row, col);
						it.align = _tableEditor.itemAlign(row, col);
						tableItem->setItem(it,false);
					}
				}

				tableItem->update();
			}
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

	XPainterTableItemEditor _tableEditor;
};

#endif //_X_PAINTER_H
