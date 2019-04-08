#ifndef CAPTURETRACKSOURCE_H
#define CAPTURETRACKSOURCE_H

#include "test/vcm_capturer.h"
#include "pc/video_track_source.h"
#include "modules/video_capture/video_capture.h"
#include "modules/video_capture/video_capture_factory.h"

class CaptureTrackSource : public webrtc::VideoTrackSource
{
public:
    static rtc::scoped_refptr<CaptureTrackSource> Create() {
        const size_t kWidth = 640;
        const size_t kHeight = 480;
        const size_t kFps = 30;
        std::unique_ptr<webrtc::test::VcmCapturer> capturer;
        std::unique_ptr<webrtc::VideoCaptureModule::DeviceInfo> info(
            webrtc::VideoCaptureFactory::CreateDeviceInfo());
        if (!info) {
          return nullptr;
        }
        size_t num_devices = info->NumberOfDevices();
        for (size_t i = 0; i < num_devices; ++i) {
          capturer = absl::WrapUnique(
              webrtc::test::VcmCapturer::Create(kWidth, kHeight, kFps, i));
          if (capturer) {
            return new
                rtc::RefCountedObject<CaptureTrackSource>(std::move(capturer));
          }
        }

        return nullptr;
    }

    void SetState(SourceState new_state) override {}

protected:
  explicit CaptureTrackSource(std::unique_ptr<webrtc::test::VcmCapturer> capturer):
        VideoTrackSource(false), capturer_(std::move(capturer)) {}

private:
  rtc::VideoSourceInterface<webrtc::VideoFrame>* source() override {
    return capturer_.get();
  }
  std::unique_ptr<webrtc::test::VcmCapturer> capturer_;
};

#endif // CAPTURETRACKSOURCE_H
