#ifndef MYWIDGET_H

#define MYWIDGET_H

#include "api/peer_connection_interface.h"
#include "api/create_peerconnection_factory.h"
#include "api/media_stream_interface.h"
#include "api/audio_codecs/builtin_audio_decoder_factory.h"
#include "api/audio_codecs/builtin_audio_encoder_factory.h"
#include "api/video_codecs/builtin_video_decoder_factory.h"
#include "api/video_codecs/builtin_video_encoder_factory.h"

#include "ui_mywidget.h"
#include "capturetracksource.h"
#include "mysdobserver.h"
#include "myconnectionobserver.h"
#include "dummysetsdobserver.h"

#include <QMainWindow>
#include <iostream>

#define BUFLEN 1024

namespace Ui {
    class MyWidget;
}
class MyVideoCapturer;

class MyWidget : public QMainWindow
{
    Q_OBJECT
public:
    explicit MyWidget(QWidget *parent = nullptr);
    ~MyWidget();

//    void setNewFrame(const QImage &img);

private Q_SLOTS:
    void OnStartClicked();
    void OnAnswerClicked();
    void OnRemoteICEClicked();

private:
    Q_SLOT void OnLocalSDPInfo(const QString &);
    Q_SLOT void OnLocalIceCandidate(const QString &);

private:
    cricket::VideoCapturer* OpenVideoCaptureDevice();
    MyVideoCapturer *m_pCapturer;
//    MyConnectionObserver *pTestConnObserver;
//    MySDObserver *pMySD;

    rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> m_pcfIface;
    rtc::scoped_refptr<webrtc::PeerConnectionInterface> m_peerConnection;

    std::unique_ptr<rtc::Thread> network_thread;
    std::unique_ptr<rtc::Thread> worker_thread;
    std::unique_ptr<rtc::Thread> signaling_thread;

    Ui::MyWidget *ui;
};

#endif // MYWIDGET_H
