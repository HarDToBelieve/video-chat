/*
 *  Copyright (c) 2015 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

// This file is intended to provide a common interface for fuzzing functions.
// It's intended to set sane defaults, such as removing logging for further
// fuzzing efficiency.

#include "api/task_queue/global_task_queue_factory.h"
#include "rtc_base/logging.h"
#include "rtc_base/task_queue_stdlib.h"

namespace {
bool g_initialized = false;
void InitializeWebRtcFuzzDefaults() {
  if (g_initialized)
    return;

// Remove default logging to prevent huge slowdowns.
// TODO(pbos): Disable in Chromium: http://crbug.com/561667
#if !defined(WEBRTC_CHROMIUM_BUILD)
  rtc::LogMessage::LogToDebug(rtc::LS_NONE);
#endif  // !defined(WEBRTC_CHROMIUM_BUILD)

  // Chromium hijacked DefaultTaskQueueFactory with own implementation, but
  // unable to use it without base::test::ScopedTaskEnvironment. Actual used
  // task queue implementation shouldn't matter for the purpose of the fuzzers,
  // so use stdlib implementation: that one is multiplatform. This is a
  // temporary solution until bugs.webrtc.org/10284 is resolved.
  webrtc::SetGlobalTaskQueueFactory(webrtc::CreateTaskQueueStdlibFactory());

  g_initialized = true;
}
}  // namespace

namespace webrtc {
extern void FuzzOneInput(const uint8_t* data, size_t size);
}  // namespace webrtc

extern "C" int LLVMFuzzerTestOneInput(const unsigned char* data, size_t size) {
  InitializeWebRtcFuzzDefaults();
  webrtc::FuzzOneInput(data, size);
  return 0;
}
