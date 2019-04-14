#ifndef BACKEND_H
#define BACKEND_H

#include "api/peer_connection_interface.h"
#include "api/create_peerconnection_factory.h"
#include "api/media_stream_interface.h"
#include "api/audio_codecs/builtin_audio_decoder_factory.h"
#include "api/audio_codecs/builtin_audio_encoder_factory.h"
#include "api/video_codecs/builtin_video_decoder_factory.h"
#include "api/video_codecs/builtin_video_encoder_factory.h"

#include "capturetracksource.h"
#include "mysdobserver.h"
#include "myconnectionobserver.h"
#include "dummysetsdobserver.h"

#include <QObject>
#include <QDebug>

class Backend: public QObject
{
    Q_OBJECT
public:
    explicit Backend();
    ~Backend();

    void initLocalInfo();

private:
    QString sdpData;
    QString iceCandidateData;

    rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> m_pcfIface;
    rtc::scoped_refptr<webrtc::PeerConnectionInterface> m_peerConnection;
    MyConnectionObserver *pTestConnObserver;
};

#endif // BACKEND_H
