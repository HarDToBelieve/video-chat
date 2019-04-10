#include "video_chat.h"

VideoChat::VideoChat(QWidget *parent) : QDialog(parent), ui(new Ui::VideoChat)
{
    ui->setupUi(this);
}

VideoChat::~VideoChat()
{
    delete ui;
}

rtc::AsyncSocket* VideoChat::CreateClientSocket(int family)
{
#ifdef WIN32
  rtc::Win32Socket* sock = new rtc::Win32Socket();
  sock->CreateT(family, SOCK_STREAM);
  return sock;
#elif defined(WEBRTC_POSIX)
  rtc::Thread* thread = rtc::Thread::Current();
  RTC_DCHECK(thread != NULL);
  auto tmp = thread->socketserver();
  return tmp->CreateAsyncSocket(family, SOCK_STREAM);
#else
#error Platform not supported.
#endif
}

bool VideoChat::InitSocketSignals()
{
    RTC_DCHECK(control_socket.get() != NULL);
    control_socket->SignalCloseEvent.connect(this, &VideoChat::OnClose);
    control_socket->SignalConnectEvent.connect(this, &VideoChat::OnConnect);
    control_socket->SignalReadEvent.connect(this, &VideoChat::OnRead);
    return true;
}

bool VideoChat::GetHeaderValue(const std::string& data, size_t eoh, const char* header_pattern, size_t* value)
{
    RTC_DCHECK(value != NULL);
    size_t found = data.find(header_pattern);
    if (found != std::string::npos && found < eoh) {
        *value = atoi(&data[found + strlen(header_pattern)]);
        return true;
    }
    return false;
}

bool VideoChat::GetHeaderValue(const std::string& data, size_t eoh, const char* header_pattern, std::string* value) {
    RTC_DCHECK(value != NULL);
    size_t found = data.find(header_pattern);
    if (found != std::string::npos && found < eoh) {
        size_t begin = found + strlen(header_pattern);
        size_t end = data.find("\r\n", begin);
        if (end == std::string::npos)
            end = eoh;
        value->assign(data.substr(begin, end - begin));
        return true;
    }
    return false;
}

bool VideoChat::ReadIntoBuffer(rtc::AsyncSocket* socket, std::string* data, size_t* content_length)
{
    char buffer[0xffff];
    do {
        size_t bytes = socket->Recv(buffer, sizeof(buffer), nullptr);
        if (bytes <= 0)
            break;
        data->append(buffer, bytes);
    } while (true);

    bool ret = false;
    size_t i = data->find("\r\n\r\n");
    if (i != std::string::npos) {
        std::cerr << "Headers received" << std::endl;
        if (GetHeaderValue(*data, i, "\r\nContent-Length: ", content_length)) {
            size_t total_response_size = (i + 4) + *content_length;
            if (data->length() >= total_response_size) {
                ret = true;
                std::string should_close;
                const char kConnection[] = "\r\nConnection: ";
                if (GetHeaderValue(*data, i, kConnection, &should_close) && should_close.compare("close") == 0) {
                    socket->Close();
                    OnClose(socket, 0);
                }
            }
        } else {
            RTC_LOG(LS_ERROR) << "No content length field specified by the server.";
        }
    }
    return ret;
}

void VideoChat::DoConnect()
{
    auto tmp = server_address.ipaddr().family();
    control_socket.reset(CreateClientSocket(tmp));
    if (!InitSocketSignals()) return;
    char buffer[1024];
    snprintf(buffer, sizeof(buffer), "GET /sign_in?%s HTTP/1.0\r\n\r\n", this->username.c_str());
    onconnect_data = buffer;

    RTC_DCHECK(control_socket->GetState() == rtc::Socket::CS_CLOSED);
    int err = control_socket->Connect(server_address);
    if (err == -1) {
        RTC_LOG(LS_ERROR) << "Cannot connect to server!";
    }
    isLoggedIn = true;
}

void VideoChat::OnResolveResult(rtc::AsyncResolverInterface *resolver)
{
    if (resolver->GetError() != 0) {
        RTC_LOG(LS_ERROR) << "Failed to resolve ip";
    } else {
        server_address = resolver->address();
        DoConnect();
    }
}

void VideoChat::OnClose(rtc::AsyncSocket *socket, int err)
{
    socket->Close();
    onconnect_data.clear();
    my_id = -1;
    if (resolver != NULL) {
        resolver->Destroy(false);
        resolver = NULL;
    }
    RTC_LOG(INFO) << "Socket closed with status " << err;
}

void VideoChat::OnConnect(rtc::AsyncSocket *socket)
{
    RTC_DCHECK(!onconnect_data.empty());
    size_t sent = socket->Send(onconnect_data.c_str(), onconnect_data.length());
    RTC_DCHECK(sent == onconnect_data.length());
    onconnect_data.clear();
}

int VideoChat::GetResponseStatus(const std::string& response)
{
    int status = -1;
    size_t pos = response.find(' ');
    if (pos != std::string::npos)
        status = atoi(&response[pos + 1]);
    return status;
}

bool VideoChat::ParseServerResponse(const std::string& response, size_t content_length, size_t* peer_id, size_t* eoh)
{
    int status = GetResponseStatus(response.c_str());
    if (status != 200) {
        RTC_LOG(LS_ERROR) << "Received error from server";
        control_socket->Close();
        onconnect_data.clear();
        my_id = -1;
        return false;
    }

    *eoh = response.find("\r\n\r\n");
    RTC_DCHECK(*eoh != std::string::npos);
    if (*eoh == std::string::npos)
        return false;

    *peer_id = -1;
    GetHeaderValue(response, *eoh, "\r\nPragma: ", peer_id);

    return true;
}

bool VideoChat::ParseEntry(const std::string& entry, std::string* name, int* id, bool* connected)
{
    RTC_DCHECK(name != NULL);
    RTC_DCHECK(id != NULL);
    RTC_DCHECK(connected != NULL);
    RTC_DCHECK(!entry.empty());

    *connected = false;
    size_t separator = entry.find(',');
    if (separator != std::string::npos) {
        *id = atoi(&entry[separator + 1]);
        name->assign(entry.substr(0, separator));
        separator = entry.find(',', separator + 1);
        if (separator != std::string::npos) {
            *connected = atoi(&entry[separator + 1]) ? true : false;
        }
    }
    return !name->empty();
}

void VideoChat::OnRead(rtc::AsyncSocket *socket)
{
    size_t content_length = 0;
    if (ReadIntoBuffer(socket, &control_data, &content_length)) {
        size_t peer_id = 0, eoh = 0;
        bool ok = ParseServerResponse(control_data, content_length, &peer_id, &eoh);
        if (ok) {
            if (my_id == -1) {
                my_id = static_cast<int>(peer_id);
                RTC_DCHECK(my_id != -1);

                if (content_length) {
                    size_t pos = eoh + 4;
                    while (pos < control_data.size()) {
                        size_t eol = control_data.find('\n', pos);
                        if (eol == std::string::npos)
                            break;
                        int id = 0;
                        std::string name;
                        bool connected;
                        if (ParseEntry(control_data.substr(pos, eol - pos), &name, &id, &connected) && id != my_id) {
                            std::cerr << "Logged in with id: " << id << " and name: " << name << std::endl;
                            peers[id] = name;
//                            callback_->OnPeerConnected(id, name);
                        }
                        pos = eol + 1;
                    }
                }
                RTC_DCHECK(my_id != -1);
//                callback_->OnSignedIn();
            }
        }

        control_data.clear();
    }
}

void VideoChat::on_pushButton_clicked()
{
    if (isLoggedIn && control_socket.get() != nullptr) {
        std::cerr << "Already logged in" << std::endl;
        return;
    }
    std::string host = ui->lineEdit_host->text().toStdString();
    int port = ui->lineEdit_port->text().toInt();
    this->username = ui->lineEdit_username->text().toStdString();

    if (!host.empty() && !this->username.empty() && port != 0) {
        server_address.SetIP(host);
        server_address.SetPort(port);

        if (server_address.IsUnresolvedIP()) {
            resolver = new rtc::AsyncResolver();
            resolver->SignalDone.connect(this, &VideoChat::OnResolveResult);
            resolver->Start(server_address);
        } else {
            DoConnect();
        }
    }
    while (!isLoggedIn) {
        std::cerr << "Waiting for logging in" << std::endl;
        rtc::Thread* thread = rtc::Thread::Current();
        RTC_CHECK(thread != NULL);
        thread->SleepMs(100);
    }
}
