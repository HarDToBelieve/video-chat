#ifndef MYSDOBSERVER_H
#define MYSDOBSERVER_H

#define slots Q_SLOTS
#define signals Q_SIGNALS
#define emit Q_EMIT

#define BUFLEN 1024

#include "api/peer_connection_interface.h"
#include "api/create_peerconnection_factory.h"
#include "api/media_stream_interface.h"
#include "api/audio_codecs/builtin_audio_decoder_factory.h"
#include "api/audio_codecs/builtin_audio_encoder_factory.h"
#include "api/video_codecs/builtin_video_decoder_factory.h"
#include "api/video_codecs/builtin_video_encoder_factory.h"

#include <QMainWindow>
#include <iostream>

class MySDObserver : public QObject, public rtc::RefCountedObject<webrtc::CreateSessionDescriptionObserver>
{
    Q_OBJECT
public:
    MySDObserver();
    ~MySDObserver();

    void OnSuccess(webrtc::SessionDescriptionInterface* desc);
    void OnFailure(const std::string& error);
signals:
    void signalSDPText(const QString &sdp);
};

#endif // MYSDOBSERVER_H
