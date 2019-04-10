#include "mainwindow.h"
#include <QApplication>
#include "rtc_base/ssl_adapter.h"
#include <iostream>

#include "mywidget.h"
#include "video_chat.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MyWidget w;

    if (!rtc::InitializeSSL()) {
        std::cerr << "Unable to initialize SSL" << std::endl;
        return -1;
    }

//    thread.Run();
    w.show();

    return app.exec();
}
