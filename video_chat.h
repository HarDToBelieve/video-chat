#ifndef VIDEO_CHAT_H
#define VIDEO_CHAT_H

#include <QDialog>
#include <QObject>

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
    std::unique_ptr<rtc::AsyncSocket> control_socket;
    rtc::SocketAddress server_address;
    std::string onconnect_data;
    std::string control_data;
    std::string username;
    Peers peers;
    rtc::AsyncResolver* resolver;
    bool isLoggedIn = false;

    void DoConnect();

    rtc::AsyncSocket* CreateClientSocket(int family);
    bool InitSocketSignals();
    bool ReadIntoBuffer(rtc::AsyncSocket *socket, std::string* data, size_t* content_lengt);
    bool GetHeaderValue(const std::string &data, size_t eoh, const char *header_pattern, size_t* value);
    bool GetHeaderValue(const std::string &data, size_t eoh, const char *header_pattern, std::string* value);
    bool ParseServerResponse(const std::string& response, size_t content_length, size_t* peer_id, size_t* eoh);
    int GetResponseStatus(const std::string& response);
    bool ParseEntry(const std::string& entry, std::string* name, int *id, bool* connected);

    void OnClose(rtc::AsyncSocket *socket, int err);
    void OnRead(rtc::AsyncSocket *socket);
    void OnConnect(rtc::AsyncSocket *socket);
    void OnResolveResult(rtc::AsyncResolverInterface* resolver);

Q_SIGNALS:

private Q_SLOTS:
    void on_pushButton_clicked();
};

#endif // VIDEO_CHAT_H
