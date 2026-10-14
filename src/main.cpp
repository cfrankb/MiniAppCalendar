#include <QtWidgets/QApplication>
#include <QLocalServer>
#include <QLocalSocket>
#include <QTranslator>
#include <QTimer>
#include <QMessageBox>
#include "mainwindow.h"

static const char *LOCK_KEY = "MiniAppCalendarLock";

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName("MiniAppCalendar");

    // --- Single-instance lock ---
    // Try to connect to an existing server. If one exists, another
    // instance is already running: bring its window to the front and exit.
    QLocalSocket socket;
    socket.connectToServer(LOCK_KEY);
    if (socket.waitForConnected(500))
    {
        // Another instance owns the lock. Activate its window and quit.
        QLocalSocket *conn = new QLocalSocket();
        conn->connectToServer(LOCK_KEY);
        if (conn->waitForConnected(500))
        {
            QByteArray msg = "ACTIVATE";
            conn->write(msg);
            conn->flush();
            conn->waitForBytesWritten(500);
        }
        conn->deleteLater();
        socket.disconnectFromServer();

        QMessageBox::information(nullptr,
                                 a.applicationName(),
                                 "MiniAppCalendar is already running.");
        return 0;
    }

    // No existing server: create one and become the lock owner.
    QLocalServer *server = new QLocalServer();
    server->removeServer(LOCK_KEY); // clean up stale lock from a prior crash
    if (!server->listen(LOCK_KEY))
    {
        QMessageBox::critical(nullptr,
                              a.applicationName(),
                              "Unable to start MiniAppCalendar:\n" +
                              server->errorString());
        return 1;
    }

    QObject::connect(server, &QLocalServer::newConnection, [&a, server]()
    {
        QLocalSocket *client = server->nextPendingConnection();
        if (client)
        {
            QObject::connect(client, &QLocalSocket::disconnected,
                             client, &QLocalSocket::deleteLater);
            while (client->canReadLine())
            {
                QByteArray line = client->readLine().trimmed();
                if (line == "ACTIVATE")
                {
                    // Bring the existing main window to the front.
                    QTimer::singleShot(0, [&a]()
                    {
                        for (QWidget *w : a.topLevelWidgets())
                        {
                            if (qobject_cast<MainWindow *>(w))
                            {
                                w->showNormal();
                                w->raise();
                                w->activateWindow();
                            }
                        }
                    });
                }
            }
        }
    });

    /*QTranslator translator;
    if (!translator.load(":/translations/translation_fr"))
    {
        printf("cannot find translation\n");
    }
    a.installTranslator(&translator);*/

    MainWindow w;
    w.setWindowIcon(QIcon(":/images/icon.gif"));
    if (argc > 1)
    {
        w.setDocument(argv[1]);
    }

    w.show();
    return a.exec();
}
