#ifndef _X_MAINWINDOW_H
#define _X_MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QSettings>
#include <QFileDialog>
#include <QStatusBar>
#include <QCloseEvent>
#include <QApplication>
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QToolBar>

class XMainWindow: public QMainWindow
{
	Q_OBJECT
public:
	XMainWindow(QWidget* parent =nullptr):QMainWindow(parent)
	{
		//createActions();
		readSettings();
		setUnifiedTitleAndToolBarOnMac(true);
		setCurrentFile(QString());
	}
	virtual ~XMainWindow(){}


public:
	void loadFile(const QString &fileName)
	{
		loadFileEvent(fileName);
		setCurrentFile(fileName);
		statusBar()->showMessage(tr("File loaded"), 2000);
	}


protected slots:
	void newFile()
	{
		if (maybeSave())
		{
			newFileEvent();
			setCurrentFile(QString());
		}
	}
	void open()
	{
		if (maybeSave())
		{
			QString fileName = QFileDialog::getOpenFileName(this);
			if (!fileName.isEmpty())
				loadFile(fileName);
		}
	}

	bool save()
	{
		if (m_curFile.isEmpty())
		{
			return saveAs();
		}
		else
		{
			return saveFile(m_curFile);
		}
	}

	bool saveAs()
	{
		QFileDialog dialog(this);
		dialog.setWindowModality(Qt::WindowModal);
		dialog.setAcceptMode(QFileDialog::AcceptSave);
		if (dialog.exec() != QDialog::Accepted)
			return false;
		return saveFile(dialog.selectedFiles().first());
	}
	void about()
	{
		QMessageBox::about(this, tr("About Application"),
			tr("The <b>Application</b> example demonstrates how to "
			"write modern GUI applications using Qt, with a menu bar, "
			"toolbars, and a status bar."));
	}
	void documentWasModified(){ setModified(true); }

	void setModified(bool yes)
	{
		m_needSave = yes;
		setWindowModified(m_needSave);
	}

protected:
	void createActions()
	{
		QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
		QToolBar *fileToolBar = addToolBar(tr("File"));
		QAction *newAct = new QAction(QIcon(), tr("&New"), this);
		newAct->setShortcuts(QKeySequence::New);
		newAct->setStatusTip(tr("Create a new file"));
		connect(newAct, &QAction::triggered, this, &XMainWindow::newFile);
		fileMenu->addAction(newAct);
		fileToolBar->addAction(newAct);

		QAction *openAct = new QAction(QIcon(), tr("&Open..."), this);
		openAct->setShortcuts(QKeySequence::Open);
		openAct->setStatusTip(tr("Open an existing file"));
		connect(openAct, &QAction::triggered, this, &XMainWindow::open);
		fileMenu->addAction(openAct);
		fileToolBar->addAction(openAct);

		QAction *saveAct = new QAction(QIcon(), tr("&Save"), this);
		saveAct->setShortcuts(QKeySequence::Save);
		saveAct->setStatusTip(tr("Save the document to disk"));
		connect(saveAct, &QAction::triggered, this, &XMainWindow::save);
		fileMenu->addAction(saveAct);
		fileToolBar->addAction(saveAct);

		QAction *saveAsAct = fileMenu->addAction(QIcon(), tr("Save &As..."), this, &XMainWindow::saveAs);
		saveAsAct->setShortcuts(QKeySequence::SaveAs);
		saveAsAct->setStatusTip(tr("Save the document under a new name"));

		fileMenu->addSeparator();

		QAction *exitAct = fileMenu->addAction(QIcon(), tr("E&xit"), this, &QWidget::close);
		exitAct->setShortcuts(QKeySequence::Quit);
		exitAct->setStatusTip(tr("Exit the application"));

		menuBar()->addSeparator();

		QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
		QAction *aboutAct = helpMenu->addAction(tr("&About"), this, &XMainWindow::about);
		aboutAct->setStatusTip(tr("Show the application's About box"));

		QAction *aboutQtAct = helpMenu->addAction(tr("About &Qt"), qApp, &QApplication::aboutQt);
		aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
	}
	void createStatusBar();
	void readSettings()
	{
		QSettings settings(QCoreApplication::applicationName() + ".ini", QSettings::IniFormat);
		restoreGeometry(settings.value("geometry").toByteArray());
		restoreState(settings.value("state").toByteArray());
	}
	void writeSettings()
	{
		QSettings settings(QCoreApplication::applicationName() + ".ini", QSettings::IniFormat);
		settings.setValue("geometry",saveGeometry());
		settings.setValue("state", saveState());
	}
	bool maybeSave()
	{
		if (!m_needSave)
			return true;
		const QMessageBox::StandardButton ret
			= QMessageBox::warning(this, tr("Application"),
			tr("The document has been modified.\n"
			"Do you want to save your changes?"),
			QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
		switch (ret) {
		case QMessageBox::Save:
			return save();
		case QMessageBox::Cancel:
			return false;
		default:
			break;
		}
		return true;
	}
	bool saveFile(const QString &fileName)
	{
		saveFileEvent(fileName);
		setCurrentFile(fileName);
		statusBar()->showMessage(tr("File saved"), 2000);
		return true;
	}
	void setCurrentFile(const QString &fileName)
	{
		m_curFile = fileName;
		setModified(false);

		QString shownName = m_curFile;
		if (m_curFile.isEmpty())
			shownName = "untitled";
		setWindowFilePath(shownName);
	}

protected:
	virtual void newFileEvent(){}
	virtual void loadFileEvent(const QString& fileName){}
	virtual void saveFileEvent(const QString& fileName){}
	void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE
	{
		if (maybeSave())
		{
			writeSettings();
			event->accept();
		}
		else
		{
			event->ignore();
		}
	}

private:
	QString m_curFile;
	bool	m_needSave;
};

#endif //_X_MAINWINDOW_H
