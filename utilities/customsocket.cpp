#include "customsocket.h"

void CustomSocket::Send(QString buf)
{
    if (this->state() == QTcpSocket::UnconnectedState) {
        qDebug() << "Socket need connecting first!";
        return;
    }

    this->write(buf.toLocal8Bit().data(), buf.size());
    this->waitForBytesWritten();
}

QString CustomSocket::Recv()
{
    return this->readAll();
}
