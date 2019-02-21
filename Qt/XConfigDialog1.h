#ifndef _X_CONFIG_DIALOG_H
#define _X_CONFIG_DIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QListView>
#include <QListWidget>
#include <QLabel>
#include <QCheckBox>
#include <QGroupBox>
#include <QStackedWidget>
#include <QPushButton>
#include <QGridLayout>
#include <QDialogButtonBox>
#include <QHash>
#include <QSplitter>
#include <QVBoxLayout>
#include <QDebug>


class XConfigDialog : public QDialog{

	Q_OBJECT

signals:
    void valueChanged(const QString& key, const QString& val);
    void okButtonClicked();
    void cancelButtonClicked();
    void applyButtonClicked();

public:
	XConfigDialog(QWidget* parent=nullptr)
        :QDialog(parent){
        setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
        setWindowTitle(tr("Config"));

        _lineEditFilter.setPlaceholderText(tr("Filter"));
        QFont font = _labelHeader.font();
        font.setBold(true);
        _labelHeader.setFont(font);
        _labelHeader.setAlignment(Qt::AlignCenter);
        //_layoutWidget.setFrameShape(QFrame::Box);

        _buttonBox = new QDialogButtonBox(
            QDialogButtonBox::Ok |
            QDialogButtonBox::Cancel |
            QDialogButtonBox::Apply,
            this);
        connect(_buttonBox, SIGNAL(clicked(QAbstractButton *)), this, SLOT(onDialogButtonBoxClicked(QAbstractButton *)));

        _splitter = new QSplitter(this);
        _splitter->setOrientation(Qt::Horizontal);
        _splitter->setChildrenCollapsible(false);
        QWidget* wLeft = new QWidget(_splitter);
        wLeft->setLayout(new QVBoxLayout(wLeft));
        wLeft->layout()->addWidget(&_lineEditFilter);
        wLeft->layout()->addWidget(&_listCategory);
        wLeft->layout()->setMargin(0);
        QWidget* wRight = new QWidget(_splitter);
        wRight->setLayout(new QVBoxLayout(wRight));
        wRight->layout()->addWidget(&_labelHeader);
        wRight->layout()->addWidget(&_layoutWidget);
        wRight->layout()->setMargin(0);
        _splitter->setStretchFactor(0, 1);
        _splitter->setStretchFactor(1, 3);

        this->setLayout(new QVBoxLayout(this));
        this->layout()->addWidget(_splitter);
        this->layout()->addWidget(_buttonBox);

       
        //_listCategory.setIconSize(QSize(24, 24));
        connect(&_listCategory, SIGNAL(currentRowChanged(int)), &_layoutWidget, SLOT(setCurrentIndex(int)));
        connect(&_listCategory, SIGNAL(currentTextChanged(const QString&)), &_labelHeader, SLOT(setText(const QString&)));

        connect(&_lineEditFilter, SIGNAL(textChanged(const QString&)), this, SLOT(filter(const QString&)));
    }
	void addItem(const QString& text, QWidget* w, const QIcon& icon=QIcon(), bool linkSignal = true)
    {
		new QListWidgetItem(icon, text,&_listCategory);
        _layoutWidget.addWidget(w);

        if (linkSignal){
            connect(w, SIGNAL(valueChanged(const QString&, const QString&)),
                this, SLOT(setValue(const QString&, const QString&)));
        }
    }

public slots:
    void setValue(const QString& key, const QString& val){  _values[key] = val; }
    int exec(int idx=0){
        _listCategory.setCurrentRow(idx);
        return QDialog::exec();
    }
    void show(int idx = 0){
        _listCategory.setCurrentRow(idx);
        QDialog::show();
    }
	void setListWidth(int w){ _listCategory.setMinimumWidth(w); }

private:
    const QStringList& widgetStrings(QWidget* w){
        Q_ASSERT(w);
        if (!_widgetStrings.contains(w)){
            _widgetStrings[w] = widgetStringList(w);
        }
        return _widgetStrings[w];
    }

    QStringList widgetStringList(QWidget* w){
        Q_ASSERT(w);
        QStringList stringList;
        foreach(const QLabel* label, w->findChildren<QLabel*>())
            stringList << label->text();
        foreach(const QPushButton* pushButton, w->findChildren<QPushButton*>())
            stringList << pushButton->text();
        foreach(const QCheckBox* checkBox, w->findChildren<QCheckBox*>())
            stringList << checkBox->text();
        foreach(const QGroupBox* groupBox, w->findChildren<QGroupBox*>())
            stringList << groupBox->title();

        QTabWidget* tabWidget = qobject_cast<QTabWidget*>(w);
        if (tabWidget){
            int tabCount = tabWidget->count();
            for (int i = 0; i < tabCount; ++i) {
                stringList << tabWidget->tabText(i);
            }
        }


        // clean up accelerators
        QMutableStringListIterator it(stringList);
        while (it.hasNext())
            it.next().remove(QLatin1Char('&'));

        return stringList;
    }

    bool filterCategory(int idx,const QString& str)
    {
        if (_listCategory.item(idx)->text().contains(str, Qt::CaseInsensitive))
            return true;
        
        QWidget* w = _layoutWidget.widget(idx);
        foreach(const QString& var, widgetStrings(w)){
            if (var.contains(str, Qt::CaseInsensitive))
                return true;
        }
        return false;
    }
    bool filterTabWidget(QTabWidget* tabWidget, int idx,const QString& str)
    {
        if (tabWidget->tabText(idx).contains(str, Qt::CaseInsensitive))
            return true;
        QWidget* w = tabWidget->widget(idx);
        foreach(const QString& var, widgetStrings(w)){
            if (var.contains(str, Qt::CaseInsensitive))
                return true;
        }
        return false;
    }
    void setWidgetHorizontalPolicy(QWidget* w, QSizePolicy::Policy horizontal){
        QSizePolicy policy = w->sizePolicy();
        policy.setHorizontalPolicy(horizontal);
        w->setSizePolicy(policy);
    }

    void apply(){
        QHash<QString, QString>::const_iterator itr = _values.constBegin();
        while (itr != _values.constEnd()){
            emit valueChanged(itr.key(), itr.value());
            ++itr;
        }
        _values.clear();
    }

private slots:
    void filter(const QString& str)
    {
        int count = _listCategory.count();
        for (int i = 0; i < count; ++i){
            bool enable = filterCategory(i,str);
            _listCategory.setRowHidden(i, !enable);
            if (enable){
                QTabWidget* tabWidget = qobject_cast<QTabWidget*>(_layoutWidget.widget(i));
                if (tabWidget){
                    int tabCount = tabWidget->count();
                    for (int tabIdx = 0; tabIdx < tabCount; ++tabIdx)
                        tabWidget->setTabEnabled(tabIdx, filterTabWidget(tabWidget, tabIdx,str));
                }
            }
        }

        _labelHeader.setVisible(true);
        _layoutWidget.setVisible(true);
        for (int i = 0; i < count; ++i){
            if (!_listCategory.isRowHidden(i)){
                _listCategory.setCurrentRow(i);
                return;
            }
        }
        _labelHeader.setVisible(false);
        _layoutWidget.setVisible(false);
    }
    void onDialogButtonBoxClicked(QAbstractButton *button){
        auto standardButton = _buttonBox->standardButton(button);
        switch (standardButton)
        {
        case QDialogButtonBox::Ok:
            apply();
            accept();
            emit okButtonClicked();
            break;
        case QDialogButtonBox::Cancel:
            _values.clear();
            reject();
            emit cancelButtonClicked();
            break;
        case QDialogButtonBox::Apply:
            apply();
            emit applyButtonClicked();
            break;
        default:
            break;
        }
    }

protected:
    QLineEdit   _lineEditFilter;
    QListWidget _listCategory;
    QLabel      _labelHeader;
    QStackedWidget      _layoutWidget;
    QDialogButtonBox*   _buttonBox;
    QHash<QWidget*, QStringList>    _widgetStrings;
    QHash<QString, QString>         _values;
    QSplitter*  _splitter;
};

#endif //_X_CONFIG_DIALOG_H
