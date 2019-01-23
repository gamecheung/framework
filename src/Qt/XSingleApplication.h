#ifndef _X_SIGNALE_APPLICATION_H
#define _X_SIGNALE_APPLICATION_H

#include <QApplication>
#include <QLocalServer>
#include <QLocalSocket>
#include <QFileInfo>

class XSingleApplication : public QApplication{
	Q_OBJECT
signals:
    void newAppRunning();

public:
    XSingleApplication(int &argc, char **argv)
        :QApplication(argc,argv)
        , m_isTheOnly(true)
    {
		m_server = new QLocalServer(this);
        m_serverName = QFileInfo(QApplication::applicationFilePath()).fileName();
        QLocalSocket socket;
        socket.connectToServer(m_serverName);
        if (socket.waitForConnected(100)){
            m_isTheOnly = false;
        }
        else{
            connect(m_server, SIGNAL(newConnection()), this, SIGNAL(newAppRunning()));
            if (!m_server->listen(m_serverName)){
                if (m_server->serverError() == QAbstractSocket::AddressInUseError){
                    QLocalServer::removeServer(m_serverName);
                    m_server->listen(m_serverName);
                }
            }
        }
    }

    bool isTheOnlyApplication()const{ return m_isTheOnly; }

private:
private:
    bool            m_isTheOnly;
    QLocalServer*   m_server;
    QString         m_serverName;
};

#endif //_X_SIGNALE_APPLICATION_H
