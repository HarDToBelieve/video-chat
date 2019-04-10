#include "mysdobserver.h"

MySDObserver::MySDObserver()
{
    std::cerr << "TestSDObserver::TestSDObserver" << std::endl;
}

MySDObserver::~MySDObserver()
{
    std::cerr << "TestSDObserver::~TestSDObserver" << std::endl;
}

void MySDObserver::OnSuccess(webrtc::SessionDescriptionInterface* desc)
{
    std::cerr << "TestSDObserver::OnSuccess" << std::endl;
    std::string sdpStr;

    desc->ToString(&sdpStr);
    std::cerr << sdpStr << std::endl;

    Q_EMIT signalSDPText(QString(sdpStr.c_str()));
}

void MySDObserver::OnFailure(const std::string& error)
{
    std::cerr << "TestSDObserver: OnFailure (due to " << error << ")" << std::endl;
}
