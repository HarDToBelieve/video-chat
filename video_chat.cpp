#include "video_chat.h"

VideoChat::VideoChat(QWidget *parent) : QDialog(parent), ui(new Ui::VideoChat)
{
    control_socket = nullptr;
    data_socket = nullptr;

    be = new Backend(this);

    model = new QStringListModel(this);

    ui->setupUi(this);
    ui->listView_peers->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->listView_peers->setModel(model);
}

VideoChat::~VideoChat()
{
    qDebug() << "CLOSING CONTROL: " << control_socket->readAll() << "\n\n";
    qDebug() << "CLOSING DATA: " << data_socket->readAll() << "\n\n";
    isSigningOut = true;

    if (control_socket != nullptr && control_socket->state() == QTcpSocket::ConnectedState) {
        control_socket->close();
        control_socket->connectToHost(host, port);
        QString tmp = QString("GET /sign_out?peer_id=%1 HTTP/1.0\r\n\r\n").arg(my_id);
        control_socket->write(tmp.toLocal8Bit().data(), tmp.size());
        control_socket->waitForBytesWritten();
        control_socket->close();
    }
    if (data_socket != nullptr && data_socket->state() == QTcpSocket::ConnectedState)
        data_socket->close();

    delete be;
    delete ui;
}

void VideoChat::InitializeConnection()
{
    if (control_socket != nullptr) delete control_socket;
    QStringList tmp;
    model->setStringList(tmp);

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
        be->initLocalInfo();
    }
}

void VideoChat::onControlConnect()
{
    if (isSigningOut) return;
    QString tmp = QString("GET /sign_in?%1 HTTP/1.0\r\n\r\n").arg(username);
    control_socket->write(tmp.toLocal8Bit().data(), tmp.size());
    control_socket->waitForBytesWritten();

    isLoggedIn = true;
}

void VideoChat::onControlRead()
{
    QString tmp_recv = control_socket->readAll();
    qDebug() << "CONTROL: " << tmp_recv << "\n\n";
    QStringList body = tmp_recv.split(QRegExp("\r\n\r\n"), QString::SkipEmptyParts);

    QRegExp rx = QRegExp("Pragma: (\\d+)");
    rx.indexIn(body[0]);
    my_id = rx.capturedTexts()[1].toInt();

    QStringList list_peers = body[1].split(QRegExp("\n"), QString::SkipEmptyParts);
    QStringList list_name;

    Q_FOREACH(QString p, list_peers) {
        QStringList tmp = p.split(QRegExp(","), QString::SkipEmptyParts);
        QString name = tmp[0];
        int status = tmp[2].toInt();
        if (name.compare(username) && status)
            list_name.append(name);
    }

    model->setStringList(list_name);

    if (data_socket == nullptr) {
        data_socket = new QTcpSocket(this);

        QObject::connect(data_socket, SIGNAL(connected()), this, SLOT(onDataConnect()));
        QObject::connect(data_socket, SIGNAL(disconnected()), this, SLOT(onDataDisconnect()));
        QObject::connect(data_socket, SIGNAL(readyRead()), this, SLOT(onDataRead()));
    }
    if (data_socket->state() == QTcpSocket::UnconnectedState) {
        data_socket->connectToHost(host, port);
        if (!data_socket->waitForConnected()) {
            qDebug() << "Error: data_socket: " << data_socket->errorString();
            Close();
        }
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
    QString tmp_recv = data_socket->readAll();
    qDebug() << "DATA: " << tmp_recv << "\n\n";

    QStringList body = tmp_recv.split(QRegExp("\r\n\r\n"), QString::SkipEmptyParts);
    if (!body[1].startsWith("{")) {
        QStringList list_peers = body[1].split(QRegExp("\n"), QString::SkipEmptyParts);
        QStringList list_name;

        Q_FOREACH(QString p, list_peers) {
            QStringList tmp = p.split(QRegExp(","), QString::SkipEmptyParts);
            QString name = tmp[0];
            int id = tmp[1].toInt();
            int status = tmp[2].toInt();
            if (name.compare(username) && status) {
                peers[name] = id;
                list_name.append(name);
            }
        }

        model->setStringList(list_name);
    } else {
        if (isCalling) {
            qDebug() << "User is calling";
            return;
        }
        QJsonDocument jsonResponse = QJsonDocument::fromJson(body[1].toUtf8());
        QJsonObject jsonObject = jsonResponse.object();
        QString type = jsonObject["type"].toString();
        if (!type.isEmpty()) {
        }
    }
    data_socket->close();
    data_socket->connectToHost(host, port);
    if (!data_socket->waitForConnected()) {
        qDebug() << "Error: data_socket: " << data_socket->errorString();
        Close();
    }
}

void VideoChat::onControlDisconnect()
{
    qDebug() << "Control socket disconneceted\n\n";
}

void VideoChat::onDataDisconnect()
{
    qDebug() << "Data socket disconneceted\n\n";
}

void VideoChat::Close()
{

}

void VideoChat::on_listView_peers_doubleClicked(const QModelIndex &index)
{
    qDebug() << index.data().toString();
    int peer_id = peers[index.data().toString()];

    isCalling = true;

    QString headers = QString("POST /message?peer_id=%1&to=%2 HTTP/1.0\r\n"
                      "Content-Length: %3\r\n"
                      "Content-Type: text/plain\r\n"
                      "\r\n").arg(my_id, peer_id);
}

void VideoChat::StreamVideo()
{
    VideoRenderer *local_render = be->getLocalRenderer();
    if (local_render && local_render->image()) {
        const uint32_t* image = reinterpret_cast<const uint32_t*>(local_render->image());
    }
}
