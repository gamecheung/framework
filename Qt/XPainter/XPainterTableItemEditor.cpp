#include "XPainterTableItemEditor.h"
#include "ui_XPainterTableItemEditor.h"
#include <QDebug>
#include <QColorDialog>
#include <QFontDialog>
#include "../../Qt/XQt.h"
#include "../../Qt/XTableWidgetHelper.h"
#include "../../Qt/XPixmapFactory.h"

XPainterTableItemEditor::XPainterTableItemEditor(QWidget *parent)
	: QDialog(parent)
{
	ui = new Ui::XPainterTableItemEditor();
	ui->setupUi(this);

	connect(ui->spinBox_RowCount, SIGNAL(valueChanged(int)), this, SLOT(setTableRowCount(int)));
	connect(ui->spinBox_ColCount, SIGNAL(valueChanged(int)), this, SLOT(setTableColCount(int)));

	connect(ui->pushButton_BgColor, SIGNAL(clicked()), this, SLOT(setItemBgColor()));
	connect(ui->pushButton_FgColor, SIGNAL(clicked()), this, SLOT(setItemFgColor()));
	connect(ui->pushButton_Font, SIGNAL(clicked()), this, SLOT(setItemFont()));
	connect(ui->lineEdit_Text, SIGNAL(textEdited(const QString&)), this, SLOT(setItemText(const QString&)));

	connect(ui->tableWidget, SIGNAL(cellChanged(int , int )), this, SLOT(updateCurrentItemInfo()));
	connect(ui->tableWidget, SIGNAL(itemSelectionChanged()), this, SLOT(updateCurrentItemInfo()));

	ui->toolButton_AlignLeft->setIcon(QIcon(XAlignPixmapFactory().pixmap(Qt::AlignLeft)));
	ui->toolButton_AlignHCenter->setIcon(QIcon(XAlignPixmapFactory().pixmap(Qt::AlignHCenter)));
	ui->toolButton_AlignRight->setIcon(QIcon(XAlignPixmapFactory().pixmap(Qt::AlignRight)));
	ui->toolButton_AlignTop->setIcon(QIcon(XAlignPixmapFactory().pixmap(Qt::AlignTop)));
	ui->toolButton_AlignVCenter->setIcon(QIcon(XAlignPixmapFactory().pixmap(Qt::AlignVCenter)));
	ui->toolButton_AlignBottom->setIcon(QIcon(XAlignPixmapFactory().pixmap(Qt::AlignBottom)));
	connect(ui->buttonGroup_Align, SIGNAL(buttonClicked(int)), this, SLOT(setItemAlign()));
	connect(ui->buttonGroup_Align2, SIGNAL(buttonClicked(int)), this, SLOT(setItemAlign()));
	//connect(ui->toolButton_AlignLeft, SIGNAL(clicked()), this, SLOT(setItemAlign()));
	//connect(ui->toolButton_AlignLeft, SIGNAL(clicked()), this, SLOT(setItemAlign()));
	//connect(ui->toolButton_AlignLeft, SIGNAL(clicked()), this, SLOT(setItemAlign()));
}

XPainterTableItemEditor::~XPainterTableItemEditor()
{
	delete ui;
}

void XPainterTableItemEditor::setRowCount(int count){
	ui->spinBox_RowCount->setValue(count);
}
void XPainterTableItemEditor::setColCount(int count){
	ui->spinBox_ColCount->setValue(count);
}
void XPainterTableItemEditor::setItemText(int row, int col, const QString& text){
	XTABLE_WIDGET_HELPER(ui->tableWidget)->setText(row, col, text);
}

void XPainterTableItemEditor::setItemAlign(int row, int col, Qt::Alignment align){
	XTABLE_WIDGET_HELPER(ui->tableWidget)->item(row, col)->setTextAlignment(align);
}
void XPainterTableItemEditor::setItemBgColor(int row, int col, const QColor& color){
	XTABLE_WIDGET_HELPER(ui->tableWidget)->item(row, col)->setBackground(color);
}
void XPainterTableItemEditor::setItemFgColor(int row, int col, const QColor& color){
	XTABLE_WIDGET_HELPER(ui->tableWidget)->item(row, col)->setForeground(color);
}
void XPainterTableItemEditor::setItemFont(int row, int col, const QFont& font){
	XTABLE_WIDGET_HELPER(ui->tableWidget)->item(row, col)->setFont(font);
}
void XPainterTableItemEditor::setColWidth(int col, int width){
	ui->tableWidget->setColumnWidth(col, width);
}
void XPainterTableItemEditor::setRowHeight(int row, int height){
	ui->tableWidget->setRowHeight(row, height);
}


void XPainterTableItemEditor::setTableRowCount(int count){
	ui->tableWidget->setRowCount(count);
}
void XPainterTableItemEditor::setTableColCount(int count){
	ui->tableWidget->setColumnCount(count);
}
void XPainterTableItemEditor::setItemBgColor(){
	auto color = QColorDialog::getColor();
	if (color.isValid()){
		auto items =XTABLE_WIDGET_HELPER( ui->tableWidget)->selectedItems();
		for (auto& item : items){
			item->setBackground(color);
		}
		SET_WIDGET_BG_COLOR(ui->pushButton_BgColor, color);
	}
}
void XPainterTableItemEditor::setItemFgColor(){
	auto color = QColorDialog::getColor();
	if (color.isValid()){
		auto items = XTABLE_WIDGET_HELPER(ui->tableWidget)->selectedItems();
		for (auto& item : items){
			item->setForeground(color);
		}
		SET_WIDGET_BG_COLOR(ui->pushButton_FgColor, color);
	}
}

void XPainterTableItemEditor::setItemText(const QString& text){
	auto items = XTABLE_WIDGET_HELPER(ui->tableWidget)->selectedItems();
	for (auto& item : items){
		item->setText(text);
	}
}



void XPainterTableItemEditor::setItemFont(){
	bool ok;
	auto font = QFontDialog::getFont(&ok);
	if (ok){
		auto items = XTABLE_WIDGET_HELPER(ui->tableWidget)->selectedItems();
		for (auto& item : items){
			item->setFont(font);
		}
		ui->pushButton_Font->setText(QString("%1,%2").arg(font.family()).arg(font.pointSize()));
	}
}


void XPainterTableItemEditor::setItemAlign(){
	auto items = XTABLE_WIDGET_HELPER(ui->tableWidget)->selectedItems();
	Qt::Alignment align = this->align();
	for (auto& item : items){
		item->setTextAlignment(align);
	}
}


void XPainterTableItemEditor::updateCurrentItemInfo(){
	int row = ui->tableWidget->currentRow();
	int col = ui->tableWidget->currentColumn();
	if (row == -1 || col == -1)
		return;

	ui->lineEdit_Text->setText(itemText(row,col));
	SET_WIDGET_BG_COLOR(ui->pushButton_BgColor, itemBgColor(row,col));
	SET_WIDGET_BG_COLOR(ui->pushButton_FgColor, itemFgColor(row,col));
	QFont font = itemFont(row, col);
	ui->pushButton_Font->setText(QString("%1,%2").arg(font.family()).arg(font.pointSize()));
	setAlign(itemAlign(row, col));
}


int XPainterTableItemEditor::rowCount()const{ return ui->tableWidget->rowCount(); }
int XPainterTableItemEditor::colCount()const{ return ui->tableWidget->columnCount(); }
int XPainterTableItemEditor::rowHeight(int row)const{ return ui->tableWidget->rowHeight(row); }
int XPainterTableItemEditor::colWidth(int col)const{ return ui->tableWidget->columnWidth(col); }
QColor XPainterTableItemEditor::itemBgColor(int row, int col)const{ 
	auto brush = XTABLE_WIDGET_HELPER(ui->tableWidget)->item(row, col)->background();
	return brush.isOpaque() ? brush.color() : Qt::white;
}
QColor XPainterTableItemEditor::itemFgColor(int row, int col)const{ 
	auto brush = XTABLE_WIDGET_HELPER(ui->tableWidget)->item(row, col)->foreground();
	return brush.isOpaque() ? brush.color() : Qt::black;
}
QFont XPainterTableItemEditor::itemFont(int row, int col)const{ return XTABLE_WIDGET_HELPER(ui->tableWidget)->item(row, col)->font(); }
QString XPainterTableItemEditor::itemText(int row, int col)const{ return XTABLE_WIDGET_HELPER(ui->tableWidget)->text(row, col); }
Qt::Alignment XPainterTableItemEditor::itemAlign(int row, int col)const{ return XTABLE_WIDGET_HELPER(ui->tableWidget)->item(row, col)->textAlignment(); }


Qt::Alignment XPainterTableItemEditor::align()const{
	Qt::Alignment align;
	if (ui->toolButton_AlignLeft->isChecked())	align |= Qt::AlignLeft;
	if (ui->toolButton_AlignHCenter->isChecked())	align |= Qt::AlignHCenter;
	if (ui->toolButton_AlignRight->isChecked())	align |= Qt::AlignRight;
	if (ui->toolButton_AlignTop->isChecked())	align |= Qt::AlignTop;
	if (ui->toolButton_AlignVCenter->isChecked())	align |= Qt::AlignVCenter;
	if (ui->toolButton_AlignBottom->isChecked())	align |= Qt::AlignBottom;
	return align;
}

void XPainterTableItemEditor::setAlign(Qt::Alignment align){
	ui->toolButton_AlignLeft->setChecked(align&Qt::AlignLeft);
	ui->toolButton_AlignHCenter->setChecked(align&Qt::AlignHCenter);
	ui->toolButton_AlignRight->setChecked(align&Qt::AlignRight);
	ui->toolButton_AlignTop->setChecked(align&Qt::AlignTop);
	ui->toolButton_AlignVCenter->setChecked(align&Qt::AlignVCenter);
	ui->toolButton_AlignBottom->setChecked(align&Qt::AlignBottom);
}

