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

#include "video_renderer.h"

#include <QObject>
#include <QDebug>

class Backend: public QObject
{
    Q_OBJECT
public:
    explicit Backend(QObject *vc);
    ~Backend();

    void initLocalInfo();
    VideoRenderer *getLocalRenderer();
    VideoRenderer *getRemoteRenderer();
    uint8_t *getDrawBuffer();
    int getDrawSize();
    int getWidth();
    int getHeight();

private:
    QString sdpData;
    QString iceCandidateData;

    rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> m_pcfIface;
    rtc::scoped_refptr<webrtc::PeerConnectionInterface> m_peerConnection;
    MyConnectionObserver *pTestConnObserver;
    std::unique_ptr<VideoRenderer> local_renderer_;
    std::unique_ptr<VideoRenderer> remote_renderer_;

    QObject *vc;
    int width_;
    int height_;

    std::unique_ptr<rtc::Thread> network_thread;
    std::unique_ptr<rtc::Thread> worker_thread;
    std::unique_ptr<rtc::Thread> signaling_thread;

};

#endif // BACKEND_H
