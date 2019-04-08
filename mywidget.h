#ifndef MYWIDGET_H

#define MYWIDGET_H

#include "api/peer_connection_interface.h"
#include "api/create_peerconnection_factory.h"
#include "api/media_stream_interface.h"
#include "api/audio_codecs/builtin_audio_decoder_factory.h"
#include "api/audio_codecs/builtin_audio_encoder_factory.h"
#include "api/video_codecs/builtin_video_decoder_factory.h"
#include "api/video_codecs/builtin_video_encoder_factory.h"

#include "ui_mainwindow.h"
#include "capturetracksource.h"
#include "mysdobserver.h"
#include "myconnectionobserver.h"
#include "dummysetsdobserver.h"

#include <QMainWindow>
#include <iostream>

#define slots Q_SLOTS
#define signals Q_SIGNALS
#define emit Q_EMIT

#define BUFLEN 1024

class MyVideoCapturer;

class MyWidget : public QMainWindow, public Ui::MainWindow
{
    Q_OBJECT
public:
    MyWidget();
    ~MyWidget();

    void setNewFrame(const QImage &img);
private slots:
    void OnStartClicked();
    void OnAnswerClicked();
    void OnRemoteICEClicked();
    void OnLocalSDPInfo(const QString &sdpText);
    void OnLocalIceCandidate(const QString &iceCandidate);
private:
    cricket::VideoCapturer* OpenVideoCaptureDevice();
    MyVideoCapturer *m_pCapturer;

    rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> m_pcfIface;
    rtc::scoped_refptr<webrtc::PeerConnectionInterface> m_peerConnection;

    std::unique_ptr<rtc::Thread> network_thread;
    std::unique_ptr<rtc::Thread> worker_thread;
    std::unique_ptr<rtc::Thread> signaling_thread;

};

#endif // MYWIDGET_H
