#include "video_chat.h"

VideoChat::VideoChat(QWidget *parent) : QDialog(parent), ui(new Ui::VideoChat)
{
    control_socket = nullptr;
    data_socket = nullptr;

    ui->setupUi(this);
}

VideoChat::~VideoChat()
{
    delete ui;
}

void VideoChat::InitializeConnection()
{
    if (control_socket != nullptr) delete control_socket;

    control_socket = new QTcpSocket(this);


    QObject::connect(control_socket, SIGNAL(connected()), this, SLOT(onControlConnect()));
    QObject::connect(control_socket, SIGNAL(disconnected()), this, SLOT(onControlDisconnect()));
    QObject::connect(control_socket, SIGNAL(readyRead()), this, SLOT(onControlRead()));

    control_socket->connectToHost(host, port);
    if (!control_socket->waitForConnected()) {
        qDebug() << "Error: control_socket: " << control_socket->errorString();
        Close();
    }
}

void VideoChat::on_login_btn_clicked()
{
    if (control_socket != nullptr) {
        qDebug() << "Already logged in";
        return;
    }
    host = ui->lineEdit_host->text();
    port = ui->lineEdit_port->text().toUShort();
    username = ui->lineEdit_username->text();

    if (host.size() && username.size() && port != 0) {
        InitializeConnection();
    }
}

void VideoChat::onControlConnect()
{
    QString tmp = QString("GET /sign_in?%1 HTTP/1.0\r\n\r\n").arg(username);
    control_socket->write(tmp.toLocal8Bit().data(), tmp.size());
    control_socket->waitForBytesWritten();
    isLoggedIn = true;
}

void VideoChat::onControlRead()
{
    if (data_socket != nullptr) delete data_socket;
    QString tmp_recv = control_socket->readAll();
    QString body = tmp_recv.split(QRegExp("\r\n"), QString::SkipEmptyParts).last().trimmed();
    my_id = body.split(QRegExp(","), QString::SkipEmptyParts)[1].toInt();

    data_socket = new QTcpSocket(this);

    QObject::connect(data_socket, SIGNAL(connected()), this, SLOT(onDataConnect()));
    QObject::connect(data_socket, SIGNAL(disconnected()), this, SLOT(onDataDisconnect()));
    QObject::connect(data_socket, SIGNAL(readyRead()), this, SLOT(onDataRead()));

    data_socket->connectToHost(host, port);
    if (!data_socket->waitForConnected()) {
        qDebug() << "Error: data_socket: " << data_socket->errorString();
        Close();
    }
}

void VideoChat::onDataConnect()
{
    QString tmp = QString("GET /wait?peer_id=%1 HTTP/1.0\r\n\r\n").arg(my_id);
    data_socket->write(tmp.toLocal8Bit().data(), tmp.size());
    data_socket->waitForBytesWritten();
}

void VideoChat::onDataRead()
{

}

void VideoChat::onControlDisconnect()
{

}
