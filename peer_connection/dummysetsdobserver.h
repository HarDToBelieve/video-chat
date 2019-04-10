#ifndef DUMMYSETSDOBSERVER_H
#define DUMMYSETSDOBSERVER_H

#include "api/peer_connection_interface.h"
#include "api/create_peerconnection_factory.h"
#include "api/media_stream_interface.h"
#include "api/audio_codecs/builtin_audio_decoder_factory.h"
#include "api/audio_codecs/builtin_audio_encoder_factory.h"
#include "api/video_codecs/builtin_video_decoder_factory.h"
#include "api/video_codecs/builtin_video_encoder_factory.h"

#include "capturetracksource.h"

#include <QMainWindow>
#include <iostream>

#define BUFLEN 1024

class DummySetSDObserver: public webrtc::SetSessionDescriptionObserver
{
public:
    static DummySetSDObserver* Create()
    {
        return new rtc::RefCountedObject<DummySetSDObserver>();
    }

    void OnSuccess();
    void OnFailure(const std::string& error);
protected:
    DummySetSDObserver() {}
    ~DummySetSDObserver() {}
};

#endif // DUMMYSETSDOBSERVER_H
