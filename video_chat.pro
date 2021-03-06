#-------------------------------------------------
#
# Project created by QtCreator 2019-04-03T10:25:36
#
#-------------------------------------------------

QT       += core gui
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = video_chat
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

#QMAKE_CXXFLAGS_DEBUG += /MD
#QMAKE_LFLAGS += /NODEFAULTLIB:MSVCRT

CONFIG += c++11
CONFIG += no_keywords

SOURCES += \
    main.cpp \
    mywidget.cpp \
    peer_connection/mysdobserver.cpp \
    peer_connection/dummysetsdobserver.cpp \
    peer_connection/myconnectionobserver.cpp \
    peer_connection/capturetracksource.cpp \
    peer_connection/vcmcapturer.cpp \
    peer_connection/video_capturer.cpp \
#    mainwindow.cpp
    video_chat.cpp \
#    peer_connection/customsocketserver.cpp
#    utilities/customsocket.cpp
    backend.cpp \
    peer_connection/video_renderer.cpp

HEADERS += \
    peer_connection/widgetstreamer.h \
    peer_connection/capturetracksource.h \
    mywidget.h \
    peer_connection/mysdobserver.h \
    peer_connection/dummysetsdobserver.h \
    peer_connection/myconnectionobserver.h \
    peer_connection/vcmcapturer.h \
    peer_connection/video_capturer.h \
#    mainwindow.h
    video_chat.h \
#    peer_connection/customsocketserver.h
#    utilities/customsocket.h
    backend.h \
    peer_connection/video_renderer.h

FORMS += \
    mywidget.ui \
    main.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

unix: LIBS += -L$$PWD/lib/unix/ -labsl_base -lssl -lwebrtc -lyuv -lstdc++ -ldl -lX11 -lpthread

INCLUDEPATH += $$PWD/./webrtc/
INCLUDEPATH += $$PWD/./absl/
INCLUDEPATH += $$PWD/./openssl/
INCLUDEPATH += $$PWD/./peer_connection/
INCLUDEPATH += $$PWD/./yuv/

unix: DEFINES += USE_UDEV
unix: DEFINES += USE_AURA=1
unix: DEFINES += USE_GLIB=1
unix: DEFINES += USE_NSS_CERTS=1
unix: DEFINES += USE_X11=1
unix: DEFINES += FULL_SAFE_BROWSING
unix: DEFINES += SAFE_BROWSING_CSD
unix: DEFINES += SAFE_BROWSING_DB_LOCAL
unix: DEFINES += CHROMIUM_BUILD
unix: DEFINES += _FILE_OFFSET_BITS=64
unix: DEFINES += _LARGEFILE_SOURCE
unix: DEFINES += _LARGEFILE64_SOURCE
unix: DEFINES += _GNU_SOURCE
unix: DEFINES += CR_CLANG_REVISION=\"357692-1\"
unix: DEFINES += __STDC_CONSTANT_MACROS
unix: DEFINES += __STDC_FORMAT_MACROS
unix: DEFINES += _LIBCPP_ABI_UNSTABLE
unix: DEFINES += _LIBCPP_DISABLE_VISIBILITY_ANNOTATIONS
unix: DEFINES += _LIBCXXABI_DISABLE_VISIBILITY_ANNOTATIONS
unix: DEFINES += _LIBCPP_ENABLE_NODISCARD
unix: DEFINES += CR_LIBCXX_REVISION=357619
unix: DEFINES += CR_SYSROOT_HASH=e7c53f04bd88d29d075bfd1f62b073aeb69cbe09
unix: DEFINES += _DEBUG
unix: DEFINES += DYNAMIC_ANNOTATIONS_ENABLED=1
unix: DEFINES += WTF_USE_DYNAMIC_ANNOTATIONS=1
unix: DEFINES += WEBRTC_ENABLE_PROTOBUF=1
unix: DEFINES += WEBRTC_INCLUDE_INTERNAL_AUDIO_DEVICE
unix: DEFINES += RTC_ENABLE_VP9
unix: DEFINES += HAVE_SCTP
unix: DEFINES += WEBRTC_NON_STATIC_TRACE_EVENT_HANDLERS=1
unix: DEFINES += WEBRTC_POSIX
unix: DEFINES += WEBRTC_LINUX
unix: DEFINES += ABSL_ALLOCATOR_NOTHROW=1
unix: DEFINES += HAVE_WEBRTC_VIDEO
