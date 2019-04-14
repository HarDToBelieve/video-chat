#include "backend.h"

Backend::Backend()
{
    m_pcfIface = nullptr;
    m_peerConnection = nullptr;
    sdpData.clear();
    iceCandidateData.clear();
}

void Backend::initLocalInfo()
{
    m_pcfIface = webrtc::CreatePeerConnectionFactory(
                nullptr, nullptr, nullptr, nullptr,
                webrtc::CreateBuiltinAudioEncoderFactory(),
                webrtc::CreateBuiltinAudioDecoderFactory(),
                webrtc::CreateBuiltinVideoEncoderFactory(),
                webrtc::CreateBuiltinVideoDecoderFactory(),
                nullptr, nullptr);

    if (!m_pcfIface.get()) {
        qDebug() << "Failed to create peer connection factory";
        return;
    }

    webrtc::PeerConnectionInterface::IceServer server;
    webrtc::PeerConnectionInterface::RTCConfiguration config;

    config.sdp_semantics = webrtc::SdpSemantics::kPlanB;
    config.enable_dtls_srtp = true;
    server.uri = "stun:stun.l.google.com:19302";
    config.servers.push_back(server);

    pTestConnObserver = new MyConnectionObserver();
    m_peerConnection = m_pcfIface->CreatePeerConnection(config, nullptr, nullptr, pTestConnObserver);

    if (!m_peerConnection->GetSenders().empty())
        return;

    rtc::scoped_refptr<webrtc::AudioTrackInterface> audio_track(m_pcfIface->CreateAudioTrack("audio_label", m_pcfIface->CreateAudioSource(cricket::AudioOptions())));

    rtc::scoped_refptr<CaptureTrackSource> video_device = CaptureTrackSource::Create();
    if (video_device) {
        rtc::scoped_refptr<webrtc::VideoTrackInterface> video_track(m_pcfIface->CreateVideoTrack("video_label", video_device));
        rtc::scoped_refptr<webrtc::MediaStreamInterface> stream;
        stream = m_pcfIface->CreateLocalMediaStream("myStream");
        stream->AddTrack(audio_track);
        stream->AddTrack(video_track);

        if (!m_peerConnection->AddStream(stream)) {
            qDebug() << "Adding stream to PeerConnection failed";
            return;
        }

        qDebug() << "Successfully added stream";

        MySDObserver *pMySD = new MySDObserver();
        QObject::connect(pMySD, SIGNAL(signalSDPText(const QString &)), this, SLOT(OnLocalSDPInfo(const QString &)));
        m_peerConnection->CreateOffer(pMySD, webrtc::PeerConnectionInterface::RTCOfferAnswerOptions());
    } else {
        qDebug() << "OpenVideoCaptureDevice failed";
    }
}

void Backend::onLocalSDPInfo(const QString &sdp)
{
    sdpData = sdp;
    qDebug() << "Local SDP data:\n" << sdpData;
    std::string sdpStr = sdp.toStdString();
    std::unique_ptr<webrtc::SessionDescriptionInterface> pSessionDescription = webrtc::CreateSessionDescription(webrtc::SdpType::kOffer, sdpStr);
    m_peerConnection->SetLocalDescription(DummySetSDObserver::Create(), pSessionDescription.release());
}

void Backend::onLocalIceCandidate(const QString &iceCandidate)
{
    iceCandidateData += iceCandidate + "\n";
    qDebug() << "Local Ice data:\n" << iceCandidateData;
}

