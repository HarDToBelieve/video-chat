#include "backend.h"
#include "video_chat.h"

Backend::Backend(QObject *vc): vc(vc)
{
    this->vc = vc;
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

        auto result_or_error = m_peerConnection->AddTrack(audio_track, {"stream_id"});
        if (!result_or_error.ok()) {
            qDebug() << "Failed to add video track to PeerConnection: "<< result_or_error.error().message();
        }
        local_renderer_.reset(new VideoRenderer(vc, video_track));
    } else {
        qDebug() << "OpenVideoCaptureDevice failed";
    }

}

VideoRenderer *Backend::getLocalRenderer()
{
    return local_renderer_.get();
}

VideoRenderer *Backend::getRemoteRenderer()
{
    return remote_renderer_.get();
}

//uint8_t *Backend::getDrawBuffer()
//{
//    return draw_buffer_.get();
//}

//int Backend::getDrawSize()
//{
//    return draw_buffer_size_;
//}

int Backend::getWidth()
{
    return width_;
}

int Backend::getHeight()
{
    return height_;
}
