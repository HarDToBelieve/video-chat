#ifndef MYCONNECTIONOBSERVER_H
#define MYCONNECTIONOBSERVER_H

#include "api/peer_connection_interface.h"
#include "api/create_peerconnection_factory.h"
#include "api/media_stream_interface.h"
#include "api/audio_codecs/builtin_audio_decoder_factory.h"
#include "api/audio_codecs/builtin_audio_encoder_factory.h"
#include "api/video_codecs/builtin_video_decoder_factory.h"
#include "api/video_codecs/builtin_video_encoder_factory.h"

#include <QMainWindow>
#include <iostream>

#define slots Q_SLOTS
#define signals Q_SIGNALS
#define emit Q_EMIT

#define BUFLEN 1024

class MyConnectionObserver : public QObject, public webrtc::PeerConnectionObserver
{
    Q_OBJECT
public:
    MyConnectionObserver();
    ~MyConnectionObserver();

    void OnIceGatheringChange(webrtc::PeerConnectionInterface::IceGatheringState new_state);
    void OnDataChannel(rtc::scoped_refptr<webrtc::DataChannelInterface> data_channel);
    void OnSignalingChange(webrtc::PeerConnectionInterface::SignalingState new_state);
    void OnConnectionChange(webrtc::PeerConnectionInterface::PeerConnectionState new_state);
    void OnAddStream(rtc::scoped_refptr<webrtc::MediaStreamInterface> stream);
    void OnRemoveStream(rtc::scoped_refptr<webrtc::MediaStreamInterface> stream);
    void OnRenegotiationNeeded();
    void OnIceConnectionChange(webrtc::PeerConnectionInterface::IceConnectionState new_state);
    void OnIceCandidate(const webrtc::IceCandidateInterface* candidate);
signals:
    void signalIceCandidate(const QString &iceCandidate);
};

#endif // MYCONNECTIONOBSERVER_H
