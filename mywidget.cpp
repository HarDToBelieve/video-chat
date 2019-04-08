#include "mywidget.h"

MyWidget::MyWidget()
{
    m_pCapturer = nullptr;
    m_pcfIface = nullptr;

    network_thread = rtc::Thread::CreateWithSocketServer();
    network_thread->Start();
    worker_thread = rtc::Thread::Create();
    worker_thread->Start();
    signaling_thread = rtc::Thread::Create();
    signaling_thread->Start();

    setupUi(this);

    QObject::connect(m_pStartButton, SIGNAL(clicked()), this, SLOT(onStartClicked()));
    QObject::connect(m_pProcessAnswerButton, SIGNAL(clicked()), this, SLOT(onAnswerClicked()));
    QObject::connect(m_pProcessRemoteICEButton, SIGNAL(clicked()), this, SLOT(onRemoteICEClicked()));
}

MyWidget::~MyWidget()
{
}

void MyWidget::OnStartClicked()
{
    if (m_pcfIface != nullptr)
    {
        std::cerr << "onStartClicked: already exists" << std::endl;
        return;
    }

    m_pcfIface = webrtc::CreatePeerConnectionFactory(
                network_thread.get(),
                worker_thread.get(),
                signaling_thread.get(),
                nullptr,
                webrtc::CreateBuiltinAudioEncoderFactory(),
                webrtc::CreateBuiltinAudioDecoderFactory(),
                webrtc::CreateBuiltinVideoEncoderFactory(),
                webrtc::CreateBuiltinVideoDecoderFactory(),
                nullptr,
                nullptr);

    if (!m_pcfIface.get()) {
        m_pcfIface = nullptr;
        return;
    }

    webrtc::PeerConnectionInterface::IceServer server;
    webrtc::PeerConnectionInterface::RTCConfiguration config;

    config.sdp_semantics = webrtc::SdpSemantics::kUnifiedPlan;
    config.enable_dtls_srtp = true;
    server.uri = "stun:stun.l.google.com:19302";
    config.servers.push_back(server);

    MyConnectionObserver *pTestConnObserver = new MyConnectionObserver();

    QObject::connect(pTestConnObserver, SIGNAL(signalIceCandidate(const QString &)), this, SLOT(onLocalIceCandidate(const QString &)));

    m_peerConnection = m_pcfIface->CreatePeerConnection(config, nullptr, nullptr, pTestConnObserver);

    if (!m_peerConnection->GetSenders().empty())
        return;

    rtc::scoped_refptr<webrtc::AudioTrackInterface> audio_track(m_pcfIface->CreateAudioTrack("audio_label", m_pcfIface->CreateAudioSource(cricket::AudioOptions())));
//    auto result_or_error = m_peerConnection->AddTrack(audio_track, {"stream_id"});
//    if (!result_or_error.ok()) {
//        std::cerr << "Failed to add audio track to PeerConnection: " << result_or_error.error().message() << std::endl;
//    }

    rtc::scoped_refptr<CaptureTrackSource> video_device = CaptureTrackSource::Create();
    if (video_device) {
        rtc::scoped_refptr<webrtc::VideoTrackInterface> video_track(m_pcfIface->CreateVideoTrack("video_label", video_device));
//        result_or_error = m_peerConnection->AddTrack(video_track, {"stream_id"});
//        if (!result_or_error.ok()) {
//            std::cerr << "Failed to add video track to PeerConnection: " << result_or_error.error().message() << std::endl;
//        }
        rtc::scoped_refptr<webrtc::MediaStreamInterface> stream;
        stream = m_pcfIface->CreateLocalMediaStream("myStream");
        stream->AddTrack(audio_track);
        stream->AddTrack(video_track);

        if (!m_peerConnection->AddStream(stream)) {
            std::cerr << "Adding stream to PeerConnection failed" << std::endl;
            return;
        }

        std::cerr << "Successfully added stream" << std::endl;

        MySDObserver *pMySD = new MySDObserver();
        QObject::connect(pMySD, SIGNAL(signalSDPText(const, QString &)), this, SLOT(onLocalSDPInfo(const QString &)));
        m_peerConnection->CreateOffer(pMySD, webrtc::PeerConnectionInterface::RTCOfferAnswerOptions());
    } else {
        std::cerr << "OpenVideoCaptureDevice failed" << std::endl;
    }
}

void MyWidget::OnAnswerClicked()
{
    std::string sdpInfo = m_pAnswerText->toPlainText().toStdString();
    std::cerr << "OnAnswerClicked: " << std::endl << sdpInfo << std::endl;
    std::unique_ptr<webrtc::SessionDescriptionInterface> pSessionDescription = webrtc::CreateSessionDescription(webrtc::SdpType::kAnswer, sdpInfo);
    m_peerConnection->SetRemoteDescription(DummySetSDObserver::Create(), pSessionDescription.release());
}

void MyWidget::OnRemoteICEClicked()
{
    std::cerr << "OnRemoteICEClicked()" << std::endl;
    QStringList iceInfo = m_pRemoteICEText->toPlainText().split('\n');

    for (int i = 0; i < iceInfo.size(); ++i) {
        QString iceLine = iceInfo.at(i);
        QStringList parts = iceLine.split(',');
        QStringList remainingParts;

        for (int j = 0; j < parts.size(); ++j) {
            QString part = parts.at(j);
            QStringList values = part.split('"', QString::SkipEmptyParts);
            if (values.size() > 2) {
                remainingParts << values.at(2);
            }
        }

        if (remainingParts.size() == 3) {
            int sdpMLineIndex = atoi(remainingParts.at(0).toLatin1());
            std::string sdpMid = remainingParts.at(1).toStdString();
            QString candidate = remainingParts.at(2);

            candidate.replace(QString("\\r\\n"), QString(""));
            std::cerr << "ICE: " << sdpMLineIndex << " " << sdpMid << " " << candidate.toStdString() << std::endl;

            webrtc::SdpParseError error;
            std::unique_ptr<webrtc::IceCandidateInterface> c(webrtc::CreateIceCandidate(sdpMid, sdpMLineIndex, candidate.toStdString(), &error));

            if (!c.get()) {
                std::cerr << "Can't parse candidate message" << std::endl;
            } else {
                if (!m_peerConnection->AddIceCandidate(c.get())) {
                    std::cerr << "Failed to apply the received candidate" << std::endl;
                }
            }
        }
    }
}

void MyWidget::OnLocalSDPInfo(const QString &sdpText)
{
    m_pOfferText->setPlainText(sdpText);
    std::string sdpStr = sdpText.toStdString();
    std::unique_ptr<webrtc::SessionDescriptionInterface> pSessionDescription = webrtc::CreateSessionDescription(webrtc::SdpType::kOffer, sdpStr);
    m_peerConnection->SetLocalDescription(DummySetSDObserver::Create(), pSessionDescription.release());
}

void MyWidget::OnLocalIceCandidate(const QString &iceCandidate)
{
    QString str = m_pOwnICEText->toPlainText();
    str += iceCandidate + "\n";
    m_pOwnICEText->setPlainText(str);
}
