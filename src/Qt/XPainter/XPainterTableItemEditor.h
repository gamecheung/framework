#ifndef XPAINTERTABLEITEMEDITOR_H
#define XPAINTERTABLEITEMEDITOR_H

#include <QDialog>
namespace Ui {class XPainterTableItemEditor;};

class XPainterTableItemEditor : public QDialog
{
	Q_OBJECT

public:
	XPainterTableItemEditor(QWidget *parent = 0);
	~XPainterTableItemEditor();

	void setRowCount(int count);
	void setColCount(int count);
	void setItemBgColor(int row,int col,const QColor& color);
	void setItemFgColor(int row, int col, const QColor& color);
	void setItemFont(int row, int col, const QFont& font);
	void setItemText(int row, int col, const QString& text);
	void setItemAlign(int row, int col, Qt::Alignment align);
	void setColWidth(int col, int width);
	void setRowHeight(int row, int height);

	int rowCount()const;
	int colCount()const;
	int rowHeight(int row)const;
	int colWidth(int col)const;
	QColor itemBgColor(int row, int col)const;
	QColor itemFgColor(int row, int col)const;
	QFont itemFont(int row, int col)const;
	QString itemText(int row, int col)const;
	Qt::Alignment itemAlign(int row, int col)const;

private slots:
	void setTableRowCount(int count);
	void setTableColCount(int count);
	void setItemBgColor();
	void setItemFgColor();
	void setItemText(const QString& text);
	void setItemFont();
	void setItemAlign();
	void updateCurrentItemInfo();

private:
	Qt::Alignment align()const;
	void setAlign(Qt::Alignment align);

private:
	Ui::XPainterTableItemEditor *ui;
};

#endif // XPAINTERTABLEITEMEDITOR_H
