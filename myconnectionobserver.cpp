#include "myconnectionobserver.h"

MyConnectionObserver::MyConnectionObserver()
{
    std::cerr << "TestConnectionObserver::TestConnectionObserver" << std::endl;
}

MyConnectionObserver::~MyConnectionObserver()
{
    std::cerr << "TestConnectionObserver::~TestConnectionObserver" << std::endl;
}

void MyConnectionObserver::OnConnectionChange(webrtc::PeerConnectionInterface::PeerConnectionState new_state)
{
    std::cerr << "TestConnectionObserver::OnConnectionChange" << std::endl;
}

void MyConnectionObserver::OnAddStream(rtc::scoped_refptr<webrtc::MediaStreamInterface> stream)
{
    std::cerr << "TestConnectionObserver::OnAddStream" << std::endl;
}

void MyConnectionObserver::OnRemoveStream(rtc::scoped_refptr<webrtc::MediaStreamInterface> stream)
{
    std::cerr << "TestConnectionObserver::OnRemoveStream" << std::endl;
}

void MyConnectionObserver::OnRenegotiationNeeded()
{
    std::cerr << "TestConnectionObserver::OnRenegotiationNeeded" << std::endl;
}

void MyConnectionObserver::OnIceConnectionChange(webrtc::PeerConnectionInterface::IceConnectionState new_state)
{
    std::cerr << "TestConnectionObserver::OnIceChange" << std::endl;
}

void MyConnectionObserver::OnIceCandidate(const webrtc::IceCandidateInterface* candidate)
{
    std::cerr << "TestConnectionObserver::OnIceCandidate" << std::endl;
    std::string iceStr;

    int mlineIdx = candidate->sdp_mline_index();
    std::string sdpMid = candidate->sdp_mid();
    candidate->ToString(&iceStr);

    size_t len = iceStr.length();
    if (len > 0 && iceStr[len-1] == '\n')
        iceStr = iceStr.substr(0, len-1);

    len = iceStr.length();
    if (len > 0 && iceStr[len-1] == '\r')
        iceStr = iceStr.substr(0, len-1);

    char buf[BUFLEN];

    snprintf(buf, BUFLEN, "{\"sdpMLineIndex\":%d,\"sdpMid\":\"%s\",\"candidate\":\"%s\\r\\n\"}", mlineIdx, sdpMid.c_str(), iceStr.c_str());

    emit signalIceCandidate(QString(buf));
}

void MyConnectionObserver::OnIceGatheringChange(webrtc::PeerConnectionInterface::IceGatheringState new_state)
{
    std::cerr << "MyConnectionObserver::OnIceGatheringChange" << std::endl;
}

void MyConnectionObserver::OnDataChannel(rtc::scoped_refptr<webrtc::DataChannelInterface> data_channel)
{
    std::cerr << "MyConnectionObserver::OnDataChannel" << std::endl;
}

void MyConnectionObserver::OnSignalingChange(webrtc::PeerConnectionInterface::SignalingState new_state)
{
    std::cerr << "MyConnectionObserver::OnSignalingChange" << std::endl;
}

void MyConnectionObserver::signalIceCandidate(const QString &iceCandidate) {}
