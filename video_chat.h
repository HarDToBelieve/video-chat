#ifndef VIDEO_CHAT_H
#define VIDEO_CHAT_H

#include <QDialog>
#include <QObject>
#include <QTcpSocket>
#include <memory>
#include <iostream>
#include <QStringListModel>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

#include "ui_main.h"
#include "rtc_base/async_socket.h"
#include "rtc_base/thread.h"
#include "rtc_base/logging.h"
#include "rtc_base/physical_socket_server.h"

//#include "video_renderer.h"
#include "backend.h"

typedef std::map<QString, int> Peers;

namespace Ui {
    class VideoChat;
}

class VideoChat : public QDialog, public sigslot::has_slots<>
{
    Q_OBJECT
public:
    explicit VideoChat(QWidget *parent = nullptr);
    ~VideoChat();

    void InitializeConnection();
    void Close();
    void StreamVideo();
private:
    Ui::VideoChat *ui;

    int my_id;
    QTcpSocket *control_socket;
    QTcpSocket *data_socket;
    QString username;
    QString host;
    quint16 port;
    Peers peers;
    bool isLoggedIn = false;
    bool isSigningOut = false;
    bool isCalling = false;
    QStringListModel *model;
    Backend *be;

private:
    Q_SLOT void on_listView_peers_doubleClicked(const QModelIndex &index);
    Q_SLOT void on_login_btn_clicked();
    Q_SLOT void onControlConnect();
    Q_SLOT void onControlDisconnect();
    Q_SLOT void onControlRead();

    Q_SLOT void onDataConnect();
    Q_SLOT void onDataDisconnect();
    Q_SLOT void onDataRead();
};

#endif // VIDEO_CHAT_H
