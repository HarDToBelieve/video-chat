#ifndef CUSTOMSOCKET_H
#define CUSTOMSOCKET_H

#include <QTcpSocket>

class CustomSocket: public QTcpSocket
{
    Q_OBJECT
public:
    CustomSocket(QString host, quint16 port): host(host), port(port) {}
    ~CustomSocket();

    void Send(QString);
    QString Recv();
private:
    QString host;
    quint16 port;

private Q_SIGNAL:
    virtual void OnConnect() = 0;
    virtual void OnDisconnect() = 0;
    virtual void OnRead() = 0;
};

#endif // CUSTOMSOCKET_H
