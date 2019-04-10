#ifndef VIDEO_CHAT_H
#define VIDEO_CHAT_H

#include <QDialog>
#include <QObject>
#include <QTcpSocket>
#include <memory>
#include <iostream>

#include "ui_main.h"
#include "rtc_base/async_socket.h"
#include "rtc_base/thread.h"
#include "rtc_base/logging.h"
#include "rtc_base/physical_socket_server.h"

typedef std::map<int, std::string> Peers;

namespace Ui {
    class VideoChat;
}

class VideoChat : public QDialog, public sigslot::has_slots<>
{
    Q_OBJECT
public:
    explicit VideoChat(QWidget *parent = nullptr);
    ~VideoChat();
private:
    Ui::VideoChat *ui;

    int my_id;
    QTcpSocket *control_socket;
    QTcpSocket *data_socket;
    QString onconnect_data;
    QString control_data;
    QString username;
    Peers peers;
    bool isLoggedIn = false;

    void InitializeConnection(QString, quint16);

private Q_SLOTS:
    void on_login_btn_clicked();
    void onConnect();
    void onDisconnect();
    void onRead();
};

#endif // VIDEO_CHAT_H
