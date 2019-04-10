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

void VideoChat::InitializeConnection(QString host, quint16 port)
{
    if (control_socket != nullptr) delete control_socket;
    if (data_socket != nullptr) delete data_socket;

    control_socket = new QTcpSocket(this);
    data_socket = new QTcpSocket(this);

    QObject::connect(control_socket, SIGNAL(connected()), this, SLOT(onConnect()));
    QObject::connect(control_socket, SIGNAL(disconnected()), this, SLOT(onDisconnect()));
    QObject::connect(control_socket, SIGNAL(readyRead()), this, SLOT(onRead()));

    control_socket->connectToHost(host, port);
    if (!control_socket->waitForConnected()) {
        qDebug() << "Error: " << control_socket->errorString();
    }
}

void VideoChat::on_login_btn_clicked()
{
    if (control_socket != nullptr) {
        qDebug() << "Already logged in";
        return;
    }
    QString host = ui->lineEdit_host->text();
    quint16 port = ui->lineEdit_port->text().toUShort();
    username = ui->lineEdit_username->text();

    if (!host.size() && !username.size() && port != 0) {
        InitializeConnection(host, port);
    }
}

void VideoChat::onConnect()
{
    if (onconnect_data.size()) {
        qDebug() << "onconnect_data is not cleared";
        return;
    }

    qint64 sent = control_socket->write(onconnect_data.toLocal8Bit().data(), onconnect_data.size());

}
