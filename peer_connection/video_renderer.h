#ifndef VIDEO_RENDERER_H
#define VIDEO_RENDERER_H

#include "api/video/video_sink_interface.h"
#include "api/video/video_frame.h"
#include "api/media_stream_interface.h"
#include "api/video/i420_buffer.h"

#include <QGraphicsView>
#include <QMutex>

#include "yuv/libyuv.h"

class VideoRenderer: public rtc::VideoSinkInterface<webrtc::VideoFrame>
{
public:
    VideoRenderer(QObject *vc, webrtc::VideoTrackInterface* track_to_render);
    virtual ~VideoRenderer() override;

    void OnFrame(const webrtc::VideoFrame& frame) override;
    const uint8_t* image() const { return image_.get(); }
    int width() const { return width_; }
    int height() const { return height_; }

private:
    void setSize(int width, int height);
    std::unique_ptr<uint8_t[]> image_;
    int width_;
    int height_;
    QObject *vc;
    QMutex mutex_setSize;
    QMutex mutex_onFrame;
    rtc::scoped_refptr<webrtc::VideoTrackInterface> rendered_track_;
};

#endif // VIDEO_RENDERER_H
